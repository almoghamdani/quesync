#include "rand.h"

#include <openssl/rand.h>
#include <random>

std::shared_ptr<unsigned char> quesync::utils::rand::bytes(unsigned int amount) {
    std::shared_ptr<unsigned char> buf(new unsigned char[amount]);

    // Generate random bytes
    RAND_bytes(buf.get(), amount);

    return buf;
}

int quesync::utils::rand::num(int min, int max) {
    std::random_device rd;                            // Obtain a random number from hardware
    std::mt19937 gen(rd());                           // Seed the generator
    std::uniform_int_distribution<> distr(min, max);  // Define the range

    // Get a random number from the generator using the distr
    return distr(gen);
}