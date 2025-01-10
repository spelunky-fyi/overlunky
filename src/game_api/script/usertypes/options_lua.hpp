#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NOptions
{
void register_usertypes(sol::state& lua);
};
