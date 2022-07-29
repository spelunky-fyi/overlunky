#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NEntityFlags
{
void register_usertypes(sol::state& lua);
};
