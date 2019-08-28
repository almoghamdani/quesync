#include "parser.h"

#include <sstream>

#include "../packet.h"
#include "packet_generator.h"

std::vector<std::string> quesync::utils::parser::split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);

    // While the string hasn't ended, get a "line" from it ending with the delimeter
    while (std::getline(token_stream, token, delimiter)) {
        // Save "line"/token in tokens vector
        tokens.push_back(token);
    }

    return tokens;
}

std::shared_ptr<quesync::packet> quesync::utils::parser::parse_packet(std::string packet) {
    std::shared_ptr<quesync::packet> p = nullptr;
    packet_type packet_type;

    // Check if the packet starts with the packet identifier, if it isn't return null
    if (packet.substr(0, strlen(PACKET_IDENTIFIER)) != PACKET_IDENTIFIER) {
        return nullptr;
    }

    // Remove the packet identifier from the packet str
    packet = packet.substr(strlen(PACKET_IDENTIFIER) + 1, packet.length());

    // Get the packet type
    packet_type = get_packet_type(packet);

    // Remove the packet type from the packet str
    packet = packet.substr(4, packet.length());

    // Generate packet
    p = packet_generator::generate_packet(packet_type);

    // If the type was found, decode the packet, if the decode was a success, return the packet type
    if (p != nullptr && p->decode(packet)) {
        return p;
    }

    return nullptr;
}

quesync::packet_type quesync::utils::parser::get_packet_type(std::string packet) {
    // Convert first 3 characters of string to the packet type (The first three should be the packet
    // type)
    return (packet_type)std::stoi(packet.substr(0, PACKET_TYPE_LEN));
}

std::string quesync::utils::parser::encode_header(quesync::header &header) {
    char buf[sizeof(header)];

    *(uint32_t *)buf = header.version;
    *(uint32_t *)(buf + sizeof(uint32_t)) = header.size;

    return std::string(buf, sizeof(header));
}

quesync::header quesync::utils::parser::decode_header(const char *buf) {
    header header;

    header.version = *(uint32_t *)buf;
    header.size = *(uint32_t *)(buf + sizeof(uint32_t));

    return header;
}