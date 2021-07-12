#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesLiquids
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
