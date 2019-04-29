#pragma once
#include "../event.h"

class FriendRequestEvent : public Event
{
public:
    FriendRequestEvent() : Event(FRIEND_REQUEST_EVENT)
    {
    }

    FriendRequestEvent(std::string requester_id) : Event(FRIEND_REQUEST_EVENT)
    {
        _json["requester_id"] = requester_id;
    }

    GET_FUNCTION(requester_id, std::string)
};