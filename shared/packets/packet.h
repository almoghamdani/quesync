#pragma once
#include <string>

// Only include quesync server if built with server
#ifdef QUESYNC_SERVER
#include "../../server/session.h"
#endif

#include "packet_types.h"

#define PACKET_IDENTIFIER "QUESYNC"
#define PACKET_DELIMETER '|'
#define PACKET_TYPE_LEN 3

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

    // A handle function for the server
    #ifdef QUESYNC_SERVER
    virtual std::string handle (Session *session) = 0;
    #endif

protected:
    PacketType _type;
};