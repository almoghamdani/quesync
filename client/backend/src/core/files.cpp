#include "files.h"

#include <chrono>
#include <fstream>

#include "client.h"

#include "../../../../shared/events/file_transmission_progress_event.h"
#include "../../../../shared/exception.h"
#include "../../../../shared/packets/download_file_packet.h"
#include "../../../../shared/packets/error_packet.h"
#include "../../../../shared/packets/file_chunk_ack_packet.h"
#include "../../../../shared/packets/file_chunk_packet.h"
#include "../../../../shared/packets/get_file_info_packet.h"
#include "../../../../shared/packets/session_auth_packet.h"
#include "../../../../shared/packets/upload_file_packet.h"
#include "../../../../shared/utils/files.h"
#include "../../../../shared/utils/parser.h"

quesync::client::modules::files::files(std::shared_ptr<quesync::client::client> client)
    : module(client), _socket(nullptr) {}

std::shared_ptr<quesync::file> quesync::client::modules::files::start_upload(
    std::string file_path) {
    packets::upload_file_packet upload_file_packet;
    std::shared_ptr<response_packet> response_packet;

    std::shared_ptr<file> file;

    std::ifstream file_stream;
    unsigned long long file_size;

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
    _upload_files[file->id] = std::make_shared<memory_file>(*file);
    _files_progress_history[file->id] = 0;

    // Get the file's chunks
    _upload_files[file->id]->chunks = utils::files::get_file_chunks(file_stream, file_size);

    // Init the upload
    init_upload(file->id);

    return file;
}

void quesync::client::modules::files::start_download(std::string file_id,
                                                     std::string download_path) {
    packets::download_file_packet download_file_packet(file_id);

    std::shared_ptr<file> file;
    std::ofstream dest_file(download_path);

    // Check if the dest file doesn't exists
    if (dest_file.fail()) {
        throw exception(error::invalid_download_file_path);
    }

    // Check if the file is already downloading
    if (_download_files.count(file_id)) {
        throw exception(error::file_not_found);
    }

    // Get the file object
    file = get_file_info(file_id);

    // Create the memory file object
    _download_files[file->id] = std::make_shared<memory_file>(*file);
    _files_progress_history[file->id] = 0;

    // Save the download path
    _download_paths[file->id] = download_path;

    // If no connected to file server, connect
    if (!_socket) {
        connect_to_file_server();
    }

    // Send to the server the download file packet to start the download
    _client->communicator()->send_and_verify(&download_file_packet,
                                             packet_type::file_download_initiated_packet);
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

void quesync::client::modules::files::com_thread() {
    bool first_iteration = true;

    std::string buf, res;

    unsigned long long amount_of_chunks;

    while (true) {
        packets::file_chunk_packet file_chunk_packet;
        packets::file_chunk_ack_packet file_chunk_ack_packet;

        std::shared_ptr<events::file_transmission_progress_event> file_progress_event;

        // If the socket isn't connected, quit the loop
        if (!_socket) {
            break;
        } else if (!first_iteration && _upload_files.empty() &&
                   _download_files.empty())  // If no uploads and downloads
        {
            // Clean the socket and quit the loop
            clean_connection();
            break;
        }

        // Recv a message from the server
        buf = socket_manager::recv(*_socket);

        // If the packet is a file chunk packet
        if (file_chunk_packet.decode(buf)) {
            // If the file isn't a file that is currently being downloaded
            if (!_download_files.count(file_chunk_packet.file_id())) {
                res = packets::error_packet(error::unknown_error).encode();
            } else {
                // Calculate the amount of chunks in the file
                amount_of_chunks = utils::files::calc_amount_of_chunks(
                    _download_files[file_chunk_packet.file_id()]->file.size);

                // Check if the index of the chunk is valid
                if (file_chunk_packet.chunk().index < amount_of_chunks) {
                    bool done = false;

                    // Save the chunk
                    _download_files[file_chunk_packet.file_id()]
                        ->chunks[file_chunk_packet.chunk().index] = file_chunk_packet.chunk();

                    // Update the next chunk index of the file
                    utils::files::update_next_index(_download_files[file_chunk_packet.file_id()]);

                    // Check if the download is done
                    done = _download_files[file_chunk_packet.file_id()]->chunks.size() ==
                           amount_of_chunks;

                    // Return to the server an ack packet
                    file_chunk_ack_packet = packets::file_chunk_ack_packet(
                        file_chunk_packet.file_id(),
                        _download_files[file_chunk_packet.file_id()]->current_index, done);
                    res = file_chunk_ack_packet.encode();

                    // If the download is done
                    if (done) {
                        // Save the file
                        save_file(_download_files[file_chunk_packet.file_id()]);

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
        } else if (file_chunk_ack_packet.decode(buf))  // If the packet is an file chunk ack packet
        {
            // Check if the file exists as an upload file
            if (!_upload_files.count(file_chunk_ack_packet.file_id())) {
                res = packets::error_packet(error::file_not_found).encode();
            } else {
                // Calculate the amount of chunks the file
                amount_of_chunks = utils::files::calc_amount_of_chunks(
                    _upload_files[file_chunk_ack_packet.file_id()]->file.size);

                // If the next chunk index is valid and the downlaod isn't done
                if (file_chunk_ack_packet.next_index() < amount_of_chunks &&
                    !file_chunk_ack_packet.done()) {
                    // Create the file chunk packet with the next chunk
                    file_chunk_packet = packets::file_chunk_packet(
                        file_chunk_ack_packet.file_id(),
                        _upload_files[file_chunk_ack_packet.file_id()]
                            ->chunks[file_chunk_ack_packet.next_index()]);
                    res = file_chunk_packet.encode();

                    // Create the file progress event
                    file_progress_event =
                        std::make_shared<events::file_transmission_progress_event>(
                            file_chunk_packet.file_id(),
                            std::min(file_chunk_ack_packet.next_index() * FILE_CHUNK_SIZE,
                                     _upload_files[file_chunk_ack_packet.file_id()]->file.size));
                } else if (file_chunk_ack_packet.done()) {
                    // Create the file progress event
                    file_progress_event =
                        std::make_shared<events::file_transmission_progress_event>(
                            file_chunk_packet.file_id(),
                            _upload_files[file_chunk_ack_packet.file_id()]->file.size);

                    // Remove the file from the downloads list
                    _upload_files.erase(file_chunk_ack_packet.file_id());
                }
            }
        }

        // If the file progress event isn't null and the bytes diff between the last event is bigger
        // then the threshold
        if (file_progress_event &&
            file_progress_event->bytes - _files_progress_history[file_progress_event->file_id] >=
                FILE_PROGRESS_THRESHOLD) {
            _files_progress_history[file_progress_event->file_id] = file_progress_event->bytes;

            try {
                // Call the file progress event
                _client->communicator()->event_handler().call_event(
                    std::static_pointer_cast<event>(file_progress_event));
            } catch (...) {
            }
        }

        if (!res.empty()) {
            // Lock the send mutex and send the response
            std::lock_guard lk(_send_mutex);
            socket_manager::send(*_socket, res);
        }

        // Clear first iteration flag
        first_iteration = false;
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

    try {
        // Create a TCP/SSL socket
        _socket = new asio::ssl::stream<tcp::socket>(socket_manager::io_context,
                                                     socket_manager::ssl_context);
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

    // Init COM thread
    _com_thread = std::thread(&files::com_thread, this);
    _com_thread.detach();
}

void quesync::client::modules::files::init_upload(std::string file_id) {
    packets::file_chunk_packet file_chunk_packet;

    // Format the first file chunk packet
    file_chunk_packet = packets::file_chunk_packet(_upload_files[file_id]->file.id,
                                                   _upload_files[file_id]->chunks[0]);

    // Send the first chunk to the file server
    std::lock_guard lk(_send_mutex);
    socket_manager::send(*_socket, file_chunk_packet.encode());
}

void quesync::client::modules::files::save_file(std::shared_ptr<quesync::memory_file> file) {
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
    fd.open(_download_paths[file->file.id], std::ios::out | std::ios::binary);
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
    // If we currently have a socket with the server, close it and delete it
    if (_socket) {
        // Close the socket
        _socket->lowest_layer().close();

        // Free the socket
        delete _socket;

        // Reset socket ptr
        _socket = nullptr;
    }
}

void quesync::client::modules::files::disconnected() {
    // Clear the files data
    _files_progress_history.clear();
    _upload_files.clear();
    _download_files.clear();
    _download_paths.clear();
}