#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NEntity
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
