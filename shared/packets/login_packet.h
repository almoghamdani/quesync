#pragma once
#include "packet.h"

#include <sstream>

#include "response_packet.h"
#include "error_packet.h"
#include "../utils.h"
#include "../quesync_exception.h"

class LoginPacket : public Packet
{
public:
    LoginPacket(std::string username, std::string password) : _username(username), _password(password)
    {
    };

    virtual std::string encode()
    {
        // Format the login packet
        return (std::stringstream() << PACKET_IDENTIFIER << PACKET_DELIMETER
                                   << std::setw(3) << std::setfill('0') << LOGIN_PACKET
                                   << _username << PACKET_DELIMETER
                                   << _password << PACKET_DELIMETER).str();
    };

    virtual void decode (std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        // Set the email and password from the params
        _username = params[0];
        _password = params[1];
    };

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session)
    {
        User *user;

        try {
            // Authenticate the usetr, if failed an exception will be thrown
            user = session->server()->authenticateUser(_username, _password);

            // Set the user in the client's session
            session->setUser(user);

            // Return autheticated packet with the user's info
            return ResponsePacket(AUTHENTICATED_PACKET, user->serialize()).encode();
        } catch (QuesyncException& ex) {
            // Return the error code
            return ErrorPacket(ex.getErrorCode()).encode();
        }  
    };
    #endif

private:
    std::string _username;
    std::string _password;
};