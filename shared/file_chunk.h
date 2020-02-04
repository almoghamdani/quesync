#pragma once

#include <memory>

#define FILE_CHUNK_SIZE 8192

namespace quesync {
struct file_chunk {
    /// Default constructor.
    file_chunk() : file_chunk(nullptr, 0){};

    /**
     * File chunk constructor.
     *
     * @param data A shared pointer to the data of the file chunk.
     * @param index The index of the file chunk.
     */
    file_chunk(std::shared_ptr<unsigned char> data, unsigned long long index) {
        this->data = data;
        this->index = index;
    };

    /// A shared pointer to the data of the file chunk.
    std::shared_ptr<unsigned char> data;

    /// The index of the file chunk.
    unsigned long long index;
};
};  // namespace quesync
