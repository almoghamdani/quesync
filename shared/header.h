#pragma once

#include <cstdint>

namespace quesync {
struct header {
    uint32_t version;
    uint32_t size;
};
};  // namespace quesync
