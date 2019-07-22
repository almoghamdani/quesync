#pragma once
#include "../event.h"

#include <ctime>

class IncomingCallEvent : public Event
{
public:
    IncomingCallEvent() : Event(INCOMING_CALL_EVENT)
    {
    }

    IncomingCallEvent(std::string channel_id) : Event(INCOMING_CALL_EVENT)
    {
        _json["channelId"] = channel_id;
    }

    GET_FUNCTION(channelId, std::string)
};