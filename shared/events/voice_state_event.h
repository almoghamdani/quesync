#pragma once
#include "../event.h"

#include "../voice_state.h"

namespace quesync {
namespace events {
struct voice_state_event : public event {
    /// Default constructor.
    voice_state_event() : event(event_type::voice_state_event) {}

    /**
     * Event constructor.
     * 
     * @param user_id The id of the user.
     * @param voice_state The new voice state of the user.
     */
    voice_state_event(std::string user_id, quesync::voice::state voice_state)
        : event(event_type::voice_state_event) {
        this->user_id = user_id;
        this->voice_state = voice_state;
    }

    virtual nlohmann::json encode() const {
        return {{"eventType", type}, {"userId", user_id}, {"voiceState", voice_state}};
    }
    virtual void decode(nlohmann::json j) {
        type = j["eventType"];
        user_id = j["userId"];
        voice_state = (voice::state)j["voiceState"];
    }

    /// The id of the user.
    std::string user_id;

    /// The new voice state of the user.
    voice::state voice_state;
};
};  // namespace events
};  // namespace quesync