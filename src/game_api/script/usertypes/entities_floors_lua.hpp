#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NEntitiesFloors
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
