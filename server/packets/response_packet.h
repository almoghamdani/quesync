#pragma once
#include "packet.h"

#include <sstream>
#include <iomanip>

class ResponsePacket : public Packet
{
public:
    ResponsePacket(PacketType type, std::string data) : Packet(type), _data(data)
    {
    };
    
    virtual std::string encode()
    {
        std::stringstream encodedPacket;

        // Set the packet identifier
        encodedPacket << PACKET_IDENTIFIER << PACKET_DELIMETER;

        // Set the type of the response and the data of the response
        encodedPacket << std::setfill('0') << std::setw(PACKET_TYPE_LEN) << _type << PACKET_DELIMETER << _data << PACKET_DELIMETER;

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

private:
    std::string _data;
};