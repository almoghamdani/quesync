#pragma once
#include "packet.h"

#include "response_packet.h"
#include "../utils.h"

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

    virtual std::string handle (Quesync *server)
    {
        // Try to authenticate the user
        if (server->authenticateUser(_username, _password) == SUCCESS)
        {
            // Encode the response packet if the authentication was successful
            return ResponsePacket(AUTHENTICATED_PACKET).encode();
        }

        return "";
    };

private:
    std::string _username;
    std::string _password;
};