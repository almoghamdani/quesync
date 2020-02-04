#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
struct message {
    /// Default constructor.
    message() : message("", "", "", "", "", (std::time_t)0){};

    /**
     * Message constructor.
     *
     * @param id The id of the message.
     * @param sender_id The id of the sender.
     * @param channel_id The id of the channel.
     * @param content The content of the message.
     * @param attachment_id The id of attachment to attach to the message. This can be null.
     * @param sent_at The date the message was sent on.
     */
    message(std::string id, std::string sender_id, std::string channel_id, std::string content,
            std::string attachment_id, std::time_t sent_at) {
        this->id = id;
        this->sender_id = sender_id;
        this->channel_id = channel_id;
        this->content = content;
        this->attachment_id = attachment_id;
        this->sent_at = sent_at;
    };

    /// The id of the message.
    std::string id;

    /// The id of the sender.
    std::string sender_id;

    /// The id of the channel.
    std::string channel_id;

    /// The content of the message.
    std::string content;

    /// The id of attachment to attach to the message. This can be null.
    std::string attachment_id;

    /// The date the message was sent on.
    std::time_t sent_at;
};

inline void to_json(nlohmann::json &j, const message &msg) {
    j = {{"id", msg.id},
         {"senderId", msg.sender_id},
         {"channelId", msg.channel_id},
         {"content", msg.content},
         {"attachmentId", msg.attachment_id},
         {"sentAt", msg.sent_at}};
}

inline void from_json(const nlohmann::json &j, message &msg) {
    msg = message(j["id"], j["senderId"], j["channelId"], j["content"], j["attachmentId"],
                  j["sentAt"]);
};
};  // namespace quesync
