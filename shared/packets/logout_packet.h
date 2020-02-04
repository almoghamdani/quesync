#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class logout_packet : public serialized_packet {
   public:
    /// Default constructor.
    logout_packet() : serialized_packet(packet_type::logout_packet){};

    virtual bool verify() const { return true; };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<user> user;
        std::string session_id;

        // If the user is not authenticated, return error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Destroy the user's session
            session->server()->session_manager()->destroy_session(session);

            // Return logged out packet
            return response_packet(packet_type::logged_out_packet).encode();
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