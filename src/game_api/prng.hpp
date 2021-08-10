#pragma once

#include <cstdint>
#include <optional>

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
    std::int64_t random_index(std::int64_t size, PRNG_CLASS type);
    /// Generate a random integer in the range `[min, size)`, returns `nil` if `min == max`
    std::optional<std::int64_t> random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type);
    /// Returns true with a chance of `1/inverse_chance`
    bool random_chance(std::int64_t inverse_chance, PRNG_CLASS type);

    /// Drop-in replacement for `math.random()`
    /// Generate a random floating point number in the range `[0, 1)`
    float random(PRNG_CLASS type);
    /// Drop-in replacement for `math.random(i)`
    /// Generate a integer number in the range `[1, i]` or `nil` if `i < 1`
    std::optional<std::int64_t> random(std::int64_t i, PRNG_CLASS type);
    /// Drop-in replacement for `math.random(min, max)`
    /// Generate a integer number in the range `[min, max]` or `nil` if `max < min`
    std::optional<std::int64_t> random(std::int64_t min, std::int64_t max, PRNG_CLASS type);

    prng_pair pairs[11];
};
