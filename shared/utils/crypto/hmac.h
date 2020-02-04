#pragma once

#include <string>

#define HMAC_KEY_SIZE (160 / 8)

namespace quesync {
namespace utils {
namespace crypto {
/**
 * Calcualte HMAC for data.
 *
 * @param data The raw data.
 * @param key The HMAC key.
 * @return The HMAC for the data.
 */
std::string hmac(std::string data, unsigned char *key);
};  // namespace crypto
};  // namespace utils
};  // namespace quesync