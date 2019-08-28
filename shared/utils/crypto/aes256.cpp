#include "aes256.h"

#include <openssl/evp.h>
#include <memory>

std::string quesync::utils::crypto::aes256::encrypt(std::string data, unsigned char *key,
                                                    unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;

    int offset = 0, len = 0, total = 0;
    std::shared_ptr<unsigned char> encrypted(new unsigned char[data.length() + 16]);

    // Allocate EVP context
    ctx = EVP_CIPHER_CTX_new();

    // Init EVP context
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    // Encrypt the data
    EVP_EncryptUpdate(ctx, encrypted.get(), &len, (unsigned char *)data.data(), (int)data.length());
    total += len;

    // Finalize the encryption
    EVP_EncryptFinal(ctx, encrypted.get() + total, &len);
    total += len;

    // Free the EVP context
    EVP_CIPHER_CTX_free(ctx);

    return std::string((char *)encrypted.get(), total);
}

std::string quesync::utils::crypto::aes256::decrypt(std::string data, unsigned char *key,
                                                    unsigned char *iv) {
    EVP_CIPHER_CTX *ctx;

    int offset = 0, len = 0, total = 0;
    std::shared_ptr<unsigned char> decrypted(new unsigned char[data.length()]);

    // Allocate EVP context
    ctx = EVP_CIPHER_CTX_new();

    // Init EVP context
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    // Decrypt the data
    EVP_DecryptUpdate(ctx, decrypted.get(), &len, (unsigned char *)data.data(), (int)data.length());
    total += len;

    // Finalize the decryption
    EVP_DecryptFinal(ctx, decrypted.get() + total, &len);
    total += len;

    // Free the EVP context
    EVP_CIPHER_CTX_free(ctx);

    return std::string((char *)decrypted.get(), total);
}