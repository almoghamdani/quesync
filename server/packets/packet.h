#pragma once
#include <string>

#include "..\quesync.h"
#include "packet_types.h"

#define PACKET_IDENTIFIER "QUESYNC"
#define PACKET_DELIMETER '|'

class Packet
{
public:
    PacketType getType() const
    {
        return _type;
    }

    virtual std::string encode() = 0;
    virtual void decode (std::string packet) = 0;
    virtual Packet *handle (Quesync *server) = 0;

protected:
    PacketType _type;
};