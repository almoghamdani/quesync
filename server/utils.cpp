#include <vector>
#include <sstream>

#include "utils.h"

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