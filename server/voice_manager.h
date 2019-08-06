#pragma once
#include "manager.h"

#include <asio.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "../shared/event.h"
#include "../shared/voice_state.h"

#define VOICE_SERVER_PORT 61111

#define MAX_DATA_LEN 8192

#define MAX_PENDING_SECONDS 15

using asio::ip::udp;

class VoiceManager : Manager
{
public:
    VoiceManager(std::shared_ptr<Quesync> server);

	std::string createVoiceSession(std::string user_id);
	void deleteVoiceSession(std::string user_id);

	void initVoiceChannel(std::string channel_id, std::vector<std::string> users);

	void joinVoiceChannel(std::string user_id, std::string channel_id);
	void leaveVoiceChannel(std::string user_id);

	std::unordered_map<std::string, VoiceState> get_voice_states(std::string channel_id);

private:
	udp::socket _socket;
	udp::endpoint _sender_endpoint;
	char _buf[MAX_DATA_LEN];

	std::unordered_map<std::string, std::unordered_map<std::string, VoiceState>> _voice_channels;
	std::unordered_map<std::string, std::string> _joined_voice_channels;

	std::unordered_map<std::string, udp::endpoint> _session_endpoints;
	std::unordered_map<std::string, std::string> _sessions;

	std::mutex _mutex;

	std::thread _voice_states_thread;

	void recv();
	void send(std::shared_ptr<char> buf, std::size_t length, udp::endpoint endpoint);

	void handle_packet(std::size_t length);

	void handle_voice_states();

	void trigger_voice_state_event(std::string channel_id, std::string user_id, QuesyncVoiceState voice_state);
};