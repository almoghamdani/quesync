#include "include/socket-manager.hpp"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using std::cout;
using std::endl;

static uv_loop_t *eventLoop;

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

/*void SocketManager::InitReadFunction(uv_udp_t socket, uv_udp_recv_cb readFunction)
{
    int socketError = 0;

    // Start receiving on the socket and setting the callback function as the read function
    if((socketError = uv_udp_recv_start(&socket, alloc_buffer, readFunction)))
    {
        throw SocketError("Unable to open the stream for receiving!", socketError);
    }
    cout << "Receiving on socket enabled!" << endl;
}*/

void SocketManager::InitSocketManager()
{
    int socketError = 0;

    // Setting the event loop as the default event loop and starting it
    eventLoop = uv_default_loop();
    cout << "Event loop Initialized!" << endl;

    // Try to start the event loop
    if((socketError = uv_run(eventLoop, UV_RUN_DEFAULT)))
    {
        throw SocketError("Unable to start event loop!", socketError);
    }
    cout << "Event loop Started!" << endl;
}

void SocketManager::createUDPSocket(uv_udp_t *sendSocket, uv_udp_t *recvSocket)
{
    int socketError = 0;
    struct sockaddr_in addr;
    struct sockaddr_in idk;
    int addrLen = sizeof(idk);
    uv_udp_send_t send_req;
    char *tryingData = "TRYING";
    uv_buf_t buf;

    buf.base = tryingData;
    buf.len = 7;

    // Initialize the send socket
    cout << "Initializing UDP socket.." << endl;
    if((socketError = uv_udp_init(eventLoop, sendSocket))) {
        throw SocketError("Unable to initiate UDP Send socket!", socketError);
    }
    cout << "UDP Socket successfully initialized!" << endl;

    // Bind the socket to a random port (port 0 generates a random port)
    uv_ip4_addr("0.0.0.0", 0, (sockaddr_in *)&addr);
    uv_udp_bind(sendSocket, (const struct sockaddr *)&addr, 0);

    // If a receive socket was requested, initialize
    if (recvSocket)
    {
        // Initialize the receive socket
        if((socketError = uv_udp_init(eventLoop, recvSocket))) {
            throw SocketError("Unable to initiate UDP Receive socket!", socketError);
        }

        cout << uv_udp_getsockname(sendSocket, (sockaddr *)&idk, &addrLen) << endl;
        cout << idk.sin_port << endl;

        // Bind the socket to listen on all incoming traffic with the send port
        cout << uv_udp_bind(recvSocket, (const struct sockaddr *)&idk, UV_UDP_REUSEADDR) << endl;
    }
}

SOCKET SocketManager::createSocket(const char *ipAddress, const char *port, bool isTCP, bool nonBlocking)
{
    SOCKET udpSocket = INVALID_SOCKET;
    int socketError;
    struct addrinfo *result = NULL,
                    hints;
    u_long nonBlockingMode = 1;

    // Zero the memory of the hints address and set the connection type as UDP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = isTCP ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_protocol = isTCP ? IPPROTO_TCP : IPPROTO_UDP;

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