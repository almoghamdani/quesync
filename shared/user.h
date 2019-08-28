#pragma once

#include <nlohmann/json.hpp>

#include "friend_request.h"

namespace quesync {
struct user {
    user() : user("", "", "", "", 0, std::vector<std::string>(), std::vector<friend_request>()){};

    user(std::string id, std::string username, std::string email, std::string nickname, int tag,
         std::vector<std::string> friends, std::vector<friend_request> friend_requests) {
        this->id = id;
        this->username = username;
        this->email = email;
        this->nickname = nickname;
        this->tag = tag;
        this->friends = friends;
        this->friend_requests = friend_requests;
    };

    std::string id;
    std::string username;
    std::string email;
    std::string nickname;
    int tag;
    std::vector<std::string> friends;
    std::vector<friend_request> friend_requests;
};

inline void to_json(nlohmann::json &j, const user &u) {
    j = {{"id", u.id},
         {"username", u.username},
         {"email", u.email},
         {"nickname", u.nickname},
         {"tag", u.tag},
         {"friends", u.friends},
         {"friendRequests", u.friend_requests}};
}

inline void from_json(const nlohmann::json &j, user &u) {
    u = user(j["id"], j["username"], j["email"], j["nickname"], j["tag"], j["friends"],
             j["friendRequests"]);
};
};  // namespace quesync
