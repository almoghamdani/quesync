#pragma once

#include <string>
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <RtAudio.h>
#include <opus.h>

#define RECV_BUFFER_SIZE 8192

class VoiceChat;
class VoiceOutput
{
public:
	VoiceOutput(std::shared_ptr<VoiceChat> voice_chat);
	~VoiceOutput();

	void enable();
	void disable();

	void deaf();
	void undeaf();

	bool deafen();

	void callbackHandler(void *output_buffer);

private:
	std::shared_ptr<VoiceChat> _voice_chat;

	std::queue<std::shared_ptr<int16_t>> _output_data;
	std::mutex _data_mutex;

	std::thread _thread;

	OpusDecoder *_opus_decoder;

	bool _enabled;
	bool _deafen;

	void outputThread();
};