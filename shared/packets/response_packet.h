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

    virtual bool decode (std::string packet)
    {
        return false;
    };

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session)
    {
        return nullptr;
    };
    #endif

protected:
    std::string _data;
};