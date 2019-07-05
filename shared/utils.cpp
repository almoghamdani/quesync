#include "utils.h"

#include <iostream>
#include <random>
#include <sstream>
#include <regex>
#include <vector>
#include <algorithm>
#include <openssl/sha.h>

#include "packets/login_packet.h"
#include "packets/error_packet.h"
#include "packets/register_packet.h"
#include "packets/search_packet.h"
#include "packets/friend_request_packet.h"
#include "packets/friendship_status_packet.h"
#include "packets/ping_packet.h"
#include "packets/profile_packet.h"
#include "packets/event_packet.h"
#include "packets/get_private_channel_packet.h"
#include "packets/send_message_packet.h"
#include "packets/get_channel_messages_packet.h"
#include "packets/session_auth_packet.h"

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
    case PING_PACKET:
        p = new PingPacket();
        break;

    case LOGIN_PACKET:
        p = new LoginPacket();
        break;

    case REGISTER_PACKET:
        p = new RegisterPacket();
        break;

    case PROFILE_REQUEST_PACKET:
        p = new ProfilePacket();
        break;

    case SEARCH_PACKET:
        p = new SearchPacket();
        break;

    case FRIEND_REQUEST_PACKET:
        p = new FriendRequestPacket();
        break;

    case FRIENDSHIP_STATUS_PACKET:
        p = new FriendshipStatusPacket();
        break;

    case GET_PRIVATE_CHANNEL_PACKET:
        p = new GetPrivateChannelPacket();
        break;

    case SEND_MESSAGE_PACKET:
        p = new SendMessagePacket();
        break;

    case GET_CHANNEL_MESSAGES_PACKET:
        p = new GetChannelMessagesPacket();
        break;

    case SESSION_AUTH_PACKET:
        p = new SessionAuthPacket();
        break;

    case ERROR_PACKET:
        p = new ErrorPacket();
        break;

    case AUTHENTICATED_PACKET:
    case SEARCH_RESULTS_PACKET:
    case FRIEND_REQUEST_SENT_PACKET:
    case FRIENDSHIP_STATUS_SET_PACKET:
    case PONG_PACKET:
    case PROFILE_PACKET:
    case PRIVATE_CHANNEL_PACKET:
    case MESSAGE_ID_PACKET:
    case CHANNEL_MESSAGES_PACKET:
        p = new ResponsePacket(packet_type, "");
        break;

    case EVENT_PACKET:
        p = new EventPacket();
        break;
    }

    // If the type was found, decode the packet, if the decode was a success, return the packet type
    if (p != nullptr && p->decode(packet))
    {
        return p;
    }
    else
    {
        // Free the packet
        if (p)
            delete p;

        return nullptr;
    }
}

void Utils::CopyString(const std::string &input, char *dst)
{
    // Copy the string
    memcpy(dst, input.c_str(), input.length());

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
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
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

std::vector<std::string> Utils::Split(const std::string &s, char delimiter)
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

int Utils::RandomNumber(int min, int max)
{
    std::random_device rd;                           // Obtain a random number from hardware
    std::mt19937 gen(rd());                          // Seed the generator
    std::uniform_int_distribution<> distr(min, max); // Define the range

    // Get a random number from the generator using the distr
    return distr(gen);
}

std::string Utils::QuesyncErrorToString(QuesyncError error)
{
    // For each error, return it's name
    switch (error)
    {
    case SUCCESS:
        return "SUCCESS";
    case UNKNOWN_ERROR:
        return "UNKNOWN_ERROR";
    case INVALID_PACKET:
        return "INVALID_PACKET";
    case CANNOT_REACH_SERVER:
        return "CANNOT_REACH_SERVER";
    case NO_CONNECTION:
        return "NO_CONNECTION";
    case USER_NOT_FOUND:
        return "USER_NOT_FOUND";
    case INCORRECT_PASSWORD:
        return "INCORRECT_PASSWORD";
    case ALREADY_AUTHENTICATED:
        return "ALREADY_AUTHENTICATED";
    case NOT_AUTHENTICATED:
        return "NOT_AUTHENTICATED";
    case INVALID_USERNAME:
        return "INVALID_USERNAME";
    case INVALID_EMAIL:
        return "INVALID_EMAIL";
    case USERNAME_ALREADY_IN_USE:
        return "USERNAME_ALREADY_IN_USE";
    case EMAIL_ALREADY_IN_USE:
        return "EMAIL_ALREADY_IN_USE";
    case NICKNAME_FULL:
        return "NICKNAME_FULL";
    case ALREADY_FRIENDS:
        return "ALREADY_FRIENDS";
    case SELF_FRIEND_REQUEST:
        return "SELF_FRIEND_REQUEST";
    case NOT_FRIENDS:
        return "NOT_FRIENDS";
    case SELF_APPROVE_FRIEND_REQUEST:
        return "SELF_APPROVE_FRIEND_REQUEST";
    case CHANNEL_NOT_FOUND:
        return "CHANNEL_NOT_FOUND";
    case ALREADY_MEMBER:
        return "ALREADY_MEMBER";
    case NOT_MEMBER_OF_CHANNEL:
        return "NOT_MEMBER_OF_CHANNEL";
    case AMOUNT_EXCEEDED_MAX:
        return "AMOUNT_EXCEEDED_MAX";
    case SELF_PRIVATE_CHANNEL:
        return "SELF_PRIVATE_CHANNEL";
    case INVALID_SESSION:
        return "INVALID_SESSION";
    }

    return "";
}

std::string Utils::EncodeHeader(Header& header)
{
	char buf[sizeof(Header)];

	*(uint32_t *)buf = header.version;
	*(uint32_t *)(buf + sizeof(uint32_t)) = header.size;

	return std::string(buf, sizeof(Header));
}

Header Utils::DecodeHeader(const char *buf)
{
	Header header;

	header.version = *(uint32_t *)buf;
	header.size = *(uint32_t *)(buf + sizeof(uint32_t));

	return header;
}

#ifdef QUESYNC_SERVER
int Utils::GenerateTag(std::string nickname, sql::Table &users_table)
{
    int tag;
    std::vector<int> tags;

    // Get all tags from the users table
    std::list<sql::Row> res = users_table.select("tag")
                                  .where("nickname = :nickname")
                                  .bind("nickname", nickname)
                                  .execute()
                                  .fetchAll();

    // Move all tags found into a vector
    for (auto it = res.begin(); it != res.end(); it++)
    {
        tags.push_back((*it)[0]);
    }

    // If all the tags are used, throw exception
    if (tags.size() == MAX_TAG)
    {
        throw QuesyncException(NICKNAME_FULL);
    }

    // Generate tags until found one that is free
    do
    {
        tag = RandomNumber(0, MAX_TAG);
    } while (find(tags.begin(), tags.end(), tag) != tags.end());

    return tag;
}
#endif

#ifdef QUESYNC_CLIENT
Napi::Object Utils::JsonToObject(Napi::Env env, nlohmann::json json)
{
    // Convert the json dump to a Napi string object
    Napi::String json_dump = Napi::String::New(env, json.dump());

    // Get it's value
    napi_value json_value = json_dump.operator napi_value();

    // Get the JavaScript json parse function
    Napi::Object json_obj = Napi::Object(env, env.Global().Get("JSON"));
    Napi::Function parse = Napi::Function(env, json_obj.Get("parse"));

    // Parse the json dump to a JavaScript Object using the function
    return Napi::Object(env, parse.Call(1, (napi_value *)&json_value));
}
#endif