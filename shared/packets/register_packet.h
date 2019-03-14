#pragma once
#include "serialized_packet.h"

#include <sstream>

#include "response_packet.h"
#include "error_packet.h"
#include "../quesync_exception.h"

class RegisterPacket : public SerializedPacket
{
  public:
    RegisterPacket() : RegisterPacket("", "", "", ""){};

    RegisterPacket(std::string username,
                   std::string password,
                   std::string email,
                   std::string nickname) : SerializedPacket(REGISTER_PACKET)
    {
        _data["username"] = username;
        _data["password"] = password;
        _data["email"] = email;
        _data["nickname"] = nickname;
    };

    virtual bool verify() const
    {
        return (exists("username") &&
                exists("password") &&
                exists("email") &&
                exists("nickname"));
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(Session *session)
    {
        User *user;

        // If the user is already authenticated, return error
        if (session->authenticated())
        {
            return ErrorPacket(ALREADY_AUTHENTICATED).encode();
        }

        try
        {
            // Register the new user, if failed an exception will be thrown
            user = session->server()->userManagement().registerUser(_data["username"],
                                                                    _data["password"],
                                                                    _data["email"],
                                                                    _data["nickname"]);

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