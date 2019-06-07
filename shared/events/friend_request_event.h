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
        _json["requester_id"] = requester_id;
		_json["sent_at"] = sent_at;
    }

    GET_FUNCTION(requester_id, std::string)
	GET_FUNCTION(sent_at, std::time_t)
};