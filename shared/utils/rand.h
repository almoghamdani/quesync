#pragma once

#include <memory>

namespace quesync {
namespace utils {
class rand {
   public:
    /**
     * Generate a buffer with random bytes using OpenSSL.
     *
     * @param amount The amount of bytes to be randomzied.
     * @return A shared pointer to a buffer with the random bytes.
     */
    static std::shared_ptr<unsigned char> bytes(unsigned int amount);

    /**
     * Generate a random number.
     *
     * @param min The minimum value of the number.
     * @param max The maximum value of the number.
     * @return The random number.
     */
    static int num(int min, int max);
};
};  // namespace utils
};  // namespace quesync