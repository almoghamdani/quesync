#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include "include/opus.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 2

int main()
{
    int socketError;
    WSADATA wsaData;
    struct sockaddr_in server, clientAddr;
    SOCKET serverSocket = INVALID_SOCKET;
    unsigned char buffer[2048] = {0};
    float pcm[5760] = {0};
    unsigned int recvLen = 0;
    int clientAddrLen = sizeof(clientAddr);
    OpusDecoder *opusDecoder;
    int opusError = 0;
    
    // Print Opus library version
    std::cout << "Opus library version: " << opus_get_version_string() << std::endl;

    std::cout << "Initializing Winsock..." << std::endl;
    //* Initialize Winsock
    if ((socketError = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
        std::cout << "WSAStartup failed, Error code: " << socketError << ". Exiting.." << std::endl;
        return 1;
    }
    std::cout << "Winsock Initialized!" << std::endl;

    // Try and create the socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        std::cout << "Error at creating a socket, Error code: " << WSAGetLastError() << ". Exiting.." << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    std::cout << "Created socket successfully!" << std::endl;

    // Set the server as IPv4 server which listens to all requests on port 55556
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(55556);

    // Try and bind the IP and Port to the server
    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed, Error code: " << WSAGetLastError() << ". Exiting.." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    std::cout << "IP and Port bind completed!" << std::endl;

    // Create the opus decoder for the recording
    opusDecoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, &opusError);

    while (1)
    {
        // Clean buffer
        memset(buffer, 0, 2048);

        // Get data from client
        if ((recvLen = recvfrom(serverSocket, (char *)buffer, 2048, 0, (struct sockaddr *)&clientAddr, &clientAddrLen)) == SOCKET_ERROR)
        {
            std::cout << "Failed to receive data, Error code: " << WSAGetLastError() << ". Skipping.." << std::endl;
            continue;
        }

        // Decode the current sample from the client
        opusError = opus_decode_float(opusDecoder, buffer, recvLen, pcm, 5760, 0);
        std::cout << opusError << std::endl;
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}