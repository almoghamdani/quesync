#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#include "include/bass.h"
#include "include/opus.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 2
#define BASS_RECORD_UPDATE_TIME 10

SOCKET ConnectSocket = INVALID_SOCKET;

BOOL CALLBACK recordHandleProc(HRECORD handle, const void *buffer, DWORD length, void *user)
{
    // Get the encoder sent as an argument
    OpusEncoder *opusEncoder = (OpusEncoder *)user;
    unsigned char *encodedData = (unsigned char *)malloc(length);
    unsigned int dataLen = 0;

    // Encode the captured data
    dataLen = opus_encode_float(opusEncoder, (const float *)buffer, 2880, encodedData, length);

    // If the socket is valid, send the encoded data through it
    if (ConnectSocket != INVALID_SOCKET)
    {
        // Try to send the data
        if (send(ConnectSocket, (const char *)encodedData, dataLen, 0) == SOCKET_ERROR)
        {
            std::cout << "Send to server failed, Error code: " << WSAGetLastError() << ". Skipping.." << std::endl;
        }
    }

    // Free the encoded data buffer
    free(encodedData);

    return TRUE;
}

int main()
{
    int socketError;
    WSADATA wsaData;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int opusError;
    OpusEncoder *opusEncoder;
    HRECORD recordHandle;

    std::cout << "Initializing Winsock..." << std::endl;
    //* Initialize Winsock
    if ((socketError = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
        std::cout << "WSAStartup failed, Error code: " << socketError << ". Exiting.." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Winsock Initialized!" << std::endl;

    // Zero the memory of the hints address and set the connection type as UDP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the server address and port
    if ((socketError = getaddrinfo("127.0.0.1", "55556", &hints, &result)) != 0) {
        std::cout << "Failed to get address info, Error Code: " << socketError << ". Exiting.." << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    std::cout << "Converted Server IP and Port successfully!" << std::endl;

    // The address should be the first item returned to the result
    ptr=result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Error at creating a socket, Error code: " << WSAGetLastError() << ". Exiting.." << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    std::cout << "Created socket successfully!" << std::endl;
    std::cout << "Connecting to server..." << std::endl;

    // Connect to server.
    if ((socketError = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR) {
        // If failed close socket
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Clear the address info for the connection details and check if there was an error
    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server, Error code: " << socketError << ". Exiting.." << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket connected to server!" << std::endl;

    //* Check if the correct version of the BASS library has been loaded
    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        std::cout << "Incorrect version of BASS library has been loaded! Exiting.." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Print BASS version
    std::cout << "BASS library version: " << BASS_GetVersion() << std::endl;

    // Print Opus library version
    std::cout << "Opus library version: " << opus_get_version_string() << std::endl;

    // TODO: Add record device selection
    // Try to init the default record device
    if (!BASS_RecordInit(0))
    {
        // Print the BASS error
        switch(BASS_ErrorGetCode())
        {
        case BASS_ERROR_DX:
            std::cout << "Missing DirectX! Exiting.." << std::endl;
            break;

        case BASS_ERROR_DEVICE:
            std::cout << "Invalid record device selected! Exiting.." << std::endl;
            break;

        case BASS_ERROR_DRIVER:
            std::cout << "Missing device driver! Exiting.." << std::endl;
            break;
        }

        return 1;
    }

    // Create the opus encoder for the recording
    opusEncoder = opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opusError);

    // Try and start recording
    recordHandle = BASS_RecordStart(RECORD_FREQUENCY, RECORD_CHANNELS, MAKELONG(BASS_SAMPLE_FLOAT, BASS_RECORD_UPDATE_TIME), recordHandleProc, opusEncoder);
    if (!recordHandle)
    {
        std::cout << "IDK" << std::endl;

        exit(EXIT_FAILURE);
    }

    // Infinity loop for this main thread
    while (true)
    {

    }

    return 0;
}