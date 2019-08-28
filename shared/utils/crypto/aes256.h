#pragma once

#include <string>

#define AES_KEY_SIZE (256 / 8)
#define AES_IV_SIZE (128 / 8)

namespace quesync {
namespace utils {
namespace crypto {
class aes256 {
   public:
    static std::string encrypt(std::string data, unsigned char *key, unsigned char *iv);
    static std::string decrypt(std::string data, unsigned char *key, unsigned char *iv);
};
};  // namespace crypto
};  // namespace utils
};  // namespace quesync