#include <iostream>

#include "session.h"
#include "utils.h"

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
            Packet *packet;

            // If no error occurred, parse the request
            if (!ec)
            {
                // Parse the packet
                packet = Utils::ParsePacket(_data);
            } else {
                // Print error
                std::cout << "An error occurred: " << ec << std::endl;
            }
        });
}