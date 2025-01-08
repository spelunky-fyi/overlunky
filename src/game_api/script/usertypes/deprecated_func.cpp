#include "deprecated_func.hpp"

#include <WinBase.h>
#include <cstdint>     // for int64_t
#include <sol/sol.hpp> //
#include <vector>      // for vector

#include "aliases.hpp"                    // for CallbackId
#include "entities_chars.hpp"             // for Player
#include "entities_items.hpp"             // for PlayerGhost
#include "entity.hpp"                     // for get_entity_ptr
#include "entity_lookup.hpp"              // for get_entities
#include "rpc.hpp"                        // for read_prng
#include "script/handle_lua_function.hpp" // for handle_function
#include "script/lua_backend.hpp"         // for LuaBackend
#include "state.hpp"                      // for darkmode
#include "vanilla_render_lua.hpp"         // for VanillaRenderContext

namespace NDeprecated
{
void register_usertypes(sol::state& lua)
{
    /// Deprecated
    /// Read the game prng state. Use [prng](#PRNG):get_pair() instead.
    lua["read_prng"] = []() -> std::vector<int64_t>
    { return read_prng(); };

    /// Deprecated
    /// Set level flag 18 on post room generation instead, to properly force every level to dark
    lua["force_dark_level"] = [](bool g)
    { API::darkmode(g); };

    /// Deprecated
    /// Use `get_entities_by(0, MASK.ANY, LAYER.BOTH)` instead
    lua["get_entities"] = get_entities;
    /// Deprecated
    /// Use `get_entities_by(0, mask, LAYER.BOTH)` instead
    lua["get_entities_by_mask"] = get_entities_by_mask;
    /// Deprecated
    /// Use `get_entities_by(0, MASK.ANY, layer)` instead
    lua["get_entities_by_layer"] = get_entities_by_layer;
    auto get_entities_overlapping = sol::overload(
        static_cast<std::vector<uint32_t> (*)(ENT_TYPE, uint32_t, float, float, float, float, LAYER)>(::get_entities_overlapping),
        static_cast<std::vector<uint32_t> (*)(std::vector<ENT_TYPE>, uint32_t, float, float, float, float, LAYER)>(::get_entities_overlapping));
    /// Deprecated
    /// Use `get_entities_overlapping_hitbox` instead
    lua["get_entities_overlapping"] = get_entities_overlapping;

    /// Deprecated
    /// As the name is misleading. use Movable.`move_state` field instead
    lua["get_entity_ai_state"] = get_entity_ai_state;

    /// Deprecated
    /// Use [replace_drop](#replace_drop)(DROP.ARROWTRAP_WOODENARROW, new_arrow_type) and [replace_drop](#replace_drop)(DROP.POISONEDARROWTRAP_WOODENARROW, new_arrow_type) instead
    lua["set_arrowtrap_projectile"] = set_arrowtrap_projectile;

    /// Deprecated
    /// This function never worked properly as too many places in the game individually check for vlads cape and calculate the blood multiplication
    /// `default_multiplier` doesn't do anything due to some changes in last game updates, `vladscape_multiplier` only changes the multiplier to some entities death's blood spit
    lua["set_blood_multiplication"] = set_blood_multiplication;

    /// Deprecated
    /// Deprecated because it's a weird old hack that crashes the game. You can modify inputs in many other ways, like editing `state.player_inputs.player_slot_1.buttons_gameplay` in PRE_UPDATE or a `set_pre_process_input` hook. Steal input from a Player, HiredHand or PlayerGhost.
    lua["steal_input"] = [](int uid)
    {
        static const auto player_ghost = to_id("ENT_TYPE_ITEM_PLAYERGHOST");
        static const auto ana = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
        static const auto egg_child = to_id("ENT_TYPE_CHAR_EGGPLANT_CHILD");

        auto backend = LuaBackend::get_calling_backend();
        if (backend->script_input.find(uid) != backend->script_input.end())
            return;
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;

        if (player->type->id == player_ghost)
        {
            auto player_gh = player->as<PlayerGhost>();
            ScriptInput* newinput = new ScriptInput();
            newinput->gameplay = 0;
            newinput->all = 0;
            newinput->orig_input = player_gh->player_inputs;
            player_gh->player_inputs = reinterpret_cast<PlayerSlot*>(newinput);
            backend->script_input[uid] = newinput;
        }
        else
        {
            if (player->type->id < ana || player->type->id > egg_child)
                return;

            ScriptInput* newinput = new ScriptInput();
            newinput->gameplay = 0;
            newinput->all = 0;
            newinput->orig_input = player->input_ptr;
            newinput->orig_ai = player->ai;
            player->input_ptr = reinterpret_cast<PlayerSlot*>(newinput);
            player->ai = nullptr;
            backend->script_input[uid] = newinput;
        }
    };
    /// Deprecated
    /// Return input previously stolen with [steal_input](#steal_input)
    lua["return_input"] = [](int uid)
    {
        static const auto player_ghost = to_id("ENT_TYPE_ITEM_PLAYERGHOST");

        auto backend = LuaBackend::get_calling_backend();
        if (backend->script_input.find(uid) == backend->script_input.end())
            return;
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return;

        if (player->type->id == player_ghost)
        {
            auto player_gh = player->as<PlayerGhost>();
            player_gh->player_inputs = backend->script_input[uid]->orig_input;
        }
        else
        {
            player->input_ptr = backend->script_input[uid]->orig_input;
            player->ai = backend->script_input[uid]->orig_ai;
        }
        backend->script_input.erase(uid);
    };
    /// Deprecated
    /// Send input to entity, has to be previously stolen with [steal_input](#steal_input)
    lua["send_input"] = [](int uid, INPUTS buttons)
    {
        auto backend = LuaBackend::get_calling_backend();
        auto it = backend->script_input.find(uid);
        if (it != backend->script_input.end())
        {
            it->second->all = buttons;
            it->second->gameplay = buttons;
        }
    };
    /// Deprecated
    /// Use `players[1].input.buttons_gameplay` for only the inputs during the game, or `.buttons` for all the inputs, even during the pause menu
    /// Of course, you can get the Player by other mean, it doesn't need to be the `players` table
    /// You can only read inputs from actual players, HH don't have any inputs
    lua["read_input"] = [](int uid) -> INPUTS
    {
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (INPUTS)0;

        if (!IsBadReadPtr(player->input_ptr, 20))
        {
            return player->input_ptr->buttons_gameplay;
        }
        return (INPUTS)0;
    };
    /// Deprecated
    /// Read input that has been previously stolen with [steal_input](#steal_input)
    /// Use `state.player_inputs.player_slots[player_slot].buttons_gameplay` for only the inputs during the game, or `.buttons` for all the inputs, even during the pause menu
    lua["read_stolen_input"] = [](int uid) -> INPUTS
    {
        auto backend = LuaBackend::get_calling_backend();
        if (backend->script_input.find(uid) == backend->script_input.end())
        {
            // this means that the input is attacked to the real input and not stolen so return early
            return (INPUTS)0;
        }
        Player* player = get_entity_ptr(uid)->as<Player>();
        if (player == nullptr)
            return (INPUTS)0;
        ScriptInput* readinput = reinterpret_cast<ScriptInput*>(player->input_ptr);
        if (!IsBadReadPtr(readinput, 20))
        {
            readinput = reinterpret_cast<ScriptInput*>(readinput->orig_input);
            if (!IsBadReadPtr(readinput, 20))
            {
                return readinput->gameplay;
            }
        }
        return (INPUTS)0;
    };

    /// Deprecated
    /// Use `entity.clear_virtual` instead.
    /// Clears a callback that is specific to an entity.
    lua["clear_entity_callback"] = [](int uid, CallbackId cb_id)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->HookHandler<Entity, CallbackType::Entity>::clear_hook(cb_id, uid);
    };
    /// Deprecated
    /// Use `entity:set_pre_update_state_machine` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// `uid` has to be the uid of a `Movable` or else stuff will break.
    /// Sets a callback that is called right before the statemachine, return `true` to skip the statemachine update.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is bool statemachine(Entity self)
    lua["set_pre_statemachine"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_update_state_machine"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_post_update_state_machine` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// `uid` has to be the uid of a `Movable` or else stuff will break.
    /// Sets a callback that is called right after the statemachine, so you can override any values the satemachine might have set (e.g. `animation_frame`).
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is nil statemachine(Entity self)
    lua["set_post_statemachine"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_post_update_state_machine"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_destroy` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when an entity is destroyed, e.g. as if by `Entity.destroy()` before the game applies any side effects.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil on_destroy(Entity self)
    lua["set_on_destroy"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_destroy"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_kill` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when an entity is eradicated, before the game applies any side effects.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil on_kill(Entity self, Entity killer)
    lua["set_on_kill"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_kill"](ent, std::move(fun));
        }
        return sol::nullopt;
    };

    /// Deprecated
    /// Use `entity:set_pre_damage` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before an entity is damaged, return `true` to skip the game's damage handling.
    /// Note that damage_dealer can be nil ! (long fall, ...)
    /// DO NOT CALL `self:damage()` in the callback !
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// The entity has to be of a [Movable](#Movable) type.
    /// <br/>The callback signature is bool on_damage(Entity self, Entity damage_dealer, int damage_amount, float vel_x, float vel_y, int stun_amount, int iframes)
    lua["set_on_damage"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid); ent != nullptr && ent->is_movable())
        {
            return lua["Movable"]["set_pre_damage"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_floor_update` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before a floor is updated (by killed neighbor), return `true` to skip the game's neighbor update handling.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is bool pre_floor_update(Entity self)
    lua["set_pre_floor_update"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid)) // TODO: Requires ent->is_floor
        {
            return lua["Floor"]["set_pre_floor_update"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_post_floor_update` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right after a floor is updated (by killed neighbor).
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil post_floor_update(Entity self)
    lua["set_post_floor_update"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid)) // TODO: Requires ent->is_floor
        {
            return lua["Floor"]["set_post_floor_update"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_trigger_action` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right when a container is opened by the player (up+whip)
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is nil on_open(Entity entity_self, Entity opener)
    lua["set_on_open"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid)) // TODO: Requires ent->is_container
        {
            return lua["Entity"]["set_pre_trigger_action"](
                ent,
                [fun = std::move(fun)](Entity* usee, Entity* user)
                {
                    if (user->is_movable() && user->as<Movable>()->movey > 0)
                    {
                        auto backend = LuaBackend::get_calling_backend();
                        handle_function<void>(backend.get(), fun, usee, user);
                    }
                });
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_collision1` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before the collision 1 event, return `true` to skip the game's collision handling.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is bool pre_collision1(Entity entity_self, Entity collision_entity)
    lua["set_pre_collision1"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_on_collision1"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity:set_pre_collision2` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right before the collision 2 event, return `true` to skip the game's collision handling.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
    /// <br/>The callback signature is bool pre_collision12(Entity self, Entity collision_entity)
    lua["set_pre_collision2"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            return lua["Entity"]["set_pre_on_collision2"](ent, std::move(fun));
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity.rendering_info:set_pre_render` in combination with `render_info:get_entity` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right after the entity is rendered.
    /// Return `true` to skip the original rendering function and all later pre_render callbacks.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is bool render(VanillaRenderContext render_ctx, Entity self)
    lua["set_pre_render"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            auto backend_id = LuaBackend::get_calling_backend_id();
            return lua["RenderInfo"]["set_pre_render"](
                ent->rendering_info,
                [backend_id, fun = std::move(fun)](RenderInfo* ri, float*, VanillaRenderContext render_ctx)
                {
                    auto backend = LuaBackend::get_backend(backend_id);
                    return handle_function<bool>(
                        backend.get(),
                        fun,
                        render_ctx,
                        ri->get_entity());
                });
        }
        return sol::nullopt;
    };
    /// Deprecated
    /// Use `entity.rendering_info:set_post_render` in combination with `render_info:get_entity` instead.
    /// Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
    /// Sets a callback that is called right after the entity is rendered.
    /// Use this only when no other approach works, this call can be expensive if overused.
    /// <br/>The callback signature is nil post_render(VanillaRenderContext render_ctx, Entity self)
    lua["set_post_render"] = [&lua](int uid, sol::function fun) -> sol::optional<CallbackId>
    {
        if (Entity* ent = get_entity_ptr(uid))
        {
            auto backend_id = LuaBackend::get_calling_backend_id();
            return lua["RenderInfo"]["set_post_render"](
                ent->rendering_info,
                [backend_id, fun = std::move(fun)](RenderInfo* ri, float*, VanillaRenderContext render_ctx)
                {
                    auto backend = LuaBackend::get_backend(backend_id);
                    return handle_function<bool>(
                        backend.get(),
                        fun,
                        render_ctx,
                        ri->get_entity());
                });
        }
        return sol::nullopt;
    };
}
} // namespace NDeprecated
