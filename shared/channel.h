#pragma once
#include "serialized_object.h"

class Channel : public SerializedObject
{
public:
    Channel() : Channel("", "", ""){};

    Channel(std::string id, std::string participant_1, std::string participant_2)
    {
        // Save all fields in the json type
        _json["id"] = id;
        _json["participant1"] = participant_1;
        _json["participant2"] = participant_1;
    };

    // Get functions for all the user's fields
    GET_FUNCTION(id, std::string)
    GET_FUNCTION(participant_1, std::string)
    GET_FUNCTION(participant_2, std::string)
};