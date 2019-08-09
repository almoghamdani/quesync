#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../quesync_exception.h"

class LeaveCallPacket : public SerializedPacket
{
public:
	LeaveCallPacket() : SerializedPacket(LEAVE_CALL_PACKET) {};

	virtual bool verify() const
	{
		return true;
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		// If the user is not authenticed, send error
		if (!session->authenticated())
		{
			return ErrorPacket(NOT_AUTHENTICATED).encode();
		}

		try
		{
			// Leave the voice channel
			session->server()->voiceManager()->leaveVoiceChannel(session->user()->id());

			// Destroy the voice session of the user
			session->server()->voiceManager()->deleteVoiceSession(session->user()->id());

			// Return success response packet
			return ResponsePacket(CALL_LEFT_PACKET, "").encode();
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