#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NEntity
{
void register_usertypes(sol::state& lua);
};
