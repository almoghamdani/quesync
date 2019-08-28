#pragma once

#include <string>

#define PBKDF2_SALT_SIZE (128 / 8)
#define PBKDF2_ITERATIONS 15000
#define PBKDF2_HASH_SIZE (512 / 8)

namespace quesync {
namespace utils {
namespace crypto {
class pbkdf2 {
   public:
    static std::string sha512(std::string password, unsigned char *salt);
    static bool sha512_compare(std::string password, std::string hash);
};
};  // namespace crypto
};  // namespace utils
};  // namespace quesync