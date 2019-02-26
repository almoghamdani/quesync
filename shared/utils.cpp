#include "utils.h"

#include <sstream>
#include <regex>
#include <openssl/sha.h>

#include "packets/login_packet.h"
#include "packets/error_packet.h"
#include "packets/register_packet.h"

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
    
    case REGISTER_PACKET:
        p = new RegisterPacket();
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
        // Free the packet
        if (p)
            delete p;

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
    std::stringstream encrypted_stream;

    // Initialzie the SHA-256 encryptor
    SHA256_Init(&sha256);

    // Hash the string
    SHA256_Update(&sha256, str.c_str(), str.size());

    // Close the SHA-256 encryptor and finalize
    SHA256_Final(hash, &sha256);

    // Copy the hashed string to the encrypted stream to create a string
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        encrypted_stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    // Return the encrypted string
    return encrypted_stream.str();
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
   std::istringstream token_stream(s);

   // While the string hasn't ended, get a "line" from it ending with the delimeter
   while (std::getline(token_stream, token, delimiter))
   {
        // Save "line"/token in tokens vector
        tokens.push_back(token);
   }

   return tokens;
}

bool Utils::isValidUsername(std::string username)
{
    // Check if the username is a valid username using the username regex
    return std::regex_search(username, std::regex(R"(^(?!.*\.\.)(?!.*\.$)[^\W][\w.]{0,29}$)"));
}

bool Utils::isValidEmail(std::string email)
{
    // Check if the e-mail is a valid e-mail using the e-mail regex
    return std::regex_search(email, std::regex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)"));
}

#ifdef QUESYNC_CLIENT
Napi::Object Utils::JsonToObject(Napi::Env env, nlohmann::json& json)
{
    // Create a new instance of a js object
    Napi::Object res = Napi::Object::New(env);

    // Go through the items in the original json
    for (auto& element : json.items())
    {
        // Set the value in the element's key in the new js object
        res[element.key()] = element.value();
    }

    return res;
}
#endif