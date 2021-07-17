#pragma once

#include <sol/forward.hpp>

class LuaBackend;

namespace NTexture
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
