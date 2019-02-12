#pragma once
#include "packet.h"

class LoginPacket : public Packet
{
public:
    virtual std::string encode();
    virtual void decode (std::string packet);
    virtual std::string handle (Quesync *server);

private:
    std::string _email;
    std::string _password;
};