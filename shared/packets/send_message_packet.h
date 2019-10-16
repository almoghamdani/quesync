#pragma once
#include "../serialized_packet.h"

#include "../response_packet.h"
#include "error_packet.h"

#include "../exception.h"
#include "../message.h"

namespace quesync {
namespace packets {
class send_message_packet : public serialized_packet {
   public:
    send_message_packet() : send_message_packet("", "", ""){};

    send_message_packet(std::string content, std::string attachment_id, std::string channel_id)
        : serialized_packet(packet_type::send_message_packet) {
        _data["content"] = content;
        _data["attachmentId"] = attachment_id;
        _data["channelId"] = channel_id;
    };

    virtual bool verify() const {
        return exists("content") && exists("attachmentId") && exists("channelId");
    };

// A handle function for the server
#ifdef QUESYNC_SERVER
    virtual std::string handle(std::shared_ptr<server::session> session) {
        std::shared_ptr<message> message;

        // If the user is not authenticed, send error
        if (!session->authenticated()) {
            return error_packet(error::not_authenticated).encode();
        }

        try {
            // Try to send the message to the channel
            message = session->server()->message_manager()->send_message(
                session->get_shared(), _data["content"], _data["attachmentId"], _data["channelId"]);

            // Return response packet with the message id
            return response_packet(packet_type::message_id_packet,
                                   nlohmann::json{{"messageId", message->id}})
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