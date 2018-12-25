#include "include/voice-chat.hpp"

using std::cout, std::endl;

VoiceChat::VoiceChat(const char *serverIP)
{
    std::thread recvThread, sendThread;
    const char *portStr = std::to_string(VOICE_CHAT_PORT).c_str();

    // * Create the UDP socket for communication with the voice chat server, a non-blocking port
    cout << "Initalizing voice chat socket" << endl;
    _voiceSocket = SocketManager::createUDPSocket(serverIP, portStr, true);
    
    // Create the receive voice chat thread and detach it
    recvThread = std::thread(&VoiceChat::receiveVoiceThread);
    recvThread.detach();

    // Create the send voice chat thread and detach it
    sendThread = std::thread(&VoiceChat::sendVoiceThread);
    sendThread.detach();
}

void VoiceChat::receiveVoiceThread() const
{
    unsigned char buffer[RECV_BUFFER_SIZE] = {0};
    opus_int16 pcm[FRAMERATE * RECORD_CHANNELS] = {0};
    int recvLen;
    OpusDecoder *opusDecoder;
    int decodedSize = 0;
    HSTREAM stream;
    int socketError;

    // Create the opus decoder for the recording
    opusDecoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, NULL);

    // Init the BASS library with the default device
    BASS_Init(-1, RECORD_FREQUENCY, 0, 0, NULL);
    BASS_ErrorGetCode();

    // Create a stream to the speakers
    stream = BASS_StreamCreate(RECORD_FREQUENCY, RECORD_CHANNELS, BASS_SAMPLE_FX, STREAMPROC_PUSH, NULL);
    BASS_ErrorGetCode();

    // Start the BASS library
    BASS_ChannelPlay(stream, 0);

    // Infinity thread
    while (true)
    {
        // Get data from client, check if the buffer isn't empty by getting the winsock error
        if ((recvLen = recv(_voiceSocket, (char *)buffer, RECV_BUFFER_SIZE, 0)) == SOCKET_ERROR && ((socketError = WSAGetLastError()) != WSAEWOULDBLOCK))
        {
            std::cout << "Failed to receive data, Error code: " << WSAGetLastError() << ". Skipping.." << std::endl;
            continue;
        }

        // If the buffer isn't empty, decode the pcm info and put it in the stream
        if (recvLen != 0 && socketError != WSAEWOULDBLOCK)
        {
            // Decode the current sample from the client
            decodedSize = opus_decode(opusDecoder, buffer, recvLen, (opus_int16 *)pcm, FRAMERATE, 0);

            // Put the decoded pcm data in the stream
            BASS_StreamPutData(stream, pcm, decodedSize * sizeof(opus_int16) * RECORD_CHANNELS);
        }
    }
}