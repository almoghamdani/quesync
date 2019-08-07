#include "voice_manager.h"

#include <algorithm>
#include <sole.hpp>

#include "quesync.h"
#include "../shared/packets/voice_packet.h"
#include "../shared/packets/participant_voice_packet.h"
#include "../shared/events/voice_state_event.h"
#include "../shared/quesync_exception.h"

VoiceManager::VoiceManager(std::shared_ptr<Quesync> server) : Manager(server),
															  _socket(server->get_io_context(), udp::endpoint(udp::v4(), VOICE_SERVER_PORT)),
															  _voice_states_thread(&VoiceManager::handle_voice_states, this)
{
	// Detach voice states thread
	_voice_states_thread.detach();

	// Start receiving
	recv();
}

void VoiceManager::recv()
{
	_socket.async_receive_from(
		asio::buffer(_buf, MAX_DATA_LEN), _sender_endpoint,
		[this](std::error_code ec, std::size_t bytes) {
			if (!ec && bytes > 0)
			{
				handle_packet(bytes);
			}

			recv();
		});
}

void VoiceManager::send(std::shared_ptr<char> buf, size_t length, udp::endpoint endpoint)
{
	_socket.async_send_to(
		asio::buffer(buf.get(), length), endpoint,
		[this, buf](std::error_code, std::size_t) {});
}

void VoiceManager::handle_packet(std::size_t length)
{
	VoicePacket packet;

	std::string participant_session;
	udp::endpoint participant_endpoint;

	ParticipantVoicePacket participant_packet;
	std::string participant_packet_encoded;
	std::shared_ptr<char> buf;

	std::unordered_map<std::string, VoiceState> channel_participants;

	std::unique_lock lk(_mutex);

	// Try to decode the packet
	if (!packet.decode(std::string(_buf, length)))
	{
		return;
	}

	// Try to find the user id and the channel id
	try
	{
		channel_participants = _voice_channels.at(packet.channel_id());
	}
	catch (...)
	{
		return;
	}

	// If the user's session isn't in the endpoints list, insert it
	if (!_session_endpoints.count(packet.session_id()))
	{
		_session_endpoints[packet.session_id()] = _sender_endpoint;
	}

	// If the user isn't a part of the channel
	if (!channel_participants.count(packet.user_id()))
	{
		return;
	}

	if (packet.voice_data_len())
	{
		// Create the pariticipant voice packet and encode it
		participant_packet = ParticipantVoicePacket(packet.user_id(), packet.voice_data(), packet.voice_data_len());
		participant_packet_encoded = participant_packet.encode();
		buf = Utils::ConvertToBuffer(participant_packet_encoded);

		// Send the participant voice packet to all other participants
		for (auto &participant : channel_participants)
		{
			// If the given participant isn't our user
			if (participant.first != packet.user_id())
			{
				try
				{
					// Try to get the session and endpoint of the participant
					participant_session = _sessions.at(participant.first);
					participant_endpoint = _session_endpoints.at(participant_session);
				}
				catch (...)
				{
					continue;
				}

				// Send the participant voice packet to the participant
				send(buf, participant_packet_encoded.length(), participant_endpoint);
			}
		}
	}
}

std::string VoiceManager::createVoiceSession(std::string user_id)
{
	std::string session_id = sole::uuid4().str();
	std::unique_lock lk(_mutex);

	_sessions[user_id] = session_id;

	return session_id;
}

void VoiceManager::deleteVoiceSession(std::string user_id)
{
	std::unique_lock lk(_mutex);

	try
	{
		_sessions.erase(user_id);
	}
	catch (...)
	{
		throw QuesyncException(VOICE_NOT_CONNECTED);
	}
}

void VoiceManager::initVoiceChannel(std::string channel_id, std::vector<std::string> users)
{
	std::unordered_map<std::string, VoiceState> user_states;
	std::unique_lock lk(_mutex);

	// If the voice channel already started
	if (_voice_channels.count(channel_id))
	{
		throw QuesyncException(CALL_ALREADY_STARTED);
	}

	// Create user states for all the users as PENDING
	for (auto &user : users)
	{
		user_states[user] = PENDING;
	}

	_voice_channels[channel_id] = user_states;
}

void VoiceManager::joinVoiceChannel(std::string user_id, std::string channel_id)
{
	std::unique_lock lk(_mutex);

	// If the user is already in a channel, leave the channel
	if (_joined_voice_channels.count(user_id))
	{
		leaveVoiceChannel(user_id);
	}

	// Check if the channel exists
	if (!_voice_channels.count(channel_id))
	{
		throw QuesyncException(CHANNEL_NOT_FOUND);
	}

	// Connect the user to the channel
	_joined_voice_channels[user_id] = channel_id;
	_voice_channels[channel_id][user_id] = CONNECTED;
	trigger_voice_state_event(channel_id, user_id, CONNECTED);
}

void VoiceManager::leaveVoiceChannel(std::string user_id)
{
	std::string channel_id;
	std::unique_lock lk(_mutex);

	// Check if in voice channel
	if (!_joined_voice_channels.count(user_id))
	{
		throw QuesyncException(VOICE_NOT_CONNECTED);
	}

	// Get the channel id of the user
	channel_id = _joined_voice_channels[user_id];

	// Remove the user from the map of joined voice channels
	_joined_voice_channels.erase(user_id);
	_voice_channels[channel_id][user_id] = DISCONNECTED;
	trigger_voice_state_event(channel_id, user_id, DISCONNECTED);

	// Check for others connected to the voice channel
	for (auto &join_pair : _joined_voice_channels)
	{
		if (join_pair.second == channel_id)
		{
			return;
		}
	}

	// If the channel has no one connected to it, remove it
	_voice_channels.erase(channel_id);
}

std::unordered_map<std::string, VoiceState> VoiceManager::get_voice_states(std::string channel_id)
{
	std::unique_lock lk(_mutex);

	// Check if the channel exists
	if (!_voice_channels.count(channel_id))
	{
		throw QuesyncException(CHANNEL_NOT_FOUND);
	}

	return _voice_channels[channel_id];
}

void VoiceManager::handle_voice_states()
{
	while (true)
	{
		// Sleep for half a second
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::unique_lock lk(_mutex);

		std::time_t current_time = std::time(nullptr);

		// For each channel
		for (auto& channel : _voice_channels)
		{
			// For each user
			for (auto& user : channel.second)
			{
				// If pending time exceeded, change the user's state
				if (current_time - user.second.change_time() > MAX_PENDING_SECONDS)
				{
					_voice_channels[channel.first][user.first] = DISCONNECTED;
					trigger_voice_state_event(channel.first, user.first, DISCONNECTED);
				}
			}
		}
	}
}

void VoiceManager::trigger_voice_state_event(std::string channel_id, std::string user_id, QuesyncVoiceState voice_state)
{
	auto channel_participants = _voice_channels[channel_id];

	VoiceStateEvent evt(user_id, voice_state);

	// For each user in the channel
	for (auto& user : channel_participants)
	{
		// Check if the user is connected to the channel
		if (user.first != user_id && user.second == CONNECTED)
		{
			_server->eventManager()->triggerEvent(evt, user.first);
		}
	}
}