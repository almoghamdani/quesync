#include "include/socket-manager.hpp"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using std::cout, std::endl;

void SocketManager::initWinsock()
{
    WSADATA wsaData;

    cout << "Initializing Winsock..." << endl;
    //* Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        // Print error code
        cout << "WSAStartup failed, Error code: " << WSAGetLastError() << ". Exiting.." << endl;
        exit(EXIT_FAILURE);
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

    // Resolve the server address and port
    if ((socketError = getaddrinfo(ipAddress, port, &hints, &result)) != 0) {
        cout << "Failed to get address info, Error Code: " << socketError << ". Exiting.." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Converted Server IP and Port successfully!" << endl;

    // Create a SOCKET for connecting to server
    udpSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (udpSocket == INVALID_SOCKET) {
        cout << "Error at creating a socket, Error code: " << WSAGetLastError() << ". Exiting.." << endl;
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Created socket successfully!" << endl;
    cout << "Connecting to server..." << endl;

    // Connect to server.
    if ((socketError = connect(udpSocket, result->ai_addr, (int)result->ai_addrlen)) == SOCKET_ERROR) {
        // If failed close socket and set it as invalid socket
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
    }

    // Clear the address info for the connection details and check if there was an error
    freeaddrinfo(result);
    if (udpSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server, Error code: " << socketError << ". Exiting.." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Socket connected to server!" << endl;

    // If the user asked to create a non-blocking socket
    if (nonBlocking)
    {
        // Try to make the socket non-blocking
        if((socketError = ioctlsocket(udpSocket, FIONBIO, &nonBlockingMode)) != NO_ERROR)
        {
            cout << "Unable to make socket non-blocking, Error code: " << socketError << ". Exiting.." << endl;
            closesocket(udpSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }

    return udpSocket;
}