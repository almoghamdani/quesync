#pragma once

#include <string>
#include <thread>
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

#define DEACTIVIATION_TIMEOUT_MS 250

struct VoiceActivation
{
	bool activated;
	uint64_t last_activated;
};

class Client;

class VoiceChat : public std::enable_shared_from_this<VoiceChat>
{
public:
	VoiceChat(Client *client, const char *server_ip);
	
	void init();

	void enable(std::string user_id, std::string session_id, std::string channel_id);
	void disable();
	bool enabled();

	void setState(bool mute, bool deafen);

	bool muted();
	bool deafen();

	void activateVoice(std::string user_id);

	std::string userId();
	std::string sessionId();
	std::string channelId();

	udp::socket &socket();
	udp::endpoint &endpoint();

	uint64_t getMS();

private:
	Client *_client;

	std::shared_ptr<VoiceInput> _input;
	std::shared_ptr<VoiceOutput> _output;

	std::string _user_id;
	std::string _session_id;
	std::string _channel_id;

	udp::socket _socket;
	udp::endpoint _endpoint;

	std::mutex _activation_mutex;
	std::unordered_map<std::string, VoiceActivation> _voice_activation;
	std::unordered_map<std::string, bool> _changed_voice_activity;

	std::thread activationThread;

	bool _enabled;

	void voiceActivationThread();
};