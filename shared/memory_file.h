#pragma once

#include <unordered_map>

#include "file.h"
#include "file_chunk.h"

namespace quesync {
struct memory_file {
    /**
     * Memory file constructor.
     *
     * @param file The file info.
     */
    memory_file(quesync::file& file) : file(file) {}

    /// The file info.
    file file;

    /// The file's chunks.
    std::unordered_map<unsigned long long, file_chunk> chunks;

    /// The current index of the transmission.
    unsigned long long current_index = 0;
};
};  // namespace quesync