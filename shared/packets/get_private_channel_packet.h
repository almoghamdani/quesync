#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../channel.h"
#include "../exception.h"

namespace quesync {
namespace packets {
class get_private_channel_packet : public serialized_packet {
   public:
    /// Default constructor.
    get_private_channel_packet() : get_private_channel_packet(""){};

    /**
     * Packet constructor.
     *
     * @param user_id The id of the user.
     */
    get_private_channel_packet(std::string user_id)
        : serialized_packet(packet_type::get_private_channel_packet) {
        _data["userId"] = user_id;
    };

    virtual bool verify() const { return exists("userId"); };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<channel> channel;

        nlohmann::json res;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Get the private channel of the 2 users, if failed an exception will be thrown
            channel = session->server()->channel_manager()->get_private_channel(
                session->get_shared(), _data["userId"]);
            res["channel"] = *channel;

            // Return response packet with the channel
            return response_packet(packet_type::private_channel_packet, res).encode();
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