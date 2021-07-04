#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

using ROOM_CODE = uint16_t; // NoAlias

struct PostRoomGenerationContext
{
    void set_room_code(int x, int y, int l, ROOM_CODE room_code);
};

namespace NLevel
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
