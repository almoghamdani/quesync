#include "voice-chat.h"

#include <iostream>
#include <chrono>

VoiceChat::VoiceChat(const char *server_ip)
	: _socket(SocketManager::io_context, udp::endpoint(udp::v4(), 0)), // Create an IPv4 UDP socket with a random port
	  _enabled(false)
{
	// Get the endpoint of the server using the given server IP and default voice chat port
	SocketManager::GetEndpoint(server_ip, VOICE_CHAT_PORT, _endpoint);

	// Create the thread of activating and deactivating voice
	activationThread = std::thread(&VoiceChat::activationThread, this);
	activationThread.detach();
}

void VoiceChat::init()
{
	_input = std::make_shared<VoiceInput>(shared_from_this());
	_output = std::make_shared<VoiceOutput>(shared_from_this());
}

void VoiceChat::enable(std::string user_id, std::string session_id, std::string channel_id)
{
	_user_id = user_id;
	_session_id = session_id;
	_channel_id = channel_id;

	_enabled = true;

	// Enable input and output
	_input->enable();
	_output->enable();
}

void VoiceChat::disable()
{
	_enabled = false;

	// Enable input and output
	_input->disable();
	_output->disable();
}

std::string VoiceChat::userId()
{
	return _user_id;
}

std::string VoiceChat::sessionId()
{
	return _session_id;
}

std::string VoiceChat::channelId()
{
	return _channel_id;
}

udp::socket &VoiceChat::socket()
{
	return _socket;
}

udp::endpoint &VoiceChat::endpoint()
{
	return _endpoint;
}

void VoiceChat::voiceActivationThread()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		if (!_enabled)
			continue;

		std::unique_lock lk(_activation_mutex);
		std::time_t current = std::time(nullptr);

		// For each user in the voice activation map
		for (auto &user : _voice_activation)
		{
			// If the user hasn't activated in the timeout
			if (user.second.activated && current - user.second.last_activated >= DEACTIVIATION_TIMEOUT_SEC)
			{
				user.second.activated = false;
			}
		}
	}
}

void VoiceChat::activateVoice(std::string user_id)
{
	std::unique_lock lk(_activation_mutex);

	// If the user already exists
	if (_voice_activation.count(user_id))
	{
		// If not activated, active
		if (!_voice_activation[user_id].activated)
		{
			_voice_activation[user_id] = VoiceActivation{true, std::time(nullptr)};
		}
	}
	else
	{
		_voice_activation[user_id] = VoiceActivation{true, std::time(nullptr)};
	}
}