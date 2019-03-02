#pragma once
#include "serialized_packet.h"

#include <iostream>
#include <sstream>

#include "response_packet.h"
#include "error_packet.h"

class FriendRequestPacket : public SerializedPacket
{
public:
    FriendRequestPacket() : FriendRequestPacket("") {};

    FriendRequestPacket(std::string recipient_id) : SerializedPacket(FRIEND_REQUEST_PACKET)
    {
        _data["recipient_id"] = recipient_id;
    };

    virtual bool verify() const
    {
        return (exists("recipient_id"));
    };

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session)
    {
        // If the user is not authenticed, send error
        if (!session->user())
        {
            return ErrorPacket(NOT_AUTHENTICATED).encode();
        }

        // If the recipient id is the user's id, return error
        if (session->user()->id() == _data["recipient_id"])
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