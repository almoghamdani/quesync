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
        // If the friend id is the user's id, return error
        if (session->user()->id() == _data["friend_id"])
        {
            return ErrorPacket(SELF_FRIEND_REQUEST).encode();
        }

        try {
            // Set the friendship status, if failed an exception will be thrown
            session->server()->userManagement().setFriendshipStatus(session->user()->id(), _data["friend_id"], _data["status"]);

            // Return confirmation for the friendship status
            return ResponsePacket(FRIENDSHIP_STATUS_SET_PACKET).encode();
        } catch (QuesyncException& ex) {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }  
    };
    #endif
};