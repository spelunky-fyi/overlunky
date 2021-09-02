#pragma once

#include "level_api_types.hpp"

#include <string>
#include <vector>

#include <sol/forward.hpp>

using LAYER = int; // NoAlias

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
    bool set_room_template(uint32_t x, uint32_t y, LAYER l, ROOM_TEMPLATE room_template);
    /// Marks the room as the origin of a machine room, should be the top-left corner of the machine room
    bool mark_as_machine_room_origin(uint32_t x, uint32_t y, LAYER l);
    /// Force a spawn chance for this level, has the same restrictions as specifying the spawn chance in the .lvl file.
    /// Note that the actual chance to spawn is `1/inverse_chance` and that is also slightly skewed because of technical reasons.
    /// Returns `false` if the given chance is not defined.
    bool set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, uint32_t inverse_chance);
    /// Change the amount of extra spawns for the given `extra_spawn_id`.
    void set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer);
    /// Defines a new short tile code, automatically picks an unused one
    /// Returns `nil` if all possible short tile codes are already in use
    std::optional<SHORT_TILE_CODE> define_short_tile_code(ShortTileCodeDef short_tile_code_def);
    /// Overrides a specific short tile code
    void change_short_tile_code(SHORT_TILE_CODE short_tile_code, ShortTileCodeDef short_tile_code_def);
};
struct PreHandleRoomTilesContext
{
    /// Gets the tile code at the specified tile coordinate
    /// Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH` and `0 <= ty < CONST.ROOM_HEIGHT`
    /// Also returns `nil` if `layer == LAYER.BACK` and the room does not have a backrooms
    std::optional<SHORT_TILE_CODE> get_short_tile_code(uint8_t tx, uint8_t ty, LAYER layer) const;
    /// Sets the tile code at the specified tile coordinate
    /// Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH` and `0 <= ty < CONST.ROOM_HEIGHT`
    /// Also returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
    bool set_short_tile_code(uint8_t tx, uint8_t ty, LAYER layer, SHORT_TILE_CODE short_tile_code);
    /// Check whether the room has a back layer
    bool has_back_layer() const;
    /// Add a back layer filled with all `0` if there is no back layer yet
    /// Does nothing if there already is a backlayer
    void add_empty_back_layer();
    /// Add a back layer that is a copy of the front layer
    /// Does nothing if there already is a backlayer
    void add_copied_back_layer();

    const LevelGenRoomData& get_room_data() const;
    LevelGenRoomData& get_mutable_room_data();

    const LevelGenRoomData room_data;
    std::optional<LevelGenRoomData> modded_room_data;
};

namespace NLevel
{
void register_usertypes(sol::state& lua);
};
