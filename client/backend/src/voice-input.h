#pragma once

#include <string>
#include <thread>
#include <memory>

#include <al.h>
#include <alc.h>
#include <opus.h>
#include <rnnoise.h>

#define AMP_REF 0.00001
#define CHECK_DB_TIMEOUT 30
#define MINIMUM_DB 40
#define VOICE_DEACTIVATE_DELAY 100

class VoiceChat;
class VoiceInput
{
public:
	VoiceInput(std::shared_ptr<VoiceChat> voice_chat);
	~VoiceInput();

	void enable();
	void disable();

private:
	std::shared_ptr<VoiceChat> _voice_chat;

	std::thread _thread;

	ALCdevice *_capture_device;
	OpusEncoder *_opus_encoder;
	DenoiseState *_rnnoise_state;

	bool _enabled;

	void inputThread();

	double calcRMS(int16_t *data, uint32_t size);
	double calcDB(double rms);
};