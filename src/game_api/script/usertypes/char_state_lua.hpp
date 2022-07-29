#pragma once

namespace sol
{
class state;
} // namespace sol

namespace NCharacterState
{
void register_usertypes(sol::state& lua);
};
