#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NBucket
{
void register_usertypes(sol::state& lua);
};
