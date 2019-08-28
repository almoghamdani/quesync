#pragma once
#include "../packet.h"

#include <iomanip>
#include <sstream>

#include "../response_packet.h"

namespace quesync {
namespace packets {
class ping_packet : public packet {
   public:
    ping_packet() : packet(packet_type::ping_packet) {}

    virtual std::string encode() {
        // Format the ping packet
        return (std::stringstream()
                << PACKET_IDENTIFIER << PACKET_DELIMETER << std::setw(PACKET_TYPE_LEN)
                << std::setfill('0') << (int)_type << PACKET_DELIMETER)
            .str();
    };

    virtual bool decode(std::string packet) { return true; }

#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        // Return a pong packet to the client
        return response_packet(packet_type::pong_packet).encode();
    }
#endif
};
};  // namespace packets
};  // namespace quesync