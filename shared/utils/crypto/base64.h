#pragma once

#include <string>

namespace quesync {
namespace utils {
namespace crypto {
class base64 {
   public:
    /**
     * Encode data using base64.
     *
     * @param data The raw data to be encoded.
     * @return The data encoded.
     */
    static std::string encode(std::string data);

    /**
     * Decode data using base64.
     *
     * @param data The data to be decoded.
     * @return The raw data decoded.
     */
    static std::string decode(std::string data);
};
};  // namespace crypto
};  // namespace utils
};  // namespace quesync