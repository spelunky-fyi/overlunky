#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NPRNG
{
void register_usertypes(sol::state& lua);
};
