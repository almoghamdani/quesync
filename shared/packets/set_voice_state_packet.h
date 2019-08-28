#pragma once
#include "../serialized_packet.h"

#include "error_packet.h"
#include "../response_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class set_voice_state_packet : public serialized_packet {
   public:
    set_voice_state_packet() : set_voice_state_packet(false, false){};

    set_voice_state_packet(bool muted, bool deafen)
        : serialized_packet(packet_type::set_voice_state_packet) {
        _data["muted"] = muted;
        _data["deafen"] = deafen;
    };

    virtual bool verify() const { return exists("muted") && exists("deafen"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        nlohmann::json res;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Set the voice state of the user
            session->server()->voice_manager()->set_voice_state(session->user()->id,
                                                                _data["muted"], _data["deafen"]);

            return response_packet(packet_type::voice_state_set_packet).encode();
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