#pragma once
#include <nlohmann/json.hpp>

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