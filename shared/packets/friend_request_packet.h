#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"

namespace quesync {
namespace packets {
class friend_request_packet : public serialized_packet {
   public:
    /// Default constructor.
    friend_request_packet() : friend_request_packet(""){};

    /**
     * Packet constructor.
     *
     * @param recipient_id The id of the recipient.
     */
    friend_request_packet(std::string recipient_id)
        : serialized_packet(packet_type::friend_request_packet) {
        _data["recipientId"] = recipient_id;
    };

    virtual bool verify() const { return exists("recipientId"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        // If the recipient id is the user's id, return error
        if (session->user()->id == _data["recipientId"]) {
            return error_packet(error::self_friend_request).encode();
        }

        try {
            // Send the user a friend request, if failed an exception will be thrown
            session->server()->user_manager()->send_friend_request(session->user()->id,
                                                                   _data["recipientId"]);

            // Return confirmation for the friend request
            return response_packet(packet_type::friend_request_sent_packet).encode();
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
