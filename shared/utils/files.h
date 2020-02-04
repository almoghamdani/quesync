#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>

#include "../file_chunk.h"
#include "../memory_file.h"

namespace quesync {
namespace utils {
class files {
   public:
    /**
     * Get a file's chunks.
     *
     * @param file_stream The stream of the file to read from.
     * @param file_size The size of the file.
     * @return A map containing the file chunks.
     */
    static std::unordered_map<unsigned long long, quesync::file_chunk> get_file_chunks(
        std::ifstream& file_stream, unsigned long long file_size);

    /**
     * Calculates the amount of chunks needed for a file.
     *
     * @param file_size The size of the file.
     * @return The amount of chunks needed for the file.
     */
    static unsigned long long calc_amount_of_chunks(unsigned long long file_size);
};
};  // namespace utils
};  // namespace quesync