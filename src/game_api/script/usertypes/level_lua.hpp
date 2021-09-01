#pragma once

#include <string>
#include <vector>
#include "aliases.hpp"

#include <sol/forward.hpp>

struct PreLoadLevelFilesContext
{
    /// Block all loading `.lvl` files and instead load the specified `.lvl` files. This includes `generic.lvl` so if you need it specify it here.
    /// All `.lvl` files are loaded relative to `Data/Levels`, but they can be completely custom `.lvl` files that ship with your mod so long as they are in said folder.
    /// Use at your own risk, some themes/levels expect a certain level file to be loaded.
    void override_level_files(std::vector<std::string> levels);
    /// Load additional levels files other than the ones that would usually be loaded. Stacks with `override_level_files` if that was called first.
    /// All `.lvl` files are loaded relative to `Data/Levels`, but they can be completely custom `.lvl` files that ship with your mod so long as they are in said folder.
    void add_level_files(std::vector<std::string> levels);
};
struct PostRoomGenerationContext
{
    /// Set the room template at the given index and layer, returns `false` if the index is outside of the level.
    bool set_room_template(int x, int y, int l, ROOM_TEMPLATE room_template);
    /// Force a spawn chance for this level, has the same restrictions as specifying the spawn chance in the .lvl file.
    /// Note that the actual chance to spawn is `1/inverse_chance` and that is also slightly skewed because of technical reasons.
    /// Returns `false` if the given chance is not defined.
    bool set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, uint32_t inverse_chance);
    /// Change the amount of extra spawns for the given `extra_spawn_id`.
    void set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer);
};

namespace NLevel
{
void register_usertypes(sol::state& lua);
};
