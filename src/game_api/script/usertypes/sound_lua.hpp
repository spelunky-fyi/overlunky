#pragma once

#include <sol/forward.hpp>

class ScriptImpl;
class SoundManager;

namespace NSound
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
