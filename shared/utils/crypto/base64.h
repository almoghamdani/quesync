#pragma once

#include <string>

namespace quesync {
namespace utils {
namespace crypto {
class base64 {
   public:
    static std::string encode(std::string data);
    static std::string decode(std::string data);
};
};  // namespace crypto
};  // namespace utils
};  // namespace quesync