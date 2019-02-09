#include <random>

#include "random.h"

void rsa::Random::Generate(unsigned int len, uint32_t *value)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // Go through each int in the value array and randomize it
    for (int i = 0; i < len; i++)
    {
        // Generate a random number
        value[i] = gen();
    }
}