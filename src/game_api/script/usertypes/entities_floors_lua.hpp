#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntitiesFloors
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
