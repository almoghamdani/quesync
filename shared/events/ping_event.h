#pragma once
#include "../event.h"

namespace quesync {
namespace events {
struct ping_event : public event {
    ping_event() : event(event_type::ping_event) {}

    ping_event(int ms) : event(event_type::ping_event) { this->ms = ms; }

    virtual nlohmann::json encode() const { return {{"eventType", type}, {"ms", ms}}; }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        ms = j["ms"];
    }

    int ms;
};
};  // namespace events
};  // namespace quesync
