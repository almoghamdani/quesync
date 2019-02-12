#pragma once
#include "packet.h"

#include <sstream>

class ResponsePacket : public Packet
{
public:
    ResponsePacket(PacketType type) : Packet(type)
    {
    };
    
    virtual std::string encode()
    {
        std::stringstream encodedPacket;

        // Set the packet identifier
        encodedPacket << PACKET_IDENTIFIER << PACKET_DELIMETER;

        // Set the type of the response
        encodedPacket << setfill('0') << setw(PACKET_TYPE_LEN) << _type << PACKET_DELIMETER;

        // Format the response type in the packet
        return encodedPacket.str();
    };

    virtual void decode (std::string packet)
    {
    };

    virtual std::string handle (Quesync *server)
    {
        return nullptr;
    };
};