#pragma once
#include "../response_packet.h"

#include "../error.h"

#define ERROR_CODE_LEN 3

namespace quesync {
namespace packets {
class error_packet : public response_packet {
   public:
    /// Default constructor.
    error_packet() : response_packet(packet_type::error_packet, std::string()){};

    /**
     * Packet constructor.
     *
     * @param ec The error code.
     */
    error_packet(error ec)
        : _ec(ec),
          response_packet(packet_type::error_packet,
                          std::string(ERROR_CODE_LEN - std::to_string((int)ec).length(), '0') +
                              std::to_string((int)ec))  // Add leading zeros to the error code
          {};

    virtual bool decode(std::string packet) {
        // Split the packet
        std::vector<std::string> params = utils::parser::split(packet, PACKET_DELIMETER);

        try {
            // Decode the error code
            _ec = (quesync::error)std::stoi(params[0]);
        } catch (...) {
            return false;
        }

        return true;
    };

    /**
     * Get the error code.
     *
     * @return The error code.
     */
    quesync::error error() const { return _ec; };

   protected:
    quesync::error _ec;
};
};  // namespace packets
};  // namespace quesync
