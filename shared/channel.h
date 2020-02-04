#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
struct channel {
    /// Default constructor.
    channel() : channel("", false, (std::time_t)0, false){};

    /**
     * Channel constructor.
     *
     * @param id The id of the channel.
     * @param is_private Is the channel a private channel.
     * @param created_at The date the channel was created on.
     * @param call_active Is a call active in the channel.
     */
    channel(std::string id, bool is_private, std::time_t created_at, bool call_active) {
        this->id = id;
        this->is_private = is_private;
        this->created_at = created_at;
        this->call_active = call_active;
    };

    /// The id of the channel.
    std::string id;

    /// Is the channel a private channel.
    bool is_private;

    /// The date the channel was created on.
    std::time_t created_at;

    /// Is a call active in the channel.
    bool call_active;
};

inline void to_json(nlohmann::json &j, const channel &c) {
    j = {{"id", c.id},
         {"isPrivate", c.is_private},
         {"createdAt", c.created_at},
         {"callActive", c.call_active}};
}

inline void from_json(const nlohmann::json &j, channel &c) {
    c = channel(j["id"], j["isPrivate"], j["createdAt"], j["callActive"]);
};
};  // namespace quesync
