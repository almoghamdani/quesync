#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class leave_call_packet : public serialized_packet {
   public:
    /// Default constructor.
    leave_call_packet() : serialized_packet(packet_type::leave_call_packet){};

    virtual bool verify() const { return true; };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Leave the voice channel
            session->server()->voice_manager()->leave_voice_channel(session->user()->id);

            // Destroy the voice session of the user
            session->server()->voice_manager()->delete_voice_session(session->user()->id);

            // Return success response packet
            return response_packet(packet_type::call_left_packet, std::string()).encode();
        } catch (exception &ex) {
            // Return the error code
            return error_packet(ex.error_code()).encode();
        } catch (...) {
            return error_packet(error::unknown_error).encode();
        }
    };
#endif
};
};  // namespace packets
};  // namespace quesync
