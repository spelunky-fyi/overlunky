#pragma once

#include <sol/forward.hpp>

class ScriptImpl;

namespace NTexture
{
    void register_usertypes(sol::state& lua, ScriptImpl* script);
};
