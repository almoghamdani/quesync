#include "session.h"

#include <iostream>
#include <termcolor/termcolor.hpp>

#include "../shared/header.h"
#include "../shared/packet.h"
#include "../shared/packets/error_packet.h"
#include "../shared/utils/parser.h"

quesync::server::session::session(tcp::socket socket, asio::ssl::context &context,
                                  std::shared_ptr<quesync::server::server> server)
    : _socket(std::move(socket), context),                  // Copy the client's socket
      _endpoint(_socket.lowest_layer().remote_endpoint()),  // Save the client's endpoint for errors
      _server(server),                                      // Save the server for data transfer,
      _user(nullptr) {}

quesync::server::session::~session() {
    // If the user authenticated
    if (_user) {
        // Unauthenticate session and free the user object
        _server->user_manager()->unauthenticate_session(_user->id);

        // Remove any memory files created by the user
        _server->file_manager()->clear_all_user_memory_files(_user->id);

        try {
            // Leave any voice channel that the user is in
            _server->voice_manager()->leave_voice_channel(_user->id);
        } catch (...) {
        }
    }

    // Close the client's socket in case it's not closed
    try {
        _socket.lowest_layer().close();
    } catch (...) {
    }
}

void quesync::server::session::start() {
    // Start the SSL handshake with the client
    handshake();
}

void quesync::server::session::handshake() {
    auto self(shared_from_this());

    _socket.async_handshake(asio::ssl::stream_base::server,
                            [this, self](const std::error_code &ec) {
                                if (!ec) {
                                    recv();
                                }
                            });
}

void quesync::server::session::recv() {
    auto self(shared_from_this());

    char *header_buf = new char[sizeof(header)];

    // Get the header of the request
    asio::async_read(
        _socket, asio::buffer(header_buf, sizeof(header)),
        [this, self, header_buf](std::error_code ec, std::size_t length) {
            header req_header = utils::parser::decode_header(header_buf);
            std::shared_ptr<char> buf = std::shared_ptr<char>(new char[req_header.size]);

            // Get a request from the user
            asio::async_read(
                _socket, asio::buffer(buf.get(), req_header.size),
                [this, self, buf, req_header](std::error_code ec, std::size_t length) {
                    header header{1, 0};
                    std::string header_str;
                    std::shared_ptr<packet> packet;
                    std::string response;

                    // If no error occurred, parse the request
                    if (!ec) {
                        // Parse the packet
                        packet =
                            utils::parser::parse_packet(std::string(buf.get(), req_header.size));

                        // If the packet has parsed successfully handle it
                        if (packet) {
                            // Handle the client's request and get a respond
                            response = packet->handle(shared_from_this());
                        } else {
                            // Return an invalid packet error packet
                            response = packets::error_packet(error::invalid_packet).encode();
                        }

                        // Set the size of the response
                        header.size = (unsigned int)response.size();

                        // Encode the header in a string
                        header_str = utils::parser::encode_header(header);

                        // Send the header + server's response to the server
                        send(header_str + response);
                    } else {
                        std::cout << termcolor::magenta << "The client "
                                  << _endpoint.address().to_string() << ":" << (int)_endpoint.port()
                                  << " disconnected!" << termcolor::reset << std::endl;
                    }
                });
        });
}

void quesync::server::session::send(std::string data) {
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
                              std::cout << termcolor::magenta << "The client "
                                        << _endpoint.address().to_string() << ":"
                                        << (int)_endpoint.port() << " disconnected!"
                                        << termcolor::reset << std::endl;
                          }
                      });
}

void quesync::server::session::send_only(std::string data) {
    auto self(shared_from_this());

    header header{1, (uint32_t)data.length()};

    // Create a buffer with the size of the data and the header
    std::shared_ptr<char> buf(new char[data.length() + sizeof(header) + 1]);

    // Copy the header
    memcpy(buf.get(), utils::parser::encode_header(header).data(), sizeof(header));

    // Copy the data
    memcpy(buf.get() + sizeof(header), data.data(), data.length());

    // Send the data to the client
    asio::async_write(_socket, asio::buffer(buf.get(), data.length() + sizeof(header)),
                      [this, self, buf](std::error_code, std::size_t) {});
}

std::shared_ptr<quesync::server::server> quesync::server::session::server() const {
    return _server;
}

std::shared_ptr<quesync::server::session> quesync::server::session::get_shared() {
    return shared_from_this();
}

void quesync::server::session::set_user(std::shared_ptr<quesync::user> user) { _user = user; }

bool quesync::server::session::authenticated() const { return (_user != nullptr); }

std::shared_ptr<quesync::user> quesync::server::session::user() const { return _user; }