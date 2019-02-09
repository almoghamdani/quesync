#include "public_key.h"

rsa::PublicKey::PublicKey(bigint p, bigint q) : 
    _e(65537), // The 4th prime of Fermet primes
    _n(p * q) // The multiply of both primes
{
}