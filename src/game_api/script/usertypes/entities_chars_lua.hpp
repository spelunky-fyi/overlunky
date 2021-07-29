#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesChars
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
