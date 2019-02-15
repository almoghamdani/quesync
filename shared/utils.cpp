#include <sstream>
#include <openssl/sha.h>

#include "utils.h"
#include "packets/login_packet.h"
#include "packets/error_packet.h"

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
    packet = packet.substr(strlen(PACKET_IDENTIFIER) + 1, packet.length());

    // Get the packet type
    packet_type = GetPacketType(packet);

    // Remove the packet type from the packet str
    packet = packet.substr(4, packet.length());

    // By the packet type create the correct packet type
    switch (packet_type)
    {
    case LOGIN_PACKET:
        p = new LoginPacket();
        break;

    case ERROR_PACKET:
        p = new ErrorPacket();
        break;

    case AUTHENTICATED_PACKET:
        p = new ResponsePacket(packet_type, "");
        break;
    }

    // If the type was found, decode the packet, if the decode was a success, return the packet type
    if (p != nullptr && p->decode(packet))
    {
        return p;
    } else {
        return nullptr;
    }
}

void Utils::CopyString(const std::string& input, char *dst)
{
    // Copy the string
    strncpy(dst, input.c_str(), input.length());

    // Null-terminate the string
    dst[input.length()] = '\0';
}

std::string Utils::SHA256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    std::stringstream encryptedStream;

    // Initialzie the SHA-256 encryptor
    SHA256_Init(&sha256);

    // Hash the string
    SHA256_Update(&sha256, str.c_str(), str.size());

    // Close the SHA-256 encryptor and finalize
    SHA256_Final(hash, &sha256);

    // Copy the hashed string to the encrypted stream to create a string
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        encryptedStream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    // Return the encrypted string
    return encryptedStream.str();
}

PacketType Utils::GetPacketType(std::string packet)
{
    // Convert first 3 characters of string to the packet type (The first three should be the packet type)
    return (PacketType)std::stoi(packet.substr(0, PACKET_TYPE_LEN));
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