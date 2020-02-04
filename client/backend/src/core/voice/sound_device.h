#pragma once
#include <nlohmann/json.hpp>
#include <vector>

namespace quesync {
namespace client {
namespace voice {
struct sound_device {
    /// The id of the sound device.
    unsigned int id;

    /// The name of the sound device.
    std::string name;

    /// Is the sound device the default device.
    bool is_default;

    /// Is the sound device is currently selected sound device.
    bool is_selected;
};

inline void to_json(nlohmann::json &j, const sound_device &d) {
    j = {
        {"id", d.id}, {"name", d.name}, {"isDefault", d.is_default}, {"isSelected", d.is_selected}};
};
};  // namespace voice
};  // namespace client
};  // namespace quesync