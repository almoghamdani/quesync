#pragma once
#include "../serialized_packet.h"

#include "error_packet.h"
#include "../response_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class login_packet : public serialized_packet {
   public:
    login_packet() : login_packet("", ""){};

    login_packet(std::string username, std::string password)
        : serialized_packet(packet_type::login_packet) {
        _data["username"] = username;
        _data["password"] = password;
    };

    virtual bool verify() const { return exists("username") && exists("password"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<user> user;
        std::string session_id;

        // If the user is already authenticated, return error
        if (session->authenticated()) {
            return error_packet(error::already_authenticated).encode();
        }

        try {
            // Authenticate the user, if failed an exception will be thrown
            user = session->server()->user_manager()->authenticate_user(
                session->get_shared(), _data["username"], _data["password"]);

            // Set the user in the client's session
            session->set_user(user);

            // Create a session for the user
            session_id =
                session->server()->session_manager()->create_session(session->get_shared());

            // Return autheticated packet with the user's info
            return response_packet(
                       packet_type::authenticated_packet,
                       nlohmann::json{{"user", *user}, {"sessionId", session_id}}.dump())
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