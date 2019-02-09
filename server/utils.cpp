#include <sstream>

#include "utils.h"
#include "packets\login_packet.h"

Packet *Utils::ParsePacket(std::string packet)
{
    Packet *p = nullptr;
    PacketType packet_type;

    // Check if the packet starts with the packet identifier, if it isn't return null
    if (packet.substr(0, strlen(PACKET_IDENTIFIER)) != PACKET_IDENTIFIER)
    {
        return nullptr;
    }

    // Remove the packet identifier from the packet str
    packet = packet.substr(strlen(PACKET_IDENTIFIER) + 1, packet.length);

    // Get the packet type
    packet_type = GetPacketType(packet);

    // Remove the packet type from the packet str
    packet = packet.substr(4, packet.length);

    // By the packet type create the correct packet type
    switch (packet_type)
    {
    case LOGIN_PACKET:
        p = new LoginPacket();
    }

    // If the type was found, decode the packet
    if (p != nullptr)
    {
        p->decode(packet);
    }

    return p;
}

PacketType Utils::GetPacketType(std::string packet)
{
    // Convert first 3 characters of string to the packet type (The first three should be the packet type)
    return (PacketType)std::stoi(packet.substr(0, 3));
}

std::vector<std::string> Utils::Split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);

   // While the string hasn't ended, get a "line" from it ending with the delimeter
   while (std::getline(tokenStream, token, delimiter))
   {
        // Save "line"/token in tokens vector
        tokens.push_back(token);
   }

   return tokens;
}