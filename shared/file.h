#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
struct file {
    file() : file("", "", "", 0, (std::time_t)0){};

    file(std::string id, std::string uploader_id, std::string name, unsigned long long size,
         std::time_t uploaded_at) {
        this->id = id;
        this->uploader_id = uploader_id;
        this->name = name;
        this->size = size;
        this->uploaded_at = uploaded_at;
    };

    std::string id;
    std::string uploader_id;
    std::string name;
    unsigned long long size;
    std::time_t uploaded_at;
};

inline void to_json(nlohmann::json &j, const file &f) {
    j = {{"id", f.id},
         {"uploaderId", f.uploader_id},
         {"name", f.name},
         {"size", f.size},
         {"uploadedAt", f.uploaded_at}};
}

inline void from_json(const nlohmann::json &j, file &f) {
    f = file(j["id"], j["uploaderId"], j["name"], j["size"], j["uploadedAt"]);
};
};  // namespace quesync
