#include "voice_manager.h"

#include <algorithm>
#include <sole.hpp>

#include "quesync.h"
#include "../shared/packets/voice_packet.h"
#include "../shared/packets/participant_voice_packet.h"
#include "../shared/quesync_exception.h"

VoiceManager::VoiceManager(std::shared_ptr<Quesync> server) : Manager(server),
															  _socket(server->get_io_context(), udp::endpoint(udp::v4(), VOICE_SERVER_PORT))
{
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

	std::string user_id;
	std::vector<std::string> channel_participants;

	// Try to decode the packet
	if (!packet.decode(std::string(_buf, length)))
	{
		throw std::exception();
	}

	// Try to find the user id and the channel id
	try
	{
		user_id = _sessions.at(packet.session_id());
		channel_participants = _voice_channels.at(packet.channel_id());
	}
	catch (...)
	{
		return;
	}

	// If the user's session isn't in the endpoints list, insert it
	if (_session_endpoints.find(packet.session_id()) == _session_endpoints.end())
	{
		_session_endpoints[packet.session_id()] = _sender_endpoint;
	}

	// If the user isn't a part of the channel
	if (find(channel_participants.begin(), channel_participants.end(), user_id) == channel_participants.end())
	{
		return;
	}

	// Create the pariticipant voice packet and encode it
	participant_packet = ParticipantVoicePacket(user_id, packet.voice_data(), packet.voice_data_len());
	participant_packet_encoded = participant_packet.encode();
	buf = Utils::ConvertToBuffer(participant_packet_encoded);

	// Send the participant voice packet to all other participants
	for (auto &participant : channel_participants)
	{
		// If the given participant isn't our user
		if (participant != user_id)
		{
			try
			{
				// Try to get the session and endpoint of the participant
				participant_session = _sessions.at(participant);
				participant_endpoint = _session_endpoints.at(participant_session);
			} catch (...) {
				continue;
			}

			// Send the participant voice packet to the participant
			send(buf, participant_packet_encoded.length(), participant_endpoint);
		}
	}
}

std::string VoiceManager::createVoiceSession(std::string user_id)
{
	std::string session_id = sole::uuid4().str();

	_sessions[user_id] = session_id;

	return session_id;
}

void VoiceManager::deleteVoiceSession(std::string user_id)
{
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
	// If the voice channel already started
	if (_voice_channels.find(channel_id) != _voice_channels.end())
	{
		throw QuesyncException(CALL_ALREADY_STARTED);
	}

	_voice_channels[channel_id] = users;
}

void VoiceManager::joinVoiceChannel(std::string user_id, std::string channel_id)
{
	// If the user is already in a channel, leave the channel
	if (_joined_voice_channels.find(user_id) != _joined_voice_channels.end())
	{
		leaveVoiceChannel(user_id);
	}

	// Check if the channel exists
	if (_voice_channels.find(channel_id) == _voice_channels.end())
	{
		throw QuesyncException(CHANNEL_NOT_FOUND);
	}

	_joined_voice_channels[user_id] = channel_id;
}

void VoiceManager::leaveVoiceChannel(std::string user_id)
{
	std::string channel_id;

	// Check if in voice channel
	if(_joined_voice_channels.find(user_id) == _joined_voice_channels.end())
	{
		throw QuesyncException(VOICE_NOT_CONNECTED);
	}

	// Get the channel id of the user
	channel_id = _joined_voice_channels[user_id];

	// Remove the user from the map of joined voice channels
	_joined_voice_channels.erase(user_id);

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