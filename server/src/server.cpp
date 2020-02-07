#include "server.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <termcolor/termcolor.hpp>

#include "database_dump.h"
#include "session.h"

quesync::server::server::server(asio::io_context &io_context, std::string sql_server_ip,
                                std::string sql_username, std::string sql_password)
    : _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT)),
      _context(asio::ssl::context::sslv23),
      _sql_cli(server::format_uri(sql_server_ip, sql_username, sql_password)) {
    // Init SSL context
    _context.set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2);
    _context.use_certificate_chain_file("server.pem");
    _context.use_private_key_file("server.pem", asio::ssl::context::pem);

    // Import database dump
    system(format_import_string(sql_server_ip, sql_username, sql_password).c_str());
}

std::string quesync::server::server::format_uri(std::string sql_server_ip, std::string sql_username,
                                                std::string sql_password) {
    std::stringstream uri;

    // Format URI
    uri << sql_username << ":" << sql_password << "@" << sql_server_ip << "/quesync";

    return uri.str();
}

std::string quesync::server::server::format_import_string(std::string sql_server_ip,
                                                          std::string sql_username,
                                                          std::string sql_password) {
    std::string database_dump_esacped =
        to_hex_string(std::string((char *)database_dump, database_dump_size));
    std::stringstream import_str;

    // Format Mysql import string
    import_str << "echo '" << database_dump_esacped << "' | mysql -u" << sql_username << " -p"
               << sql_password << " -h" << sql_server_ip;

    // Redirect to null
#ifdef _WIN32
    import_str << " > NUL 2>&1";
#else
    import_str << " > /dev/null 2>&1";
#endif


    return import_str.str();
}

std::string quesync::server::server::to_hex_string(std::string str) {
    std::stringstream hex;

    for (char &c : str) {
        hex << "\\x" << std::hex << (int)c;
    }

    return hex.str();
}

quesync::server::server::~server() {
    // Close all socket handlers
    _acceptor.cancel();

    // Close all SQL sessions
    _sql_cli.close();
}

void quesync::server::server::start() {
    // Initialize managers
    _user_manager = std::make_shared<quesync::server::user_manager>(shared_from_this());
    _event_manager = std::make_shared<quesync::server::event_manager>(shared_from_this());
    _channel_manager = std::make_shared<quesync::server::channel_manager>(shared_from_this());
    _message_manager = std::make_shared<quesync::server::message_manager>(shared_from_this());
    _session_manager = std::make_shared<quesync::server::session_manager>(shared_from_this());
    _voice_manager = std::make_shared<quesync::server::voice_manager>(shared_from_this());
    _file_manager = std::make_shared<quesync::server::file_manager>(shared_from_this());

    std::cout << termcolor::cyan << "Listening for TCP connections.." << termcolor::reset
              << std::endl;

    // Start acception requests
    accept_client();
}

asio::io_context &quesync::server::server::get_io_context() {
    return (asio::io_context &)_acceptor.get_executor().context();
}

asio::ssl::context &quesync::server::server::get_ssl_context() { return _context; }

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

std::shared_ptr<quesync::server::file_manager> quesync::server::server::file_manager() {
    return _file_manager;
}

sql::Session quesync::server::server::get_sql_session() { return _sql_cli.getSession(); }

sql::Schema quesync::server::server::get_sql_schema(sql::Session &session) {
    return sql::Schema(session, "quesync");
}