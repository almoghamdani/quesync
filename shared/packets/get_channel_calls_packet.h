#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../call.h"
#include "../exception.h"

namespace quesync {
namespace packets {
class get_channel_calls_packet : public serialized_packet {
   public:
    get_channel_calls_packet() : get_channel_calls_packet("", 0, 0){};

    get_channel_calls_packet(std::string channel_id, unsigned int amount, unsigned int offset)
        : serialized_packet(packet_type::get_channel_calls_packet) {
        _data["channelId"] = channel_id;
        _data["amount"] = amount;
        _data["offset"] = offset;
    };

    virtual bool verify() const {
        return exists("channelId") && exists("amount") && exists("offset");
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::vector<call> calls;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Try to get the calls of the channel
            calls = session->server()->voice_manager()->get_channel_calls(
                session->get_shared(), _data["channelId"], _data["amount"], _data["offset"]);

            // Return response packet with the calls
            return response_packet(packet_type::channel_calls_packet, (nlohmann::json)calls)
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
