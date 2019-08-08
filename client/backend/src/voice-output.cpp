#include "voice-output.h"

#include <chrono>

#include "voice-chat.h"

#include "../../../shared/utils.h"
#include "../../../shared/packets/participant_voice_packet.h"

VoiceOutput::VoiceOutput(std::shared_ptr<VoiceChat> voice_chat)
	: _voice_chat(voice_chat), _enabled(false)
{
	int opus_error = 0;

	// Open default device
	_device = alcOpenDevice(NULL);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cout << "An error occurred in opening default device! Exiting.." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create the context for the audio scene
	_context = alcCreateContext(_device, NULL);
	alcMakeContextCurrent(_context);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cout << "An error occurred in creating device context! Exiting.." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Init the source
	alGenSources((ALuint)1, &_source);
	alSourcef(_source, AL_PITCH, 1);
	alSourcef(_source, AL_GAIN, 1);
	alSource3f(_source, AL_POSITION, 0, 0, 0);
	alSource3f(_source, AL_VELOCITY, 0, 0, 0);
	alSourcei(_source, AL_LOOPING, AL_FALSE);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cout << "An error occurred in creating sound source! Exiting.." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create the opus decoder for the voice
	_opus_decoder = opus_decoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, &opus_error);
	if (opus_error != OPUS_OK)
	{
		std::cout << "An error occurred in initializing Opus Decoder! Exiting.." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create the thread of the output and detach it
	_thread = std::thread(&VoiceOutput::outputThread, this);
	_thread.detach();
}

VoiceOutput::~VoiceOutput()
{
	// Clean unused buffers of the source
	cleanUnusedBuffers(_source);

	// Delete the source
	alDeleteSources(1, &_source);

	// Destory device's context
	alcDestroyContext(_context);

	// Close the device
	alcCloseDevice(_device);

	// Destory opus decoder
	opus_decoder_destroy(_opus_decoder);
}

void VoiceOutput::enable()
{
	_enabled = true;
}

void VoiceOutput::disable()
{
	_enabled = false;
}

void VoiceOutput::outputThread()
{
	int decoded_size = 0, recv_bytes = 0;
	char recv_buffer[RECV_BUFFER_SIZE] = {0};

	int16_t pcm[FRAME_SIZE] = {0};

	udp::endpoint sender_endpoint;
	ParticipantVoicePacket voice_packet;

	while (true)
	{
		ALuint buffer;

		// If disabled, skip
		if (!_enabled)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(750));
			continue;
		}

		// Clean unused buffers of the source
		cleanUnusedBuffers(_source);

		// Generate buffer for the new data
		alGenBuffers((ALuint)1, &buffer);

		// Receive from the server the encoded voice sample into the recv buffer
		recv_bytes = (int)_voice_chat->socket().receive_from(asio::buffer(recv_buffer, RECV_BUFFER_SIZE), sender_endpoint);

		// If the sender is the server endpoint handle the voice sample
		if (sender_endpoint == _voice_chat->endpoint())
		{
			// Decode the voice packet
			voice_packet.decode(std::string(recv_buffer, recv_bytes));

			// Activate the user's voice
			_voice_chat->activateVoice(voice_packet.user_id());

			// Decode the current sample from the client
			decoded_size = opus_decode(_opus_decoder, (const unsigned char *)voice_packet.voice_data(), voice_packet.voice_data_len(), (opus_int16 *)pcm, FRAME_SIZE, 0);

			// Fill buffer
			alBufferData(buffer, AL_FORMAT_MONO16, (opus_int16 *)pcm, decoded_size * sizeof(opus_int16), RECORD_FREQUENCY);

			// Queue the buffer to the source
			alSourceQueueBuffers(_source, 1, &buffer);
		}

		// Clean unused buffers of the source
		cleanUnusedBuffers(_source);

		// If the source isn't playing, play it
		if (!isSourcePlaying(_source))
		{
			alSourcePlay(_source);
		}
	}
}

void VoiceOutput::cleanUnusedBuffers(ALuint source)
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

bool VoiceOutput::isSourcePlaying(ALuint source)
{
	ALint source_state;

	// Get the source state
	alGetSourcei(source, AL_SOURCE_STATE, &source_state);

	return source_state == AL_PLAYING;
}