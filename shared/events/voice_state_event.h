#pragma once
#include "../event.h"

#include <ctime>

#include "../voice_state.h"

class VoiceStateEvent : public Event
{
public:
    VoiceStateEvent() : Event(VOICE_STATE_EVENT)
    {
    }

    VoiceStateEvent(std::string user_id, QuesyncVoiceState voice_state) : Event(VOICE_STATE_EVENT)
    {
        _json["userId"] = user_id;
		_json["voiceState"] = voice_state;
    }

    GET_FUNCTION(userId, std::string)
	GET_FUNCTION(voiceState, QuesyncVoiceState)
};