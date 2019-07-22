#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../events/incoming_call_event.h"
#include "../quesync_exception.h"

class CallRequestPacket : public SerializedPacket
{
public:
	CallRequestPacket() : CallRequestPacket(""){};

	CallRequestPacket(std::string channel_id) : SerializedPacket(CALL_REQUEST_PACKET)
	{
		_data["channelId"] = channel_id;
	};

	virtual bool verify() const
	{
		return exists("channelId");
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		IncomingCallEvent call_event;

		std::string voice_session_id;

		std::vector<std::string> users;

		nlohmann::json res;

		// If the user is not authenticed, send error
        if (!session->authenticated())
        {
            return ErrorPacket(NOT_AUTHENTICATED).encode();
        }

		// Add the user to the list of users
		users.push_back(session->user()->id());

		try
		{
			// Get the channel members
			users = session->server()->channelManager()->getChannelMembers(session->getShared(), _data["channelId"]);

			// Create a voice session for the user
			voice_session_id = session->server()->voiceManager()->createVoiceSession(session->user()->id());

			// Create a voice channel for the users
			session->server()->voiceManager()->initVoiceChannel(_data["channelId"], _data["users"]);

			// Set the res
			res["voiceSessionId"] = voice_session_id;

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
		catch (...) {
            return ErrorPacket(UNKNOWN_ERROR).encode(); 
        }
	};
#endif
};