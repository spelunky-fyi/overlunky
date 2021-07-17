#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NEntityCasting
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
