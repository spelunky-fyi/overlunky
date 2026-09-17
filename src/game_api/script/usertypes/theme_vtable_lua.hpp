#pragma once

#include "hookable_vtable.hpp" // for HookableVTable
#include "level_api.hpp"       // for ThemeInfo
#include "member_function.hpp" // for MemberFun

namespace sol
{
class state;
} // namespace sol

namespace NThemeVTables
{

template <auto fun>
using MemFun = MemberFun<fun>::BaseLessType;

void register_usertypes(sol::state& lua);

using ThemeVTable = HookableVTable<
    ThemeInfo,
    CallbackType::Theme,
    VTableEntry<"dtor", 0, void()>,
    VTableEntry<"reset_theme_flags", 1, void()>,
    VTableEntry<"init_flags", 2, void()>,
    VTableEntry<"init_level", 3, void()>,
    VTableEntry<"init_rooms", 4, void()>,
    VTableEntry<"generate_path", 5, void(bool)>,
    VTableEntry<"special_rooms", 6, void()>,
    VTableEntry<"player_coffin", 7, void()>,
    VTableEntry<"dirk_coffin", 8, void()>,
    VTableEntry<"idol", 9, void()>,
    VTableEntry<"vault", 10, void()>,
    VTableEntry<"coffin", 11, void()>,
    VTableEntry<"feeling", 12, void()>,
    VTableEntry<"spawn_level", 13, void(uint64_t, uint64_t, uint64_t)>,
    VTableEntry<"spawn_border", 14, void()>,
    VTableEntry<"post_process_level", 15, void()>,
    VTableEntry<"spawn_traps", 16, void()>,
    VTableEntry<"post_process_entities", 17, void()>,
    VTableEntry<"spawn_procedural", 18, void()>,
    VTableEntry<"spawn_background", 19, void()>,
    VTableEntry<"spawn_lights", 20, void()>,
    VTableEntry<"spawn_transition", 21, void()>,
    VTableEntry<"post_transition", 22, void()>,
    VTableEntry<"spawn_players", 23, void()>,
    VTableEntry<"spawn_effects", 24, void()>,
    VTableEntry<"theme_id", 26, uint8_t()>,
    VTableEntry<"base_id", 27, uint8_t()>,
    VTableEntry<"ent_floor_spreading", 28, uint32_t()>,
    VTableEntry<"ent_floor_spreading2", 29, uint32_t()>,
    VTableEntry<"transition_styled_floor", 30, bool()>,
    VTableEntry<"transition_modifier", 31, uint32_t()>,
    VTableEntry<"ent_transition_styled_floor", 32, uint32_t()>,
    VTableEntry<"ent_backwall", 33, uint32_t()>,
    VTableEntry<"ent_border", 34, uint32_t()>,
    VTableEntry<"ent_critter", 35, uint32_t()>,
    VTableEntry<"gravity", 36, float()>,
    VTableEntry<"player_damage", 37, bool()>,
    VTableEntry<"soot", 38, bool()>,
    VTableEntry<"texture_backlayer_lut", 39, uint32_t()>,
    VTableEntry<"backlayer_light_level", 40, float()>,
    VTableEntry<"loop", 41, bool()>,
    VTableEntry<"vault_level", 42, uint8_t()>,
    VTableEntry<"theme_flag", 43, bool(uint8_t)>,
    VTableEntry<"texture_dynamic", 44, uint32_t(int32_t)>,
    VTableEntry<"pre_transition", 45, void()>,
    VTableEntry<"exit_room_y_level", 46, uint32_t()>,
    VTableEntry<"shop_chance", 47, uint32_t()>,
    VTableEntry<"spawn_decoration", 48, void()>,
    VTableEntry<"spawn_decoration2", 49, void()>,
    VTableEntry<"spawn_extra", 50, MemFun<&ThemeInfo::spawn_extra>>,
    VTableEntry<"do_procedural_spawn", 51, void(SpawnInfo*)>>;
// static ThemeVTable theme_vtable(lua, lua["ThemeInfo"], "THEME_OVERRIDE");
ThemeVTable& get_theme_info_vtable(sol::state& lua);
}; // namespace NThemeVTables
