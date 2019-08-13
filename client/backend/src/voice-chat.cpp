#include "voice-chat.h"

#include <iostream>
#include <chrono>

#include "client.h"

#include "../../../shared/packets/voice_packet.h"

VoiceChat::VoiceChat(Client *client, const char *server_ip)
	: _client(client),
	  _socket(SocketManager::io_context, udp::endpoint(udp::v4(), 0)), // Create an IPv4 UDP socket with a random port
	  _enabled(false)
{
	// Get the endpoint of the server using the given server IP and default voice chat port
	SocketManager::GetEndpoint(server_ip, VOICE_CHAT_PORT, _endpoint);

	// Create the thread of activating and deactivating voice
	activationThread = std::thread(&VoiceChat::voiceActivationThread, this);
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

	// Send dummy message to the voice server so it can recongnize the client
	sendDummyMessage();

	// Enable voice chat and input and output
	_enabled = true;
	_input->enable();
	_output->enable();
}

void VoiceChat::disable()
{
	// Disable voice chat and input and output
	_enabled = false;
	_input->disable();
	_output->disable();
}

bool VoiceChat::enabled()
{
	return _enabled;
}

void VoiceChat::setState(bool mute, bool deafen)
{
	if (mute)
	{
		_input->mute();
	} else {
		_input->unmute();
	}

	if (deafen) {
		_output->deaf();
	} else {
		_output->undeaf();
	}
}

bool VoiceChat::muted()
{
	return _input->muted();
}

bool VoiceChat::deafen()
{
	return _output->deafen();
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
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!_enabled)
			continue;

		std::unique_lock lk(_activation_mutex);
		uint64_t current = getMS();

		// For each user in the voice activation map
		for (auto &user : _voice_activation)
		{
			// If the user hasn't activated in the timeout
			if (user.second.activated && current - user.second.last_activated >= DEACTIVIATION_TIMEOUT_MS)
			{
				user.second.activated = false;

				_changed_voice_activity[user.first] = false;
			}
		}

		// If user's activity has changed, call the voice activity event
		if (_changed_voice_activity.size())
		{
			try
			{
				_client->communicator().eventHandler().callEvent("voice-activity", nlohmann::json{{"changedActivity", _changed_voice_activity}});
			}
			catch (...)
			{
			}
		}

		// Clear the changed voice activity
		_changed_voice_activity.clear();
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
			_voice_activation[user_id] = VoiceActivation{true, getMS()};
			_changed_voice_activity[user_id] = true;
		}
		else
		{
			_voice_activation[user_id].last_activated = getMS();
		}
	}
	else
	{
		_voice_activation[user_id] = VoiceActivation{true, getMS()};
		_changed_voice_activity[user_id] = true;
	}
}

uint64_t VoiceChat::getMS()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void VoiceChat::sendDummyMessage()
{
	VoicePacket voice_packet(_user_id, _session_id, _channel_id, nullptr, 0);
	std::string voice_packet_encoded = voice_packet.encode();

	// Send the dummy packet to the server
	_socket.send_to(asio::buffer(voice_packet_encoded.c_str(), voice_packet_encoded.length()), _endpoint);
}