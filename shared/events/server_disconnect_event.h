#pragma once
#include "../event.h"

#include "../error.h"

namespace quesync {
namespace events {
struct server_disconnect_event : public event {
    server_disconnect_event() : event(event_type::server_disconnect_event) {}

    virtual nlohmann::json encode() const { return {{"eventType", type}}; }

    virtual void decode(nlohmann::json j) { type = j["eventType"]; }
};
};  // namespace events
};  // namespace quesync
