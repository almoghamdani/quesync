#pragma once
#include <ctime>

typedef enum QuesyncVoiceState
{
	DISCONNECTED,
	CONNECTED,
	PENDING
} QuesyncVoiceState;

class VoiceState
{
public:
	VoiceState() : VoiceState(PENDING, false, false) {}

	VoiceState(QuesyncVoiceState voice_state, bool muted, bool deafen)
		: _voice_state(voice_state),
		  _state_change_time(std::time(nullptr)),
		  _muted(muted),
		  _deafen(deafen)
	{
	}

	VoiceState &operator=(QuesyncVoiceState voice_state)
	{
		_voice_state = voice_state;
		_state_change_time = std::time(nullptr);

		return *this;
	}

	bool operator==(const QuesyncVoiceState &voice_state)
	{
		return _voice_state == voice_state;
	}

	QuesyncVoiceState voice_state() const
	{
		return _voice_state;
	}

	std::time_t change_time() const
	{
		return _state_change_time;
	}

	bool muted() const
	{
		return _muted;
	}

	void mute() { _muted = true; }
	void unmute() { _muted = false; }

	bool deafen() const
	{
		return _deafen;
	}

	void deaf() { _deafen = true; }
	void undeaf() { _deafen = false; }

private:
	QuesyncVoiceState _voice_state;
	std::time_t _state_change_time;

	bool _muted;
	bool _deafen;
};

inline void to_json(nlohmann::json &j, const VoiceState &voice_state)
{
	j = nlohmann::json{{"state", voice_state.voice_state()}, {"muted", voice_state.muted()}, {"deafen", voice_state.deafen()}};
}

inline void from_json(const nlohmann::json &j, VoiceState &voice_state)
{
	voice_state = VoiceState(j["state"], j["muted"], j["deafen"]);
}