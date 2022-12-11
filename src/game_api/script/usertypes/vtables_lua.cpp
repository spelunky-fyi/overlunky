#include "entity_lua.hpp"

#include "entities_items.hpp"                      // for Container
#include "entity.hpp"                              // for Entity
#include "hookable_vtable.hpp"                     // for HookableVTable
#include "movable.hpp"                             // for Movable
#include "render_api.hpp"                          // for RenderInfo
#include "script/usertypes/vanilla_render_lua.hpp" // for VanillaRenderContext
#include "state.hpp"                               // for State

namespace NVTables
{
void register_usertypes(sol::state& lua)
{
    // Define all vtables
    using EntityVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        VTableEntry<"dtor", 0x0, void()>,
        VTableEntry<"update_state_machine", 0x2, void()>,
        VTableEntry<"kill", 0x3, void(bool, Entity*)>,
        VTableEntry<"collision1", 0x4, void(Entity*)>,
        VTableEntry<"destroy", 0x5, void()>,
        VTableEntry<"get_held_entity", 0x16, Entity*()>,
        VTableEntry<"trigger_action", 0x18, void(Entity*)>,
        VTableEntry<"collision2", 0x1a, void(Entity*)>>;
    static EntityVTable entity_vtable(lua, lua["Entity"], "ENTITY_OVERRIDE");

    using MovableVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"damage", 0x30, void(Entity*, int8_t, uint32_t, float*, float*, uint16_t, uint8_t)>>;
    static MovableVTable movable_vtable(lua, lua["Movable"], "ENTITY_OVERRIDE");

    using FloorVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"floor_update", 0x26, void(Movable*)>>;
    static FloorVTable floor_vtable(lua, lua["Floor"], "ENTITY_OVERRIDE");

    using RenderInfoVTable = HookableVTable<
        RenderInfo,
        CallbackType::Entity,
        VTableEntry<"dtor", 0x0, void()>,
        VTableEntry<"render", 0x3, void(), BackBinder<VanillaRenderContext>>>;
    static RenderInfoVTable render_info_vtable(lua, lua["RenderInfo"], "ENTITY_OVERRIDE");

    using ThemeVTable = HookableVTable<
        ThemeInfo,
        CallbackType::Theme,
        VTableEntry<"dtor", 0, void()>,
        VTableEntry<"reset_theme_flags", 1, void()>,
        VTableEntry<"init_flags", 2, void()>,
        VTableEntry<"init_level", 3, void()>,
        VTableEntry<"init_rooms", 4, void()>,
        VTableEntry<"generate_path", 5, void()>,
        VTableEntry<"special_rooms", 6, void()>,
        VTableEntry<"player_coffin", 7, void()>,
        VTableEntry<"dirk_coffin", 8, void()>,
        VTableEntry<"idol", 9, void()>,
        VTableEntry<"vault", 10, void()>,
        VTableEntry<"coffin", 11, void()>,
        VTableEntry<"feeling", 12, void()>,
        VTableEntry<"spawn_level", 13, void()>,
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
        /* this just doesn't work
        VTableEntry<"lvl_file", 25, char*()>, */
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
        VTableEntry<"spawn_extra", 50, void()>,
        /* Pretty sure this doesn't even exist, nobody calls it and it crashes the game */
        VTableEntry<"unknown_v51", 51, void()>>;
    static ThemeVTable theme_vtable(lua, lua["ThemeInfo"], "THEME_OVERRIDE");

    // Define the implementations for the LuaBackend handlers
    HookHandler<Entity, CallbackType::Entity>::set_hook_dtor_impl(
        [](std::uint32_t uid, std::function<void(std::uint32_t)> fun)
        {
            Entity* ent = get_entity_ptr(uid);
            std::uint32_t callback_id = entity_vtable.reserve_callback_id(ent);
            entity_vtable.set_pre<void(Entity*), 0x0>(
                ent,
                callback_id,
                [fun = std::move(fun)](Entity* ent_inner)
                { fun(ent_inner->uid); return false; });
            return callback_id;
        });
    HookHandler<Entity, CallbackType::Entity>::set_unhook_impl(
        [](std::uint32_t callback_id, std::uint32_t uid)
        {
            Entity* ent = get_entity_ptr(uid);
            entity_vtable.unhook(ent, callback_id);
        });

    HookHandler<RenderInfo, CallbackType::Entity>::set_hook_dtor_impl(
        [](std::uint32_t uid, std::function<void(std::uint32_t)> fun)
        {
            Entity* ent = get_entity_ptr(uid);
            std::uint32_t callback_id = render_info_vtable.reserve_callback_id(ent->rendering_info);
            render_info_vtable.set_pre<void(RenderInfo*), 0x0>(
                ent->rendering_info,
                callback_id,
                [fun = std::move(fun)](RenderInfo* render_info)
                { fun(render_info->get_aux_id()); return false; });
            return callback_id;
        });
    HookHandler<RenderInfo, CallbackType::Entity>::set_unhook_impl(
        [](std::uint32_t callback_id, std::uint32_t uid)
        {
            Entity* ent = get_entity_ptr(uid);
            render_info_vtable.unhook(ent->rendering_info, callback_id);
        });

    // Add support for hooking entity dtors
    Entity::set_hook_dtor_impl(
        [](Entity* ent, std::function<void(Entity*)> fun)
        {
            std::uint32_t callback_id = entity_vtable.reserve_callback_id(ent);
            entity_vtable.set_pre<void(Entity*), 0x0>(
                ent,
                callback_id,
                [fun = std::move(fun)](Entity* ent_inner)
                { fun(ent_inner); return false; });
            return callback_id;
        },
        [](Entity* ent, std::uint32_t callback_id)
        {
            entity_vtable.unhook(ent, callback_id);
        });

    HookHandler<ThemeInfo, CallbackType::Theme>::set_hook_dtor_impl(
        [](std::uint32_t theme_id, std::function<void(std::uint32_t)> fun)
        {
            auto state = State::get().ptr_main();
            ThemeInfo* theme = state->level_gen->themes[theme_id - 1];
            std::uint32_t callback_id = theme_vtable.reserve_callback_id(theme);
            theme_vtable.set_pre<void(ThemeInfo*), 0x0>(
                theme,
                callback_id,
                [fun = std::move(fun)](ThemeInfo* theme_inner)
                { fun(theme_inner->get_theme_id()); return false; });
            return callback_id;
        });
    HookHandler<ThemeInfo, CallbackType::Theme>::set_unhook_impl(
        [](std::uint32_t callback_id, std::uint32_t theme_id)
        {
            auto state = State::get().ptr_main();
            ThemeInfo* theme = state->level_gen->themes[theme_id - 1];
            theme_vtable.unhook(theme, callback_id);
        });
}
}; // namespace NVTables
