#pragma once
#include "packet.h"

#include <iostream>
#include <sstream>

#include "response_packet.h"
#include "error_packet.h"
#include "serialized_packet.h"
#include "../utils.h"
#include "../quesync_exception.h"

class LoginPacket : public SerializedPacket
{
  public:
    LoginPacket() : LoginPacket("", ""){};

    LoginPacket(std::string username, std::string password) : SerializedPacket(LOGIN_PACKET)
    {
        _data["username"] = username;
        _data["password"] = password;
    };

    virtual bool verify() const
    {
        return (exists("username") &&
                exists("password"));
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(Session *session)
    {
        std::shared_ptr<User> user;

        // If the user is already authenticated, return error
        if (session->authenticated())
        {
            return ErrorPacket(ALREADY_AUTHENTICATED).encode();
        }

        try
        {
            // Authenticate the user, if failed an exception will be thrown
            user = session->server()->userManagement().authenticateUser(session->getShared(), _data["username"], _data["password"]);

            // Set the user in the client's session
            session->setUser(user);

            // Return autheticated packet with the user's info
            return ResponsePacket(AUTHENTICATED_PACKET, user->serialize()).encode();
        }
        catch (QuesyncException &ex)
        {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }
    };
#endif
};