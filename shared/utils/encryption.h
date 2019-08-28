#pragma once

#include <string>

#include "../voice_header.h"
#include "crypto/aes256.h"
#include "crypto/base64.h"
#include "crypto/hmac.h"
#include "rand.h"

namespace quesync {
namespace utils {
class encryption {
   public:
    template <typename T>
    static std::string encrypt_voice_packet(T *packet, unsigned char *aes_key,
                                            unsigned char *hmac_key) {
        std::shared_ptr<unsigned char> iv = rand::bytes(AES_IV_SIZE);
        voice::header voice_header;

        std::string encrypted, hmac;

        // Encode the packet
        encrypted = crypto::aes256::encrypt(packet->encode(), aes_key, iv.get());

        // Calculate HMAC
        hmac = crypto::hmac(encrypted, hmac_key);

        // Format voice header
        memcpy(voice_header.iv, iv.get(), 16);
        memcpy(voice_header.hmac, hmac.c_str(), hmac.length());

        return std::string((char *)&voice_header, sizeof(voice::header)) + encrypted;
    }

    template <typename T>
    static std::shared_ptr<T> decrypt_voice_packet(std::string data, unsigned char *aes_key,
                                                   unsigned char *hmac_key) {
        std::shared_ptr<T> packet = nullptr;
        voice::header voice_header;

        std::string encrypted, decrypted, hmac;

        // Get the voice header
        memcpy(&voice_header, data.data(), sizeof(voice::header));

        // Get the encrypted packet
        encrypted = data.substr(sizeof(voice::header));

        // Calculate HMAC
        hmac = crypto::hmac(encrypted, hmac_key);

        // If the HMAC equals
        if (memcmp(hmac.data(), voice_header.hmac, HMAC_KEY_SIZE) == 0) {
            // Decrypt the packet
            decrypted = crypto::aes256::decrypt(encrypted, aes_key, voice_header.iv);

            // Decode the packet
            packet = std::make_shared<T>();
            if (!packet->decode(decrypted)) {
                packet = nullptr;
            }
        }

        return packet;
    }
};
};  // namespace utils
};  // namespace quesync