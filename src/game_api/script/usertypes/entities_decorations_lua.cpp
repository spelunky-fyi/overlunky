#include "entities_decorations_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, basic_table_...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_decorations.hpp" // for PalaceSign, CrossBeam, DecoRegen...
#include "entity.hpp"               // for Entity
#include "state_structs.hpp"        // IWYU pragma: keep

namespace NEntitiesDecorations
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_crossbeam"] = &Entity::as<CrossBeam>;
    lua["Entity"]["as_destructiblebg"] = &Entity::as<DestructibleBG>;
    lua["Entity"]["as_palacesign"] = &Entity::as<PalaceSign>;
    lua["Entity"]["as_decoregeneratingblock"] = &Entity::as<DecoRegeneratingBlock>;

    lua.new_usertype<CrossBeam>(
        "CrossBeam",
        "attached_to_side_uid",
        &CrossBeam::attached_to_side_uid,
        "attached_to_top_uid",
        &CrossBeam::attached_to_top_uid,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DestructibleBG>(
        "DestructibleBG",
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<PalaceSign>(
        "PalaceSign",
        "illumination",
        &PalaceSign::illumination,
        "arrow_illumination",
        &PalaceSign::arrow_illumination,
        "arrow_change_timer",
        &PalaceSign::arrow_change_timer,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<DecoRegeneratingBlock>(
        "DecoRegeneratingBlock",
        sol::base_classes,
        sol::bases<Entity>());
}
}; // namespace NEntitiesDecorations
