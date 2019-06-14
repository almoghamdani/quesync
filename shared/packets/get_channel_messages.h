#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../message.h"
#include "../quesync_exception.h"

class GetChannelMessagesPacket : public SerializedPacket
{
public:
	GetChannelMessagesPacket() : GetChannelMessagesPacket("", 0, 0){};

	GetChannelMessagesPacket(std::string channel_id, unsigned int amount, unsigned int offset) : SerializedPacket(SEND_MESSAGE_PACKET)
	{
		_data["channelId"] = channel_id;
		_data["amount"] = amount;
		_data["offset"] = offset;
	};

	virtual bool verify() const
	{
		return (exists("channelId") &&
				exists("amount") &&
				exists("offset"));
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		std::vector<Message> messages;

		// If the user is not authenticed, send error
		if (!session->authenticated())
		{
			return ErrorPacket(NOT_AUTHENTICATED).encode();
		}

		try
		{
			// Try to send the message to the channel
			messages = session->server()->messageManager()->getMessages(session->getShared(), _data["channelId"], _data["amount"], _data["offset"]);

			// Return response packet with the message id
			return ResponsePacket(CHANNEL_MESSAGES_PACKET, ((nlohmann::json)messages).dump()).encode();
		}
		catch (QuesyncException &ex)
		{
			// Return the error code
			return ErrorPacket(ex.getErrorCode()).encode();
		}
	};
#endif
};