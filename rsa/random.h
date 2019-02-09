#include <stdint.h>

namespace rsa
{
    class Random
    {
        static void Generate(unsigned int len, uint32_t *value);
    };
};