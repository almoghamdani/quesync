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
        std::shared_ptr<Profile> profile;
        std::string profile_serialized;

        // If the user isn't authenticated, throw error
        if (!session->authenticated())
        {
            return ErrorPacket(NOT_AUTHENTICATED).encode();
        }

        try
        {
            // Try get the profile of the requested user
            profile = std::shared_ptr<Profile>(session->server()->userManager()->getUserProfile(_data["userId"]));

            // Serialize the profile object and deallocate it
            profile_serialized = profile->serialize();

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