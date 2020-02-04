#pragma once
#include "../event.h"

#include <ctime>

namespace quesync {
namespace events {
struct friend_request_event : public event {
    /// Default constructor.
    friend_request_event() : event(event_type::friend_request_event) {}

    /**
     * Event constructor.
     *
     * @param requester_id The id of the requester.
     * @param sent_at The date the friend request was sent on.
     */
    friend_request_event(std::string requester_id, std::time_t sent_at)
        : event(event_type::friend_request_event) {
        this->requester_id = requester_id;
        this->sent_at = sent_at;
    }

    virtual nlohmann::json encode() const {
        return {{"eventType", type}, {"requesterId", requester_id}, {"sentAt", sent_at}};
    }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        requester_id = j["requesterId"];
        sent_at = (int)j["sentAt"];
    }

    /// The id of the requester.
    std::string requester_id;

    /// The date the friend request was sent on.
    std::time_t sent_at;
};
};  // namespace events
};  // namespace quesync