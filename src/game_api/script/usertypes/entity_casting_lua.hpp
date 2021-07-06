#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntityCasting
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
