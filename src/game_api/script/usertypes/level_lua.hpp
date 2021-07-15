#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

using ROOM_TEMPLATE = uint16_t;     // NoAlias
using PROCEDURAL_CHANCE = uint32_t; // NoAlias

struct PostRoomGenerationContext
{
    /// Set the room template at the given index and layer, returns `false` if the index is outside of the level.
    bool set_room_template(int x, int y, int l, ROOM_TEMPLATE room_template);
    /// Force a spawn chance for this level, has the same restrictions as specifying the spawn chance in the .lvl file.
    /// Note that the actual chance to spawn is `1/inverse_chance` and that is also slightly skewed because of technical reasons.
    /// Returns `false` if the given chance is not defined.
    bool set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, uint32_t inverse_chance);
};

namespace NLevel
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
