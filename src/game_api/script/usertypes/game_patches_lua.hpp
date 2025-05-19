#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NGamePatches
{
void register_usertypes(sol::state& lua);
};
