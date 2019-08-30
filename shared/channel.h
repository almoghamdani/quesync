#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
struct channel {
    channel() : channel("", false, (std::time_t)0, false){};

    channel(std::string id, bool is_private, std::time_t created_at, bool call_active) {
        this->id = id;
        this->is_private = is_private;
        this->created_at = created_at;
        this->call_active = call_active;
    };

    std::string id;
    bool is_private;
    std::time_t created_at;
    bool call_active;
};

inline void to_json(nlohmann::json &j, const channel &c) {
    j = {{"id", c.id}, {"isPrivate", c.is_private}, {"createdAt", c.created_at}, {"callActive", c.call_active}};
}

inline void from_json(const nlohmann::json &j, channel &c) {
    c = channel(j["id"], j["isPrivate"], j["createdAt"], j["callActive"]);
};
};  // namespace quesync
