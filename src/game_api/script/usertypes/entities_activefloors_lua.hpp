#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NEntitiesActiveFloors
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
