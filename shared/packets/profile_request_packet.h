#pragma once
#include "../serialized_packet.h"

#include "error_packet.h"
#include "../response_packet.h"

#include "../exception.h"
#include "../profile.h"

namespace quesync {
namespace packets {
class profile_request_packet : public serialized_packet {
   public:
    profile_request_packet() : profile_request_packet(""){};

    profile_request_packet(std::string user_id)
        : serialized_packet(packet_type::profile_request_packet) {
        _data["userId"] = user_id;
    };

    virtual bool verify() const { return exists("userId"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<profile> profile;

        // If the user isn't authenticated, throw error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Try get the profile of the requested user
            profile = std::shared_ptr<quesync::profile>(
                session->server()->user_manager()->get_user_profile(_data["userId"]));

            // Return the profile of the user
            return response_packet(packet_type::profile_packet, *profile).encode();
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