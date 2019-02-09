#include "quesync.h"
#include "session.h"

Quesync::Quesync(asio::io_context& io_context) : _acceptor(io_context, tcp::endpoint(tcp::v4(), MAIN_SERVER_PORT))
{
}

void Quesync::start()
{
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
                std::make_shared<Session>(std::move(socket), this)->start();
            }

            // Accept the next client
            acceptClient();
        });
}