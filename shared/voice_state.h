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
	VoiceState &operator=(QuesyncVoiceState voice_state)
	{
		_voice_state = voice_state;
		_state_change_time = std::time(nullptr);

		return *this;
	}

	bool operator==(const QuesyncVoiceState& voice_state)
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

private:
	QuesyncVoiceState _voice_state;
	std::time_t _state_change_time;
};

inline void to_json(nlohmann::json &j, const VoiceState &voice_state)
{
	j = voice_state.voice_state();
}

inline void from_json(const nlohmann::json &j, VoiceState &voice_state)
{
	voice_state = (QuesyncVoiceState)(int)j;
}