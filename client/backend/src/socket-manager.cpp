#include "socket-manager.h"

// Declare the io context of the socket mananger since it's a static member of the class
asio::io_context SocketManager::io_context;

template <typename T>
void SocketManager::GetEndpoint(const char *ip_address, int port, T &endpoint)
{
    // Create an endpoint from the ip address given and the port
    endpoint = T(asio::ip::address::from_string(ip_address), port);
}

// Declaring 2 templates for the GetEndpoint for the 2 possible classes of endpoints to fix link errors
template void SocketManager::GetEndpoint(const char *ip_address, int port, udp::endpoint &endpoint);
template void SocketManager::GetEndpoint(const char *ip_address, int port, tcp::endpoint &endpoint);

QuesyncError SocketManager::SendServerWithResponse(tcp::socket &socket, char *data, const int MAX_RESPONSE_LEN)
{
    int error = 0;
    QuesyncError quesync_error = SUCCESS;

    try
    {
        // Send the server the ping packet
        socket.write_some(asio::buffer(data, strlen(data)));

        // Get a response from the server
        socket.read_some(asio::buffer(data, MAX_RESPONSE_LEN));
    }
    catch (std::system_error &ex)
    {
        // Get error code
        error = ex.code().value();

#ifdef _WIN32
        // If the server cannot be reached
        if (error == WSAENOTCONN || error == WSAESHUTDOWN || error == WSAETIMEDOUT || error == WSAECONNREFUSED || error == WSAEHOSTDOWN || error == WSAEHOSTUNREACH || error == WSAEDESTADDRREQ)
        {
            quesync_error = CANNOT_REACH_SERVER;
        }
        // If the network connection is down
        else if (error == WSAENETDOWN || error == WSAENETUNREACH || error == WSAENETRESET || error == WSAECONNABORTED)
        {
            quesync_error = NO_CONNECTION;
        }
        // Any other error
        else
        {
            quesync_error = UNKNOWN_ERROR;
        }
#else
        // If the server cannot be reached
        if (error == EADDRNOTAVAIL || error == EDESTADDRREQ || error == ECONNREFUSED || error == EHOSTDOWN || error == EHOSTUNREACH || error == EFAULT || error == ENOTSOCK || error == EPIPE || error == ETIMEDOUT || error == ESHUTDOWN)
        {
            quesync_error = CANNOT_REACH_SERVER;
        }
        // If the network connection is down
        else if (error == ENETDOWN || error == ENETRESET || error == ENETUNREACH || error == ECONNABORTED || error == ECONNRESET || error == ENOTCONN)
        {
            quesync_error = NO_CONNECTION;
        }
        // Any other error
        else
        {
            quesync_error = UNKNOWN_ERROR;
        }
#endif
    }

    return quesync_error;
}