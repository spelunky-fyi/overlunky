#pragma once

class SoundManager;
namespace sol
{
class state;
} // namespace sol

namespace NSound
{
void register_usertypes(sol::state& lua, SoundManager* sound_manager);
};
