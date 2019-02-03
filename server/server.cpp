#include <iostream>

#include <asio.hpp>

#define SERVER_PORT 61111

using asio::ip::udp;

int main()
{
    int recvBytes;
    udp::endpoint sender_endpoint;
    asio::io_context io_context;
    char data[500];

    // Create a UDP server socket on quesync port
    udp::socket sock(io_context, udp::endpoint(udp::v4(), SERVER_PORT));

    while (true)
    {
        // Receive bytes from the client
        recvBytes = sock.receive_from(asio::buffer(data, 500), sender_endpoint);

        // Send the bytes back to the client
        sock.send_to(asio::buffer(data, recvBytes), sender_endpoint);
    }
    
    return 0;
}