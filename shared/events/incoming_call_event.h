#pragma once
#include "../event.h"

namespace quesync {
namespace events {
struct incoming_call_event : public event {
    incoming_call_event() : event(event_type::incoming_call_event) {}

    incoming_call_event(std::string channel_id) : event(event_type::incoming_call_event) {
        this->channel_id = channel_id;
    }

    virtual nlohmann::json encode() const {
        return {{"eventType", type}, {"channelId", channel_id}};
    }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        channel_id = j["channelId"];
    }

    std::string channel_id;
};
};  // namespace events
};  // namespace quesync