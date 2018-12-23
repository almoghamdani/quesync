#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include <opus.h>
#include <bass.h>

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
    unsigned char buffer[5000] = {0};
    opus_int16 pcm[12000] = {0};
    int recvLen;
    int clientAddrLen = sizeof(clientAddr);
    OpusDecoder *opusDecoder;
    int opusError = 0;
    HSTREAM stream;
    int cnt = 0;
    
    // Print BASS version
    std::cout << "BASS library version: " << BASS_GetVersion() << std::endl;

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

    // Init the BASS library with the default device
    BASS_Init(-1, RECORD_FREQUENCY, 0, 0, NULL);
    std::cout << BASS_ErrorGetCode() << std::endl;

    // Create a stream to the speakers
    stream = BASS_StreamCreate(RECORD_FREQUENCY, 2, BASS_SAMPLE_FX, STREAMPROC_PUSH, NULL);
    std::cout << BASS_ErrorGetCode() << std::endl;

    // Start the BASS library
    std::cout << BASS_ChannelPlay(stream, 0) << std::endl;

    while (true)
    {
        // Get data from client
        if ((recvLen = recvfrom(serverSocket, (char *)buffer, 5000, 0, (struct sockaddr *)&clientAddr, &clientAddrLen)) == SOCKET_ERROR)
        {
            std::cout << "Failed to receive data, Error code: " << WSAGetLastError() << ". Skipping.." << std::endl;
            continue;
        }

        std::cout << recvLen << std::endl;
        // Decode the current sample from the client
        opusError = opus_decode(opusDecoder, buffer, recvLen, (opus_int16 *)pcm, 960, 0);
        std::cout << opusError << std::endl;

        if (recvLen != 0)
        {
            std::cout << BASS_StreamPutData(stream, pcm, opusError * sizeof(opus_int16) * 2) << std::endl;
        }

        //std::cout << BASS_ChannelUpdate(stream, 0) << std::endl;
        //std::cout << BASS_ErrorGetCode() << std::endl;

        /*cnt++;
        if (cnt == 10)
        { 
            std::cout << BASS_Update(200) << std::endl;
            std::cout << BASS_ErrorGetCode() << std::endl;
            cnt = 0;
        }*/
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}