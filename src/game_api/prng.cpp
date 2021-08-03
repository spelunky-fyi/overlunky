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

std::size_t PRNG::random_index(std::size_t size, PRNG_CLASS type)
{
    prng_pair& pair = pairs[type];

    const std::uint64_t lower = pair.first;
    const std::uint64_t upper = pair.second;

    const std::uint64_t rest = upper - lower;

    pair = {
        static_cast<std::uint64_t>(static_cast<std::int64_t>(upper) * -0x2c7cc17fb0b3a8b5),
        pair.second = rest * 0x8000000 | rest >> 0x25,
    };

    return (lower & 0xffffffff) * size >> 0x20;
}
