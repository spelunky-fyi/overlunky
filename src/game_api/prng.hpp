#pragma once

#include <cstdint>

struct PRNG
{
    PRNG() = delete;
    PRNG(const PRNG&) = delete;
    PRNG(PRNG&&) = delete;

    static PRNG& get();
    static PRNG& get_local();

    enum PRNG_CLASS
    {
        LEVEL_GEN = 0
    };

    using prng_pair = std::pair<std::uint64_t, std::uint64_t>;
    prng_pair get_and_advance(PRNG_CLASS type);

    /// Generate a random integer in the range `[0, size)`
    std::uint64_t random_index(std::uint64_t size, PRNG_CLASS type);
    /// Generate a random integer in the range `[min, size)`
    std::uint64_t random_int(std::uint64_t min, std::uint64_t max, PRNG_CLASS type);

    prng_pair pairs[11];
};
