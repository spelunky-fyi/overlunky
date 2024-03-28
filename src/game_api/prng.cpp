#include "prng.hpp"

#include <limits> // for numeric_limits

#include "state.hpp" // for State

PRNG& PRNG::get_main()
{
    static PRNG* prng = &State::get_main()->prng;
    return *prng;
}
PRNG& PRNG::get_local()
{
    return State::get()->prng;
}

void PRNG::seed(int64_t seed)
{
    auto next_pair = [useed = static_cast<uint64_t>(seed)]() mutable
    {
        // advance state
        useed = (uint64_t((useed & 0xffffffff) == 0) - (useed & 0xffffffff)) * -0x61939c2f98956567;
        useed = (((useed >> 0x1c) ^ useed) >> 0x17) ^ useed;

        // generate next pair
        PRNG::prng_pair useed_pair;
        useed_pair.first = useed * -0x61939c2f98956567;
        useed_pair.second = (useed * -0x7cc4ab2b38000000 | useed_pair.first >> 0x25) * -0x61939c2f98956567;
        useed_pair.first = (useed_pair.first >> 0x1c ^ useed_pair.first) >> 0x17 ^ useed_pair.first;
        return useed_pair;
    };

    for (auto& pair : pairs)
    {
        pair = next_pair();
    }
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

std::int64_t PRNG::internal_random_index(std::int64_t size, PRNG_CLASS type)
{
    prng_pair pair = get_and_advance(type);
    return static_cast<std::int64_t>((pair.first & 0xffffffff) * size >> 0x20);
}
std::optional<std::int64_t> PRNG::internal_random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type)
{
    if (max <= min)
    {
        return std::nullopt;
    }

    static auto wrap = [](std::int64_t val, std::int64_t _min, std::int64_t _max)
    {
        const auto diff = _max - _min;

        if (val < _min)
            val += diff * ((_min - val) / diff + 1);

        return _min + (val - _min) % diff;
    };

    prng_pair pair = get_and_advance(type);

    // Technically not a uniform distribution, but we have 64bit to map to a range that is many orders of magnitude smaller
    // So in the grand scheme this is close enough to a uniform distribution
    return wrap(static_cast<std::int64_t>(pair.first), min, max);
}

bool PRNG::random_chance(std::int64_t inverse_chance, PRNG_CLASS type)
{
    if (inverse_chance <= 0ll)
        return false;
    if (inverse_chance == 1)
        return true;
    return random_int(0, inverse_chance - 1, type) == 0;
}

float PRNG::random_float(PRNG_CLASS type)
{
    prng_pair pair = get_and_advance(type);
    return static_cast<float>(pair.first) / static_cast<float>(std::numeric_limits<std::uint64_t>::max());
}
std::optional<std::int64_t> PRNG::random_index(std::int64_t i, PRNG_CLASS type)
{
    return random_int(1, i, type);
}
std::optional<std::int64_t> PRNG::random_int(std::int64_t min, std::int64_t max, PRNG_CLASS type)
{
    if (min <= max)
    {
        return internal_random_int(min, max + 1, type);
    }
    return std::nullopt;
}
std::pair<int64_t, int64_t> PRNG::get_pair(PRNG_CLASS type)
{
    if (type >= 0 && type <= 9)
    {
        return pairs[type];
    }
    return {0, 0};
}
void PRNG::set_pair(PRNG_CLASS type, int64_t first, int64_t second)
{
    if (type >= 0 && type <= 9)
    {
        pairs[type].first = first;
        pairs[type].second = second;
    }
}
