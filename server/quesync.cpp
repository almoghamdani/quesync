#include "quesync.h"

#include <iostream>

#include "session.h"

Quesync::Quesync(asio::io_context& io_context) : 
    _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT)),
    _sess("localhost", 33060, "server", "123456789"),
    _db(_sess, "quesync"),
    _userManagement(_db)
{
}

Quesync::~Quesync()
{
    // Close all socket handlers
    _acceptor.cancel();

    // Free the database and close it
    //_db->close();
    //delete _db;
}

void Quesync::start()
{
    std::cout << "Listening for TCP connections.." << std::endl;

    // Start acception requests
    acceptClient();
}

void Quesync::acceptClient()
{
    // Start an async accept
    _acceptor.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
            // If no error occurred during the connection to the client start a session with it
            if (!ec)
            {
                // Print the client ip and port
                std::cout << "Client connected from " << socket.remote_endpoint().address().to_string() << ":" << (int)socket.remote_endpoint().port() << std::endl;

                // Create a shared session for the client socket
                std::make_shared<Session>(std::move(socket), this)->start();
            } else {
                // Print error
                std::cout << "An error occurred: " << ec << std::endl;
            }

            // Accept the next client
            acceptClient();
        });
}

UserManagement& Quesync::userManagement()
{
    return _userManagement;
}

sql::Schema& Quesync::db()
{
    return _db;
}