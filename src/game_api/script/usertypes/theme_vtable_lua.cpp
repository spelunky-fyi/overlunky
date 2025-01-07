#include "theme_vtable_lua.hpp"

#include "thread_utils.hpp" // for HeapBase

namespace NThemeVTables
{
ThemeVTable& get_theme_info_vtable(sol::state& lua)
{
    static ThemeVTable theme_vtable(lua, lua["ThemeInfo"], "THEME_OVERRIDE");
    return theme_vtable;
}

void register_usertypes(sol::state& lua)
{
    static auto& theme_vtable = get_theme_info_vtable(lua);

    HookHandler<ThemeInfo, CallbackType::Theme>::set_hook_dtor_impl(
        [](std::uint32_t theme_id, std::function<void(std::uint32_t)> fun)
        {
            auto level_gen = HeapBase::get_main().level_gen();
            ThemeInfo* theme = level_gen->themes[theme_id - 1];
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
            auto level_gen = HeapBase::get_main().level_gen();
            ThemeInfo* theme = level_gen->themes[theme_id - 1];
            theme_vtable.unhook(theme, callback_id);
        });
}
}; // namespace NThemeVTables
