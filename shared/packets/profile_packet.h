#pragma once
#include "packet.h"

#include <iostream>
#include <sstream>

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"
#include "../utils.h"
#include "../quesync_exception.h"

class ProfilePacket : public SerializedPacket
{
  public:
    ProfilePacket() : ProfilePacket(""){};

    ProfilePacket(std::string user_id) : SerializedPacket(PROFILE_REQUEST_PACKET)
    {
        _data["user_id"] = user_id;
    };

    virtual bool verify() const
    {
        return exists("user_id");
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(Session *session)
    {
        Profile *profile;
        std::string profile_serialized;

        // If the user isn't authenticated, throw error
        if (!session->authenticated())
        {
            return ErrorPacket(NOT_AUTHENTICATED).encode();
        }

        try
        {
            // Try get the profile of the requested user
            profile = session->server()->userManagement().getUserProfile(_data["user_id"]);

            // Serialize the profile object and deallocate it
            profile_serialized = profile->serialize();
            delete profile;

            // Return the profile of the user
            return ResponsePacket(PROFILE_PACKET, profile_serialized).encode();
        }
        catch (QuesyncException &ex)
        {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }
    };
#endif
};