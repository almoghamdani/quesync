#pragma once
#include "serialized_object.h"

class User : public SerializedObject
{
public:
    User(std::string username, std::string email, std::string nickname, std::string id)
    {
        // Save all fields in the json type
        _json["username"] = username;
        _json["email"] = email;
        _json["nickname"] = nickname;
        _json["id"] = id;
    };

    // Get functions for all the user's fields
    GET_FUNCTION(username, std::string)
    GET_FUNCTION(email, std::string)
    GET_FUNCTION(nickname, std::string)
    GET_FUNCTION(id, std::string)
}