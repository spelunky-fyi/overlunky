#pragma once

#include <sol/forward.hpp>

class LuaBackend;
class SoundManager;

namespace NSound
{
void register_usertypes(sol::state& lua, LuaBackend* script);
};
