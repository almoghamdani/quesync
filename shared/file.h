#pragma once

#include <ctime>
#include <nlohmann/json.hpp>

namespace quesync {
struct file {
    /// Default constructor.
    file() : file("", "", "", 0, (std::time_t)0){};

    /**
     * File constructor.
     *
     * @param id The id of the file.
     * @param uploader_id The id of the uploader.
     * @param name The name of the file.
     * @param size The size of the file.
     * @param uploaded_at The date the file was uploaded.
     */
    file(std::string id, std::string uploader_id, std::string name, unsigned long long size,
         std::time_t uploaded_at) {
        this->id = id;
        this->uploader_id = uploader_id;
        this->name = name;
        this->size = size;
        this->uploaded_at = uploaded_at;
    };

    /// The id of the file.
    std::string id;

    /// The id of the uploader.
    std::string uploader_id;

    /// The name of the file.
    std::string name;

    /// The size of the file.
    unsigned long long size;

    /// The date the file was uploaded.
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
