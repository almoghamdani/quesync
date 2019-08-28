#pragma once

#include <nlohmann/json.hpp>
#include <ctime>

namespace quesync {
struct message {
    message() : message("", "", "", "", (std::time_t)0){};

    message(std::string id, std::string sender_id, std::string channel_id, std::string content,
            std::time_t sent_at) {
        this->id = id;
        this->sender_id = sender_id;
        this->channel_id = channel_id;
        this->content = content;
        this->sent_at = sent_at;
    };

    std::string id;
    std::string sender_id;
    std::string channel_id;
    std::string content;
    std::time_t sent_at;
};

inline void to_json(nlohmann::json &j, const message &msg) {
    j = {{"id", msg.id},
         {"senderId", msg.sender_id},
         {"channelId", msg.channel_id},
         {"content", msg.content},
         {"sentAt", msg.sent_at}};
}

inline void from_json(const nlohmann::json &j, message &msg) {
    msg = message(j["id"], j["senderId"], j["channelId"], j["content"], j["sentAt"]);
};
};  // namespace quesync
