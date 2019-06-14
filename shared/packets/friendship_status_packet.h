#pragma once
#include "serialized_packet.h"

#include "response_packet.h"
#include "error_packet.h"

class FriendshipStatusPacket : public SerializedPacket
{
  public:
    FriendshipStatusPacket() : FriendshipStatusPacket("", false){};

    FriendshipStatusPacket(std::string friend_id, bool status) : SerializedPacket(FRIENDSHIP_STATUS_PACKET)
    {
        _data["friendId"] = friend_id;
        _data["status"] = status;
    };

    virtual bool verify() const
    {
        return (exists("friendId") &&
                exists("status"));
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

        // If the friend id is the user's id, return error
        if (session->user()->id() == _data["friendId"])
        {
            return ErrorPacket(SELF_FRIEND_REQUEST).encode();
        }

        try
        {
            // Set the friendship status, if failed an exception will be thrown
            session->server()->userManager()->setFriendshipStatus(session->user()->id(), _data["friendId"], _data["status"]);

            // Return confirmation for the friendship status
            return ResponsePacket(FRIENDSHIP_STATUS_SET_PACKET).encode();
        }
        catch (QuesyncException &ex)
        {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }
    };
#endif
};