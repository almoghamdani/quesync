#pragma once

#include "utils/crypto/aes256.h"
#include "utils/crypto/hmac.h"

namespace quesync {
namespace voice {
struct header {
    unsigned char iv[AES_IV_SIZE];
    unsigned char hmac[HMAC_KEY_SIZE];
};
};  // namespace voice
};  // namespace quesync
