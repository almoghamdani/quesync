#pragma once

#include <nlohmann/json.hpp>

namespace quesync {
struct profile {
    /// Default constructor.
    profile() : profile("", "", 0, ""){};

    /**
     * Profile constructor.
     *
     * @param id The id of the user.
     * @param nickname The nickname of the user.
     * @param tag The tag of the user.
     * @param photo The photo of the user in base64.
     */
    profile(std::string id, std::string nickname, int tag, std::string photo) {
        this->id = id;
        this->nickname = nickname;
        this->tag = tag;
        this->photo = photo;
    };

    /// The id of the user.
    std::string id;

    /// The nickname of the user.
    std::string nickname;

    /// The tag of the user.
    int tag;

    /// The photo of the user in base64.
    std::string photo;
};

inline void to_json(nlohmann::json &j, const profile &p) {
    j = {{"id", p.id}, {"nickname", p.nickname}, {"tag", p.tag}, {"photo", p.photo}};
}

inline void from_json(const nlohmann::json &j, profile &p) {
    p = profile(j["id"], j["nickname"], j["tag"], j["photo"]);
};
};  // namespace quesync
