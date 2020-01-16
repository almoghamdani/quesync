#include "base64.h"

#include <memory>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

std::string quesync::utils::crypto::base64::encode(std::string data) {
    BIO *bio, *b64;
    BUF_MEM *buf_ptr;

    std::string encoded;

    // Initialize base64 encoder
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Ignore newlines - write everything in one line
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // Encode the data
    BIO_write(bio, data.c_str(), (int)data.length());
    BIO_flush(bio);

    // Get the pointer to the data
    BIO_get_mem_ptr(bio, &buf_ptr);

    // Create an encoded string from it
    encoded = std::string(buf_ptr->data, buf_ptr->length);

    // Free BIO
    BIO_free_all(bio);

    return encoded;
}

std::string quesync::utils::crypto::base64::decode(std::string data) {
    auto calcDecodeLength = [](std::string &data) {
        int padding = 0;

        if (*(data.end() - 1) == '=' && *(data.end() - 2) == '=') {
            padding = 2;
        } else if (*(data.end() - 1) == '=') {
            padding = 1;
        }

        return (data.length() * 3) / 4 - padding;
    };

    BIO *bio, *b64;

    int decode_len = (int)calcDecodeLength(data), len = 0;
    
    std::shared_ptr<char> buf(new char[data.length()]);

    // Initialize base64 decoder
    bio = BIO_new_mem_buf(data.c_str(), (int)data.length());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    // Ignore newlines - write everything in one line
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // Decode the base64
    len = BIO_read(bio, buf.get(), (int)data.length());

    // Free BIO
    BIO_free_all(bio);

    return std::string(buf.get(), len);
}
