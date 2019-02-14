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

protected:
    nlohmann::json _json;
};