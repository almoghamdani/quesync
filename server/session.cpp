#include <iostream>

#include "session.h"
#include "utils.h"

void copyString(const std::string& input, char *dst, size_t dst_size)
{
    // Copy the string
    strncpy(dst, input.c_str(), dst_size - 1);

    // Null-terminate the string
    dst[dst_size - 1] = '\0';
}

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
            std::string response;

            // If no error occurred, parse the request
            if (!ec)
            {
                // Parse the packet
                packet = Utils::ParsePacket(_data);

                // If the packet has parsed successfully handle it
                if (packet)
                {
                    // Handle the client's request and get a respond
                    response = packet->handle(_server);
                } else {
                    response = "";
                }

                // Send the server's response to the server
                send(response);
            } else {
                // Print error
                std::cout << "An error occurred: " << ec << std::endl;
            }
        });
}

void Session::send(std::string data)
{
    auto self(shared_from_this());

    // Copy the data string to the data array
    copyString(data, _data, data.length() + 1);

    // Send the data to the client
    asio::async_write(_socket, asio::buffer(_data, data.length() + 1),
        [this, self](std::error_code ec, std::size_t)
        {
            // If no error occurred, return to the receiving function
            if (!ec)
            {
                recv();
            } else 
            {
                // Print error
                std::cout << "An error occurred: " << ec << std::endl;
            }
        });
}