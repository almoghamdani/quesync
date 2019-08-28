#include "hmac.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>

std::string quesync::utils::crypto::hmac(std::string data, unsigned char *key) {
    unsigned int len = 0;
    std::shared_ptr<unsigned char> hmac(new unsigned char[EVP_MAX_MD_SIZE]);

    // Calculate HMAC
    HMAC(EVP_sha1(), key, HMAC_KEY_SIZE, (unsigned char *)data.data(), data.length(), hmac.get(),
         &len);

    return std::string((char *)hmac.get(), len);
}