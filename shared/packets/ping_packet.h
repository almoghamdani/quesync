#pragma once
#include "packet.h"

#include <iomanip>
#include <sstream>

#include "response_packet.h"

class PingPacket : public Packet
{
  public:
    PingPacket() : Packet(PING_PACKET) {}

    virtual std::string encode()
    {
        // Format the ping packet
        return (std::stringstream() << PACKET_IDENTIFIER << PACKET_DELIMETER
                                    << std::setw(PACKET_TYPE_LEN) << std::setfill('0') << _type << PACKET_DELIMETER)
            .str();
    };

    virtual bool decode(std::string packet)
    {
        return true;
    }

#ifdef QUESYNC_SERVER
    virtual std::string handle(Session *session)
    {
        // Return a pong packet to the client
        return ResponsePacket(PONG_PACKET).encode();
    }
#endif
};