#include "pbkdf2.h"

#include <openssl/evp.h>
#include <memory>

#include "../parser.h"
#include "base64.h"

std::string quesync::utils::crypto::pbkdf2::sha512(std::string password, unsigned char *salt) {
    std::shared_ptr<unsigned char> hash(new unsigned char[PBKDF2_HASH_SIZE]);

    // Hash using PBKDF2
    PKCS5_PBKDF2_HMAC(password.data(), password.length(), salt, PBKDF2_SALT_SIZE, PBKDF2_ITERATIONS,
                      EVP_sha512(), PBKDF2_HASH_SIZE, hash.get());

    // Encode the hash with base64 and combine it with the salt
    return base64::encode(std::string((char *)hash.get(), PBKDF2_HASH_SIZE)) + "$" +
           base64::encode(std::string((char *)salt, PBKDF2_SALT_SIZE));
}

bool quesync::utils::crypto::pbkdf2::sha512_compare(std::string password, std::string hash) {
    std::vector<std::string> tokens = parser::split(hash, '$');

    // Check that the hash has the hash and the salt
    if (tokens.size() != 2) {
        return false;
    }

    // Hash the given password and encode it using base64 and compare it with the saved hash
    return sha512(password, (unsigned char *)base64::decode(tokens[1]).data()) == hash;
}