#pragma once
#include "event_packet.h"

class FriendRequestEventPacket : public EventPacket
{
  public:
    FriendRequestEventPacket(std::string requester_id) : EventPacket(FRIEND_REQUEST_EVENT, nlohmann::json{{"requester_id", requester_id}})
    {
    }
};