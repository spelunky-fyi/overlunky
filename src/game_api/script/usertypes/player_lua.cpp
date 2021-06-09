#include "player_lua.hpp"

#include "state_structs.hpp"

#include <sol/sol.hpp>

namespace NPlayer
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<PlayerSlotSettings>(
        "PlayerSlotSettings",
        "controller_vibration",
        sol::readonly(&PlayerSlotSettings::controller_vibration),
        "auto_run_enabled",
        sol::readonly(&PlayerSlotSettings::auto_run_enabled),
        "controller_right_stick",
        sol::readonly(&PlayerSlotSettings::controller_right_stick));
    lua.new_usertype<PlayerSlot>(
        "PlayerSlot",
        "buttons_gameplay",
        sol::readonly(&PlayerSlot::buttons_gameplay),
        "buttons",
        sol::readonly(&PlayerSlot::buttons),
        "input_mapping_keyboard",
        sol::readonly(&PlayerSlot::input_mapping_keyboard),
        "input_mapping_controller",
        sol::readonly(&PlayerSlot::input_mapping_controller),
        "player_id",
        sol::readonly(&PlayerSlot::player_id),
        "is_participating",
        sol::readonly(&PlayerSlot::is_participating));
    lua.new_usertype<InputMapping>(
        "InputMapping",
        "jump",
        sol::readonly(&InputMapping::jump),
        "attack",
        sol::readonly(&InputMapping::attack),
        "bomb",
        sol::readonly(&InputMapping::bomb),
        "rope",
        sol::readonly(&InputMapping::rope),
        "walk_run",
        sol::readonly(&InputMapping::walk_run),
        "use_door_buy",
        sol::readonly(&InputMapping::use_door_buy),
        "pause_menu",
        sol::readonly(&InputMapping::pause_menu),
        "journal",
        sol::readonly(&InputMapping::journal),
        "left",
        sol::readonly(&InputMapping::left),
        "right",
        sol::readonly(&InputMapping::right),
        "up",
        sol::readonly(&InputMapping::up),
        "down",
        sol::readonly(&InputMapping::down));
    lua.new_usertype<PlayerInputs>(
        "PlayerInputs",
        "player_slot_1",
        sol::readonly(&PlayerInputs::player_slot_1),
        "player_slot_2",
        sol::readonly(&PlayerInputs::player_slot_2),
        "player_slot_3",
        sol::readonly(&PlayerInputs::player_slot_3),
        "player_slot_4",
        sol::readonly(&PlayerInputs::player_slot_4),
        "player_slot_1_settings",
        sol::readonly(&PlayerInputs::player_slot_1_settings),
        "player_slot_2_settings",
        sol::readonly(&PlayerInputs::player_slot_2_settings),
        "player_slot_3_settings",
        sol::readonly(&PlayerInputs::player_slot_3_settings),
        "player_slot_4_settings",
        sol::readonly(&PlayerInputs::player_slot_4_settings));
}
}; // namespace NPlayer
