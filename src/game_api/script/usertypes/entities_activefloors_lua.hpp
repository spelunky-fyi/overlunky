#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesActiveFloors
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
