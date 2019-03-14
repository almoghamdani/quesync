#pragma once
#include "serialized_object.h"

class User : public SerializedObject
{
  public:
    User() : User("", "", "", "", 0){};

    User(std::string id, std::string username, std::string email, std::string nickname, int tag)
    {
        // Save all fields in the json type
        _json["id"] = id;
        _json["username"] = username;
        _json["email"] = email;
        _json["nickname"] = nickname;
        _json["tag"] = tag;
    };

    // Get functions for all the user's fields
    GET_FUNCTION(id, std::string)
    GET_FUNCTION(username, std::string)
    GET_FUNCTION(email, std::string)
    GET_FUNCTION(nickname, std::string)
    GET_FUNCTION(tag, std::string)
};