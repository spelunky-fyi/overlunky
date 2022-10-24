#include "steam_lua.hpp"

#include <detours.h> // for DetourAttach, DetourTransactionBegin
#include <sol/sol.hpp>

#include "memory.hpp"
#include "steam_api.hpp"
#include "vtable_hook.hpp" // for get_hook_function, register_hook_function

void set_on_get_feat(sol::function get_feat_func = nullptr)
{
    if (get_feat_func)
    {
        hook_get_feat([get_feat_func](FEAT feat)
                      {
            sol::optional<bool> maybe = get_feat_func(feat);
            if (maybe)
                return maybe.value();
            return false; });
        return;
    }
    hook_get_feat(nullptr);
}

void set_on_set_feat(sol::function set_feat_func = nullptr)
{
    if (set_feat_func)
    {
        hook_set_feat([set_feat_func](FEAT feat)
                      { set_feat_func(feat); });
        return;
    }
    hook_set_feat(nullptr);
}

namespace NSteam
{
void register_usertypes(sol::state& lua)
{
    /// Check if the user has performed a feat (Real Steam achievement or a hooked one). Returns: `unlocked, hidden, name, description`
    lua["get_feat"] = get_feat;

    /// Bypass Steam achievements with your own callback when the game asks if a feat is unlocked. The game will call this function every frame for every feat when rendering the Feats page. Do not do any complicated stuff in here, just return predetermined things. The callback signature is `bool get_feat(FEAT)`.
    lua["set_on_get_feat"] = set_on_get_feat;

    /// Bypass Steam achievements with your own callback instead when the game tries to unlock a vanilla feat. It may be called again if you don't  The callback signature is `nil set_feat(FEAT)`.
    lua["set_on_set_feat"] = set_on_set_feat;

    /// Helper function to set the title and description for a FEAT with change_string, as well as the hidden state.
    lua["change_feat"] = [](FEAT feat, bool hidden, std::u16string name, std::u16string description)
    {
        return change_feat(feat, hidden, name, description);
    };

    lua.create_named_table("FEAT");
    lua.create_named_table("FEAT_UID");
    for (unsigned int i = 0; i < g_AllAchievements.size(); ++i)
    {
        lua["FEAT_UID"][g_AchievementNames[i]] = g_AllAchievements[i];
        lua["FEAT"][g_AchievementNames[i]] = i + 1;
    }
    lua["FEAT_NAME"] = sol::as_table(g_AchievementNames);
}
} // namespace NSteam
