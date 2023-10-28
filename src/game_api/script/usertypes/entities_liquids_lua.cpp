#include "entities_liquids_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, basic_table_core...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_liquids.hpp" // for Liquid, Lava, Lava::emitted_light
#include "entity.hpp"           // for Entity
#include "illumination.hpp"     // IWYU pragma: keep

namespace NEntitiesLiquids
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_liquid"] = &Entity::as<Liquid>;
    lua["Entity"]["as_lava"] = &Entity::as<Lava>;

    lua.new_usertype<Liquid>(
        "Liquid",
        "fx_surface",
        &Liquid::fx_surface,
        "get_liquid_flags",
        &Liquid::get_liquid_flags,
        "set_liquid_flags",
        &Liquid::set_liquid_flags,
        sol::base_classes,
        sol::bases<Entity>());

    lua.new_usertype<Lava>(
        "Lava",
        "emitted_light",
        &Lava::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Liquid>());
}
} // namespace NEntitiesLiquids
