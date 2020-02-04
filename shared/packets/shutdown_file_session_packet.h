#pragma once

#include <string>

#include "../packet.h"

#define SHUTDOWN_FILE_SESSION_PACKET_HEADER "QUESYNC_SHUTDOWN_FILE_SESSION"

namespace quesync {
namespace packets {
class shutdown_file_session_packet {
   public:
    /**
     * Encode the packet.
     *
     * @return The packet encoded.
     */
    std::string encode() { return SHUTDOWN_FILE_SESSION_PACKET_HEADER; }

    /**
     * Decode the packet.
     *
     * @param buf The packet's encoded data.
     * @return True if the packet was decoded successfully or false otherwise.
     */
    bool decode(std::string buf) {
        // If incorrect size of packet, ignore
        if (buf.size() != sizeof(SHUTDOWN_FILE_SESSION_PACKET_HEADER) - 1) {
            return false;
        } else if (buf != SHUTDOWN_FILE_SESSION_PACKET_HEADER)  // Check header
        {
            return false;
        }

        return true;
    }
};
};  // namespace packets
};  // namespace quesync