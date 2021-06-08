#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NGui
{
    void register_usertypes(sol::state& lua, ScriptImpl* script);
};
