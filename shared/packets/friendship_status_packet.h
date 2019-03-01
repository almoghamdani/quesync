#pragma once
#include "serialized_packet.h"

#include <iostream>
#include <sstream>

#include "response_packet.h"
#include "error_packet.h"

class FriendshipStatusPacket : public SerializedPacket
{
public:
    FriendshipStatusPacket() : FriendshipStatusPacket("", false) {};

    FriendshipStatusPacket(std::string friend_id, bool status) : SerializedPacket(FRIENDSHIP_STATUS_PACKET)
    {
        _data["friend_id"] = friend_id;
        _data["status"] = status;
    };

    virtual bool verify() const
    {
        return (exists("friend_id") &&
                exists("status"));
    };

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session)
    {
        // If the recipient id is the user's id, return error
        if (session->user()->id() == _data["friend_id"])
        {
            return ErrorPacket(SELF_FRIEND_REQUEST).encode();
        }

        try {
            // Send the user a friend request, if failed an exception will be thrown
            session->server()->userManagement().sendFriendRequest(session->user()->id(), _data["recipient_id"]);

            // Return confirmation for the friend request
            return ResponsePacket(FRIEND_REQUEST_SENT_PACKET).encode();
        } catch (QuesyncException& ex) {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }  
    };
    #endif
};