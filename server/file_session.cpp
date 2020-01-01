#include "file_session.h"

#include "../shared/header.h"
#include "../shared/packets/error_packet.h"
#include "../shared/packets/file_chunk_packet.h"
#include "../shared/packets/session_auth_packet.h"
#include "../shared/response_packet.h"
#include "../shared/utils/files.h"

quesync::server::file_session::file_session(tcp::socket socket, asio::ssl::context &context,
                                            std::shared_ptr<quesync::server::server> server)
    : _socket(std::move(socket), context),  // Copy the client's socket
      _server(server),
      _user(nullptr),
      _strand(_server->get_io_context()) {}

quesync::server::file_session::~file_session() {
    // Close the client's socket in case it's not closed
    try {
        _socket.lowest_layer().close();
    } catch (...) {
    }
}

void quesync::server::file_session::start() {
    // Start the SSL handshake with the client
    handshake();
}

void quesync::server::file_session::add_upload_file(std::shared_ptr<quesync::file> file) {
    std::lock_guard lk(_uploads_mutex);

    // If there is a file pending for upload with the same id
    if (_upload_files.count(file->id)) {
        throw exception(error::unknown_error);
    }

    // Create a new file for upload
    _upload_files[file->id] = std::make_shared<memory_file>(*file);
}

void quesync::server::file_session::add_download_file(std::shared_ptr<quesync::file> file) {
    std::shared_ptr<memory_file> file_info = std::make_shared<memory_file>(*file);

    packets::file_chunk_packet file_chunk_packet;
    std::string file_chunk_packet_encoded;
    header header;

    std::shared_ptr<char> packet_buf;

    std::unique_lock downloads_lk(_downloads_mutex);

    // If the file is already being downloaded
    if (_downloads_progress.count(file->id)) {
        throw exception(error::file_already_downloading);
    }

    // Init the download progress of the file
    _downloads_progress[file->id] = 0;

    // Unlock the downloads mutex
    downloads_lk.unlock();

    // Get the chunks for the file
    file_info->chunks = _server->file_manager()->get_file_chunks(file->id);

    // Format the first file chunk packet
    file_chunk_packet = packets::file_chunk_packet(file->id, file_info->chunks[0]);

    // Encode the packet
    file_chunk_packet_encoded = file_chunk_packet.encode();

    // Format the header and convert the packet to buffer
    header.size = (uint32_t)file_chunk_packet_encoded.size();
    packet_buf = utils::memory::convert_to_buffer<char>(utils::parser::encode_header(header) +
                                                        file_chunk_packet_encoded);

    // Send async the file chunk packet
    asio::async_write(
        _socket, asio::buffer(packet_buf.get(), file_chunk_packet_encoded.size() + sizeof(header)),
        _strand.wrap([this, file_info, packet_buf](std::error_code ec, std::size_t) {
            if (!ec) {
                handle_download_chunk_sent(file_info);
            }
        }));
}

void quesync::server::file_session::remove_file(std::string file_id) {
    std::unique_lock downloads_lk(_downloads_mutex, std::defer_lock);
    std::unique_lock uplaods_lk(_uploads_mutex, std::defer_lock);

    // Lock the files mutexes
    std::lock(downloads_lk, uplaods_lk);

    // Erase the file from the uploads and downloads
    _downloads_progress.erase(file_id);
    _upload_files.erase(file_id);
}

void quesync::server::file_session::handshake() {
    auto self(shared_from_this());

    _socket.async_handshake(asio::ssl::stream_base::server,
                            [this, self](const std::error_code &ec) {
                                if (!ec) {
                                    recv();
                                }
                            });
}

void quesync::server::file_session::recv() {
    auto self(shared_from_this());

    std::shared_ptr<char> header_buf = std::shared_ptr<char>(new char[sizeof(header)]);

    // Get the header of the packet
    asio::async_read(
        _socket, asio::buffer(header_buf.get(), sizeof(header)),
        _strand.wrap([this, self, header_buf](std::error_code ec, std::size_t length) {
            if (ec) return;

            header packet_header = utils::parser::decode_header(header_buf.get());
            std::shared_ptr<char> buf = std::shared_ptr<char>(new char[packet_header.size]);

            // Get a packet from the user
            asio::async_read(
                _socket, asio::buffer(buf.get(), packet_header.size),
                _strand.wrap([this, self, buf, packet_header](std::error_code ec, std::size_t length) {
                    header header{1, 0};
                    std::string header_str;

                    std::string buf_str;
                    std::string res;

                    // If no error occurred, parse the packet
                    if (!ec) {
                        buf_str = std::string(buf.get(), packet_header.size);

                        // Handle the packet
                        res = handle_packet(buf_str);

                        // If the resposne isn't empty
                        if (!res.empty()) {
                            // Set the size of the response
                            header.size = (unsigned int)res.size();

                            // Encode the header in a string
                            header_str = utils::parser::encode_header(header);

                            // Send the header + server's response to the server
                            send(header_str + res);
                        } else {
                            recv();
                        }
                    }
                }));
        }));
}

void quesync::server::file_session::send(std::string data) {
    auto self(shared_from_this());

    // Create a buffer with the size of the data and copy the data to it
    std::shared_ptr<char> buf(new char[data.length()]);
    memcpy(buf.get(), data.data(), data.length());

    // Send the data to the client
    asio::async_write(_socket, asio::buffer(buf.get(), data.length()),
                      _strand.wrap([this, self, buf](std::error_code ec, std::size_t) {
                          // If no error occurred, return to the receiving function
                          if (!ec) {
                              recv();
                          }
                      }));
}

std::string quesync::server::file_session::handle_packet(std::string buf) {
    packets::session_auth_packet session_auth_packet;
    packets::file_chunk_packet file_chunk_packet;

    std::string res;

    unsigned long long amount_of_chunks;

    std::unique_lock uploads_lk(_uploads_mutex, std::defer_lock);

    // Check if the user is yet to be authenticated and the packet is a session
    // auth packet
    if (!_user &&
        buf.substr(0, sizeof(PACKET_IDENTIFIER) + PACKET_TYPE_LEN + 1) == "QUESYNC|010|" &&
        session_auth_packet.decode(buf.substr(sizeof(PACKET_IDENTIFIER) + PACKET_TYPE_LEN + 1))) {
        try {
            // Try to get the user of the given session id
            _user = _server->user_manager()
                        ->get_authenticated_session_of_user(
                            _server->session_manager()->get_user_id_for_session(
                                session_auth_packet.session_id()))
                        ->user();

            // Register the file session for the user
            _server->file_manager()->register_user_file_session(shared_from_this(), _user->id);

            res = response_packet(packet_type::authenticated_packet).encode();
        } catch (exception &ex) {
            res = packets::error_packet(ex.error_code()).encode();
        }
    } else if (!_user)  // Check if the user is not authenticated
    {
        res = packets::error_packet(error::not_authenticated).encode();
    } else if (file_chunk_packet.decode(buf))  // Check if the packet is a file chunk packet
    {
        // Lock the uploads mutex
        uploads_lk.lock();

        // If the file isn't a file that is currently being uploaded
        if (!_upload_files.count(file_chunk_packet.file_id())) {
            res = packets::error_packet(error::file_not_found).encode();
        } else {
            // Calculate the amount of chunks in the file
            amount_of_chunks = utils::files::calc_amount_of_chunks(
                _upload_files[file_chunk_packet.file_id()]->file.size);

            // Check if the index of the chunk is valid
            if (file_chunk_packet.chunk().index < amount_of_chunks) {
                bool done = false;

                // Save the chunk
                _upload_files[file_chunk_packet.file_id()]
                    ->chunks[file_chunk_packet.chunk().index] = file_chunk_packet.chunk();

                // Check if the upload is done
                done =
                    _upload_files[file_chunk_packet.file_id()]->chunks.size() == amount_of_chunks;

                // If the uplaod is done
                if (done) {
                    // Save the file
                    _server->file_manager()->save_file(_upload_files[file_chunk_packet.file_id()]);

                    // Remove the file from the uploads list
                    _upload_files.erase(file_chunk_packet.file_id());
                }
            }
        }
    }

    return res;
}

void quesync::server::file_session::handle_download_chunk_sent(
    std::shared_ptr<quesync::memory_file> file_info) {
    packets::file_chunk_packet file_chunk_packet;
    std::string file_chunk_packet_encoded;
    header header;

    std::shared_ptr<char> packet_buf;

    std::unique_lock downloads_lk(_downloads_mutex);

    unsigned long long current_chunk = 0;

    // If the file is currently being downloaded
    if (_downloads_progress.count(file_info->file.id)) {
        // Get the current chunk and increase to the next chunk
        current_chunk = _downloads_progress[file_info->file.id]++;

        // Unlock the downloads mutex
        downloads_lk.unlock();

        // Remove the current chunk
        file_info->chunks.erase(current_chunk);

        // If the file has more chunks to upload to client
        if (file_info->chunks.count(current_chunk + 1)) {
            // Format the file chunk packet
            file_chunk_packet = packets::file_chunk_packet(file_info->file.id,
                                                           file_info->chunks[current_chunk + 1]);

            // Encode the packet
            file_chunk_packet_encoded = file_chunk_packet.encode();

            // Format the header and convert the packet to buffer
            header.size = (uint32_t)file_chunk_packet_encoded.size();
            packet_buf = utils::memory::convert_to_buffer<char>(
                utils::parser::encode_header(header) + file_chunk_packet_encoded);

            // Send async the file chunk packet
            asio::async_write(
                _socket,
                asio::buffer(packet_buf.get(), file_chunk_packet_encoded.size() + sizeof(header)),
                _strand.wrap([this, file_info, packet_buf](std::error_code ec, std::size_t) {
                    if (!ec) {
                        handle_download_chunk_sent(file_info);
                    }
                }));
        } else {
            // Lock the downloads mutex
            downloads_lk.lock();

            // Remove the file as being downloaded
            _downloads_progress.erase(file_info->file.id);

            // Unlock the downloads mutex
            downloads_lk.unlock();
        }
    }
}