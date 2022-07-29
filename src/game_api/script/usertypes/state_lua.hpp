#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NState
{
void register_usertypes(sol::state& lua);
};
