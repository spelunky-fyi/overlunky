#include "vtables_lua.hpp"

#include "entity.hpp"                              // for Entity
#include "entity_structs.hpp"                      // for CollisionInfo
#include "hookable_vtable.hpp"                     // for HookableVTable
#include "member_function.hpp"                     // for MemberFun
#include "movable.hpp"                             // for Movable
#include "render_api.hpp"                          // for RenderInfo
#include "script/usertypes/theme_vtable_lua.hpp"   // for NThemeVTables
#include "script/usertypes/vanilla_render_lua.hpp" // for VanillaRenderContext
#include "sound_manager.hpp"                       // for SoundMeta
#include "state.hpp"                               // for State

template <auto fun>
using MemFun = MemberFun<fun>::BaseLessType;

namespace NVTables
{
void register_usertypes(sol::state& lua)
{
    NThemeVTables::register_usertypes(lua);

    // Define all vtables
    using EntityVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        VTableEntry<"dtor", 0, void()>,
        VTableEntry<"create_rendering_info", 1, MemFun<&Entity::create_rendering_info>>,
        VTableEntry<"update_state_machine", 2, MemFun<&Entity::update_state_machine>>,
        VTableEntry<"kill", 3, MemFun<&Entity::kill>>,
        VTableEntry<"on_collision1", 4, MemFun<&Entity::on_collision1>>,
        VTableEntry<"destroy", 5, MemFun<&Entity::destroy>>,
        VTableEntry<"generate_stomp_damage_particles", 8, MemFun<&Entity::generate_stomp_damage_particles>>,
        VTableEntry<"can_be_pushed", 10, MemFun<&Entity::can_be_pushed>>,
        VTableEntry<"is_in_liquid", 12, MemFun<&Entity::is_in_liquid>>,
        VTableEntry<"set_invisible", 15, MemFun<&Entity::set_invisible>>,
        VTableEntry<"flip", 16, MemFun<&Entity::flip>>,
        VTableEntry<"set_draw_depth", 17, MemFun<&Entity::set_draw_depth>>,
        VTableEntry<"reset_draw_depth", 18, MemFun<&Entity::reset_draw_depth>>,
        VTableEntry<"friction", 19, MemFun<&Entity::friction>>,
        VTableEntry<"set_as_sound_source", 20, MemFun<&Entity::set_as_sound_source>>,
        VTableEntry<"remove_item", 21, MemFun<&Entity::remove_item>>,
        VTableEntry<"get_held_entity", 22, MemFun<&Entity::get_held_entity>>,
        VTableEntry<"trigger_action", 24, MemFun<&Entity::trigger_action>>,
        VTableEntry<"activate", 25, MemFun<&Entity::activate>>,
        VTableEntry<"on_collision2", 26, MemFun<&Entity::on_collision2>>,
        VTableEntry<"get_metadata", 27, MemFun<&Entity::get_metadata>>,
        VTableEntry<"apply_metadata", 28, MemFun<&Entity::apply_metadata>>,
        VTableEntry<"walked_on", 29, MemFun<&Entity::on_walked_on_by>>,
        VTableEntry<"walked_off", 30, MemFun<&Entity::on_walked_off_by>>,
        VTableEntry<"ledge_grab", 31, MemFun<&Entity::on_ledge_grab>>,
        VTableEntry<"stood_on", 32, MemFun<&Entity::on_stood_on_by>>,
        VTableEntry<"liberate_from_shop", 35, MemFun<&Entity::liberate_from_shop>>,
        VTableEntry<"init", 36, MemFun<&Entity::apply_db>>>;
    static EntityVTable entity_vtable(lua, lua["Entity"], "ENTITY_OVERRIDE");

    using MovableVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"can_jump", 37, MemFun<&Movable::can_jump>>,
        VTableEntry<"get_collision_info", 38, MemFun<&Movable::get_collision_info>>,
        VTableEntry<"sprint_factor", 39, MemFun<&Movable::sprint_factor>>,
        VTableEntry<"calculate_jump_velocity", 40, MemFun<&Movable::calculate_jump_velocity>>,
        // VTableEntry<"get_animation_map", 41, MemFun<&Movable::get_animation_map>>,
        VTableEntry<"apply_velocity", 42, MemFun<&Movable::apply_velocity>>,
        /// NoDoc
        VTableEntry<"stomp_damage", 43, MemFun<&Movable::get_damage>>,
        VTableEntry<"get_damage", 43, MemFun<&Movable::get_damage>>,
        VTableEntry<"is_on_fire", 45, bool()>,
        VTableEntry<"damage", 48, bool(Entity*, int8_t, uint16_t, Vec2*, uint8_t, uint16_t, uint8_t, bool)>,
        VTableEntry<"on_hit", 49, void(Entity*)>,
        VTableEntry<"get_damage_sound", 50, uint32_t(DAMAGE_TYPE)>,
        VTableEntry<"stun", 51, void(uint16_t)>,
        VTableEntry<"freeze", 52, void(uint8_t)>,
        VTableEntry<"light_on_fire", 53, void(uint8_t)>,
        VTableEntry<"set_cursed", 54, void(bool)>,
        VTableEntry<"web_collision", 55, void()>,
        VTableEntry<"check_out_of_bounds", 58, void()>,
        VTableEntry<"set_standing_on", 59, void(int32_t)>,
        VTableEntry<"standing_on", 60, Entity*()>,
        VTableEntry<"stomped_by", 61, void(Entity*)>,
        VTableEntry<"thrown_by", 62, void(Entity*)>,
        VTableEntry<"cloned_to", 63, void(Entity*)>,
        VTableEntry<"pick_up", 67, void(Entity*)>,
        VTableEntry<"picked_up_by", 68, void(Entity*)>,
        VTableEntry<"drop", 69, void(Entity*)>,
        VTableEntry<"collect_treasure", 70, void(int32_t, uint32_t)>,
        VTableEntry<"initialize", 75, void()>,
        VTableEntry<"process_input", 78, void()>,
        VTableEntry<"picked_up", 80, void()>,
        VTableEntry<"fall", 83, void()>,
        VTableEntry<"apply_friction", 84, void()>,
        VTableEntry<"crush", 90, void(Entity*)>>;
    static MovableVTable movable_vtable(lua, lua["Movable"], "ENTITY_OVERRIDE");

    using FloorVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"floor_update", 0x26, void()>>;
    static FloorVTable floor_vtable(lua, lua["Floor"], "ENTITY_OVERRIDE");

    using DoorVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"enter_attempt", 40, float(Entity*)>,
        VTableEntry<"hide_hud", 41, float(Entity*)>,
        VTableEntry<"enter", 42, uint8_t(Entity*)>,
        VTableEntry<"light_level", 44, float()>,
        VTableEntry<"is_unlocked", 45, bool()>,
        VTableEntry<"can_enter", 46, bool(Entity*)>>;
    static DoorVTable door_vtable(lua, lua["Door"], "ENTITY_OVERRIDE");

    using RenderInfoVTable = HookableVTable<
        RenderInfo,
        CallbackType::Entity,
        VTableEntry<"dtor", 0x0, void()>,
        VTableEntry<"render", 0x3, void(float*), BackBinder<VanillaRenderContext>>>;
    static RenderInfoVTable render_info_vtable(lua, lua["RenderInfo"], "RENDER_INFO_OVERRIDE");

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
            movable_vtable.unhook(ent, callback_id);
            floor_vtable.unhook(ent, callback_id);
            door_vtable.unhook(ent, callback_id);
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
            movable_vtable.unhook(ent, callback_id);
            floor_vtable.unhook(ent, callback_id);
            door_vtable.unhook(ent, callback_id);
        });
}
}; // namespace NVTables
