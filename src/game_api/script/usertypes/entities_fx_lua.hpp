#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesFX
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
