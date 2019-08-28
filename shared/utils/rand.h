#pragma once

#include <memory>

namespace quesync {
namespace utils {
class rand {
   public:
    static std::shared_ptr<unsigned char> bytes(unsigned int amount);
    static int num(int min, int max);
};
};  // namespace utils
};  // namespace quesync