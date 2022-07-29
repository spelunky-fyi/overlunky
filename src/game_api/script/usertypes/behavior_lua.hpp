#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NBehavior
{
void register_usertypes(sol::state& lua);
};
