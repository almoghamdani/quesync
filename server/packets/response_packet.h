#pragma once
#include "packet.h"

#include <format>

class ResponsePacket : public Packet
{
public:
    ResponsePacket(PacketType type) : Packet(type)
    {
    };
    
    virtual std::string encode()
    {
        // Format the response type in the packet
        return fmt::format("{1}{0}{2}", PACKET_DELIMETER, PACKET_IDENTIFIER, _type);
    };

    virtual void decode (std::string packet)
    {
    };

    virtual std::string handle (Quesync *server)
    {
    };
};