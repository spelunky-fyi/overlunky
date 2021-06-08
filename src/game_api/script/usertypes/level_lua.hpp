#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NLevel
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
