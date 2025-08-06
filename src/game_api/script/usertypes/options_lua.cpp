#include "options_lua.hpp"

#include <sol/sol.hpp>
#include <string>

#include "script/lua_backend.hpp"

namespace NOptions
{
void register_usertypes(sol::state& lua)
{
    /// Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
    /// limits, you can override them in the UI with double click.
    // lua["register_option_int"] = [](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
    lua["register_option_int"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, int value, int min, int max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, IntOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, int value, int min, int max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", IntOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
    /// limits, you can override them in the UI with double click.
    // lua["register_option_float"] = [](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
    lua["register_option_float"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, float value, float min, float max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, FloatOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, float value, float min, float max)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", FloatOption{value, min, max}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_bool"] = [](std::string name, std::string desc, std::string long_desc, bool value)
    lua["register_option_bool"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, bool value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, BoolOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, bool value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", BoolOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
    // lua["register_option_string"] = [](std::string name, std::string desc, std::string long_desc, std::string value)
    lua["register_option_string"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, std::string value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, StringOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, std::string value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", StringOption{value}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        });
    /// Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
    /// with a double `\0\0` at the end. `value` is the default index 1..n.
    // lua["register_option_combo"] = [](std::string name, std::string desc, std::string long_desc, std::string opts, int value)
    lua["register_option_combo"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, std::string opts, int value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, ComboOption{value - 1, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, std::string long_desc, std::string opts)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, ComboOption{0, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = 1;
        },
        [](std::string name, std::string desc, std::string opts, int value)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", ComboOption{value - 1, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = value;
        },
        [](std::string name, std::string desc, std::string opts)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", ComboOption{0, opts}};
            if (backend->lua["options"][name] == sol::nil)
                backend->lua[sol::create_if_nil]["options"][name] = 1;
        });
    /// Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.
    // lua["register_option_button"] = [](std::string name, std::string desc, std::string long_desc, sol::function on_click)
    lua["register_option_button"] = sol::overload(
        [](std::string name, std::string desc, std::string long_desc, sol::function callback)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, long_desc, ButtonOption{callback}};
            backend->lua[sol::create_if_nil]["options"][name] = -1;
        },
        [](std::string name, std::string desc, sol::function callback)
        {
            auto backend = LuaBackend::get_calling_backend();
            backend->options[name] = {desc, "", ButtonOption{callback}};
            backend->lua[sol::create_if_nil]["options"][name] = -1;
        });
    /// Add custom options using the window drawing functions. Everything drawn in the callback will be rendered in the options window and the return value saved to `options[name]` or overwriting the whole `options` table if using and empty name.
    /// `value` is the default value, and pretty important because anything defined in the callback function will only be defined after the options are rendered. See the example for details.
    /// <br/>The callback signature is optional<any> on_render(GuiDrawContext draw_ctx)
    lua["register_option_callback"] = [](std::string name, sol::object value, sol::function on_render)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->options[name] = {"", "", CustomOption{on_render}};
        if (backend->lua["options"][name] == sol::nil)
        {
            if (name != "")
                backend->lua[sol::create_if_nil]["options"][name] = value;
            else
                backend->lua[sol::create_if_nil]["options"] = value;
        }
    };

    /// Removes an option by name. To make complicated conditionally visible options you should probably just use register_option_callback though.
    lua["unregister_option"] = [](std::string name)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->options.erase(name);
        backend->lua["options"][name] = sol::nil;
    };
}
}; // namespace NOptions
