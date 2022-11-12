#include "steam_lua.hpp"

#include <detours.h> // for DetourAttach, DetourTransactionBegin
#include <sol/sol.hpp>

#include "memory.hpp"
#include "steam_api.hpp"
#include "vtable_hook.hpp" // for get_hook_function, register_hook_function

namespace NSteam
{
void register_usertypes(sol::state& lua)
{
    /// Check if the user has performed a feat (Real Steam achievement or a hooked one). Returns: `bool unlocked, bool hidden, string name, string description`
    lua["get_feat"] = get_feat;

    /// Get the visibility of a feat
    lua["get_feat_hidden"] = get_feat_hidden;

    /// Set the visibility of a feat
    lua["set_feat_hidden"] = set_feat_hidden;

    /// Helper function to set the title and description strings for a FEAT with change_string, as well as the hidden state.
    lua["change_feat"] = [](FEAT feat, bool hidden, std::u16string name, std::u16string description)
    {
        return change_feat(feat, hidden, name, description);
    };

    lua.create_named_table("FEAT");
    for (unsigned int i = 0; i < g_AllAchievements.size(); ++i)
    {
        lua["FEAT"][g_AchievementNames[i]] = i + 1;
    }
}
} // namespace NSteam
