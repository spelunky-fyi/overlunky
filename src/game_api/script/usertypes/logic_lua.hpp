#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NLogic
{
void register_usertypes(sol::state& lua);
};
