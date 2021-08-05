#include "prng_lua.hpp"

#include "prng.hpp"

#include <sol/sol.hpp>

namespace NPRNG
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<PRNG>(
        "PRNG",
        "random_index",
        &PRNG::random_index,
        "random_int",
        &PRNG::random_int);

    /// The global prng state, calling any function on it will advance the prng state, thus desynchronizing clients if it does not happen on both clients.
    lua["prng"] = &PRNG::get();

    /// Determines what class of prng is used, for example when choosing `PRNG_CLASS.LEVEL_GEN` to generate a random number random Tiamat spawns will not be affected.
    lua.create_named_table("PRNG_CLASS", "LEVEL_GEN", PRNG::LEVEL_GEN);
}
} // namespace NPRNG
