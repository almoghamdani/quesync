#pragma once

#include <unordered_map>

#include "file.h"
#include "file_chunk.h"

namespace quesync {
struct memory_file {
    memory_file(quesync::file& file) : file(file) {}

    file file;
    std::unordered_map<unsigned long long, file_chunk> chunks;
    unsigned long long current_index = 0;
};
};  // namespace quesync