#pragma once
#include <string>

#include "packets\packet_types.h"

class Utils
{
public:
    static PacketType GetPacketType(std::string packet);
    static std::vector<std::string> Split(const std::string& s, char delimiter);
};