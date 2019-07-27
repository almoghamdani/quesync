#include "voice-chat.h"

#include <chrono>
#include <rnnoise.h>

#include "../../../shared/utils.h"
#include "../../../shared/packets/voice_packet.h"
#include "../../../shared/packets/participant_voice_packet.h"

bool close = false;

VoiceChat::VoiceChat(const char *server_ip, std::string user_id, std::string session_id, std::string channel_id)
	: _socket(SocketManager::io_context, udp::endpoint(udp::v4(), 0)), // Create an IPv4 UDP socket with a random port
	  _user_id(user_id),
	  _session_id(session_id),
	  _channel_id(channel_id)
{
	close = false;

	// Get the endpoint of the server using the given server IP and default voice chat port
	SocketManager::GetEndpoint(server_ip, VOICE_CHAT_PORT, _endpoint);

	// Create the thread of sending the voice from the client to the server and detach it
	sendThread = std::thread(&VoiceChat::sendVoiceThread, this);
	sendThread.detach();

	// Create the thread of receiving the voice from the server and detach it
	recvThread = std::thread(&VoiceChat::recvVoiceThread, this);
	recvThread.detach();
}

VoiceChat::~VoiceChat()
{
	close = true;
}

void VoiceChat::sendVoiceThread()
{
	int dataLen = 0;
	unsigned char encoded_buffer[FRAME_SIZE * sizeof(opus_int16)] = {0};

	ALbyte buffer[FRAME_SIZE * sizeof(opus_int16)] = {0};
	float rnnoise_buffer[FRAME_SIZE] = {0};
	ALint sample = 0;
	ALCdevice *capture_device;

	int opus_error;
	OpusEncoder *opus_encoder;

	// Allocate RNNoise state
	DenoiseState *rnnoise_state;
	rnnoise_state = rnnoise_create(NULL);

	VoicePacket voice_packet;
	std::string voice_packet_encoded;

	// Create the opus encoder for the recording
	opus_encoder = opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opus_error);

	// Try to open the default capture device
	capture_device = alcCaptureOpenDevice(NULL, RECORD_FREQUENCY, AL_FORMAT_MONO16, FRAME_SIZE);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cout << "An error occurred opening default capture device! Exiting.." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Start to capture using the default device
	alcCaptureStart(capture_device);

	// Infinity thread while the socket isn't closed (this class deleted from memory)
	while (!close)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(750));

		// Get the amount of samples waiting in the device's buffer
		alcGetIntegerv(capture_device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);

		// If there is enough samples to send the server to match the server's framerate, send it
		if (sample >= FRAME_SIZE)
		{
			// Get the capture samples
			alcCaptureSamples(capture_device, (ALCvoid *)buffer, FRAME_SIZE);

			// Copy all PCM 16-bit short to an array of floats
			for (size_t i = 0; i < FRAME_SIZE; i++)
			{
				rnnoise_buffer[i] = ((opus_int16 *)buffer)[i];
			}

			// Process frame using RNNoise to reduce background noise
			float f = rnnoise_process_frame(rnnoise_state, rnnoise_buffer, rnnoise_buffer);

			// Copy back the result data to the buffer
			for (size_t i = 0; i < FRAME_SIZE; i++)
			{
				((opus_int16 *)buffer)[i] = rnnoise_buffer[i];
			}

			// Encode the captured data
			dataLen = opus_encode(opus_encoder, (const opus_int16 *)buffer, FRAME_SIZE, encoded_buffer, FRAME_SIZE * sizeof(opus_int16));

			// Create the voice packet and encode it
			voice_packet = VoicePacket(_user_id, _session_id, _channel_id, (char *)encoded_buffer, dataLen);
			voice_packet_encoded = voice_packet.encode();

			// Send the encoded voice packet to the server
			_socket.send_to(asio::buffer(voice_packet_encoded.c_str(), voice_packet_encoded.length()), _endpoint);
		}
	}

	// Deallocate the RNNoise state
	rnnoise_destroy(rnnoise_state);

	// Stop the capture
	alcCaptureStop(capture_device);

	// Close the capture device
	alcCaptureCloseDevice(capture_device);
}

void VoiceChat::recvVoiceThread()
{
	int decoded_size = 0, recv_bytes = 0;
	char recv_buffer[RECV_BUFFER_SIZE] = {0};

	OpusDecoder *opus_decoder;
	opus_int16 pcm[FRAME_SIZE] = {0};

	ALCcontext *context;
	ALCdevice *device;
	ALuint source;

	udp::endpoint sender_endpoint;
	ParticipantVoicePacket voice_packet;

	// Create the opus decoder for the voice
	opus_decoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, NULL);

	// Open default device
	device = alcOpenDevice(NULL);

	// Create the context for the audio scene
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	// Init the source
	alGenSources((ALuint)1, &source);
	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);

	while (!close)
	{
		ALuint buffer;

		// Clean unused buffers of the source
		cleanUnusedBuffers(source);

		// Generate buffer for the new data
		alGenBuffers((ALuint)1, &buffer);

		// Receive from the server the encoded voice sample into the recv buffer
		recv_bytes = _socket.receive_from(asio::buffer(recv_buffer, RECV_BUFFER_SIZE), sender_endpoint);

		// If the sender is the server endpoint handle the voice sample
		if (sender_endpoint == _endpoint)
		{
			// Decode the voice packet
			voice_packet.decode(std::string(recv_buffer, recv_bytes));

			// Decode the current sample from the client
			decoded_size = opus_decode(opus_decoder, (const unsigned char *)voice_packet.voice_data(), voice_packet.voice_data_len(), (opus_int16 *)pcm, FRAME_SIZE, 0);

			// Fill buffer
			alBufferData(buffer, AL_FORMAT_MONO16, pcm, decoded_size * sizeof(opus_int16), RECORD_FREQUENCY);

			// Queue the buffer to the source
			alSourceQueueBuffers(source, 1, &buffer);
		}

		// Clean unused buffers of the source
		cleanUnusedBuffers(source);

		// If the source isn't playing, play it
		if (!isSourcePlaying(source))
		{
			alSourcePlay(source);
		}
	}

	// Clean unused buffers of the source
	cleanUnusedBuffers(source);

	// Delete the source
	alDeleteSources(1, &source);

	// Close the device
	alcCloseDevice(device);
}

void VoiceChat::cleanUnusedBuffers(ALuint source)
{
	ALint amount_of_buffers;
	ALuint *old_buffers;

	// Get the amount of buffers that already processed and allocate memory for them
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &amount_of_buffers);
	old_buffers = new ALuint[amount_of_buffers];

	// Unqueue the buffers
	alSourceUnqueueBuffers(source, amount_of_buffers, old_buffers);

	// Delete the buffers
	alDeleteBuffers(amount_of_buffers, old_buffers);
	delete old_buffers;
}

bool VoiceChat::isSourcePlaying(ALuint source)
{
	ALint source_state;

	// Get the source state
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);

	return source_state == AL_PLAYING;
}