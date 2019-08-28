#include "server.h"

#include <iostream>
#include <termcolor/termcolor.hpp>

#include "session.h"

quesync::server::server::server(asio::io_context &io_context)
    : _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT)),
      _context(asio::ssl::context::sslv23),
      _sess("localhost", 33060, "server", "123456789"),
      _db(_sess, "quesync") {
    _context.set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2);
    _context.use_certificate_chain_file("server.pem");
    _context.use_private_key_file("server.pem", asio::ssl::context::pem);

    // Use the quesync database
    _sess.sql("USE quesync").execute();
}

quesync::server::server::~server() {
    // Close all socket handlers
    _acceptor.cancel();

    // Free the database and close it
    //_db->close();
    // delete _db;
}

void quesync::server::server::start() {
    // Initialize managers
    _user_manager = std::make_shared<quesync::server::user_manager>(shared_from_this());
    _event_manager = std::make_shared<quesync::server::event_manager>(shared_from_this());
    _channel_manager = std::make_shared<quesync::server::channel_manager>(shared_from_this());
    _message_manager = std::make_shared<quesync::server::message_manager>(shared_from_this());
    _session_manager = std::make_shared<quesync::server::session_manager>(shared_from_this());
    _voice_manager = std::make_shared<quesync::server::voice_manager>(shared_from_this());

    std::cout << termcolor::cyan << "Listening for TCP connections.." << termcolor::reset
              << std::endl;

    // Start acception requests
    accept_client();
}

asio::io_context &quesync::server::server::get_io_context() { return _acceptor.get_io_context(); }

void quesync::server::server::accept_client() {
    // Start an async accept
    _acceptor.async_accept([this](std::error_code ec, tcp::socket socket) {
        // If no error occurred during the connection to the client start a session with it
        if (!ec) {
            // Print the client ip and port
            std::cout << termcolor::green << "Client connected from "
                      << socket.remote_endpoint().address().to_string() << ":"
                      << (int)socket.remote_endpoint().port() << termcolor::reset << std::endl;

            // Create a shared session for the client socket
            std::make_shared<session>(std::move(socket), _context, shared_from_this())->start();
        } else {
            // Print error
            std::cout << termcolor::red << "An error occurred: " << ec << termcolor::reset
                      << std::endl;
        }

        // Accept the next client
        accept_client();
    });
}

std::shared_ptr<quesync::server::user_manager> quesync::server::server::user_manager() {
    return _user_manager;
}

std::shared_ptr<quesync::server::event_manager> quesync::server::server::event_manager() {
    return _event_manager;
}

std::shared_ptr<quesync::server::channel_manager> quesync::server::server::channel_manager() {
    return _channel_manager;
}

std::shared_ptr<quesync::server::message_manager> quesync::server::server::message_manager() {
    return _message_manager;
}

std::shared_ptr<quesync::server::session_manager> quesync::server::server::session_manager() {
    return _session_manager;
}

std::shared_ptr<quesync::server::voice_manager> quesync::server::server::voice_manager() {
    return _voice_manager;
}

sql::Schema &quesync::server::server::db() { return _db; }