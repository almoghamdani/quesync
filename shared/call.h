#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
struct call {
    /// Default constructor.
    call() : call("", "", "", (std::time_t)0, (std::time_t)0, false){};

    /**
     * Call constructor.
     *
     * @param id The id of the call.
     * @param caller_id The id of the caller.
     * @param channel_id The id of the channel.
     * @param start_date The date the call started.
     * @param joined Did the user join the call.
     */
    call(std::string id, std::string caller_id, std::string channel_id, std::time_t start_date,
         bool joined)
        : call(id, caller_id, channel_id, start_date, (std::time_t)0, joined) {}

    /**
     * Call constructor.
     *
     * @param id The id of the call.
     * @param caller_id The id of the caller.
     * @param channel_id The id of the channel.
     * @param start_date The date the call started.
     * @param end_date The date the call ended.
     * @param joined Did the user join the call.
     */
    call(std::string id, std::string caller_id, std::string channel_id, std::time_t start_date,
         std::time_t end_date, bool joined) {
        this->id = id;
        this->caller_id = caller_id;
        this->channel_id = channel_id;
        this->start_date = start_date;
        this->end_date = end_date;
        this->joined = joined;
    };

    /// The id of the call.
    std::string id;

    /// The id of the caller.
    std::string caller_id;

    /// The id of the channel.
    std::string channel_id;

    /// The date the call started.
    std::time_t start_date;

    /// The date the call ended.
    std::time_t end_date;

    /// Did the user join the call.
    bool joined;
};

inline void to_json(nlohmann::json &j, const call &c) {
    j = {{"id", c.id},
         {"callerId", c.caller_id},
         {"channelId", c.channel_id},
         {"startDate", c.start_date},
         {"endDate", c.end_date},
         {"joined", c.joined}};
}

inline void from_json(const nlohmann::json &j, call &c) {
    c = call(j["id"], j["callerId"], j["channelId"], j["startDate"], j["endDate"], j["joined"]);
};
};  // namespace quesync
