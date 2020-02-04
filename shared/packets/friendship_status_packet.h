#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class friendship_status_packet : public serialized_packet {
   public:
    /// Default constructor.
    friendship_status_packet() : friendship_status_packet("", false){};

    /**
     * Packet constructor.
     *
     * @param friend_id The id of the friend.
     * @param status The new friendship status.
     */
    friendship_status_packet(std::string friend_id, bool status)
        : serialized_packet(packet_type::friendship_status_packet) {
        _data["friendId"] = friend_id;
        _data["status"] = status;
    };

    virtual bool verify() const { return exists("friendId") && exists("status"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        // If the friend id is the user's id, return error
        if (session->user()->id == _data["friendId"]) {
            return error_packet(error::self_friend_request).encode();
        }

        try {
            // Set the friendship status, if failed an exception will be thrown
            session->server()->user_manager()->set_friendship_status(
                session->user()->id, _data["friendId"], _data["status"]);

            // Return confirmation for the friendship status
            return response_packet(packet_type::friendship_status_set_packet).encode();
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
