#pragma once

#include <nlohmann/json.hpp>

#include "event_type.h"

namespace quesync {
struct event {
    /// Default constructor.
    event() : event((event_type)0) {}

    /**
     * Event constructor.
     *
     * @param type The type of the event.
     */
    event(event_type type) { this->type = type; }

    /**
     * Encode the event.
     *
     * @return The event data encoded.
     */
    virtual nlohmann::json encode() const { return {{"eventType", type}}; }

    /**
     * Decode the event from json.
     *
     * @param j The json to decode the event from.
     */
    virtual void decode(nlohmann::json j) { type = (event_type)j["eventType"].get<int>(); };

    /// The type of the event.
    event_type type;
};

inline void to_json(nlohmann::json &j, const event &e) { j = e.encode(); }

inline void from_json(const nlohmann::json &j, event &e) { e.decode(j); };
};  // namespace quesync
