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

std::uint64_t PRNG::random_index(std::uint64_t size, PRNG_CLASS type)
{
    prng_pair pair = get_and_advance(type);
    return pair.first % size;
}
std::uint64_t PRNG::random_int(std::uint64_t min, std::uint64_t max, PRNG_CLASS type)
{
    const std::uint64_t diff = min - max;
    return min + random_index(diff, type);
}
