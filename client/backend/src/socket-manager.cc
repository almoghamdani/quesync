#include "socket-manager.hpp"

using std::cout;
using std::endl;

// Declare the io context of the socket mananger since it's a static member of the class
asio::io_context SocketManager::io_context;

template <typename T>
void SocketManager::GetEndpoint(const char *ip_address, int port, T& endpoint)
{
    // Create an endpoint from the ip address given and the port
    endpoint = T(asio::ip::address::from_string(ip_address), port);
}

// Declaring 2 templates for the GetEndpoint for the 2 possible classes of endpoints to fix link errors
template void SocketManager::GetEndpoint(const char *ip_address, int port, udp::endpoint& endpoint);
template void SocketManager::GetEndpoint(const char *ip_address, int port, tcp::endpoint& endpoint);