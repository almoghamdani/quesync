#include "voice-input.h"

#include <chrono>
#include <limits>

#include "voice-chat.h"

#include "../../../shared/utils.h"
#include "../../../shared/packets/voice_packet.h"

VoiceInput::VoiceInput(std::shared_ptr<VoiceChat> voice_chat)
	: _voice_chat(voice_chat), _enabled(false), _muted(false)
{
	int opus_error = 0;

	// Try to open the default capture device
	_capture_device = alcCaptureOpenDevice(NULL, RECORD_FREQUENCY, AL_FORMAT_MONO16, FRAME_SIZE);

	// Create the opus encoder for the recording
	_opus_encoder = opus_encoder_create(RECORD_FREQUENCY, RECORD_CHANNELS, OPUS_APPLICATION_VOIP, &opus_error);
	if (opus_error != OPUS_OK)
	{
		std::cout << "An error occurred in initializing Opus Encoder! Exiting.." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Init RNNoise
	_rnnoise_state = rnnoise_create(NULL);

	// Create the thread of the input and detach it
	_thread = std::thread(&VoiceInput::inputThread, this);
	_thread.detach();
}

VoiceInput::~VoiceInput()
{
	// Stop the capture
	alcCaptureStop(_capture_device);

	// Close the capture device
	alcCaptureCloseDevice(_capture_device);

	// Deallocate the RNNoise state
	rnnoise_destroy(_rnnoise_state);

	// Destory opus encoder
	opus_encoder_destroy(_opus_encoder);
}

void VoiceInput::enable()
{
	// Start to capture using the default device
	alcCaptureStart(_capture_device);

	_enabled = true;
}

void VoiceInput::disable()
{
	// Stop capturing using the default device
	alcCaptureStop(_capture_device);

	_enabled = false;
}

void VoiceInput::inputThread()
{
	int16_t buffer[FRAME_SIZE] = {0};
	ALint sample = 0;

	int encodedDataLen = 0;
	int16_t encoded_buffer[FRAME_SIZE] = {0};

	float rnnoise_buffer[FRAME_SIZE] = {0};

	uint8_t current_db_sample = 0;
	int16_t db_check_samples[FRAME_SIZE * (CHECK_DB_TIMEOUT / 10)];
	double db = 0;
	uint64_t last_activated = _voice_chat->getMS();

	VoicePacket voice_packet;
	std::string voice_packet_encrypted;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(750));

		// If disabled, skip
		if (!_enabled)
			continue;

		uint64_t idk = _voice_chat->getMS();

		// Get the amount of samples waiting in the device's buffer
		alcGetIntegerv(_capture_device, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &sample);

		// If there is enough samples to send the server to match the server's framerate, send it
		if (sample >= FRAME_SIZE)
		{
			// Get the capture samples
			alcCaptureSamples(_capture_device, (ALCvoid *)buffer, FRAME_SIZE);

			// If muted, continue
			if (_muted)
			{
				continue;
			}

			// Copy all PCM 16-bit short to an array of floats
			for (int i = 0; i < FRAME_SIZE; i++)
			{
				rnnoise_buffer[i] = ((opus_int16 *)buffer)[i];
			}

			// Process frame using RNNoise to reduce background noise
			rnnoise_process_frame(_rnnoise_state, rnnoise_buffer, rnnoise_buffer);

			// Copy back the result data to the buffer
			for (int i = 0; i < FRAME_SIZE; i++)
			{
				((opus_int16 *)buffer)[i] = (opus_int16)rnnoise_buffer[i];
			}

			// Copy the current sample to the buffer of db check samples
			for (int i = 0; i < FRAME_SIZE; i++)
			{
				db_check_samples[current_db_sample * FRAME_SIZE + i] = buffer[i];
			}

			// Check if reached the amount of samples needed for db check
			current_db_sample++;
			if (current_db_sample >= (CHECK_DB_TIMEOUT / 10))
			{
				current_db_sample = 0;

				// Calculate the db of the db check samples
				db = calcDB(calcRMS(db_check_samples, FRAME_SIZE * (CHECK_DB_TIMEOUT / 10)));
			}

			// If the current samples are over the minimum db or we are in the stop transition time
			if (db > MINIMUM_DB || _voice_chat->getMS() - last_activated < VOICE_DEACTIVATE_DELAY)
			{
				// Activate the user's voice
				_voice_chat->activateVoice(_voice_chat->userId());

				// Encode the captured data
				encodedDataLen = opus_encode(_opus_encoder, (const opus_int16 *)buffer, FRAME_SIZE, (unsigned char *)encoded_buffer, FRAME_SIZE * sizeof(opus_int16));

				// Create the voice packet and encrypt it
				voice_packet = VoicePacket(_voice_chat->userId(), _voice_chat->sessionId(), _voice_chat->channelId(), (char *)encoded_buffer, encodedDataLen);
				voice_packet_encrypted = Utils::EncryptVoicePacket(&voice_packet, _voice_chat->AESKey().get(), _voice_chat->HMACKey().get());

				// Send the encoded voice packet to the server
				_voice_chat->socket().send_to(asio::buffer(voice_packet_encrypted.c_str(), voice_packet_encrypted.length()), _voice_chat->endpoint());

				// If the current samples are over the minimum db, set the last played time
				if (db > MINIMUM_DB)
					last_activated = _voice_chat->getMS();
			}
		}
	}
}

void VoiceInput::mute()
{
	_muted = true;
}

void VoiceInput::unmute()
{
	_muted = false;
}

bool VoiceInput::muted()
{
	return _muted;
}

double VoiceInput::calcRMS(int16_t *data, uint32_t size)
{
	double square_sum = 0, rms;
	float relative_value;

	const float max_value = std::numeric_limits<int16_t>::max();

	for (unsigned int i = 0; i < size; i++)
	{
		relative_value = data[i] / max_value;
		square_sum += relative_value * relative_value;
	}

	rms = sqrt(square_sum / size);
	return rms;
}

double VoiceInput::calcDB(double rms)
{
	return 20 * log10(rms / AMP_REF);
}