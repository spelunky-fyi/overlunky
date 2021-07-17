#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NEntity
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
