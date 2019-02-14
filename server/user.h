#pragma once
#include "serialized_object.h"

class User : public SerializedObject
{
public:
    User(std::string username)
    {
        // Save all fields in the json type
        _json["username"] = username;
    };

    std::string getUsername()
    {
        return _json["username"];
    };
}