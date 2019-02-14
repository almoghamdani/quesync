#pragma once
#include "packet.h"

#include "response_packet.h"
#include "error_packet.h"
#include "../utils.h"
#include "../quesync_exception.h"

class LoginPacket : public Packet
{
public:
    virtual std::string encode()
    {
        return std::string("");
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
    virtual std::string handle (Quesync *server)
    {
        User *user;

        try {
            // Authenticate the usetr, if failed an exception will be thrown
            user = server->authenticateUser(_username, _password);

            return ResponsePacket(AUTHENTICATED_PACKET, user->serialize()).encode();
        } catch (QuesyncException& ex) {
            return ErrorPacket(ex.getErrorCode()).encode();
        }  
    };
    #endif

private:
    std::string _username;
    std::string _password;
};