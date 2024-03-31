#pragma once

#include <cstddef>  // for size_t
#include <cstdint>  // for int64_t, uint64_t
#include <limits>   // for numeric_limits
#include <optional> // for optional
#include <utility>  // for pair

struct PRNG
{
    PRNG() = delete;
    PRNG(const PRNG&) = delete;
    PRNG(PRNG&&) = delete;

    static PRNG& get_main();
    static PRNG& get_local();

    /// Same as `seed_prng`
    void seed(int64_t seed);

    enum PRNG_CLASS
    {
        PROCEDURAL_SPAWNS = 0,
        CHAR_AI = 1,
        PARTICLES = 2,
        ENTITY_VARIATION = 3,
        // ?? = 4,
        EXTRA_SPAWNS = 5, // also AI
        // ?? = 6, // somehow related to spawning characters, cause it advances
        LIQUID = 7,
        LEVEL_DECO = 8,
        FX = 9
    };

    using prng_pair = std::pair<std::uint64_t, std::uint64_t>;
    prng_pair get_and_advance(PRNG_CLASS type);

    std::pair<int64_t, int64_t> get_pair(PRNG_CLASS type) const
    {
        if (type >= 0 && type <= 9)
        {
            return pairs[type];
        }
        return {0, 0};
    }
    void set_pair(PRNG_CLASS type, int64_t first, int64_t second)
    {
        if (type >= 0 && type <= 9)
        {
            pairs[type].first = first;
            pairs[type].second = second;
        }
    }

    /// Generate a random integer in the range `[0, size)`
    std::int64_t internal_random_index(std::int64_t size, PRNG_CLASS type)
    {
        prng_pair pair = get_and_advance(type);
        return static_cast<std::int64_t>((pair.first & 0xffffffff) * size >> 0x20);
    }
    /// Returns true with a chance of `1/inverse_chance`
    bool random_chance(std::int64_t inverse_chance, PRNG_CLASS type)
    {
        // TODO: even thou those first checks just return, shouldn't it still advance the given pair?
        if (inverse_chance <= 0ll)
            return false;
        if (inverse_chance == 1)
            return true;
        return internal_random_int(0, inverse_chance, type) == 0;
    }

    /// Generate a random floating point number in the range `[0, 1)`
    float random_float(PRNG_CLASS type)
    {
        prng_pair pair = get_and_advance(type);
        // TODO: shouldn't this cast to double and then cast result to float?
        return static_cast<float>(pair.first) / static_cast<float>(std::numeric_limits<std::uint64_t>::max());
    }
    /// Drop-in replacement for `math.random()`
    float random()
    {
        return random_float(static_cast<PRNG_CLASS>(7));
    }
    /// Generate a integer number in the range `[1, i]` or `nil` if `i < 1`
    std::optional<std::int64_t> random_index(std::int64_t i, PRNG_CLASS type)
    {
        if (i < 1)
            return std::nullopt;

        return internal_random_int(1, i + 1, type).value();
    }
    /// Drop-in replacement for `math.random(i)`
    std::optional<std::int64_t> random(std::int64_t i)
    {
        return random_index(i, static_cast<PRNG_CLASS>(7));
    }
    /// Generate a integer number in the range `[min, max]`
    std::int64_t random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type)
    {
        if (max < min)
            std::swap(min, max);

        return internal_random_int(min, max + 1, type).value();
    }
    /// Drop-in replacement for `math.random(min, max)`
    std::int64_t random(std::int64_t min, std::int64_t max)
    {
        return random_int(min, max, static_cast<PRNG_CLASS>(7));
    }

  private:
    /// Generate a random integer in the range `[min, size]`, returns `nil` if `min <= size`
    std::optional<std::int64_t> internal_random_int(std::int64_t min, std::int64_t size, PRNG_CLASS type);

    std::array<prng_pair, 10> pairs;
};
