#pragma once

#include <string>
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
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

	void mute();
	void unmute();

	bool muted();

	void callbackHandler(void *input_buffer);

private:
	std::shared_ptr<VoiceChat> _voice_chat;

	std::queue<std::shared_ptr<int16_t>> _input_data;
	std::mutex _data_mutex;
	std::condition_variable _data_cv;

	std::thread _thread;

	OpusEncoder *_opus_encoder;
	DenoiseState *_rnnoise_state;

	bool _enabled;
	bool _muted;

	void inputThread();

	double calcRMS(int16_t *data, uint32_t size);
	double calcDB(double rms);
};