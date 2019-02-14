#pragma once
#include "packet.h"

#include "response_packet.h"
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

    virtual std::string handle (Quesync *server)
    {
        User *user;

        try {
            // Authenticate the usetr, if failed an exception will be thrown
            user = server->authenticateUser(_username, _password);

            return ResponsePacket(AUTHENTICATED_PACKET).encode();
        } catch (QuesyncException& ex) {
            return std::to_string(ex.getErrorCode());
        }  
    };

private:
    std::string _username;
    std::string _password;
};