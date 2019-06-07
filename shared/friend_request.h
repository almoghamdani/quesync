#pragma once
#include "serialized_object.h"

#include <ctime>

class FriendRequest : public SerializedObject
{
public:
	FriendRequest() : FriendRequest("", "", (std::time_t)0){};

	FriendRequest(std::string friend_id, std::string friend_type, std::time_t sent_at)
	{
		// Save all fields in the json type
		_json["friendId"] = friend_id;
		_json["friendType"] = friend_type;
		_json["sentAt"] = sent_at;
	};

	// Get functions for all the user's fields
	GET_FUNCTION(friend_id, std::string)
	GET_FUNCTION(friend_type, std::string)
	GET_FUNCTION(sent_at, std::time_t)
};

inline void to_json(nlohmann::json &j, const FriendRequest &friend_req)
{
	j = friend_req.json();
}

inline void from_json(const nlohmann::json &j, FriendRequest &friend_req)
{
	friend_req = FriendRequest(j.at("friendId").get<std::string>(), j.at("friendType").get<std::string>(), j.at("sentAt").get<std::time_t>());
}