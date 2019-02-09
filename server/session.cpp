#include <iostream>

#include "session.h"

Session::Session(tcp::socket socket, Quesync *server) : 
    _socket(std::move(socket)), // Copy the client's socket
    _server(server) // Save the server for data transfer
{
}

void Session::start()
{
    // Start receiving from the client
    recv();
}

void Session::recv()
{
    auto self(shared_from_this());

    // Get a request from the user
    _socket.async_read_some(asio::buffer(_data, MAX_DATA_LEN),
        [this, self](std::error_code ec, std::size_t length)
        {
            // If no error occurred, parse the request
            if (!ec)
            {
                std::cout << _data << std::endl;
            } else {
                // Print error
                std::cout << "An error occurred: " << ec << std::endl;
            }
        });
}