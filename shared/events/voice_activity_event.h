#pragma once
#include "../event.h"

#include <unordered_map>

namespace quesync {
namespace events {
struct voice_activity_event : public event {
    voice_activity_event() : event(event_type::voice_activity_event) {}

    voice_activity_event(std::unordered_map<std::string, bool> changed_voice_activity)
        : event(event_type::voice_activity_event) {
        this->changed_voice_activity = changed_voice_activity;
    }

    virtual nlohmann::json encode() const {
        return {{"eventType", type}, {"changedActivity", changed_voice_activity}};
    }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        changed_voice_activity = j["changedActivity"].get<std::unordered_map<std::string, bool>>();
    }

    std::unordered_map<std::string, bool> changed_voice_activity;
};
};  // namespace events
};  // namespace quesync
