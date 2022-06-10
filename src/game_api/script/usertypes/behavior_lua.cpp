#include "behavior_lua.hpp"

#include "movable.hpp"
#include "movable_behavior.hpp"
#include "script/lua_backend.hpp"

#include <sol/sol.hpp>

namespace NBehavior
{
void register_usertypes(sol::state& lua)
{
    /// Make a `CustomMovableBehavior`, if `base_behavior` is `nil` you will have to set all of the
    /// behavior functions. If a behavior with `behavior_name` already exists for your script it will
    /// be returned instead.
    lua["make_custom_behavior"] = [](std::string_view behavior_name, uint8_t state_id, VanillaMovableBehavior* base_behavior) -> CustomMovableBehavior*
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        if (CustomMovableBehavior* existing_behavior = backend->get_custom_movable_behavior(behavior_name))
        {
            return existing_behavior;
        }
        return backend->make_custom_movable_behavior(behavior_name, state_id, base_behavior);
    };

    lua["Movable"]["get_base_behavior"] = get_base_behavior;
    lua["Movable"]["add_behavior"] = add_behavior;
    lua["Movable"]["clear_behavior"] = clear_behavior;
    lua["Movable"]["clear_behaviors"] = clear_behaviors;

    auto update_movable = sol::overload(
        static_cast<void (*)(Movable*)>(::update_movable),
        static_cast<void (*)(Movable*, bool)>(::update_movable),
        static_cast<void (*)(Movable*, Vec2, float, bool, bool)>(::update_movable));
    lua["Movable"]["update_physics"] = update_movable;

    /// Opaque handle to a movable behavior
    lua.new_usertype<MovableBehavior>(
        "MovableBehavior");
    /// Opaque handle to a movable behavior from the vanilla game
    lua.new_usertype<VanillaMovableBehavior>(
        "VanillaMovableBehavior",
        sol::base_classes,
        sol::bases<MovableBehavior>());
    /// Opaque handle to a custom movable behavior from a script mod
    lua.new_usertype<CustomMovableBehavior>(
        "CustomMovableBehavior",
        sol::base_classes,
        sol::bases<MovableBehavior>());

    /// Get the `state_id` of a behavior, this is the id that needs to be returned from a behavior's
    /// `get_next_state_id` to enter this state, given that the behavior is added to the movable.
    lua["MovableBehavior"]["get_state_id"] = [](MovableBehavior* behavior) -> uint8_t
    {
        return behavior->get_state_id();
    };

    /// Set the `force_state` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `force_state` is already set it will be overridden. The signature
    /// of the function is `bool force_state(movable, base_fun)`, when the function returns `true` the movable will
    /// enter this behavior. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_force_state"] = [](CustomMovableBehavior* behavior, sol::function force_state) -> void
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        behavior->custom_force_state = [=, force_state = std::move(force_state)](Movable* movable, std::function<bool(Movable*)> base_fun)
        {
            return backend->handle_function_with_return<bool>(force_state, movable, std::move(base_fun)).value_or(false);
        };
    };
    /// Set the `on_enter` function of a `CustomMovableBehavior`, this will be called when the movable
    /// enters the state. If an `on_enter` is already set it will be overridden. The signature of the
    /// function is `nil on_enter(movable, base_fun))`. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_on_enter"] = [](CustomMovableBehavior* behavior, sol::function on_enter) -> void
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        behavior->custom_on_enter = [=, on_enter = std::move(on_enter)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            backend->handle_function(on_enter, movable, std::move(base_fun));
        };
    };
    /// Set the `on_exit` function of a `CustomMovableBehavior`, this will be called when the movable
    /// leaves the state. If an `on_exit` is already set it will be overridden. The signature of the
    /// function is `nil on_exit(movable, base_fun))`. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_on_exit"] = [](CustomMovableBehavior* behavior, sol::function on_exit) -> void
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        behavior->custom_on_exit = [=, on_exit = std::move(on_exit)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            backend->handle_function(on_exit, movable, std::move(base_fun));
        };
    };
    /// Set the `update_render` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `update_render` is already set it will be overridden. The signature
    /// of the function is `nil update_render(movable, base_fun))`, use it to change the color, texture,
    /// animation frame, etc. of the movable. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_update_render"] = [](CustomMovableBehavior* behavior, sol::function update_render) -> void
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        behavior->custom_update_render = [=, update_render = std::move(update_render)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            backend->handle_function(update_render, movable, std::move(base_fun));
        };
    };
    /// Set the `update_physics` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `update_physics` is already set it will be overridden. The signature
    /// of the function is `nil update_physics(movable, base_fun))`, use this to update the movex, movey, velocityx,
    /// velocityy, etc. of the movable, then call `update_movable` to update the movable. If no base behavior is set
    /// `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_update_physics"] = [](CustomMovableBehavior* behavior, sol::function update_physics) -> void
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        behavior->custom_update_physics = [=, update_physics = std::move(update_physics)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            backend->handle_function(update_physics, movable, std::move(base_fun));
        };
    };
    /// Set the `get_next_state_id` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `get_next_state_id` is already set it will be overridden. The signature
    /// of the function is `int get_next_state_id(movable, base_fun))`, use this to move to another state, return `nil`.
    /// or this behaviors `state_id` to remain in this behavior. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_get_next_state_id"] = [](CustomMovableBehavior* behavior, sol::function get_next_state_id) -> void
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        behavior->custom_get_next_state_id = [=, get_next_state_id = std::move(get_next_state_id)](Movable* movable, std::function<uint8_t(Movable*)> base_fun)
        {
            return backend->handle_function_with_return<uint8_t>(get_next_state_id, movable, std::move(base_fun)).value_or(behavior->state_id);
        };
    };
}
}; // namespace NBehavior
