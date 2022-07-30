#include "prng_lua.hpp"

#include <algorithm>   // for max
#include <cstdint>     // for int64_t
#include <new>         // for operator new
#include <optional>    // for optional
#include <sol/sol.hpp> // for global_table, proxy_key_t, state, overload, call
#include <string>      // for allocator, operator==
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max, get

#include "prng.hpp" // for PRNG, PRNG::ENTITY_VARIATION, PRNG::EXTRA_SPAWNS

namespace NPRNG
{
void register_usertypes(sol::state& lua)
{
    auto random = sol::overload(static_cast<float (PRNG::*)()>(&PRNG::random), static_cast<std::optional<std::int64_t> (PRNG::*)(std::int64_t)>(&PRNG::random), static_cast<std::optional<std::int64_t> (PRNG::*)(std::int64_t, std::int64_t)>(&PRNG::random));

    /// Seed the game prng.
    lua["seed_prng"] = [](int64_t seed)
    {
        PRNG::get().seed(seed);
    };

    /// PRNG (short for Pseudo-Random-Number-Generator) holds 10 128bit wide buffers of memory that are mutated on every generation of a random number.
    /// The game uses specific buffers for specific scenarios, for example the third buffer is used every time particles are spawned to determine a random velocity.
    /// The used buffer is determined by [PRNG_CLASS](#PRNG_CLASS). If you want to make a mod that does not affect level generation but still uses the prng then you want to stay away from specific buffers.
    /// If you don't care what part of the game you affect just use `prng.random`.
    lua.new_usertype<PRNG>(
        "PRNG",
        "seed",
        &PRNG::seed,
        "random_float",
        &PRNG::random_float,
        "random_chance",
        &PRNG::random_chance,
        "random_index",
        &PRNG::random_index,
        "random_int",
        &PRNG::random_int,
        "random",
        random,
        "get_pair",
        &PRNG::get_pair,
        "set_pair",
        &PRNG::set_pair);

    /// The global prng state, calling any function on it will advance the prng state, thus desynchronizing clients if it does not happen on both clients.
    lua["prng"] = &PRNG::get();

    /// Determines what class of prng is used, which in turn determines which parts of the game's future prng is affected. See more info at [PRNG](#PRNG)
    /// For example when choosing `PRNG_CLASS.PROCEDURAL_SPAWNS` to generate a random number, random Tiamat spawns will not be affected.
    /// Any integer in the range [0, 9] is a valid class, some are however not documented because of missing information.
    lua.create_named_table("PRNG_CLASS", "PROCEDURAL_SPAWNS", PRNG::PROCEDURAL_SPAWNS, "PARTICLES", PRNG::PARTICLES, "ENTITY_VARIATION", PRNG::ENTITY_VARIATION, "EXTRA_SPAWNS", PRNG::EXTRA_SPAWNS, "LEVEL_DECO", PRNG::LEVEL_DECO);
}
} // namespace NPRNG
