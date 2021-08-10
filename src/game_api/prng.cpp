#include "prng.hpp"

#include "state.hpp"

PRNG& PRNG::get()
{
    const auto& state = State::get();
    static PRNG* prng = (PRNG*)((size_t)state.ptr() - 0xb0);
    return *prng;
}
PRNG& PRNG::get_local()
{
    const auto& state = State::get();
    static PRNG* prng = (PRNG*)((size_t)state.ptr_local() - 0xb0);
    return *prng;
}

PRNG::prng_pair PRNG::get_and_advance(PRNG_CLASS type)
{
    prng_pair& pair = pairs[type];
    prng_pair copy = pair;

    const std::uint64_t lower = pair.first;
    const std::uint64_t upper = pair.second;

    const std::uint64_t rest = upper - lower;

    pair = {
        static_cast<std::uint64_t>(static_cast<std::int64_t>(upper) * -0x2c7cc17fb0b3a8b5),
        pair.second = rest * 0x8000000 | rest >> 0x25,
    };

    return copy;
}

std::int64_t PRNG::random_index(std::int64_t size, PRNG_CLASS type)
{
    prng_pair pair = get_and_advance(type);
    return static_cast<std::int64_t>((pair.first & 0xffffffff) * size >> 0x20);
}
std::optional<std::int64_t> PRNG::random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type)
{
    if (max <= min)
    {
        return std::nullopt;
    }

    static auto wrap = [](std::int64_t val, std::int64_t min, std::int64_t max)
    {
        const auto diff = max - min;

        if (val < min)
            val += diff * ((min - val) / diff + 1);

        return min + (val - min) % diff;
    };

    prng_pair pair = get_and_advance(type);

    // Technically not a uniform distribution, but we have 64bit to map to a range that is many orders of magnitude smaller
    // So in the grand scheme this is close enough to a uniform distribution
    return wrap(static_cast<std::int64_t>(pair.first), min, max);
}
bool PRNG::random_chance(std::int64_t inverse_chance, PRNG_CLASS type)
{
    return random_int(0, inverse_chance, type) == 0;
}

float PRNG::random(PRNG_CLASS type)
{
    prng_pair pair = get_and_advance(type);
    return static_cast<float>(pair.first) / static_cast<float>(std::numeric_limits<std::uint64_t>::max());
}
std::optional<std::int64_t> PRNG::random(std::int64_t i, PRNG_CLASS type)
{
    return random(1, i, type);
}
std::optional<std::int64_t> PRNG::random(std::int64_t min, std::int64_t max, PRNG_CLASS type)
{
    if (min <= max)
    {
        return random_int(min, max + 1, type);
    }
    return std::nullopt;
}