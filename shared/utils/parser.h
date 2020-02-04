#pragma once

#include <string>
#include <vector>

#include "../header.h"
#include "../packet.h"

namespace quesync {
namespace utils {
class parser {
   public:
    /**
     * Split a string by a delimiter.
     *
     * @param s The raw string.
     * @param delimiter The character to split the string on.
     * @return A vector with all substrings.
     */
    static std::vector<std::string> split(const std::string &s, char delimiter);

    /**
     * Parse a packet to an object.
     *
     * @param packet The packet's encoded data.
     * @return A shared pointer to the packet object.
     */
    static std::shared_ptr<packet> parse_packet(std::string packet);

    /**
     * Gets the packet type from an encoded packet.
     *
     * @param packet The packet's encoded data.
     * @return The type of the packet.
     */
    static packet_type get_packet_type(std::string packet);

    /**
     * Encode an header.
     *
     * @param header The header to be encoded.
     * @return The encoded header.
     */
    static std::string encode_header(header &header);

    /**
     * Decode an header.
     *
     * @param buf The encoded header.
     * @return The header object decoded.
     */
    static header decode_header(const char *buf);
};
};  // namespace utils
};  // namespace quesync