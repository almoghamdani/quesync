#pragma once
#include "../event.h"

#include "../call.h"

namespace quesync {
namespace events {
struct incoming_call_event : public event {
    /// Default constructor.
    incoming_call_event() : event(event_type::incoming_call_event) {}

    /**
     * Event constructor.
     *
     * @param call The call info.
     */
    incoming_call_event(call call) : event(event_type::incoming_call_event) { this->call = call; }

    virtual nlohmann::json encode() const { return {{"eventType", type}, {"call", call}}; }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        call = j["call"];
    }

    /// The call info.
    call call;
};
};  // namespace events
};  // namespace quesync