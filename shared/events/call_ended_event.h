#pragma once
#include "../event.h"

namespace quesync {
namespace events {
struct call_ended_event : public event {
    /// Default constructor.
    call_ended_event() : event(event_type::call_ended_event) {}

    /**
     * Event constructor.
     *
     * @param channel_id The id of the channel.
     */
    call_ended_event(std::string channel_id) : event(event_type::call_ended_event) {
        this->channel_id = channel_id;
    }

    virtual nlohmann::json encode() const {
        return {{"eventType", type}, {"channelId", channel_id}};
    }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        channel_id = j["channelId"];
    }

    /// The id of the channel.
    std::string channel_id;
};
};  // namespace events
};  // namespace quesync
