#include "prng_lua.hpp"

#include "prng.hpp"

#include <sol/sol.hpp>

namespace NPRNG
{
void register_usertypes(sol::state& lua)
{
    auto random = sol::overload(static_cast<float (PRNG::*)()>(&PRNG::random), static_cast<std::optional<std::int64_t> (PRNG::*)(std::int64_t)>(&PRNG::random), static_cast<std::optional<std::int64_t> (PRNG::*)(std::int64_t, std::int64_t)>(&PRNG::random), static_cast<float (PRNG::*)(PRNG::PRNG_CLASS)>(&PRNG::random), static_cast<std::optional<std::int64_t> (PRNG::*)(std::int64_t, PRNG::PRNG_CLASS)>(&PRNG::random), static_cast<std::optional<std::int64_t> (PRNG::*)(std::int64_t, std::int64_t, PRNG::PRNG_CLASS)>(&PRNG::random));
    lua.new_usertype<PRNG>(
        "PRNG",
        "random_int",
        &PRNG::random_int,
        "random_chance",
        &PRNG::random_chance,
        "random",
        random);

    /// The global prng state, calling any function on it will advance the prng state, thus desynchronizing clients if it does not happen on both clients.
    lua["prng"] = &PRNG::get();

    /// Determines what class of prng is used, for example when choosing `PRNG_CLASS.LEVEL_GEN` to generate a random number random Tiamat spawns will not be affected.
    lua.create_named_table("PRNG_CLASS", "PROCEDURAL_SPAWNS", PRNG::PROCEDURAL_SPAWNS, "PARTICLES", PRNG::PARTICLES, "ENTITY_VARIATION", PRNG::ENTITY_VARIATION, "EXTRA_SPAWNS", PRNG::EXTRA_SPAWNS, "LEVEL_DECO", PRNG::LEVEL_DECO);
}
} // namespace NPRNG
