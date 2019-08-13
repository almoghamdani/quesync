#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../quesync_exception.h"

class SetVoiceStatePacket : public SerializedPacket
{
public:
	SetVoiceStatePacket() : SetVoiceStatePacket(false, false){};

	SetVoiceStatePacket(bool muted, bool deafen) : SerializedPacket(SET_VOICE_STATE_PACKET)
	{
		_data["muted"] = muted;
		_data["deafen"] = deafen;
	};

	virtual bool verify() const
	{
		return exists("muted") &&
			   exists("deafen");
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		nlohmann::json res;

		// If the user is not authenticed, send error
		if (!session->authenticated())
		{
			return ErrorPacket(NOT_AUTHENTICATED).encode();
		}

		try
		{
			// Set the voice state of the user
			session->server()->voiceManager()->setVoiceState(session->user()->id(), _data["muted"], _data["deafen"]);

			return ResponsePacket(VOICE_STATE_SET_PACKET).encode();
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