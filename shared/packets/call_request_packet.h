#pragma once
#include "packet.h"

#include <asio.hpp>

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../events/incoming_call_event.h"
#include "../quesync_exception.h"

class CallRequestPacket : public SerializedPacket
{
public:
	CallRequestPacket() : CallRequestPacket("", false, false){};

	CallRequestPacket(std::string channel_id, bool muted, bool deafen) : SerializedPacket(CALL_REQUEST_PACKET)
	{
		_data["channelId"] = channel_id;
		_data["muted"] = muted;
		_data["deafen"] = deafen;
	};

	virtual bool verify() const
	{
		return exists("channelId") &&
			   exists("muted") &&
			   exists("deafen");
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		IncomingCallEvent call_event;

		std::pair<std::string, VoiceEncryptionInfo> voice_session_details;
		std::unordered_map<std::string, VoiceState> voice_states;
		std::vector<std::string> users;

		nlohmann::json res;

		// If the user is not authenticed, send error
		if (!session->authenticated())
		{
			return ErrorPacket(NOT_AUTHENTICATED).encode();
		}

		try
		{
			// Get the channel members
			users = session->server()->channelManager()->getChannelMembers(session->getShared(), _data["channelId"]);
			users.push_back(session->user()->id());

			try
			{
				// Try to leave the current voice channel if in one
				session->server()->voiceManager()->leaveVoiceChannel(session->user()->id());
			}
			catch (...)
			{
			}

			// Create a voice session for the user
			voice_session_details = session->server()->voiceManager()->createVoiceSession(session->user()->id());

			// Generate OTP for the session
			res["voiceSessionOTP"] = Utils::Base64Encode(session->server()->voiceManager()->generateOTP(voice_session_details.first));

			// Create a voice channel for the users
			session->server()->voiceManager()->initVoiceChannel(_data["channelId"], users);

			// Join the voice channel
			session->server()->voiceManager()->joinVoiceChannel(session->user()->id(), _data["channelId"], _data["muted"], _data["deafen"]);

			// Get the voice states of the channel
			voice_states = session->server()->voiceManager()->get_voice_states(_data["channelId"]);

			// Remove the user from the voice states
			voice_states.erase(session->user()->id());

			// Set the voice session details and the voice states
			res["voiceSessionAESKey"] = Utils::Base64Encode(std::string((char *)voice_session_details.second.aes_key.get(), AES_KEY_SIZE));
			res["voiceSessionHMACKey"] = Utils::Base64Encode(std::string((char *)voice_session_details.second.hmac_key.get(), HMAC_KEY_SIZE));
			res["voiceSessionId"] = voice_session_details.first;
			res["voiceStates"] = voice_states;

			// Create the call event
			call_event = IncomingCallEvent(_data["channelId"]);

			// Send the call event to the other users
			for (auto &user : users)
			{
				if (user != session->user()->id())
				{
					session->server()->eventManager()->triggerEvent(call_event, user);
				}
			}

			// Return response packet with the voice info
			return ResponsePacket(CALL_STARTED_PACKET, res.dump()).encode();
		}
		catch (QuesyncException &ex)
		{
			// Return the error code
			return ErrorPacket(ex.getErrorCode()).encode();
		}
		catch (...)
		{
			return ErrorPacket(UNKNOWN_ERROR).encode();
		}
	};
#endif
};