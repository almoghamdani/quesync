#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
namespace voice {
enum class state_type { disconnected, connected, pending };

class state {
   public:
    state() : state(state_type::pending, false, false) {}

    state(state_type voice_state, bool muted, bool deafen)
        : _voice_state(voice_state),
          _state_change_time(std::time(nullptr)),
          _muted(muted),
          _deafen(deafen) {}

    state &operator=(state_type voice_state) {
        _voice_state = voice_state;
        _state_change_time = std::time(nullptr);

        return *this;
    }

    bool operator==(const state_type &voice_state) { return _voice_state == voice_state; }

    state_type voice_state() const { return _voice_state; }

    std::time_t change_time() const { return _state_change_time; }

    bool muted() const { return _muted; }

    void mute() { _muted = true; }
    void unmute() { _muted = false; }

    bool deafen() const { return _deafen; }

    void deaf() { _deafen = true; }
    void undeaf() { _deafen = false; }

   private:
    state_type _voice_state;
    std::time_t _state_change_time;

    bool _muted;
    bool _deafen;
};

inline void to_json(nlohmann::json &j, const state &voice_state) {
    j = nlohmann::json{{"state", (int)voice_state.voice_state()},
                       {"muted", voice_state.muted()},
                       {"deafen", voice_state.deafen()}};
}

inline void from_json(const nlohmann::json &j, state &voice_state) {
    voice_state = state(j["state"], j["muted"], j["deafen"]);
}
};  // namespace voice
};  // namespace quesync