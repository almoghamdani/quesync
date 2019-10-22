#pragma once

#include <nlohmann/json.hpp>

namespace quesync {
struct profile {
    profile() : profile("", "", 0, ""){};

    profile(std::string id, std::string nickname, int tag, std::string photo) {
        this->id = id;
        this->nickname = nickname;
        this->tag = tag;
        this->photo = photo;
    };

    std::string id;
    std::string nickname;
    int tag;
    std::string photo;
};

inline void to_json(nlohmann::json &j, const profile &p) {
    j = {{"id", p.id}, {"nickname", p.nickname}, {"tag", p.tag}, {"photo", p.photo}};
}

inline void from_json(const nlohmann::json &j, profile &p) {
    p = profile(j["id"], j["nickname"], j["tag"], j["photo"]);
};
};  // namespace quesync
