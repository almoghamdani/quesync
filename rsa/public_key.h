#include <bigint.h>

namespace rsa
{
    class PublicKey
    {
    public:
        PublicKey(bigint p, bigint q);

    private:
        bigint _n;
        bigint _e;
    };
};