#pragma once

#include <nlohmann/json.hpp>
#include <ctime>

namespace quesync {
struct friend_request {
    /// Default constructor.
    friend_request() : friend_request("", "", (std::time_t)0){};

    /**
     * Friend request constructor.
     * 
     * @param friend_id The id of the friend.
     * @param friend_type The type of friend.
     * @param sent_at The date the friend request was sent at.
     */
    friend_request(std::string friend_id, std::string friend_type, std::time_t sent_at) {
        this->friend_id = friend_id;
        this->friend_type = friend_type;
        this->sent_at = sent_at;
    };

    /// The id of the friend.
    std::string friend_id;

    /// The type of friend.
    std::string friend_type;

    /// The date the friend request was sent at.
    std::time_t sent_at;
};

inline void to_json(nlohmann::json &j, const friend_request &friend_req) {
    j = {{"friendId", friend_req.friend_id},
         {"friendType", friend_req.friend_type},
         {"sentAt", friend_req.sent_at}};
}

inline void from_json(const nlohmann::json &j, friend_request &friend_req) {
    friend_req = friend_request(j["friendId"], j["friendType"], j["sentAt"]);
}
};  // namespace quesync