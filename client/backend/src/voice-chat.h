#pragma once

#include "socket-manager.h"

#include <iostream>
#include <string>
#include <thread>

#include <al.h>
#include <alc.h>
#include <opus.h>

#define VOICE_CHAT_PORT 61111

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 1
#define FRAME_SIZE 480

#define RECV_BUFFER_SIZE 8192

class VoiceChat
{
public:
	VoiceChat(const char *server_ip, std::string user_id, std::string session_id, std::string channel_id);
	~VoiceChat();

private:
	std::string _user_id;
	std::string _session_id;
	std::string _channel_id;

	udp::socket _socket;
	udp::endpoint _endpoint;

	std::thread sendThread;
	std::thread recvThread;

	void sendVoiceThread();
	void recvVoiceThread();

	void cleanUnusedBuffers(ALuint source);
	bool isSourcePlaying(ALuint source);
};