#include "behavior_lua.hpp"

#include <algorithm>    // for max
#include <cstdint>      // for uint8_t
#include <exception>    // for exception
#include <fmt/format.h> // for format_error
#include <functional>   // for function, _Func_impl_no_alloc<>::_...
#include <new>          // for operator new
#include <optional>     // for optional
#include <sol/sol.hpp>  // for proxy_key_t, function, basic_prote...
#include <string>       // for allocator, operator==, string
#include <string_view>  // for string_view
#include <tuple>        // for get
#include <type_traits>  // for move, declval, forward
#include <utility>      // for min, max

#include "math.hpp"               // for Vec2
#include "movable.hpp"            // IWYU pragma: keep
#include "movable_behavior.hpp"   // for CustomMovableBehavior, update_movable
#include "script/lua_backend.hpp" // for LuaBackend

namespace NBehavior
{
void register_usertypes(sol::state& lua)
{
    /// Make a `CustomMovableBehavior`, if `base_behavior` is `nil` you will have to set all of the
    /// behavior functions. If a behavior with `behavior_name` already exists for your script it will
    /// be returned instead.
    lua["make_custom_behavior"] = [](std::string_view behavior_name, uint8_t state_id, VanillaMovableBehavior* base_behavior) -> CustomMovableBehavior*
    {
        auto backend = LuaBackend::get_calling_backend();
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
    lua["Movable"]["generic_update_world"] = update_movable;

    /// Opaque handle to a movable behavior used in some Movable functions
    lua.new_usertype<MovableBehavior>(
        "MovableBehavior",
        sol::no_constructor);
    /// Opaque handle to a movable behavior from the vanilla game
    lua.new_usertype<VanillaMovableBehavior>(
        "VanillaMovableBehavior",
        sol::no_constructor,
        sol::base_classes,
        sol::bases<MovableBehavior>());

    /// Opaque handle to a custom movable behavior from a script mod
    lua.new_usertype<CustomMovableBehavior>(
        "CustomMovableBehavior",
        sol::no_constructor,
        "base_behavior",
        &CustomMovableBehavior::base_behavior,
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
        auto backend_id = LuaBackend::get_calling_backend_id();
        behavior->custom_force_state = [=, force_state = std::move(force_state)](Movable* movable, std::function<bool(Movable*)> base_fun)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            return backend->handle_function_with_return<bool>(force_state, movable, std::move(base_fun)).value_or(false);
        };
    };
    /// Set the `on_enter` function of a `CustomMovableBehavior`, this will be called when the movable
    /// enters the state. If an `on_enter` is already set it will be overridden. The signature of the
    /// function is `nil on_enter(movable, base_fun))`. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_on_enter"] = [](CustomMovableBehavior* behavior, sol::function on_enter) -> void
    {
        auto backend_id = LuaBackend::get_calling_backend_id();
        behavior->custom_on_enter = [=, on_enter = std::move(on_enter)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            backend->handle_function(on_enter, movable, std::move(base_fun));
        };
    };
    /// Set the `on_exit` function of a `CustomMovableBehavior`, this will be called when the movable
    /// leaves the state. If an `on_exit` is already set it will be overridden. The signature of the
    /// function is `nil on_exit(movable, base_fun))`. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_on_exit"] = [](CustomMovableBehavior* behavior, sol::function on_exit) -> void
    {
        auto backend_id = LuaBackend::get_calling_backend_id();
        behavior->custom_on_exit = [=, on_exit = std::move(on_exit)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            backend->handle_function(on_exit, movable, std::move(base_fun));
        };
    };
    /// Set the `update_logic` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `update_logic` is already set it will be overridden. The signature
    /// of the function is `nil update_logic(movable, base_fun))`, use it to change the color, texture,
    /// some timers, etc. of the movable. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_update_logic"] = [](CustomMovableBehavior* behavior, sol::function update_logic) -> void
    {
        auto backend_id = LuaBackend::get_calling_backend_id();
        behavior->custom_update_logic = [=, update_logic = std::move(update_logic)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            backend->handle_function(update_logic, movable, std::move(base_fun));
        };
    };
    /// Set the `update_world` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `update_world` is already set it will be overridden. The signature
    /// of the function is `nil update_world(movable, base_fun))`, use this to update the move, velocity,
    /// current_animation, etc. of the movable, then call `mov:generic_update_world` to update the movable. If no
    /// base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_update_world"] = [](CustomMovableBehavior* behavior, sol::function update_world) -> void
    {
        auto backend_id = LuaBackend::get_calling_backend_id();
        behavior->custom_update_world = [=, update_world = std::move(update_world)](Movable* movable, std::function<void(Movable*)> base_fun)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            backend->handle_function(update_world, movable, std::move(base_fun));
        };
    };
    /// Set the `get_next_state_id` function of a `CustomMovableBehavior`, this will be called every frame when
    /// the movable is updated. If an `get_next_state_id` is already set it will be overridden. The signature
    /// of the function is `int get_next_state_id(movable, base_fun))`, use this to move to another state, return `nil`.
    /// or this behaviors `state_id` to remain in this behavior. If no base behavior is set `base_fun` will be `nil`.
    lua["CustomMovableBehavior"]["set_get_next_state_id"] = [](CustomMovableBehavior* behavior, sol::function get_next_state_id) -> void
    {
        auto backend_id = LuaBackend::get_calling_backend_id();
        behavior->custom_get_next_state_id = [=, get_next_state_id = std::move(get_next_state_id)](Movable* movable, std::function<uint8_t(Movable*)> base_fun)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            return backend->handle_function_with_return<uint8_t>(get_next_state_id, movable, std::move(base_fun)).value_or(behavior->state_id);
        };
    };
}
}; // namespace NBehavior
