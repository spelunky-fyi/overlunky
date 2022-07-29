#pragma once

namespace sol
{
class state;
} // namespace sol

void require_json_lua(sol::state& lua);
void require_inspect_lua(sol::state& lua);
void require_format_lua(sol::state& lua);
void require_serpent_lua(sol::state& lua);
