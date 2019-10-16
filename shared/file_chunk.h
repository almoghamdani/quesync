#pragma once

#include <memory>

#define FILE_CHUNK_SIZE 8192

namespace quesync {
struct file_chunk {
    file_chunk() : file_chunk(nullptr, 0){};

    file_chunk(std::shared_ptr<unsigned char> data, unsigned long long index) {
        this->data = data;
        this->index = index;
    };

    std::shared_ptr<unsigned char> data;
    unsigned long long index;
};
};  // namespace quesync
