#pragma once
#include "serialized_packet.h"

#include "response_packet.h"
#include "error_packet.h"

class FriendRequestPacket : public SerializedPacket
{
  public:
    FriendRequestPacket() : FriendRequestPacket(""){};

    FriendRequestPacket(std::string recipient_id) : SerializedPacket(FRIEND_REQUEST_PACKET)
    {
        _data["recipientId"] = recipient_id;
    };

    virtual bool verify() const
    {
        return (exists("recipientId"));
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

        // If the recipient id is the user's id, return error
        if (session->user()->id() == _data["recipientId"])
        {
            return ErrorPacket(SELF_FRIEND_REQUEST).encode();
        }

        try
        {
            // Send the user a friend request, if failed an exception will be thrown
            session->server()->userManager()->sendFriendRequest(session->user()->id(), _data["recipientId"]);

            // Return confirmation for the friend request
            return ResponsePacket(FRIEND_REQUEST_SENT_PACKET).encode();
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