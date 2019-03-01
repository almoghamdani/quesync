#pragma once
#include "packet.h"

#include <sstream>
#include <iomanip>

class ResponsePacket : public Packet
{
public:
    ResponsePacket(PacketType type) : ResponsePacket(type, "")
    {
    };

    ResponsePacket(PacketType type, std::string data) : Packet(type), _data(data)
    {
    };
    
    virtual std::string encode()
    {
        std::stringstream encoded_packet;

        // Set the packet identifier
        encoded_packet << PACKET_IDENTIFIER << PACKET_DELIMETER;

        // Set the type of the response
        encoded_packet << std::setfill('0') << std::setw(PACKET_TYPE_LEN) << _type << PACKET_DELIMETER;

        // If data was entered add it to the encoded response packet
        if (_data.length())
        {
            encoded_packet << _data << PACKET_DELIMETER;
        }

        // Format the response type in the packet
        return encoded_packet.str();
    };

    virtual bool decode (std::string packet)
    {
        // Split the packet
        std::vector<std::string> params = Utils::Split(packet, PACKET_DELIMETER);

        try {
            // Get the data from the packet, it should be the first parameter
            _data = params[0];
        } catch (...) {
            return false;
        }

        return true;
    };

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session)
    {
        return nullptr;
    };
    #endif

    std::string data() const
    {
        return _data;
    };

protected:
    std::string _data;
};