#pragma once

#include <memory>
#include <unordered_map>

#include "../packet_type.h"
#include "../response_packet.h"

#define PACKET_ENTRY(packet_name) \
    { packet_type::packet_name, &packet_generator::init_packet<packets::packet_name> }
#define RESPONSE_PACKET_ENTRY(packet_name)                                                 \
    {                                                                                      \
        packet_type::packet_name,                                                          \
            &packet_generator::init_packet<response_packet, (int)packet_type::packet_name> \
    }

namespace quesync {
namespace utils {
class packet_generator {
   public:
    static std::shared_ptr<packet> generate_packet(packet_type type) {
        try {
            return packet_initalizers.at(type)();
        } catch (...) {
            return nullptr;
        }
    }

    template <typename T, int type = -1, typename std::enable_if<type == -1, T>::type* = nullptr>
    static std::shared_ptr<packet> init_packet() {
        return std::shared_ptr<packet>(new T);
    }

    template <typename T, int type = -1, typename std::enable_if<type != -1, T>::type* = nullptr>
    static std::shared_ptr<packet> init_packet() {
        return std::shared_ptr<packet>(new T((packet_type)type, std::string()));
    }

   private:
    static const std::unordered_map<packet_type, std::function<std::shared_ptr<packet>()>>
        packet_initalizers;
};
};  // namespace utils
};  // namespace quesync