#pragma once

#include <string>
#include <thread>
#include <memory>

#include <al.h>
#include <alc.h>
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

private:
	std::shared_ptr<VoiceChat> _voice_chat;

	std::thread _thread;

	ALCcontext *_context;
	ALCdevice *_device;
	ALuint _source;

	OpusDecoder *_opus_decoder;

	bool _enabled;
	bool _deafen;

	void outputThread();

	void cleanUnusedBuffers(ALuint source);
	bool isSourcePlaying(ALuint source);
};