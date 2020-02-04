#pragma once
#include "../event.h"

#include <ctime>

#include "../message.h"

namespace quesync {
namespace events {
struct message_event : public event {
    /// Default constructor.
    message_event() : event(event_type::message_event) {}

    /**
     * Event constructor.
     * 
     * @param message The message info.
     */
    message_event(message message) : event(event_type::message_event) { this->message = message; }

    virtual nlohmann::json encode() const { return {{"eventType", type}, {"message", message}}; }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        message = j["message"];
    }

    /// The message info.
    message message;
};
};  // namespace events
};  // namespace quesync