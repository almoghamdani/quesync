#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../quesync_exception.h"

class JoinCallRequestPacket : public SerializedPacket
{
public:
	JoinCallRequestPacket() : JoinCallRequestPacket(""){};

	JoinCallRequestPacket(std::string channel_id) : SerializedPacket(JOIN_CALL_REQUEST_PACKET)
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
		std::string voice_session_id;

		nlohmann::json res;

		// If the user is not authenticed, send error
        if (!session->authenticated())
        {
            return ErrorPacket(NOT_AUTHENTICATED).encode();
        }

		try
		{
			// Create a voice session for the user
			voice_session_id = session->server()->voiceManager()->createVoiceSession(session->user()->id());

			// Try to join the voice channel
			session->server()->voiceManager()->joinVoiceChannel(session->user->id(), _data["channelId"]);

			// Set the res
			res["voiceSessionId"] = voice_session_id;

			// Return response packet with the voice info
			return ResponsePacket(JOIN_CALL_APPROVED_PACKET, res.dump()).encode();
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