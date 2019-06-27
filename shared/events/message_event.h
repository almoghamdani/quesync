#pragma once
#include "../event.h"

#include <ctime>

#include "../message.h"

class MessageEvent : public Event
{
public:
    MessageEvent() : Event(MESSAGE_EVENT)
    {
    }

    MessageEvent(Message message) : Event(MESSAGE_EVENT)
    {
        _json["message"] = (nlohmann::json)message;
    }

    GET_FUNCTION(id, std::string)
	GET_FUNCTION(senderId, std::string)
	GET_FUNCTION(channelId, std::string)
	GET_FUNCTION(content, std::string)
	GET_FUNCTION(sentAt, std::time_t)
};