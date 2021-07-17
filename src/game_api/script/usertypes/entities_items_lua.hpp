#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NEntitiesItems
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
