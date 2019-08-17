#include "voice_manager.h"

#include <algorithm>
#include <sole.hpp>

#include "quesync.h"
#include "../shared/packets/voice_packet.h"
#include "../shared/packets/participant_voice_packet.h"
#include "../shared/packets/voice_otp_packet.h"
#include "../shared/events/voice_state_event.h"
#include "../shared/events/call_ended_event.h"
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
	VoiceOTPPacket otp_packet;

	VoiceEncryptionInfo encryption_info;
	std::string user_session;
	std::shared_ptr<VoicePacket> packet;

	VoiceEncryptionInfo participant_encryption_info;
	std::string participant_session;
	udp::endpoint participant_endpoint;

	ParticipantVoicePacket participant_packet;
	std::string participant_packet_encrypted;
	std::shared_ptr<char> buf;

	std::unordered_map<std::string, VoiceState> channel_participants;

	std::unique_lock lk(_mutex);

	// If the packet is an OTP packet
	if (otp_packet.decode(std::string(_buf, length)))
	{
		try
		{
			// Get user's session
			user_session = _otps[otp_packet.otp()];

			// Save endpoint
			_session_endpoints[user_session] = _sender_endpoint;

			// Remove OTP
			_otps.erase(otp_packet.otp());
		}
		catch (...)
		{
		}

		return;
	}

	// Try to find the participant session by his endpoint and get it's encryption info
	try
	{
		user_session = std::find_if(_session_endpoints.begin(), _session_endpoints.end(),
									[this](const std::pair<std::string, udp::endpoint> &p) {
										return p.second == _sender_endpoint;
									})
						   ->first;

		encryption_info = _session_keys.at(user_session);
	}
	catch (...)
	{
		return;
	}

	// Try to decrypt the voice packet
	packet = Utils::DecryptVoicePacket<VoicePacket>(std::string(_buf, length), encryption_info.aes_key.get(), encryption_info.hmac_key.get());
	if (!packet)
	{
		return;
	}

	// Try to find the channel participants
	try
	{
		channel_participants = _voice_channels.at(packet->channel_id());
	}
	catch (...)
	{
		return;
	}

	// If the user isn't a part of the channel
	if (!channel_participants.count(packet->user_id()))
	{
		return;
	}

	if (packet->voice_data_len())
	{
		// Create the pariticipant voice packet and encode it
		participant_packet = ParticipantVoicePacket(packet->user_id(), packet->voice_data(), packet->voice_data_len());

		// Send the participant voice packet to all other participants
		for (auto &participant : channel_participants)
		{
			// If the given participant isn't our user
			if (participant.first != packet->user_id())
			{
				try
				{
					// Try to get the session, key and endpoint of the participant
					participant_session = _sessions.at(participant.first);
					participant_endpoint = _session_endpoints.at(participant_session);
					participant_encryption_info = _session_keys.at(participant_session);
				}
				catch (...)
				{
					continue;
				}

				// Encrypt the packet and convert it to buffer
				participant_packet_encrypted = Utils::EncryptVoicePacket<ParticipantVoicePacket>(&participant_packet, participant_encryption_info.aes_key.get(), participant_encryption_info.hmac_key.get());
				buf = Utils::ConvertToBuffer<char>(participant_packet_encrypted);

				// Send the participant voice packet to the participant
				send(buf, participant_packet_encrypted.length(), participant_endpoint);
			}
		}
	}
}

std::pair<std::string, VoiceEncryptionInfo> VoiceManager::createVoiceSession(std::string user_id)
{
	std::unique_lock lk(_mutex);

	// If a session doesn't exists for the user
	if (!_sessions.count(user_id))
	{
		_sessions[user_id] = sole::uuid4().str();
	}

	// Create the aes key and hmac key for the session
	_session_keys[_sessions[user_id]] = VoiceEncryptionInfo{Utils::RandBytes(AES_KEY_SIZE), Utils::RandBytes(HMAC_KEY_SIZE)};

	return *_session_keys.find(_sessions[user_id]);
}

void VoiceManager::deleteVoiceSession(std::string user_id)
{
	std::unique_lock lk(_mutex);

	try
	{
		// Remove OTP if exists
		_otps.erase(_sessions[user_id]);
	}
	catch (...)
	{
	}

	try
	{
		_session_endpoints.erase(_sessions[user_id]);
		_session_keys.erase(_sessions[user_id]);
		_sessions.erase(user_id);
	}
	catch (...)
	{
		throw QuesyncException(VOICE_NOT_CONNECTED);
	}
}

std::string VoiceManager::generateOTP(std::string session_id)
{
	std::unique_lock lk(_mutex);

	std::string otp;

	// Generate random bytes for the OTP
	std::shared_ptr<unsigned char> bytes = Utils::RandBytes(OTP_SIZE);

	// Save the OTP
	otp = std::string((char *)bytes.get(), OTP_SIZE);
	_otps[otp] = session_id;

	return otp;
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
		user_states[user] = VoiceState(PENDING, false, false);
	}

	_voice_channels[channel_id] = user_states;
}

bool VoiceManager::isVoiceChannelActive(std::string channel_id)
{
	std::unique_lock lk(_mutex);

	return _voice_channels.count(channel_id);
}

void VoiceManager::joinVoiceChannel(std::string user_id, std::string channel_id, bool muted, bool deafen)
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
	_voice_channels[channel_id][user_id] = VoiceState(CONNECTED, muted, deafen);
	trigger_voice_state_event(channel_id, user_id, _voice_channels[channel_id][user_id]);
}

void VoiceManager::leaveVoiceChannel(std::string user_id)
{
	std::string channel_id;
	std::unique_lock lk(_mutex);

	CallEndedEvent call_ended_event;

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
	trigger_voice_state_event(channel_id, user_id, _voice_channels[channel_id][user_id]);

	// Check for others connected to the voice channel
	for (auto &join_pair : _joined_voice_channels)
	{
		if (join_pair.second == channel_id)
		{
			return;
		}
	}

	// Send call ended event for all participants of the call
	call_ended_event = CallEndedEvent(channel_id);
	for (auto &user : _voice_channels[channel_id])
	{
		if (user.first != user_id)
			_server->eventManager()->triggerEvent(call_ended_event, user.first);
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
		for (auto &channel : _voice_channels)
		{
			// For each user
			for (auto &user : channel.second)
			{
				// If pending time exceeded, change the user's state
				if (user.second == PENDING && current_time - user.second.change_time() > MAX_PENDING_SECONDS)
				{
					_voice_channels[channel.first][user.first] = DISCONNECTED;
					trigger_voice_state_event(channel.first, user.first, _voice_channels[channel.first][user.first]);
				}
			}
		}
	}
}

void VoiceManager::setVoiceState(std::string user_id, bool muted, bool deafen)
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

	if (muted)
	{
		_voice_channels[channel_id][user_id].mute();
	}
	else
	{
		_voice_channels[channel_id][user_id].unmute();
	}

	if (deafen)
	{
		_voice_channels[channel_id][user_id].deaf();
	}
	else
	{
		_voice_channels[channel_id][user_id].undeaf();
	}

	// Send event to all participants
	trigger_voice_state_event(channel_id, user_id, _voice_channels[channel_id][user_id]);
}

void VoiceManager::trigger_voice_state_event(std::string channel_id, std::string user_id, VoiceState voice_state)
{
	auto channel_participants = _voice_channels[channel_id];

	VoiceStateEvent evt(user_id, voice_state);

	// For each user in the channel
	for (auto &user : channel_participants)
	{
		// Check if the user is connected to the channel
		if (user.first != user_id && user.second == CONNECTED)
		{
			_server->eventManager()->triggerEvent(evt, user.first);
		}
	}
}