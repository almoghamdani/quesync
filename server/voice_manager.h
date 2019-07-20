#pragma once
#include "manager.h"

#include <asio.hpp>
#include <string>
#include <vector>
#include <unordered_map>

#include "../shared/event.h"

#define VOICE_SERVER_PORT 61111

#define MAX_DATA_LEN 1024

using asio::ip::udp;

class VoiceManager : Manager
{
public:
    VoiceManager(std::shared_ptr<Quesync> server);

	std::string createVoiceSession(std::string user_id);
	void deleteVoiceSession(std::string user_id);

	std::string createVoiceChannel(std::vector<std::string> users);
	void closeVoiceChannel(std::string channel_id);

private:
	udp::socket _socket;
	udp::endpoint _sender_endpoint;
	char _buf[MAX_DATA_LEN];

	std::unordered_map<std::string, std::vector<std::string>> _voice_channels;

	std::unordered_map<std::string, udp::endpoint> _session_endpoints;
	std::unordered_map<std::string, std::string> _sessions;

	void recv();
	void send(std::size_t length);
};