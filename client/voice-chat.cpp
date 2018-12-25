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

void VoiceChat::sendVoiceThread() const
{
    ALbyte buffer[MAX_FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};
    unsigned char encodedBuffer[FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};
    ALint sample = 0;
    ALCdevice *captureDevice;
    int opusError;
    OpusEncoder *opusEncoder;
    int dataLen = 0;

    // Create the opus encoder for the recording
    opusEncoder = opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opusError);

    // Try to open the default capture device
    captureDevice = alcCaptureOpenDevice(NULL, RECORD_FREQUENCY, AL_FORMAT_STEREO16, FRAMERATE);
    if (alGetError() != AL_NO_ERROR)
    {
        cout << "An error occurred opening default capture device! Exiting.." << endl;
        exit(EXIT_FAILURE);
    }

    // Start to capture using the default device
    alcCaptureStart(captureDevice);

    // Infinity thread
    while (true)
    {
        // Get the amount of samples waiting in the device's buffer
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);

        // If there is enough samples to send the server to match the server's framerate, send it
        if (sample >= FRAMERATE)
        {
            // Get the capture samples
            alcCaptureSamples(captureDevice, (ALCvoid *)buffer, FRAMERATE);
            
            // Encode the captured data
            dataLen = opus_encode(opusEncoder, (const opus_int16 *)buffer, FRAMERATE, encodedBuffer, FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16));

            // If the socket is valid, send the encoded data through it
            if (_voiceSocket != INVALID_SOCKET)
            {
                // Try to send the data
                if (send(_voiceSocket, (const char *)encodedBuffer, dataLen, 0) == SOCKET_ERROR)
                {
                    cout << "Send to server failed, Error code: " << WSAGetLastError() << ". Skipping.." << endl;
                }
            }
        }
    }
}