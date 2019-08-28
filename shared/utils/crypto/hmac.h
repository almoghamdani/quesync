#pragma once

#include <string>

#define HMAC_KEY_SIZE (160 / 8)

namespace quesync {
namespace utils {
namespace crypto {
std::string hmac(std::string data, unsigned char *key);
};  // namespace crypto
};  // namespace utils
};  // namespace quesync