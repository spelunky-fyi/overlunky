#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesItems
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
