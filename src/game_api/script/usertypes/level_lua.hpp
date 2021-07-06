#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

using ROOM_TEMPLATE = uint16_t; // NoAlias

struct PostRoomGenerationContext
{
    void set_room_template(int x, int y, int l, ROOM_TEMPLATE room_template);
};

namespace NLevel
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
