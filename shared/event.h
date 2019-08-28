#pragma once

#include <nlohmann/json.hpp>

#include "event_type.h"

namespace quesync {
struct event {
    event() : event((event_type)0) {}

    event(event_type type) { this->type = type; }

    virtual nlohmann::json encode() const { return {{"eventType", type}}; }
    virtual void decode(nlohmann::json j) {
        type = (event_type)j["eventType"].get<int>();
    };

    event_type type;
};

inline void to_json(nlohmann::json &j, const event &e) { j = e.encode(); }

inline void from_json(const nlohmann::json &j, event &e) {
    e.decode(j);
};
};  // namespace quesync
