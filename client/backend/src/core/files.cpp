#include "files.h"

#include <chrono>
#include <fstream>

#include "client.h"

#include "../../../../shared/exception.h"
#include "../../../../shared/packets/download_file_packet.h"
#include "../../../../shared/packets/error_packet.h"
#include "../../../../shared/packets/file_chunk_packet.h"
#include "../../../../shared/packets/file_transmission_stop_packet.h"
#include "../../../../shared/packets/get_file_info_packet.h"
#include "../../../../shared/packets/session_auth_packet.h"
#include "../../../../shared/packets/upload_file_packet.h"
#include "../../../../shared/utils/files.h"
#include "../../../../shared/utils/parser.h"

quesync::client::modules::files::files(std::shared_ptr<quesync::client::client> client)
    : module(client), _socket(nullptr), _stop_threads(true), _io_context(nullptr) {}

std::shared_ptr<quesync::file> quesync::client::modules::files::start_upload(
    std::string file_path) {
    packets::upload_file_packet upload_file_packet;
    std::shared_ptr<response_packet> response_packet;

    std::shared_ptr<file> file;
    std::shared_ptr<memory_file> file_info;

    std::ifstream file_stream;
    unsigned long long file_size;

    std::unique_lock lk(_uploads_mutex, std::defer_lock);

    // Try to open the file for reading
    file_stream.open(file_path, std::ios::binary | std::ios::in);
    if (file_stream.fail()) {
        throw exception(error::file_not_found);
    }

    // If no connected to file server, connect
    if (!_socket) {
        connect_to_file_server();
    }

    // Read the entire stream and get the amount of chars read to get the file size
    file_stream.ignore(std::numeric_limits<std::streamsize>::max());
    file_size = file_stream.gcount();
    if (!file_size) {
        throw exception(error::empty_file);
    }

    // Reset file to beg
    file_stream.clear();  // Reset EOF
    file_stream.seekg(0, std::ios_base::beg);

    // Send to the server the upload file packet
    upload_file_packet = packets::upload_file_packet(get_file_name(file_path), file_size);
    response_packet = _client->communicator()->send_and_verify(
        &upload_file_packet, packet_type::file_upload_initiated_packet);

    // Create the file object
    file = std::make_shared<quesync::file>(response_packet->json()["file"].get<quesync::file>());

    // Create the memory file object
    file_info = std::make_shared<memory_file>(*file);

    // Get the file's chunks
    file_info->chunks = utils::files::get_file_chunks(file_stream, file_size);

    // Lock the uploads data mutex
    _uploads_mutex.lock();

    // Init the upload progress
    _uploads_progress[file->id] = 0;

    // Unlock the uploads data mutex
    _uploads_mutex.unlock();

    // Init the upload
    upload(file_info);

    return file;
}

void quesync::client::modules::files::start_download(std::string file_id,
                                                     std::string download_path) {
    packets::download_file_packet download_file_packet(file_id);

    std::shared_ptr<file> file;
    std::ofstream dest_file(download_path);

    std::unique_lock lk(_downloads_mutex, std::defer_lock);

    // Check if the dest file doesn't exists
    if (dest_file.fail()) {
        throw exception(error::invalid_download_file_path);
    }

    // Lock the data mutex
    lk.lock();

    // Check if the file is already downloading
    if (_download_files.count(file_id)) {
        throw exception(error::file_not_found);
    }

    // Unlock the data mutex
    lk.unlock();

    // Get the file object
    file = get_file_info(file_id);

    // Lock the data mutex
    lk.lock();

    // Create the memory file object
    _download_files[file->id] = std::make_shared<memory_file>(*file);

    // Save the download path
    _download_paths[file->id] = download_path;

    // Unlock the data mutex
    lk.unlock();

    // If no connected to file server, connect
    if (!_socket) {
        connect_to_file_server();
    }

    // Send to the server the download file packet to start the download
    _client->communicator()->send_and_verify(&download_file_packet,
                                             packet_type::file_download_initiated_packet);
}

void quesync::client::modules::files::stop_file_transmission(std::string file_id) {
    packets::file_transmission_stop_packet transmission_stop_packet(file_id);

    std::unique_lock downloads_lk(_downloads_mutex, std::defer_lock);
    std::unique_lock uploads_lk(_uploads_mutex, std::defer_lock);

    // Lock the data mutexes
    std::lock(downloads_lk, uploads_lk);

    // If the file isn't downloaded or uploaded, throw error
    if (!_download_files.count(file_id) && !_uploads_progress.count(file_id)) {
        throw exception(error::file_not_found);
    }

    // Unlock the data mutex
    downloads_lk.unlock();
    uploads_lk.unlock();

    // Send to the server the file transmission stop packet
    _client->communicator()->send_and_verify(&transmission_stop_packet,
                                             packet_type::file_transmission_stopped_packet);

    // Lock the data mutex
    std::lock(downloads_lk, uploads_lk);

    // Remove it from the downloads if exists
    _uploads_progress.erase(file_id);
    _download_files.erase(file_id);
    _download_paths.erase(file_id);
}

std::shared_ptr<quesync::file> quesync::client::modules::files::get_file_info(std::string file_id) {
    packets::get_file_info_packet get_file_info_packet(file_id);
    std::shared_ptr<response_packet> response_packet;

    std::shared_ptr<file> file;

    // Send to the server the download file packet
    response_packet = _client->communicator()->send_and_verify(&get_file_info_packet,
                                                               packet_type::file_info_packet);

    // Create the file object
    file = std::make_shared<quesync::file>(response_packet->json()["file"].get<quesync::file>());

    return file;
}

void quesync::client::modules::files::handle_packet(std::string buf) {
    unsigned long long amount_of_chunks;

    packets::file_chunk_packet file_chunk_packet;

    std::shared_ptr<events::file_transmission_progress_event> file_progress_event;

    std::unique_lock downloads_lk(_downloads_mutex);

    // If the packet is a file chunk packet
    if (file_chunk_packet.decode(buf)) {
        // If the file is a file that is currently being downloaded
        if (_download_files.count(file_chunk_packet.file_id())) {
            // Calculate the amount of chunks in the file
            amount_of_chunks = utils::files::calc_amount_of_chunks(
                _download_files[file_chunk_packet.file_id()]->file.size);

            // Check if the index of the chunk is valid
            if (file_chunk_packet.chunk().index < amount_of_chunks) {
                bool done = false;

                // Save the chunk
                _download_files[file_chunk_packet.file_id()]
                    ->chunks[file_chunk_packet.chunk().index] = file_chunk_packet.chunk();

                // Check if the download is done
                done =
                    _download_files[file_chunk_packet.file_id()]->chunks.size() == amount_of_chunks;

                // If the download is done
                if (done) {
                    // Save the file in a different thread
                    std::thread([this, file = _download_files[file_chunk_packet.file_id()],
                                 download_path = _download_paths[file_chunk_packet.file_id()]] {
                        save_file(file, download_path);
                    })
                        .detach();

                    // Create the file progress event
                    file_progress_event =
                        std::make_shared<events::file_transmission_progress_event>(
                            file_chunk_packet.file_id(),
                            _download_files[file_chunk_packet.file_id()]->file.size);

                    // Remove the file from the downloads list
                    _download_files.erase(file_chunk_packet.file_id());
                    _download_paths.erase(file_chunk_packet.file_id());
                } else {
                    // Create the file progress event
                    file_progress_event =
                        std::make_shared<events::file_transmission_progress_event>(
                            file_chunk_packet.file_id(),
                            _download_files[file_chunk_packet.file_id()]->chunks.size() *
                                FILE_CHUNK_SIZE);
                }
            }
        }
    }

    // Unlock the downloads mutex
    downloads_lk.unlock();

    // If the file progress event isn't null
    if (file_progress_event) {
        // Set the event for the file
        std::lock_guard events_lk(_events_mutex);
        _events[file_progress_event->file_id] = file_progress_event;
    }
}

void quesync::client::modules::files::events_thread() {
    while (true) {
        std::unique_lock events_lk(_events_mutex, std::defer_lock);

        std::this_thread::sleep_for(std::chrono::milliseconds(EVENTS_THREAD_SLEEP));

        // Lock the events mutex
        events_lk.lock();

        // If signaled to exit and the events vector is empty, break
        if (_stop_threads && _events.empty()) {
            break;
        }

        // For each event, trigger it for the client
        for (auto& file_event : _events) {
            try {
                _client->communicator()->event_handler().call_event(file_event.second);
            } catch (...) {
            }
        }

        // Clear the events map
        _events.clear();
    }
}

void quesync::client::modules::files::connect_to_file_server() {
    packets::session_auth_packet session_auth_packet(_client->auth()->get_session_id());
    std::string session_auth_packet_encoded = session_auth_packet.encode();
    std::string res;

    tcp::endpoint server_endpoint;

    // Get the endpoint of the files server
    socket_manager::get_endpoint(_client->communicator()->server_ip().c_str(), FILES_SERVER_PORT,
                                 server_endpoint);

    // Create the I/O Context object
    _io_context = std::make_shared<asio::io_context>();

    try {
        // Create a TCP/SSL socket
        _socket = new asio::ssl::stream<tcp::socket>(*_io_context, socket_manager::ssl_context);
        _socket->set_verify_mode(asio::ssl::verify_peer);

        // Connect to the server
        _socket->lowest_layer().connect(server_endpoint);

        // Try to handshake
        _socket->handshake(asio::ssl::stream_base::client);
    } catch (std::system_error& ex) {
        throw exception(socket_manager::error_for_system_error(ex));
    } catch (...) {
        throw exception(error::unknown_error);
    }

    // Send to the server the session auth packet
    socket_manager::send(*_socket, session_auth_packet_encoded);

    // Get a response from the server
    res = socket_manager::recv(*_socket);
    if (utils::parser::parse_packet(res)->type() != packet_type::authenticated_packet) {
        throw exception(error::unknown_error);
    }

    // If the events thread is still alive, join it
    if (_events_thread.joinable()) {
        _events_thread.join();
    }

    // If the I/O thread is still alive, join it
    if (_io_thread.joinable()) {
        _io_thread.join();
    }

    // Start receiving
    recv();

    // Run a background thread that will handle I/O
    _io_thread = std::thread([this]() { _io_context->run(); });

    // Allow the threads to run
    _stop_threads = false;

    // Init events thread
    _events_thread = std::thread(&files::events_thread, this);
}

void quesync::client::modules::files::recv() {
    std::shared_ptr<char> header_buf = std::shared_ptr<char>(new char[sizeof(header)]);

    // Get the header of the packet
    asio::async_read(
        *_socket, asio::buffer(header_buf.get(), sizeof(header)),
        [this, header_buf](std::error_code ec, std::size_t length) {
            header packet_header = utils::parser::decode_header(header_buf.get());
            std::shared_ptr<char> buf = std::shared_ptr<char>(new char[packet_header.size]);

            // If got an error, clean the connection
            if (ec) {
                clean_connection();
                return;
            }

            // Get a packet from the user
            asio::async_read(*_socket, asio::buffer(buf.get(), packet_header.size),
                             [this, buf, packet_header](std::error_code ec, std::size_t length) {
                                 header header{1, 0};
                                 std::string header_str;

                                 std::string buf_str;
                                 std::string res;

                                 // If no error occurred, parse the packet
                                 if (!ec) {
                                     buf_str = std::string(buf.get(), packet_header.size);

                                     // Handle the packet
                                     handle_packet(buf_str);

                                     // Check if the files COM is idle
                                     check_if_idle();

                                     // Recv again if the socket isn't closed
                                     if (_socket) {
                                         recv();
                                     }
                                 } else {
                                     // Clean the connection on error
                                     clean_connection();
                                 }
                             });
        });
}

void quesync::client::modules::files::upload(std::shared_ptr<quesync::memory_file> file_info) {
    packets::file_chunk_packet file_chunk_packet;
    std::string file_chunk_packet_encoded;
    header header;

    std::shared_ptr<char> packet_buf;

    // Format the first file chunk packet
    file_chunk_packet = packets::file_chunk_packet(file_info->file.id, file_info->chunks[0]);

    // Encode the packet
    file_chunk_packet_encoded = file_chunk_packet.encode();

    // Format the header and convert the packet to buffer
    header.size = file_chunk_packet_encoded.size();
    packet_buf = utils::memory::convert_to_buffer<char>(utils::parser::encode_header(header) +
                                                        file_chunk_packet_encoded);

    // Send async the file chunk packet
    asio::async_write(
        *_socket, asio::buffer(packet_buf.get(), file_chunk_packet_encoded.size() + sizeof(header)),
        [this, file_info, packet_buf](std::error_code ec, std::size_t) {
            // On error, clean connection
            if (ec) {
                clean_connection();
                return;
            }

            // Handle the upload file chunk sent
            handle_upload_chunk_sent(file_info);

            // Check if the file COM is idle
            check_if_idle();
        });
}

void quesync::client::modules::files::handle_upload_chunk_sent(
    std::shared_ptr<quesync::memory_file> file_info) {
    packets::file_chunk_packet file_chunk_packet;
    std::string file_chunk_packet_encoded;

    header header;
    std::shared_ptr<char> packet_buf;

    std::shared_ptr<events::file_transmission_progress_event> file_progress_event;

    std::unique_lock uploads_lk(_uploads_mutex), events_lk(_events_mutex, std::defer_lock);

    unsigned long long current_chunk = 0;

    // If the file is in the uploads list
    if (_uploads_progress.count(file_info->file.id)) {
        // Get the current chunk and increase to the next chunk
        current_chunk = _uploads_progress[file_info->file.id]++;

        // Unlock the uploads mutex
        uploads_lk.unlock();

        // Remove the current chunk
        file_info->chunks.erase(current_chunk);

        // Create the file progress event
        file_progress_event = std::make_shared<events::file_transmission_progress_event>(
            file_info->file.id,
            std::min((current_chunk + 1) * FILE_CHUNK_SIZE, file_info->file.size));

        // Lock the events lock
        events_lk.lock();

        // Set the event for the file
        _events[file_info->file.id] = file_progress_event;

        // Unlock the events lock
        events_lk.unlock();

        // If the file has more chunks to upload
        if (file_info->chunks.count(current_chunk + 1)) {
            // Format the file chunk packet
            file_chunk_packet = packets::file_chunk_packet(file_info->file.id,
                                                           file_info->chunks[current_chunk + 1]);

            // Encode the packet
            file_chunk_packet_encoded = file_chunk_packet.encode();

            // Format the header and convert the packet to buffer
            header.size = file_chunk_packet_encoded.size();
            packet_buf = utils::memory::convert_to_buffer<char>(
                utils::parser::encode_header(header) + file_chunk_packet_encoded);

            // Send async the file chunk packet
            asio::async_write(
                *_socket,
                asio::buffer(packet_buf.get(), file_chunk_packet_encoded.size() + sizeof(header)),
                [this, file_info, packet_buf](std::error_code ec, std::size_t) {
                    // On error, clean connection
                    if (ec) {
                        clean_connection();
                        return;
                    }

                    // Handle the upload file chunk sent
                    handle_upload_chunk_sent(file_info);

                    // Check if the file COM is idle
                    check_if_idle();
                });
        } else {
            // Lock the uploads mutex
            uploads_lk.lock();

            // Remove the file as being uploaded
            _uploads_progress.erase(file_info->file.id);

            // Unlock the uploads mutex
            uploads_lk.unlock();
        }
    }
}

void quesync::client::modules::files::check_if_idle() {
    std::unique_lock downloads_lk(_downloads_mutex, std::defer_lock);
    std::unique_lock uploads_lk(_uploads_mutex, std::defer_lock);

    // Lock the data mutexes
    std::lock(downloads_lk, uploads_lk);

    // If the downloads list or the uploads list isn't empty
    if (!_download_files.empty() || !_uploads_progress.empty()) {
        return;
    }

    // Unlock the data mutex
    downloads_lk.unlock();
    uploads_lk.unlock();

    // If reached here, it means both uploads and downloads are empty, so clean connection
    clean_connection();
}

void quesync::client::modules::files::save_file(std::shared_ptr<quesync::memory_file> file,
                                                std::string download_path) {
    std::ofstream fd;
    std::string file_content;

    std::vector<file_chunk> chunks;
    std::vector<std::shared_ptr<unsigned char>> chunks_buffers;

    // Reserve memory for the all the file chunks and the chunk buffers
    chunks.resize(file->chunks.size());
    chunks_buffers.resize(file->chunks.size());

    // Get all the chunks from the chunks map
    std::transform(file->chunks.begin(), file->chunks.end(), chunks.begin(),
                   [](const decltype(file->chunks)::value_type& p) { return p.second; });

    // Sort the chunks
    std::sort(chunks.begin(), chunks.end(),
              [](const file_chunk& a, const file_chunk& b) { return a.index < b.index; });

    // Get all the buffers of the chunks
    std::transform(chunks.begin(), chunks.end(), chunks_buffers.begin(),
                   [](const file_chunk& p) { return p.data; });

    // Merge the buffers to get the full file content
    file_content = utils::memory::merge_buffers(chunks_buffers, FILE_CHUNK_SIZE);

    // Get only the file content without padding
    file_content = file_content.substr(0, file->file.size);

    // Open the dest file
    fd.open(download_path, std::ios::out | std::ios::binary);
    if (fd.fail()) {
        return;
    }

    // Write the file
    fd << file_content;
    fd.close();
}

std::string quesync::client::modules::files::get_file_name(std::string file_path) {
    return file_path.substr(file_path.find_last_of("/\\") + 1);
}

void quesync::client::modules::files::clean_connection() {
    // Stop the I/O threads
    if (_io_context) {
        _io_context->stop();
    }

    // Signal the threads to exit
    _stop_threads = true;

    // If the socket is connected to the server
    if (_socket && _socket->lowest_layer().is_open()) {
        // Close the socket
        _socket->lowest_layer().close();
    }

    // If the events thread is still alive, join it
    if (_events_thread.joinable()) {
        _events_thread.join();
    }

    // If the I/O thread is still alive, join it
    if (_io_thread.joinable() && std::this_thread::get_id() != _io_thread.get_id()) {
        _io_thread.join();
    }

    // If the socket object exists
    if (_socket) {
        // Free the socket
        delete _socket;

        // Reset socket ptr
        _socket = nullptr;
    }

    // Free the I/O Context
    _io_context = nullptr;

    // Clear the files data
    _uploads_progress.clear();
    _download_files.clear();
    _download_paths.clear();
    _events.clear();
}

void quesync::client::modules::files::logged_out() { clean_connection(); }