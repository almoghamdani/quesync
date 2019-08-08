#pragma once

#include <string>
#include <thread>
#include <ctime>
#include <mutex>
#include <memory>
#include <unordered_map>

#include "socket-manager.h"
#include "voice-input.h"
#include "voice-output.h"

#define VOICE_CHAT_PORT 61111

#define RECORD_FREQUENCY 48000
#define RECORD_CHANNELS 1
#define FRAME_SIZE 480

#define DEACTIVIATION_TIMEOUT_SEC 1

struct VoiceActivation
{
	bool activated;
	std::time_t last_activated;
};

class VoiceChat : public std::enable_shared_from_this<VoiceChat>
{
public:
	VoiceChat(const char *server_ip);
	
	void init();

	void enable(std::string user_id, std::string session_id, std::string channel_id);
	void disable();

	void activateVoice(std::string user_id);

	std::string userId();
	std::string sessionId();
	std::string channelId();

	udp::socket &socket();
	udp::endpoint &endpoint();

private:
	std::shared_ptr<VoiceInput> _input;
	std::shared_ptr<VoiceOutput> _output;

	std::string _user_id;
	std::string _session_id;
	std::string _channel_id;

	udp::socket _socket;
	udp::endpoint _endpoint;

	std::mutex _activation_mutex;
	std::unordered_map<std::string, VoiceActivation> _voice_activation;

	std::thread activationThread;

	bool _enabled;

	void voiceActivationThread();
};