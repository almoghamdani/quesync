#include "voice-output.h"

#include <limits>
#include <chrono>
#include <sole.hpp>

#include "voice-chat.h"

#include "../../../shared/utils.h"
#include "../../../shared/packets/participant_voice_packet.h"

VoiceOutput::VoiceOutput(std::shared_ptr<VoiceChat> voice_chat)
	: _voice_chat(voice_chat), _enabled(false), _deafen(false)
{
	int opus_error = 0;

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
	// Destory opus decoder
	opus_decoder_destroy(_opus_decoder);
}

void VoiceOutput::callbackHandler(void *output_buffer)
{
	std::unique_lock lk(_data_mutex);

	// If empty, clean output buffer
	if (_output_data.empty())
	{
		memset(output_buffer, 0, FRAME_SIZE * PLAYBACK_CHANNELS * sizeof(int16_t));
		return;
	}

	// Pop the last output data
	std::shared_ptr<int16_t> buf = _output_data.front();
	_output_data.pop();

	// Unlock the mutex
	lk.unlock();

	// Copy the buffer to the output buffer + Convert from MONO to STEREO
	for (int i = 0; i < FRAME_SIZE; i++)
	{
		*((int16_t *)output_buffer + i * PLAYBACK_CHANNELS) = *((int16_t *)output_buffer + i * PLAYBACK_CHANNELS + 1) = buf.get()[i];
	}
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
	std::shared_ptr<ParticipantVoicePacket> voice_packet;

	while (true)
	{
		std::shared_ptr<int16_t> buffer(new int16_t[FRAME_SIZE]);

		// If disabled, skip
		if (!_enabled)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(750));
			continue;
		}

		// Receive from the server the encoded voice sample into the recv buffer
		recv_bytes = (int)_voice_chat->socket().receive_from(asio::buffer(recv_buffer, RECV_BUFFER_SIZE), sender_endpoint);

		// If the sender is the server endpoint handle the voice sample
		if (sender_endpoint == _voice_chat->endpoint())
		{
			// Decode the voice packet
			voice_packet = Utils::DecryptVoicePacket<ParticipantVoicePacket>(std::string(recv_buffer, recv_bytes), _voice_chat->AESKey().get(), _voice_chat->HMACKey().get());
			if (!voice_packet)
			{
				continue;
			}

			// Activate the user's voice
			_voice_chat->activateVoice(voice_packet->user_id());

			// If deafen, continue
			if (_deafen)
			{
				continue;
			}

			// Decode the current sample from the client
			decoded_size = opus_decode(_opus_decoder, (const unsigned char *)voice_packet->voice_data(), voice_packet->voice_data_len(), (opus_int16 *)pcm, FRAME_SIZE, 0);

			// Copy the PCM to the buffer
			memcpy(buffer.get(), pcm, FRAME_SIZE * sizeof(int16_t));

			// Push the output buffer to the output data queue
			std::unique_lock lk(_data_mutex);
			_output_data.push(buffer);
		}
	}
}

void VoiceOutput::deaf()
{
	_deafen = true;
}

void VoiceOutput::undeaf()
{
	_deafen = false;
}

bool VoiceOutput::deafen()
{
	return _deafen;
}