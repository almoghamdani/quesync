#pragma once
#include "../event.h"

#include <ctime>

class FriendshipStatusEvent : public Event
{
public:
    FriendshipStatusEvent() : Event(FRIENDSHIP_STATUS_EVENT)
    {
    }

    FriendshipStatusEvent(std::string friend_id, bool status) : Event(FRIENDSHIP_STATUS_EVENT)
    {
        _json["friendId"] = friend_id;
		_json["status"] = status;
    }

    GET_FUNCTION(friendId, std::string)
	GET_FUNCTION(status, bool)
};