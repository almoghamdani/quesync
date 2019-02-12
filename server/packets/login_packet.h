#pragma once
#include "packet.h"

#include "response_packet.h"
#include "..\utils.h"

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
        _email = params[0];
        _password = params[1];
    };

    virtual std::string handle (Quesync *server)
    {
        // Encode the response packet
        return ResponsePacket(AUTHENTICATED_PACKET).encode();
    };

private:
    std::string _email;
    std::string _password;
};