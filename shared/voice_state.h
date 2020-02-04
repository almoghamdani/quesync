#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
namespace voice {
enum class state_type { disconnected, connected, pending };

class state {
   public:
    /// Default constructor.
    state() : state(state_type::pending, false, false) {}

    /**
     * Voice state constructor.
     *
     * @param voice_state The current state type.
     * @param muted The mute status of the input device.
     * @param deafen The mute status of the output device.
     */
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

    /**
     * Get the voice state type.
     *
     * @return The voice state type.
     */
    state_type voice_state() const { return _voice_state; }

    /**
     * Get the last change time.
     *
     * @return The last change time.
     */
    std::time_t change_time() const { return _state_change_time; }

    /**
     * Get if the voice input is muted.
     *
     * @return The mute status of the input device.
     */
    bool muted() const { return _muted; }

    /**
     * Mute the input device.
     */
    void mute() { _muted = true; }

    /**
     * Unmute the input device.
     */
    void unmute() { _muted = false; }

    /**
     * Get if the voice output is muted.
     *
     * @return The mute status of the output device.
     */
    bool deafen() const { return _deafen; }

    /**
     * Mute the output device.
     */
    void deaf() { _deafen = true; }

    /**
     * Unmute the output device.
     */
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