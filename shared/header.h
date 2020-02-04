#pragma once

#include <cstdint>

namespace quesync {
struct header {
    /// The version of the Quesync protocol.
    uint32_t version;

    /// The size of the packet.
    uint32_t size;
};
};  // namespace quesync
