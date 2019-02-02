#include "include/voice-chat.hpp"

using std::cout;
using std::endl;

VoiceChat::VoiceChat(const char *serverIP) : 
    _socket(SocketManager::io_context, udp::endpoint(udp::v4(), 0)) // Create an IPv4 UDP socket with a random port
{
    // Get the endpoint of the server using the given server IP and default voice chat port
    SocketManager::GetEndpoint(serverIP, VOICE_CHAT_PORT, _endpoint);

    // Create the thread of sending the voice from the client to the server and detach it
    sendThread = std::thread(&VoiceChat::sendVoiceThread, this);
    sendThread.detach();

    // Create the thread of receiving the voice from the server and detach it
    recvThread = std::thread(&VoiceChat::recvVoiceThread, this);
    recvThread.detach();
}

void VoiceChat::sendVoiceThread()
{
    int dataLen = 0;
    unsigned char encodedBuffer[FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};

    ALbyte buffer[MAX_FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};
    ALint sample = 0;
    ALCdevice *captureDevice;

    int opusError;
    OpusEncoder *opusEncoder;

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

    // Infinity thread while the socket isn't closed (this class deleted from memory)
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Get the amount of samples waiting in the device's buffer
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);

        // If there is enough samples to send the server to match the server's framerate, send it
        if (sample >= FRAMERATE)
        {
            // Get the capture samples
            alcCaptureSamples(captureDevice, (ALCvoid *)buffer, FRAMERATE);

            // Encode the captured data
            dataLen = opus_encode(opusEncoder, (const opus_int16 *)buffer, FRAMERATE, encodedBuffer, FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16));

            // Send the encoded buffer to the server
            _socket.send_to(asio::buffer(encodedBuffer, dataLen), _endpoint);
        }
    }
}

void VoiceChat::recvVoiceThread()
{
    int decodedSize = 0, recvBytes = 0;
    char recvBuffer[RECV_BUFFER_SIZE] = { 0 };

    OpusDecoder *opusDecoder;
    opus_int16 pcm[FRAMERATE * RECORD_CHANNELS] = {0};
    
    HSTREAM playStream;

    udp::endpoint sender_endpoint;

    // Create the opus decoder for the recording
    opusDecoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, NULL);

    // Init the BASS library with the default device
    BASS_Init(-1, RECORD_FREQUENCY, 0, 0, NULL);
    BASS_ErrorGetCode();

    // Create a stream to the speakers
    playStream = BASS_StreamCreate(RECORD_FREQUENCY, RECORD_CHANNELS, BASS_SAMPLE_FX, STREAMPROC_PUSH, NULL);
    BASS_ErrorGetCode();

    // Start the BASS library to play on the stream
    BASS_ChannelPlay(playStream, 0);

    while (true)
    {
        // Receive from the server the encoded voice sample into the recv buffer
        recvBytes = _socket.receive_from(asio::buffer(recvBuffer, RECV_BUFFER_SIZE), sender_endpoint);

        // If the sender is the server endpoint handle the voice sample
        if (sender_endpoint == _endpoint)
        {
            // Decode the current sample from the client
            decodedSize = opus_decode(opusDecoder, (const unsigned char *)recvBuffer, recvBytes, (opus_int16 *)pcm, FRAMERATE, 0);

            // Put the decoded pcm data in the stream
            BASS_StreamPutData(playStream, pcm, decodedSize * sizeof(opus_int16) * RECORD_CHANNELS);
        }
    }
}

/*static OpusDecoder *opusDecoder;
static HSTREAM playStream;

VoiceChat::VoiceChat(const char *serverIP) : _ipAddress(serverIP)
{
    // * Create the UDP socket for communication with the voice chat server, a non-blocking port
    cout << "Initalizing voice chat socket" << endl;
    SocketManager::CreateUDPSocket(&_socket);

    // Initiate the local voice stream and set the receive function on the socket
    InitVoiceStreeam();
    SocketManager::InitReadFunction(&_socket, receiveVoiceThread);

    // Create the send voice chat thread
    uv_thread_create(&_sendThread, (uv_thread_cb)&VoiceChat::sendVoiceThread, this);
}

VoiceChat::~VoiceChat()
{
    // Close and free the voice chat socket and set it invalid for the threads to be closed
    //closesocket(_voiceSocket);
    //_voiceSocket = INVALID_SOCKET;
}

void VoiceChat::InitVoiceStreeam()
{
    // Create the opus decoder for the recording
    opusDecoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, NULL);

    // Init the BASS library with the default device
    BASS_Init(-1, RECORD_FREQUENCY, 0, 0, NULL);
    BASS_ErrorGetCode();

    // Create a stream to the speakers
    playStream = BASS_StreamCreate(RECORD_FREQUENCY, RECORD_CHANNELS, BASS_SAMPLE_FX, STREAMPROC_PUSH, NULL);
    BASS_ErrorGetCode();

    // Start the BASS library to play on the stream
    BASS_ChannelPlay(playStream, 0);
}

void VoiceChat::receiveVoiceThread(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
    static opus_int16 pcm[FRAMERATE * RECORD_CHANNELS] = {0};
    static int decodedSize = 0;

    // If the buffer isn't empty, decode the pcm info and put it in the stream
    if (nread > 0)
    {
        // Decode the current sample from the client
        decodedSize = opus_decode(opusDecoder, (const unsigned char *)buf->base, nread, (opus_int16 *)pcm, FRAMERATE, 0);

        // Put the decoded pcm data in the stream
        BASS_StreamPutData(playStream, pcm, decodedSize * sizeof(opus_int16) * RECORD_CHANNELS);
    }
}

void VoiceChat::sendVoiceThread(VoiceChat *voiceChat)
{
    ALbyte buffer[MAX_FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};
    unsigned char encodedBuffer[FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16)] = {0};
    ALint sample = 0;
    ALCdevice *captureDevice;
    int opusError;
    OpusEncoder *opusEncoder;
    int dataLen = 0;
    struct sockaddr_in server_addr;
    uv_udp_send_t send_req;
    uv_buf_t socketBuffer;

    // Set the base of the socket buffer as the encoded buffer
    socketBuffer.base = (char *)encodedBuffer;

    // Set the dest
    uv_ip4_addr(voiceChat->_ipAddress.c_str(), VOICE_CHAT_PORT, (sockaddr_in *)&server_addr);

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

    // Infinity thread while the socket isn't closed (this class deleted from memory)
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Get the amount of samples waiting in the device's buffer
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);

        // If there is enough samples to send the server to match the server's framerate, send it
        if (sample >= FRAMERATE)
        {
            // Get the capture samples
            alcCaptureSamples(captureDevice, (ALCvoid *)buffer, FRAMERATE);

            // Encode the captured data
            dataLen = opus_encode(opusEncoder, (const opus_int16 *)buffer, FRAMERATE, encodedBuffer, FRAMERATE * RECORD_CHANNELS * sizeof(opus_int16));

            // Set the length of the data buffer
            socketBuffer.len = dataLen;

            // Try to send the data
            if (uv_udp_send(&send_req, &voiceChat->_socket, &socketBuffer, 1, (const sockaddr *)&server_addr, NULL))
            {
                cout << "Send to server failed. Skipping.." << endl;
            }
        }
    }
}*/