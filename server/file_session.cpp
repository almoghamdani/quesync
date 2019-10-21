#include "file_session.h"

#include "../shared/header.h"
#include "../shared/packets/error_packet.h"
#include "../shared/packets/file_chunk_ack_packet.h"
#include "../shared/packets/file_chunk_packet.h"
#include "../shared/packets/session_auth_packet.h"
#include "../shared/response_packet.h"
#include "../shared/utils/files.h"

quesync::server::file_session::file_session(tcp::socket socket, asio::ssl::context &context,
                                            std::shared_ptr<quesync::server::server> server)
    : _socket(std::move(socket), context),  // Copy the client's socket
      _server(server),
      _user(nullptr) {}

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
    std::lock_guard lk(_files_mutex);

    // If there is a file pending for upload with the same id
    if (_upload_files.count(file->id)) {
        throw exception(error::unknown_error);
    }

    // Create a new file for upload
    _upload_files[file->id] = std::make_shared<memory_file>(*file);
}

void quesync::server::file_session::add_download_file(std::shared_ptr<quesync::file> file) {
    std::lock_guard lk(_files_mutex);

    packets::file_chunk_packet file_chunk_packet;
    std::string file_chunk_packet_encoded;
    header header;

    std::shared_ptr<char> packet_buf;

    // If the file is already been downloaded
    if (_download_files.count(file->id)) {
        throw exception(error::file_already_downloading);
    }

    // Add the file to the downloads list
    _download_files[file->id] = std::make_shared<memory_file>(*file);

    // Get the chunks for the file
    _download_files[file->id]->chunks = _server->file_manager()->get_file_chunks(file->id);

    // Format the initial file chunk packet
    file_chunk_packet = packets::file_chunk_packet(file->id, _download_files[file->id]->chunks[0]);

    // Encode the packet
    file_chunk_packet_encoded = file_chunk_packet.encode();

    // Format the header and convert the packet to buffer
    header.size = file_chunk_packet_encoded.size();
    packet_buf = utils::memory::convert_to_buffer<char>(utils::parser::encode_header(header) +
                                                        file_chunk_packet_encoded);

    // Send the initial download packet
    asio::async_write(
        _socket, asio::buffer(packet_buf.get(), file_chunk_packet_encoded.size() + sizeof(header)),
        [this, packet_buf](std::error_code, std::size_t) {});
}

void quesync::server::file_session::remove_file(std::string file_id) {
    std::lock_guard lk(_files_mutex);

    // Erase the file from the downloads and the
    _download_files.erase(file_id);
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

    char *header_buf = new char[sizeof(header)];

    // Get the header of the packet
    asio::async_read(
        _socket, asio::buffer(header_buf, sizeof(header)),
        [this, self, header_buf](std::error_code ec, std::size_t length) {
            header packet_header = utils::parser::decode_header(header_buf);
            std::shared_ptr<char> buf = std::shared_ptr<char>(new char[packet_header.size]);

            // Get a packet from the user
            asio::async_read(
                _socket, asio::buffer(buf.get(), packet_header.size),
                [this, self, buf, packet_header](std::error_code ec, std::size_t length) {
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
                    } else {
                    }
                });
        });
}

void quesync::server::file_session::send(std::string data) {
    auto self(shared_from_this());

    // Create a buffer with the size of the data and copy the data to it
    std::shared_ptr<char> buf(new char[data.length()]);
    memcpy(buf.get(), data.data(), data.length());

    // Send the data to the client
    asio::async_write(_socket, asio::buffer(buf.get(), data.length()),
                      [this, self, buf](std::error_code ec, std::size_t) {
                          // If no error occurred, return to the receiving function
                          if (!ec) {
                              recv();
                          } else {
                          }
                      });
}

std::string quesync::server::file_session::handle_packet(std::string buf) {
    packets::session_auth_packet session_auth_packet;
    packets::file_chunk_packet file_chunk_packet;
    packets::file_chunk_ack_packet file_chunk_ack_packet;

    std::string res;

    unsigned long long amount_of_chunks;

    std::lock_guard lk(_files_mutex);

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

                // Update the next chunk index of the file
                utils::files::update_next_index(_upload_files[file_chunk_packet.file_id()]);

                // Check if the upload is done
                done =
                    _upload_files[file_chunk_packet.file_id()]->chunks.size() == amount_of_chunks;

                // Return to the client an ack packet
                file_chunk_ack_packet = packets::file_chunk_ack_packet(
                    file_chunk_packet.file_id(),
                    _upload_files[file_chunk_packet.file_id()]->current_index, done);
                res = file_chunk_ack_packet.encode();

                // If the uplaod is done
                if (done) {
                    // Save the file
                    _server->file_manager()->save_file(_upload_files[file_chunk_packet.file_id()]);

                    // Remove the file from the uploads list
                    _upload_files.erase(file_chunk_packet.file_id());
                }
            }
        }
    } else if (file_chunk_ack_packet.decode(buf))  // Check if the packet is a file chunk ack packet
    {
        // If the file isn't a file that is currently being downloaded
        if (!_download_files.count(file_chunk_ack_packet.file_id())) {
            res = packets::error_packet(error::file_not_found).encode();
        } else {
            // Calculate the amount of chunks in the file
            amount_of_chunks = utils::files::calc_amount_of_chunks(
                _download_files[file_chunk_ack_packet.file_id()]->file.size);

            // If the next chunk index is valid and the downlaod isn't done
            if (file_chunk_ack_packet.next_index() < amount_of_chunks &&
                !file_chunk_ack_packet.done()) {
                // Create the file chunk packet with the next chunk
                file_chunk_packet =
                    packets::file_chunk_packet(file_chunk_ack_packet.file_id(),
                                               _download_files[file_chunk_ack_packet.file_id()]
                                                   ->chunks[file_chunk_ack_packet.next_index()]);
                res = file_chunk_packet.encode();
            } else if (file_chunk_ack_packet.done()) {
                // Remove the file from the downloads list
                _download_files.erase(file_chunk_ack_packet.file_id());
            }
        }
    }

    return res;
}