#include "vtables_lua.hpp"

#include "entities_chars.hpp"                      // for PowerupCapable
#include "entities_floors.hpp"                     // for Floor
#include "entities_items.hpp"                      // for Powerup and more ...
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
        // apply_texture
        // format_shopitem_name // can't edit the string from lua unless i make so special type for it
        VTableEntry<"generate_damage_particles", 8, MemFun<&Entity::generate_damage_particles>>,
        // get_type_field_a8
        VTableEntry<"can_be_pushed", 10, MemFun<&Entity::can_be_pushed>>,
        // v11
        VTableEntry<"is_in_liquid", 12, MemFun<&Entity::is_in_liquid>>,
        // check_type_properties_flags_19
        // get_type_field_60
        VTableEntry<"set_invisible", 15, MemFun<&Entity::set_invisible>>,
        VTableEntry<"flip", 16, MemFun<&Entity::flip>>,
        VTableEntry<"set_draw_depth", 17, MemFun<&Entity::set_draw_depth>>,
        VTableEntry<"reset_draw_depth", 18, MemFun<&Entity::reset_draw_depth>>,
        VTableEntry<"friction", 19, MemFun<&Entity::friction>>,
        VTableEntry<"set_as_sound_source", 20, MemFun<&Entity::set_as_sound_source>>,
        VTableEntry<"remove_item", 21, MemFun<&Entity::remove_item>>,
        VTableEntry<"get_held_entity", 22, MemFun<&Entity::get_held_entity>>,
        // v23
        VTableEntry<"trigger_action", 24, MemFun<&Entity::trigger_action>>,
        VTableEntry<"activate", 25, MemFun<&Entity::activate>>,
        VTableEntry<"on_collision2", 26, MemFun<&Entity::on_collision2>>,
        VTableEntry<"get_metadata", 27, MemFun<&Entity::get_metadata>>,
        VTableEntry<"apply_metadata", 28, MemFun<&Entity::apply_metadata>>,
        VTableEntry<"walked_on", 29, MemFun<&Entity::on_walked_on_by>>,
        VTableEntry<"walked_off", 30, MemFun<&Entity::on_walked_off_by>>,
        VTableEntry<"ledge_grab", 31, MemFun<&Entity::on_ledge_grab>>,
        VTableEntry<"stood_on", 32, MemFun<&Entity::on_stood_on_by>>,
        // toggle_backlayer_illumination
        // v34
        VTableEntry<"liberate_from_shop", 35, MemFun<&Entity::liberate_from_shop>>,
        VTableEntry<"init", 36, MemFun<&Entity::apply_db>>>;
    static EntityVTable entity_vtable(lua, lua["Entity"], "ENTITY_OVERRIDE");

    using MovableVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"can_jump", 37, MemFun<&Movable::can_jump>>,
        // <"get_collision_info", 38, MemFun<&Movable::get_collision_info>>, // dunno if it works, or if it's even called by the game
        VTableEntry<"sprint_factor", 39, MemFun<&Movable::sprint_factor>>,
        VTableEntry<"calculate_jump_velocity", 40, MemFun<&Movable::calculate_jump_velocity>>,
        // <"get_animation_map", 41, MemFun<&Movable::get_animation_map>>,
        VTableEntry<"apply_velocity", 42, MemFun<&Movable::apply_velocity>>,
        /// NoDoc
        VTableEntry<"stomp_damage", 43, MemFun<&Movable::get_damage>>,
        VTableEntry<"get_damage", 43, MemFun<&Movable::get_damage>>,
        // <"get_stomp_damage", 43, MemFun<&Movable::get_stomp_damage>>, // still don't know the difference between this and get_damage
        VTableEntry<"is_on_fire", 45, MemFun<&Movable::is_on_fire>>,
        VTableEntry<"attack", 46, MemFun<&Movable::attack>>,
        VTableEntry<"thrown_into", 47, MemFun<&Movable::thrown_into>>,
        VTableEntry<"damage", 48, MemFun<&Movable::damage>>,
        VTableEntry<"on_hit", 49, MemFun<&Movable::on_hit>>,
        VTableEntry<"get_damage_sound", 50, MemFun<&Movable::get_damage_sound>>,
        VTableEntry<"stun", 51, MemFun<&Movable::stun>>,
        VTableEntry<"freeze", 52, MemFun<&Movable::freeze>>,
        VTableEntry<"light_on_fire", 53, MemFun<&Movable::light_on_fire>>,
        VTableEntry<"set_cursed", 54, MemFun<&Movable::set_cursed>>,
        VTableEntry<"web_collision", 55, MemFun<&Movable::on_spiderweb_collision>>,
        // set_last_owner_uid
        // get_last_owner_uid
        VTableEntry<"check_out_of_bounds", 58, MemFun<&Movable::check_out_of_bounds>>,
        VTableEntry<"set_standing_on", 59, MemFun<&Movable::set_standing_on>>,
        VTableEntry<"standing_on", 60, MemFun<&Movable::standing_on>>,
        VTableEntry<"stomped_by", 61, MemFun<&Movable::on_stomped_on_by>>,
        VTableEntry<"thrown_by", 62, MemFun<&Movable::on_thrown_by>>,
        VTableEntry<"cloned_to", 63, MemFun<&Movable::copy_extra_info>>,
        // get_type_id
        // doesnt_have_spikeshoes (potentially wrong name)
        // is_player_mount_or_monster
        VTableEntry<"pick_up", 67, MemFun<&Movable::pick_up>>,
        /// NoDoc
        VTableEntry<"picked_up_by", 68, MemFun<&Movable::can_be_picked_up_by>>,
        VTableEntry<"can_be_picked_up_by", 68, MemFun<&Movable::can_be_picked_up_by>>,
        VTableEntry<"drop", 69, MemFun<&Movable::drop>>,
        VTableEntry<"collect_treasure", 70, MemFun<&Movable::collect_treasure>>,
        VTableEntry<"apply_movement", 71, MemFun<&Movable::apply_movement>>,
        // damage_entity
        // v73
        VTableEntry<"is_powerup_capable", 74, MemFun<&Movable::is_powerup_capable>>, // dunno if called by the game, might be inlined?
        VTableEntry<"initialize", 75, MemFun<&Movable::initialize>>,
        // check_is_falling
        // v77
        VTableEntry<"process_input", 78, MemFun<&Movable::process_input>>,
        // post_collision_damage_related?
        VTableEntry<"picked_up", 80, MemFun<&Movable::on_picked_up>>,
        VTableEntry<"release", 81, MemFun<&Movable::on_release>>,
        VTableEntry<"generate_landing_effects", 82, MemFun<&Movable::generate_landing_effects>>,
        VTableEntry<"fall", 83, MemFun<&Movable::handle_fall_logic>>,
        VTableEntry<"apply_friction", 84, MemFun<&Movable::apply_friction>>,
        VTableEntry<"can_break_block", 85, MemFun<&Movable::can_break_block>>,
        VTableEntry<"break_block", 86, MemFun<&Movable::break_block>>,
        // v87
        // v88
        // v89
        VTableEntry<"crush", 90, MemFun<&Movable::on_crushed_by>>,
        // on_fall_onto // interesting, needs testing
        VTableEntry<"body_destruction", 92, MemFun<&Movable::on_body_destruction>>>;
    static MovableVTable movable_vtable(lua, lua["Movable"], "ENTITY_OVERRIDE");

    using FloorVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"floor_update", 0x26, MemFun<&Floor::on_neighbor_destroyed>>>;
    static FloorVTable floor_vtable(lua, lua["Floor"], "ENTITY_OVERRIDE");

    using DoorVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"enter_attempt", 40, MemFun<&Door::on_enter_attempt>>,
        VTableEntry<"hide_hud", 41, MemFun<&Door::hide_ui>>,
        VTableEntry<"enter", 42, MemFun<&Door::enter>>,
        // entered_from_front_layer
        VTableEntry<"light_level", 44, MemFun<&Door::light_level>>,
        VTableEntry<"is_unlocked", 45, MemFun<&Door::is_unlocked>>,
        VTableEntry<"can_enter", 46, MemFun<&Door::can_enter>>>;
    static DoorVTable door_vtable(lua, lua["Door"], "ENTITY_OVERRIDE");

    using RenderInfoVTable = HookableVTable<
        RenderInfo,
        CallbackType::Entity,
        VTableEntry<"dtor", 0x0, void()>,
        VTableEntry<"draw", 0x1, void()>,
        VTableEntry<"render", 0x3, void(Vec2*), BackBinder<VanillaRenderContext>>>;
    static RenderInfoVTable render_info_vtable(lua, lua["RenderInfo"], "RENDER_INFO_OVERRIDE");

    using PowerupCapableVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"blood_collision", 93, MemFun<&PowerupCapable::on_blood_collision>>>;
    // can_clear_last_owner
    static PowerupCapableVTable powerup_capable_vtable(lua, lua["PowerupCapable"], "ENTITY_OVERRIDE");

    using PowerupVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        // <"get_hud_sprite", 93, MemFun<&Powerup::get_hud_sprite>>,
        VTableEntry<"putting_on", 96, MemFun<&Powerup::on_putting_on>>,
        VTableEntry<"putting_off", 97, MemFun<&Powerup::on_putting_off>>>;
    static PowerupVTable powerup_vtable(lua, lua["Powerup"], "ENTITY_OVERRIDE");

    using BackpackVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"trigger_explosion", 99, MemFun<&Backpack::trigger_explosion>>>;
    static BackpackVTable backpack_vtable(lua, lua["Backpack"], "ENTITY_OVERRIDE");

    using JetpackVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"acceleration", 100, MemFun<&Jetpack::acceleration>>>;
    static JetpackVTable jetpack_vtable(lua, lua["Jetpack"], "ENTITY_OVERRIDE");

    using PurchasableVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"equip", 93, MemFun<&Purchasable::equip>>>;
    static PurchasableVTable purchasable_vtable(lua, lua["Purchasable"], "ENTITY_OVERRIDE");

    using DummyPurchasableEntityVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"trigger_explosion", 94, MemFun<&DummyPurchasableEntity::trigger_explosion>>>;
    static DummyPurchasableEntityVTable dummy_purchasable_entity_vtable(lua, lua["DummyPurchasableEntity"], "ENTITY_OVERRIDE");

    using OlmecCannonVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"spawn_projectile", 93, MemFun<&OlmecCannon::spawn_projectile>>>;
    static OlmecCannonVTable olmec_cannon_vtable(lua, lua["OlmecCannon"], "ENTITY_OVERRIDE");

    using RollingItemVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"give_powerup", 93, MemFun<&RollingItem::give_powerup>>>;
    static RollingItemVTable rolling_item_vtable(lua, lua["RollingItem"], "ENTITY_OVERRIDE");

    // Arrow // poison_arrow // light_up ?
    // Torch // light_up // get_flame_offset // get_flame_type
    // Bow // get_arrow_special_offset
    // Generator // randomize_timer ?
    // SoundMeta // start // kill //update maybe?
    // CritterSlime + CritterSnail // get_speed
    // Ghist // on_body_destroyed
    // YetiQueen + YetiKing // ??
    // Anubis // set_next_attack_timer // normal_attack // play_attack_sound
    // Spider // ??
    // WalkingMonster // can_aggro
    // NPC ....
    // RoomOwner ...
    // Monster ...
    // Mount ...

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
