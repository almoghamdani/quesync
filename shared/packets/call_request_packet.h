#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../quesync_exception.h"

class CallRequestPacket : public SerializedPacket
{
public:
	CallRequestPacket() : CallRequestPacket(std::vector<std::string>()){};

	CallRequestPacket(std::vector<std::string> users) : SerializedPacket(CALL_REQUEST_PACKET)
	{
		_data["users"] = users;
	};

	virtual bool verify() const
	{
		return exists("users");
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		std::string voice_session_id;
		std::string voice_channel_id;

		std::vector<std::string> users = _data["users"];

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
			// Create a voice session for the user
			voice_session_id = session->server()->voiceManager()->createVoiceSession(session->user()->id());

			// Create a voice channel for the users
			voice_channel_id = session->server()->voiceManager()->createVoiceChannel(_data["users"]);

			// Set the res
			res["voiceSessionId"] = voice_session_id;
			res["voiceChannelId"] = voice_channel_id;

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