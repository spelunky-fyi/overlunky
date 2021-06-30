#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesMounts
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
