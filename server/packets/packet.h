#pragma once
#include <string>

#include "..\quesync.h"
#include "packet_types.h"

#define PACKET_IDENTIFIER "QUESYNC"
#define PACKET_DELIMETER '|'

class Packet
{
public:
    Packet()
    {
    };

    Packet(PacketType type) : _type(type)
    {
    };

    virtual std::string encode() = 0;
    virtual void decode (std::string packet) = 0;
    virtual std::string handle (Quesync *server) = 0;

protected:
    PacketType _type;
};