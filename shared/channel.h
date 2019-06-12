#pragma once
#include "serialized_object.h"

#include <ctime>

class Channel : public SerializedObject
{
public:
    Channel() : Channel("", (std::time_t)0){};

    Channel(std::string id, std::time_t created_at)
    {
        // Save all fields in the json type
        _json["id"] = id;
        _json["createdAt"] = created_at;
    };

    // Get functions for all the user's fields
    GET_FUNCTION(id, std::string)
    GET_FUNCTION(createdAt, std::time_t)
};