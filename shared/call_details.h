#pragma once

#include <nlohmann/json.hpp>
#include <unordered_map>

#include "call.h"
#include "voice_state.h"

namespace quesync {
struct call_details {
    call_details() : call_details(quesync::call(), {}){};

    call_details(call call, std::unordered_map<std::string, voice::state> voice_states) {
        this->call = call;
        this->voice_states = voice_states;
    };

    call call;
    std::unordered_map<std::string, voice::state> voice_states;
};

inline void to_json(nlohmann::json &j, const call_details &c) {
    j = {{"call", c.call}, {"voiceStates", c.voice_states}};
}

inline void from_json(const nlohmann::json &j, call_details &c) {
    c = call_details(j["call"], j["voiceStates"]);
};
};  // namespace quesync
