#pragma once

#include <nlohmann/json.hpp>
#include <vector>

namespace quesync {
namespace client {
namespace voice {
struct sound_device {
    unsigned int id;
    std::string name;
    bool is_default;
    bool is_selected;
};

inline void to_json(nlohmann::json &j, const sound_device &d) {
    j = {{"id", d.id}, {"name", d.name}, {"isDefault", d.is_default}, {"isSelected", d.is_selected}};
};
};  // namespace voice
};  // namespace client
};  // namespace quesync