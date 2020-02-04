#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"
#include "../user.h"

namespace quesync {
namespace packets {
class session_auth_packet : public serialized_packet {
   public:
    /// Default constructor.
    session_auth_packet() : session_auth_packet(""){};

    /**
     * Packet constructor.
     *
     * @param session_id The id of the session.
     */
    session_auth_packet(std::string session_id)
        : serialized_packet(packet_type::session_auth_packet) {
        _data["sessionId"] = session_id;
    };

    /**
     * Get the session id.
     *
     * @return The id of the session.
     */
    std::string session_id() { return _data["sessionId"]; }

    virtual bool verify() const { return exists("sessionId"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<user> user;

        // If the user is already authenticated, return error
        if (session->authenticated()) {
            return error_packet(error::already_authenticated).encode();
        }

        try {
            // Try to get the user id from the session id
            user = session->server()->user_manager()->authenticate_user_by_session(
                session->get_shared(), _data["sessionId"]);

            // Set the user in the client's session
            session->set_user(user);

            // Return autheticated packet with the user's info
            return response_packet(packet_type::authenticated_packet,
                                   nlohmann::json{{"user", *user}})
                .encode();
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