#pragma once

#include <cstddef>  // for size_t
#include <cstdint>  // for int64_t, uint64_t
#include <optional> // for optional
#include <utility>  // for pair

struct PRNG
{
    PRNG() = delete;
    PRNG(const PRNG&) = delete;
    PRNG(PRNG&&) = delete;

    static PRNG& get();
    static PRNG& get_local();

    /// Same as `seed_prng`
    void seed(int64_t seed);

    enum PRNG_CLASS
    {
        PROCEDURAL_SPAWNS = 0,
        // ?? = 1,
        PARTICLES = 2,
        ENTITY_VARIATION = 3,
        // ?? = 4,
        EXTRA_SPAWNS = 5,
        // ?? = 6,
        // SOME_ENTITY_STUFF = 7,
        LEVEL_DECO = 8,
        // ?? = 9
    };

    using prng_pair = std::pair<std::uint64_t, std::uint64_t>;
    prng_pair get_and_advance(PRNG_CLASS type);

    std::pair<int64_t, int64_t> get_pair(size_t index);
    void set_pair(size_t index, int64_t first, int64_t second);

    /// Generate a random integer in the range `[0, size)`
    std::int64_t internal_random_index(std::int64_t size, PRNG_CLASS type);
    /// Generate a random integer in the range `[min, size)`, returns `nil` if `min == max`
    std::optional<std::int64_t> internal_random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type);

    /// Returns true with a chance of `1/inverse_chance`
    bool random_chance(std::int64_t inverse_chance, PRNG_CLASS type);

    /// Generate a random floating point number in the range `[0, 1)`
    float random_float(PRNG_CLASS type);
    /// Drop-in replacement for `math.random()`
    float random()
    {
        return random_float(static_cast<PRNG_CLASS>(7));
    }
    /// Generate a integer number in the range `[1, i]` or `nil` if `i < 1`
    std::optional<std::int64_t> random_index(std::int64_t i, PRNG_CLASS type);
    /// Drop-in replacement for `math.random(i)`
    std::optional<std::int64_t> random(std::int64_t i)
    {
        return random_index(i, static_cast<PRNG_CLASS>(7));
    }
    /// Generate a integer number in the range `[min, max]` or `nil` if `max < min`
    std::optional<std::int64_t> random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type);
    /// Drop-in replacement for `math.random(min, max)`
    std::optional<std::int64_t> random(std::int64_t min, std::int64_t max)
    {
        return random_int(min, max, static_cast<PRNG_CLASS>(7));
    }
    std::array<prng_pair, 10> pairs;
};
