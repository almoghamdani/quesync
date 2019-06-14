#pragma once
#include "serialized_object.h"

#include "friend_request.h"

class User : public SerializedObject
{
public:
    User() : User("", "", "", "", 0, std::vector<std::string>(), std::vector<FriendRequest>()){};

    User(std::string id,
         std::string username,
         std::string email,
         std::string nickname,
         int tag,
         std::vector<std::string> friends,
         std::vector<FriendRequest> friend_requests)
    {
        // Save all fields in the json type
        _json["id"] = id;
        _json["username"] = username;
        _json["email"] = email;
        _json["nickname"] = nickname;
        _json["tag"] = tag;
        _json["friends"] = friends;
        _json["friendRequests"] = friend_requests;
    };

    GET_FUNCTION(id, std::string)
    GET_FUNCTION(username, std::string)
    GET_FUNCTION(email, std::string)
    GET_FUNCTION(nickname, std::string)
    GET_FUNCTION(tag, std::string)
    GET_FUNCTION(friends, std::vector<std::string>)
    GET_FUNCTION(friendRequests, std::vector<FriendRequest>)
};