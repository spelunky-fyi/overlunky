#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesMonsters
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
