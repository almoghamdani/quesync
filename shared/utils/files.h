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
    static std::unordered_map<unsigned long long, quesync::file_chunk> get_file_chunks(
        std::ifstream& file_stream, unsigned long long file_size);
    static unsigned long long calc_amount_of_chunks(unsigned long long file_size);
    static void update_next_index(std::shared_ptr<quesync::memory_file> file);
};
};  // namespace utils
};  // namespace quesync