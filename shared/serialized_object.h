#pragma once
#include <nlohmann/json.hpp>

// This macro can be used to define get function for the different fields of the sub-classes
#define GET_FUNCTION(param, type)  type param() \
                                        { \
                                            return _json[#param]; \
                                        }

class SerializedObject
{
public:
    std::string serialize() const
    {
        // Convert the json object to string
        return _json.dump();
    };

    void deserialize(std::string json)
    {
        // Parse the json serialized string to the class's json
        _json = nlohmann::json::parse(json);
    };

protected:
    nlohmann::json _json;
};