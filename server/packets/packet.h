#pragma once
#include <string>

#include "packet_types.h"

class Packet
{
public:
    PacketType getType() const
    {
        return _type;
    }

    virtual std::string encode() = 0;
    virtual void decode (std::string packet) = 0;

protected:
    PacketType _type;
};