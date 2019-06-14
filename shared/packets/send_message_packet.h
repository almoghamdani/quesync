#pragma once
#include "packet.h"

#include <sstream>

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../message.h"
#include "../quesync_exception.h"

class SendMessagePacket : public SerializedPacket
{
public:
	SendMessagePacket() : SendMessagePacket("", ""){};

	SendMessagePacket(std::string content, std::string channel_id) : SerializedPacket(SEND_MESSAGE_PACKET)
	{
		_data["content"] = content;
		_data["channelId"] = channel_id;
	};

	virtual bool verify() const
	{
		return (exists("content") &&
				exists("channelId"));
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		std::shared_ptr<Message> message;

		// If the user is not authenticed, send error
		if (!session->authenticated())
		{
			return ErrorPacket(NOT_AUTHENTICATED).encode();
		}

		try
		{
			// Try to send the message to the channel
			message = session->server()->messageManager()->sendMessage(session->getShared(), _data["content"], _data["channelId"]);

			// Return response packet with the message id
			return ResponsePacket(MESSAGE_ID_PACKET, (nlohmann::json{{ "messageId", message->id() }}).dump()).encode();
		}
		catch (QuesyncException &ex)
		{
			// Return the error code
			return ErrorPacket(ex.getErrorCode()).encode();
		}
	};
#endif
};