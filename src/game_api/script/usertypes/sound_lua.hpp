#pragma once

#include <sol/forward.hpp>

class SoundManager;

namespace NSound
{
void register_usertypes(sol::state& lua, SoundManager* sound_manager);
};
