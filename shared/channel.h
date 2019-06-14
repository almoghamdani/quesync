#pragma once
#include "serialized_object.h"

#include <ctime>

class Channel : public SerializedObject
{
public:
    Channel() : Channel("", false, (std::time_t)0){};

    Channel(std::string id, bool is_private, std::time_t created_at)
    {
        // Save all fields in the json type
        _json["id"] = id;
        _json["isPrivate"] = is_private;
        _json["createdAt"] = created_at;
    };

    GET_FUNCTION(id, std::string)
    GET_FUNCTION(isPrivate, bool)
    GET_FUNCTION(createdAt, std::time_t)
};