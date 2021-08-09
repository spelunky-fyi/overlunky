#include "drops_lua.hpp"

#include "drops.hpp"
#include "rpc.hpp"

#include <sol/sol.hpp>

namespace NDrops
{
void register_usertypes(sol::state& lua)
{
    /// Alters the drop chance for the provided monster-item combination (use e.g. set_drop_chance(DROPCHANCE.MOLE_MATTOCK, 10) for a 1 in 10 chance)
    lua["set_drop_chance"] = set_drop_chance;
    /// Changes a particular drop, e.g. what Van Horsing throws at you (use e.g. replace_drop(DROP.VAN_HORSING_DIAMOND, ENT_TYPE.ITEM_PLASMACANNON))
    lua["replace_drop"] = replace_drop;

    lua.create_named_table("DROPCHANCE"
                           //, "BONEBLOCK_SKELETONKEY", 0
                           //, "", ...see__drops.hpp__for__a__list__of__possible__dropchances...
                           //, "YETI_PITCHERSMITT", 10
    );
    for (size_t x = 0; x < dropchance_entries.size(); ++x)
    {
        lua["DROPCHANCE"][dropchance_entries.at(x).caption] = x;
    }

    lua.create_named_table("DROP"
                           //, "ALTAR_DICE_CLIMBINGGLOVES", 0
                           //, "", ...see__drops.hpp__for__a__list__of__possible__drops...
                           //, "YETI_PITCHERSMITT", 85
    );
    for (size_t x = 0; x < drop_entries.size(); ++x)
    {
        lua["DROP"][drop_entries.at(x).caption] = x;
    }
}
}; // namespace NDrops
