#pragma once
#include "../event.h"

#include <ctime>

class CallEndedEvent : public Event
{
public:
    CallEndedEvent() : Event(CALL_ENDED_EVENT)
    {
    }

    CallEndedEvent(std::string channel_id) : Event(CALL_ENDED_EVENT)
    {
        _json["channelId"] = channel_id;
    }

    GET_FUNCTION(channelId, std::string)
};