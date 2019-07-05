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

QuesyncError SocketManager::SendServerWithResponse(tcp::socket &socket, const char *data, char *recv_data, const int MAX_RESPONSE_LEN)
{
    QuesyncError quesync_error = SUCCESS;

    try
    {
        // Send the server the ping packet
        socket.write_some(asio::buffer(data, strlen(data)));

        // Get a response from the server
        socket.read_some(asio::buffer(recv_data, MAX_RESPONSE_LEN));
    }
    catch (std::system_error &ex)
    {
        // Get Quesync error code by the system error
        quesync_error = SocketManager::ErrorForSystemError(ex);
    }

    return quesync_error;
}

QuesyncError SocketManager::SendServer(tcp::socket & socket, const char * data, size_t size)
{
    QuesyncError quesync_error = SUCCESS;

    try
    {
        // Send the server the data
        socket.write_some(asio::buffer(data, size));
    }
    catch (std::system_error &ex)
    {
        // Get Quesync error code by the system error
        quesync_error = SocketManager::ErrorForSystemError(ex);
    }

    return quesync_error;
}

QuesyncError SocketManager::GetResponse(tcp::socket &socket, char *data, const int MAX_RESPONSE_LEN)
{
    QuesyncError quesync_error = SUCCESS;

    try
    {
        // Get a response from the server
        socket.read_some(asio::buffer(data, MAX_RESPONSE_LEN));
    }
    catch (std::system_error &ex)
    {
        // Get Quesync error code by the system error
        quesync_error = SocketManager::ErrorForSystemError(ex);
    }

    return quesync_error;
}

QuesyncError SocketManager::ErrorForSystemError(std::system_error &ex)
{
    // Get error code
    int error = ex.code().value();

#ifdef _WIN32
    // If the server cannot be reached
    if (error == WSAENOTCONN || error == WSAESHUTDOWN || error == WSAETIMEDOUT || error == WSAECONNREFUSED || error == WSAEHOSTDOWN || error == WSAEHOSTUNREACH || error == WSAEDESTADDRREQ)
    {
        return CANNOT_REACH_SERVER;
    }
    // If the network connection is down
    else if (error == WSAENETDOWN || error == WSAENETUNREACH || error == WSAENETRESET || error == WSAECONNABORTED)
    {
        return NO_CONNECTION;
    }
    // Any other error
    else
    {
        return UNKNOWN_ERROR;
    }
#else
    // If the server cannot be reached
    if (error == EADDRNOTAVAIL || error == EDESTADDRREQ || error == ECONNREFUSED || error == EHOSTDOWN || error == EHOSTUNREACH || error == EFAULT || error == ENOTSOCK || error == EPIPE || error == ETIMEDOUT || error == ESHUTDOWN)
    {
        return CANNOT_REACH_SERVER;
    }
    // If the network connection is down
    else if (error == ENETDOWN || error == ENETRESET || error == ENETUNREACH || error == ECONNABORTED || error == ECONNRESET || error == ENOTCONN)
    {
        return NO_CONNECTION;
    }
    // Any other error
    else
    {
        return UNKNOWN_ERROR;
    }
#endif
}
