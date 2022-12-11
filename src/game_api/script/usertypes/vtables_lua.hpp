#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NVTables
{
void register_usertypes(sol::state& lua);
};
