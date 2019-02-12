#include "packet.h"

class ResponsePacket : public Packet
{
public:
    ResponsePacket(PacketType type) : _type(type)
    {
    }
};