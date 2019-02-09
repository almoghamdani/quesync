#pragma once
#include <string>
#include <vector>

#include "packets\packet.h"
#include "packets\packet_types.h"

class Utils
{
public:
    static Packet *ParsePacket(std::string packet);
    static PacketType GetPacketType(std::string packet);
    static std::vector<std::string> Split(const std::string& s, char delimiter);
};