#pragma once

#include <cstdint>  // for uint32_t, int32_t
#include <optional> // for optional
#include <string>   // for string
#include <tuple>    // for tuple
#include <vector>   // for vector

#include "aliases.hpp"         // for LAYER, SHORT_TILE_CODE, PROCEDURAL_CH...
#include "level_api_types.hpp" // for ShortTileCodeDef (ptr only), LevelGen...

namespace sol
{
class state;
} // namespace sol

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
    bool set_room_template(uint32_t x, uint32_t y, LAYER layer, ROOM_TEMPLATE room_template);
    /// Marks the room as the origin of a machine room, should be the top-left corner of the machine room
    /// Run this after setting the room template for the room, otherwise the machine room will not spawn correctly
    bool mark_as_machine_room_origin(uint32_t x, uint32_t y, LAYER layer);
    /// Marks the room as a set-room, a corresponding `setroomy-x` template must be loaded, else the game will crash
    bool mark_as_set_room(uint32_t x, uint32_t y, LAYER layer);
    /// Unmarks the room as a set-room
    bool unmark_as_set_room(uint32_t x, uint32_t y, LAYER layer);
    /// Set the shop type for a specific room, does nothing if the room is not a shop
    bool set_shop_type(uint32_t x, uint32_t y, LAYER layer, int32_t shop_type);
    /// Force a spawn chance for this level, has the same restrictions as specifying the spawn chance in the .lvl file.
    /// Note that the actual chance to spawn is `1/inverse_chance` and that is also slightly skewed because of technical reasons.
    /// Returns `false` if the given chance is not defined.
    bool set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, uint32_t inverse_chance);
    /// Correctly sets the template of a backlayer room. Be careful replacing some room templates.
    /// Can cause crashes if you set a backlayer room and then a room in the front that only has !dual rooms tries to spawn (e.g. the udjat entrance, during the `ON.POST_ROOM_GENERATION` callback)
    /// Although, if the room template has variants without !dual, they will be chosen to allow the set backlayer room to exist.
    /// You can use the `ON.PRE/POST_SPAWN_BACKLAYER_ROOMS` and `ON.PRE/POST_SET_RANDOM_BACKLAYER_ROOMS` callbacks to make it easier to prevent crashes, but front layer rooms will already be spawned.
    void set_backlayer_room_template(uint32_t x, uint32_t y, ROOM_TEMPLATE room_template);
    /// Change the amount of extra spawns for the given `extra_spawn_id`.
    void set_num_extra_spawns(std::uint32_t extra_spawn_id, std::uint32_t num_spawns_front_layer, std::uint32_t num_spawns_back_layer);
    /// Defines a new short tile code, automatically picks an unused character or returns a used one in case of an exact match
    /// Returns `nil` if all possible short tile codes are already in use
    std::optional<SHORT_TILE_CODE> define_short_tile_code(ShortTileCodeDef short_tile_code_def);
    /// Overrides a specific short tile code, this means it will change for the whole level
    void change_short_tile_code(SHORT_TILE_CODE short_tile_code, ShortTileCodeDef short_tile_code_def);
};
struct PreHandleRoomTilesContext
{
    /// Gets the tile code at the specified tile coordinate
    /// Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH`, `0 <= ty < CONST.ROOM_HEIGHT` and `layer` in `{LAYER.FRONT, LAYER.BACK}`
    /// Also returns `nil` if `layer == LAYER.BACK` and the room does not have a back layer
    std::optional<SHORT_TILE_CODE> get_short_tile_code(uint32_t tx, uint32_t ty, LAYER layer) const;
    /// Sets the tile code at the specified tile coordinate
    /// Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH`, `0 <= ty < CONST.ROOM_HEIGHT` and `layer` in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`
    /// Also returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
    bool set_short_tile_code(uint32_t tx, uint32_t ty, LAYER layer, SHORT_TILE_CODE short_tile_code);
    /// Finds all places a short tile code is used in the room, `layer` must be in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`
    /// Returns an empty list if `layer == LAYER.BACK` and the room does not have a back layer
    std::vector<std::tuple<uint32_t, uint32_t, LAYER>> find_all_short_tile_codes(LAYER layer, SHORT_TILE_CODE short_tile_code);
    /// Replaces all instances of `short_tile_code` in the given layer with `replacement_short_tile_code`, `layer` must be in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`
    /// Returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
    bool replace_short_tile_code(LAYER layer, SHORT_TILE_CODE short_tile_code, SHORT_TILE_CODE replacement_short_tile_code);
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
