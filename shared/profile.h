#pragma once
#include "serialized_object.h"

class Profile : public SerializedObject
{
  public:
    Profile() : Profile("", "", 0){};

    Profile(std::string id, std::string nickname, int tag)
    {
        // Save all fields in the json type
        _json["id"] = id;
        _json["nickname"] = nickname;
        _json["tag"] = tag;
    };

    // Get functions for all the user's fields
    GET_FUNCTION(id, std::string)
    GET_FUNCTION(nickname, std::string)
    GET_FUNCTION(tag, std::string)
};