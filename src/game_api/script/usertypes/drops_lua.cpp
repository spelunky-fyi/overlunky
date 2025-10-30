#include "drops_lua.hpp"

#include <cstddef>     // for size_t
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, global_table, state, table_proxy
#include <tuple>       // for get
#include <type_traits> // for move
#include <utility>     // for max, min
#include <vector>      // for vector

#include "drops.hpp" // for drop_entries, dropchance_entries, replace_drop

namespace NDrops
{
void register_usertypes(sol::state& lua)
{
    /// Alters the drop chance for the provided monster-item combination (use e.g. set_drop_chance(DROPCHANCE.MOLE_MATTOCK, 10) for a 1 in 10 chance)
    /// Use `-1` as dropchance_id to reset all to default
    lua["set_drop_chance"] = set_drop_chance;
    /// Changes a particular drop, e.g. what Van Horsing throws at you (use e.g. replace_drop(DROP.VAN_HORSING_DIAMOND, ENT_TYPE.ITEM_PLASMACANNON))
    /// Use `0` as type to reset this drop to default, use `-1` as drop_id to reset all to default
    /// Check all the available drops [here](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/drops.cpp)
    lua["replace_drop"] = replace_drop;

    lua.create_named_table("DROPCHANCE"
                           //, "BONEBLOCK_SKELETONKEY", 0
                           //, "", ...see__[drops.cpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/drops.cpp\]__for__a__list__of__possible__dropchances...
                           //, "YETI_PITCHERSMITT", 10
    );
    for (size_t x = 0; x < dropchance_entries.size(); ++x)
    {
        lua["DROPCHANCE"][dropchance_entries.at(x).caption] = x;
    }

    lua.create_named_table("DROP"
                           //, "ALTAR_DICE_CLIMBINGGLOVES", 0
                           //, "", ...see__[drops.cpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/drops.cpp\]__for__a__list__of__possible__drops...
                           //, "YETI_PITCHERSMITT", 85
    );
    for (size_t x = 0; x < drop_entries.size(); ++x)
    {
        lua["DROP"][drop_entries.at(x).caption] = x;
    }
}
}; // namespace NDrops
