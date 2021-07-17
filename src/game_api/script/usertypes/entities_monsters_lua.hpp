#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NEntitiesMonsters
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
