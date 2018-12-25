#include "include/socket-manager.hpp"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using std::cout;
using std::endl;

void SocketManager::initWinsock()
{
    WSADATA wsaData;

    cout << "Initializing Winsock..." << endl;
    //* Initialize Winsock, if falied throw startup failure exception
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw SocketError("WSAStartup failed", WSAGetLastError());
    }
    cout << "Winsock Initialized!" << endl;
}

SOCKET SocketManager::createUDPSocket(const char *ipAddress, const char *port, bool nonBlocking)
{
    SOCKET udpSocket = INVALID_SOCKET;
    int socketError;
    struct addrinfo *result = NULL,
                    hints;
    u_long nonBlockingMode = 1;

    // Zero the memory of the hints address and set the connection type as UDP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the server address and port, if failed throw exception
    if ((socketError = getaddrinfo(ipAddress, port, &hints, &result)) != 0) {
        throw SocketError("Failed to get address info", WSAGetLastError());
    }
    cout << "Converted Server IP and Port successfully!" << endl;

    // Create a SOCKET for connecting to server
    udpSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (udpSocket == INVALID_SOCKET) {
        freeaddrinfo(result); // Free the address info
        throw SocketError("Error at creating a socket", WSAGetLastError());
    }
    cout << "Created socket successfully!" << endl;
    cout << "Connecting to server..." << endl;

    // Connect to server.
    if ((socketError = connect(udpSocket, result->ai_addr, (int)result->ai_addrlen)) == SOCKET_ERROR) {
        // If failed close socket and set it as invalid socket
        closesocket(udpSocket);
        freeaddrinfo(result);
        throw SocketError("Unable to connect to server", socketError);
    }
    cout << "Socket connected to server!" << endl;

    // Free the server's address info since it's not needed anymore
    freeaddrinfo(result);

    // If the user asked to create a non-blocking socket
    if (nonBlocking)
    {
        // Try to make the socket non-blocking
        if((socketError = ioctlsocket(udpSocket, FIONBIO, &nonBlockingMode)) != NO_ERROR)
        {
            closesocket(udpSocket);
            throw SocketError("Unable to make socket non-blocking", socketError);
        }
    }

    return udpSocket;
}