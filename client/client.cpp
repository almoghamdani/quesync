#define _WIN32_WINNT 0x501

#include <stdlib.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mingw.thread.h>

#include <al.h>
#include <alc.h>
#include <opus.h>
#include <bass.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 2
#define FRAMERATE 960

using std::cout;
using std::endl;

void receivePCMInfo()
{
    SOCKET recvSocket = INVALID_SOCKET;
    unsigned char buffer[5000] = {0};
    opus_int16 pcm[12000] = {0};
    int recvLen;
    OpusDecoder *opusDecoder;
    int decodedSize = 0;
    HSTREAM stream;
    int socketError;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    struct sockaddr_in sin;
    int sinAddrLen = sizeof(sin);
    std::string localPortStr = "THIS_IS_PORT_MESSAGE";

    // Zero the memory of the hints address and set the connection type as UDP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the server address and port
    if ((socketError = getaddrinfo("127.0.0.1", "55556", &hints, &result)) != 0) {
        cout << "Failed to get address info, Error Code: " << socketError << ". Exiting.." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Converted Server IP and Port successfully!" << endl;

    // The address should be the first item returned to the result
    ptr=result;

    // Create a SOCKET for connecting to server
    recvSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (recvSocket == INVALID_SOCKET) {
        cout << "Error at creating a socket, Error code: " << WSAGetLastError() << ". Exiting.." << endl;
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Created socket successfully!" << endl;
    cout << "Connecting to server..." << endl;

    // Connect to server.
    if ((socketError = connect(recvSocket, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR) {
        // If failed close socket
        closesocket(recvSocket);
        recvSocket = INVALID_SOCKET;
    }

    // Clear the address info for the connection details and check if there was an error
    freeaddrinfo(result);
    if (recvSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server, Error code: " << socketError << ". Exiting.." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Socket connected to server!" << endl;

    // Try to send the data
    if (send(recvSocket, localPortStr.c_str(), strlen(localPortStr.c_str()), 0) == SOCKET_ERROR)
    {
        cout << "Send to server failed, Error code: " << WSAGetLastError() << ". Skipping.." << endl;
    }

    // Create the opus decoder for the recording
    opusDecoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, NULL);

    // Init the BASS library with the default device
    BASS_Init(-1, RECORD_FREQUENCY, 0, 0, NULL);
    BASS_ErrorGetCode();

    // Create a stream to the speakers
    stream = BASS_StreamCreate(RECORD_FREQUENCY, 2, BASS_SAMPLE_FX, STREAMPROC_PUSH, NULL);
    BASS_ErrorGetCode();

    // Start the BASS library
    BASS_ChannelPlay(stream, 0);

    while (true)
    {
        // Get data from client
        if ((recvLen = recv(recvSocket, (char *)buffer, 5000, 0)) == SOCKET_ERROR)
        {
            std::cout << "Failed to receive data, Error code: " << WSAGetLastError() << ". Skipping.." << std::endl;
            continue;
        }

        cout << "Received data length: " << recvLen << endl;

        // If the buffer isn't empty, decode the pcm info and put it in the stream
        if (recvLen != 0)
        {
            // Decode the current sample from the client
            decodedSize = opus_decode(opusDecoder, buffer, recvLen, (opus_int16 *)pcm, 960, 0);

            cout << "Encoded data samples: " << decodedSize << endl;

            // Put the decoded pcm data in the stream
            BASS_StreamPutData(stream, pcm, decodedSize * sizeof(opus_int16) * 2);
        }
    }
}

int main()
{
    ALbyte buffer[20000] = {0};
    unsigned char encodedBuffer[FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};
    ALint sample = 0;
    ALCdevice *captureDevice;
    SOCKET ConnectSocket = INVALID_SOCKET;
    int socketError;
    WSADATA wsaData;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int opusError;
    OpusEncoder *opusEncoder;
    int dataLen = 0;
    std::thread recvThread;

    // Print Opus library version
    cout << "Opus library version: " << opus_get_version_string() << endl;

    cout << "Initializing Winsock..." << endl;
    //* Initialize Winsock
    if ((socketError = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) {
        cout << "WSAStartup failed, Error code: " << socketError << ". Exiting.." << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Winsock Initialized!" << endl;

    // Zero the memory of the hints address and set the connection type as UDP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the server address and port
    if ((socketError = getaddrinfo("127.0.0.1", "55556", &hints, &result)) != 0) {
        cout << "Failed to get address info, Error Code: " << socketError << ". Exiting.." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Converted Server IP and Port successfully!" << endl;

    // The address should be the first item returned to the result
    ptr=result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Error at creating a socket, Error code: " << WSAGetLastError() << ". Exiting.." << endl;
        freeaddrinfo(result);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Created socket successfully!" << endl;
    cout << "Connecting to server..." << endl;

    // Connect to server.
    if ((socketError = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR) {
        // If failed close socket
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Clear the address info for the connection details and check if there was an error
    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server, Error code: " << socketError << ". Exiting.." << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    cout << "Socket connected to server!" << endl;

    // Start the recv thread
    recvThread = std::thread(receivePCMInfo);
    recvThread.detach();

    // Create the opus encoder for the recording
    opusEncoder = opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opusError);

    // Try to open the default capture device
    captureDevice = alcCaptureOpenDevice(NULL, RECORD_FREQUENCY, AL_FORMAT_STEREO16, 960);
    if (alGetError() != AL_NO_ERROR)
    {
        cout << "An error occurred opening default capture device! Exiting.." << endl;
        exit(EXIT_FAILURE);
    }

    // Start to capture using the default device
    alcCaptureStart(captureDevice);

    while (true)
    {
        // Get the amount of samples waiting in the device's buffer
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);

        if (sample >= FRAMERATE)
        {
            // Get the capture samples
            alcCaptureSamples(captureDevice, (ALCvoid *)buffer, FRAMERATE);
            
            // Encode the captured data
            dataLen = opus_encode(opusEncoder, (const opus_int16 *)buffer, FRAMERATE, encodedBuffer, FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16));

            //cout << "Sent data length: " << dataLen << endl;

            // If the socket is valid, send the encoded data through it
            if (ConnectSocket != INVALID_SOCKET)
            {
                // Try to send the data
                if (send(ConnectSocket, (const char *)encodedBuffer, dataLen, 0) == SOCKET_ERROR)
                {
                    cout << "Send to server failed, Error code: " << WSAGetLastError() << ". Skipping.." << endl;
                }
            }
        }
    }

    return 0;
}