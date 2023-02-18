#include "entity_lua.hpp"

#include "entities_items.hpp"                      // for Container
#include "entity.hpp"                              // for Entity
#include "hookable_vtable.hpp"                     // for HookableVTable
#include "movable.hpp"                             // for Movable
#include "render_api.hpp"                          // for RenderInfo
#include "script/usertypes/vanilla_render_lua.hpp" // for VanillaRenderContext

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
        VTableEntry<"on_collision1", 0x4, void(Entity*)>,
        VTableEntry<"destroy", 0x5, void()>,
        VTableEntry<"get_held_entity", 0x16, Entity*()>,
        VTableEntry<"trigger_action", 0x18, void(Entity*)>,
        VTableEntry<"on_collision2", 0x1a, void(Entity*)>>;
    static EntityVTable entity_vtable(lua, lua["Entity"], "ENTITY_OVERRIDE");

    using MovableVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"damage", 0x30, bool(Entity*, int8_t, uint16_t, Vec2*, uint8_t, uint16_t, uint8_t, bool)>>;
    static MovableVTable movable_vtable(lua, lua["Movable"], "ENTITY_OVERRIDE");

    using FloorVTable = HookableVTable<
        Entity,
        CallbackType::Entity,
        EntityVTable,
        VTableEntry<"floor_update", 0x26, void()>>;
    static FloorVTable floor_vtable(lua, lua["Floor"], "ENTITY_OVERRIDE");

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
}
}; // namespace NVTables
