#pragma once
#include "../event.h"

#include <ctime>

class FriendRequestEvent : public Event
{
public:
    FriendRequestEvent() : Event(FRIEND_REQUEST_EVENT)
    {
    }

    FriendRequestEvent(std::string requester_id, std::time_t sent_at) : Event(FRIEND_REQUEST_EVENT)
    {
        _json["requesterId"] = requester_id;
		_json["sentAt"] = sent_at;
    }

    GET_FUNCTION(requesterId, std::string)
	GET_FUNCTION(sentAt, std::time_t)
};