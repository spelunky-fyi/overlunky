#pragma once

#include <cstdint>

struct PRNG
{
    static PRNG& get();
    static PRNG& get_local();

    enum PRNG_CLASS
    {
        LEVEL_GEN = 0
    };

    std::size_t random_index(std::size_t size, PRNG_CLASS type);

    using prng_pair = std::pair<std::uint64_t, std::uint64_t>;
    prng_pair pairs[11];
};
