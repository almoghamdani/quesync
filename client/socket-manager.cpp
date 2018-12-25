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