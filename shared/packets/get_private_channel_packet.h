#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"

#include "../quesync_exception.h"

class GetPrivateChannelPacket : public SerializedPacket
{
public:
	GetPrivateChannelPacket() : GetPrivateChannelPacket(""){};

	GetPrivateChannelPacket(std::string user_id) : SerializedPacket(GET_PRIVATE_CHANNEL_PACKET)
	{
		_data["userId"] = user_id;
	};

	virtual bool verify() const
	{
		return exists("userId");
	};

// A handle function for the server
#ifdef QUESYNC_SERVER
	virtual std::string handle(Session *session)
	{
		std::shared_ptr<Channel> channel;

		nlohmann::json res;

		// If the user is not authenticed, send error
        if (!session->authenticated())
        {
            return ErrorPacket(NOT_AUTHENTICATED).encode();
        }

		try
		{
			// Get the private channel of the 2 users, if failed an exception will be thrown
			channel = session->server()->channelManager()->getPrivateChannel(session->getShared(), _data["userId"]);
			res["channel"] = channel->json();

			// Check if a call is active in the channel
			res["callActive"] = session->server()->voiceManager()->isVoiceChannelActive(channel->id());

			// Return response packet with the channel
			return ResponsePacket(PRIVATE_CHANNEL_PACKET, res.dump()).encode();
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