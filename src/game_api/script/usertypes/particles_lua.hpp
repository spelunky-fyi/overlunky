#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NParticles
{
void register_usertypes(sol::state& lua);
};
