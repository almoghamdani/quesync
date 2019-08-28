#pragma once

#include <string>
#include <vector>

#include "../header.h"
#include "../packet.h"

namespace quesync {
namespace utils {
class parser {
   public:
    static std::vector<std::string> split(const std::string &s, char delimiter);

    static std::shared_ptr<packet> parse_packet(std::string packet);
    static packet_type get_packet_type(std::string packet);

    static std::string encode_header(header &header);
    static header decode_header(const char *buf);
};
};  // namespace utils
};  // namespace quesync