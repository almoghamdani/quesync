#pragma once

#include <string>

#define AES_KEY_SIZE (256 / 8)
#define AES_IV_SIZE (128 / 8)

namespace quesync {
namespace utils {
namespace crypto {
class aes256 {
   public:
    /**
     * Encrypt data using AES256.
     *
     * @param data The raw data to be encrypted.
     * @param key The key to be used for encryption.
     * @param iv The initialization vector to be used for encryption.
     * @return The data encrypted.
     */
    static std::string encrypt(std::string data, unsigned char *key, unsigned char *iv);

    /**
     * Decrypt data using AES256.
     *
     * @param data The data to be decrypted.
     * @param key The key to be used for decryption.
     * @param iv The initialization vector to be used for decryption.
     * @return The raw data decrypted.
     */
    static std::string decrypt(std::string data, unsigned char *key, unsigned char *iv);
};
};  // namespace crypto
};  // namespace utils
};  // namespace quesync