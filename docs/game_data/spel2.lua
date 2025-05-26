---@meta
---@diagnostic disable: duplicate-doc-alias

---@class Meta
---@field name string
---@field version string
---@field description string
---@field author string

---@type Meta
meta = nil

---@type StateMemory
state = nil
---@type GameManager
game_manager = nil
---@type Online
online = nil
---@type Player[]
players = nil
---@type SaveData
savegame = nil
---@type any
options = nil
---@type PRNG
prng = nil

---Create a global `exports` table and put stuff in it,
---and other scripts will be able to `import` your script like a library
---@class Exports

---@type Exports
exports = nil

---The json library converts tables to json and json to tables
---Check https://github.com/rxi/json.lua for more information
---@class Json
---@field decode fun(str: string): table @Decode a json string into a table
---@field encode fun(tbl: table): string @Encode a table into a json string
---@type Json
json = nil

io.open_data = io.open
io.open_mod = io.open
os.remove_data = os.remove
os.remove_mod = os.remove


-- Functions


---Return any type of object or multiple objects as a debug string.
---@vararg any
---@return string
function inspect(...) end
---Formatting function, use e.g. as f"my_var = {my_var}"
---@param f_string string
---@return string
function f(f_string) end
---Formatting function, use e.g. as F"my_var = {my_var}"
---@param f_string string
---@return string
function F(f_string) end

---Returns Player (or PlayerGhost if `get_player(1, true)`) with this player slot
---@param slot integer
---@param or_ghost boolean
---@return Player
function get_player(slot, or_ghost) end
---Returns PlayerGhost with this player slot 1..4
---@param slot integer
---@return PlayerGhost
function get_playerghost(slot) end
---Standard lua print function, prints directly to the terminal but not to the game
---@return nil
function lua_print() end
---Print a log message on screen.
---@param message string
---@return nil
function print(message) end
---Print a log message to in-game console with a comment identifying the script that sent it.
---@param message string
---@return nil
function console_print(message) end
---Prinspect to in-game console.
---@vararg any
---@return nil
function console_prinspect(...) end
---Same as `print`
---@param message string
---@return nil
function message(message) end
---Prints any type of object by first funneling it through `inspect`, no need for a manual `tostring` or `inspect`.
---@vararg any
---@return nil
function prinspect(...) end
---Same as `prinspect`
---@vararg any
---@return nil
function messpect(...) end
---Dump the object (table, container, class) as a recursive table, for pretty printing in console. Don't use this for anything except debug printing. Unsafe.
---@param any any
---@param depth integer?
---@return table
function dump(any, depth) end
---Adds a command that can be used in the console.
---@param name string
---@param cmd function
---@return nil
function register_console_command(name, cmd) end
---Returns unique id for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback). You can also return `false` from your function to clear the callback.
---Add per level callback function to be called every `frames` engine frames
---Ex. frames = 100 - will call the function on 100th frame from this point. This might differ in the exact timing of first frame depending as in what part of the frame you call this function
---or even be one frame off if called right before the time_level variable is updated
---If you require precise timing, choose the start of your interval in one of those safe callbacks:
---The SCREEN callbacks: from ON.LOGO to ON.ONLINE_LOBBY or custom callbacks ON.FRAME, ON.SCREEN, ON.START, ON.LOADING, ON.RESET, ON.POST_UPDATE
---Timer is paused on pause and cleared on level transition.
---@param cb function
---@param frames integer
---@return CallbackId
function set_interval(cb, frames) end
---Returns unique id for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback).
---Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
---@param cb function
---@param frames integer
---@return CallbackId
function set_timeout(cb, frames) end
---Returns unique id for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback). You can also return `false` from your function to clear the callback.
---Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.
---@param cb function
---@param frames integer
---@return CallbackId
function set_global_interval(cb, frames) end
---Returns unique id for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback).
---Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.
---@param cb function
---@param frames integer
---@return CallbackId
function set_global_timeout(cb, frames) end
---Returns unique id for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback).
---Add global callback function to be called on an [event](https://spelunky-fyi.github.io/overlunky/#Events).
---@param cb function
---@param event ON
---@return CallbackId
function set_callback(cb, event) end
---Clear previously added callback `id` or call without arguments inside any callback to clear that callback after it returns.
---@param id CallbackId?
---@return nil
function clear_callback(id) end
---Load another script by id "author/name" and import its `exports` table. Returns:
---
---- `table` if the script has exports
---- `nil` if the script was found but has no exports
---- `false` if the script was not found but optional is set to true
---- an error if the script was not found and the optional argument was not set
---@param id string
---@param version string?
---@param optional boolean?
---@return table
function import(id, version, optional) end
---Check if another script is enabled by id "author/name". You should probably check this after all the other scripts have had a chance to load.
---@param id string
---@param version string
---@return boolean
function script_enabled(id, version) end
---Some random hash function
---@param x integer
---@return integer
function lowbias32(x) end
---Reverse of some random hash function
---@param x integer
---@return integer
function lowbias32_r(x) end
---Get your sanitized script id to be used in import.
---@return string
function get_id() end
---Show a message that looks like a level feeling.
---@param message string
---@return nil
function toast(message) end
---Show a message coming from an entity
---@param entity_uid integer
---@param message string
---@param sound_type integer
---@param top boolean
---@return nil
function say(entity_uid, message, sound_type, top) end
---Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
---limits, you can override them in the UI with double click.
---@param name string
---@param desc string
---@param long_desc string
---@param value integer
---@param min integer
---@param max integer
---@return nil
function register_option_int(name, desc, long_desc, value, min, max) end
---Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
---limits, you can override them in the UI with double click.
---@param name string
---@param desc string
---@param long_desc string
---@param value number
---@param min number
---@param max number
---@return nil
function register_option_float(name, desc, long_desc, value, min, max) end
---Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
---@param name string
---@param desc string
---@param long_desc string
---@param value boolean
---@return nil
function register_option_bool(name, desc, long_desc, value) end
---Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
---@param name string
---@param desc string
---@param long_desc string
---@param value string
---@return nil
function register_option_string(name, desc, long_desc, value) end
---Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
---with a double `\0\0` at the end. `value` is the default index 1..n.
---@param name string
---@param desc string
---@param long_desc string
---@param opts string
---@param value integer
---@return nil
function register_option_combo(name, desc, long_desc, opts, value) end
---Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.
---@param name string
---@param desc string
---@param long_desc string
---@param on_click function
---@return nil
function register_option_button(name, desc, long_desc, on_click) end
---Add custom options using the window drawing functions. Everything drawn in the callback will be rendered in the options window and the return value saved to `options[name]` or overwriting the whole `options` table if using and empty name.
---`value` is the default value, and pretty important because anything defined in the callback function will only be defined after the options are rendered. See the example for details.
---The callback signature is optional<any> on_render(GuiDrawContext draw_ctx)
---@param name string
---@param value any
---@param on_render fun(draw_ctx: GuiDrawContext): any?
---@return nil
function register_option_callback(name, value, on_render) end
---Removes an option by name. To make complicated conditionally visible options you should probably just use register_option_callback though.
---@param name string
---@return nil
function unregister_option(name) end
---Spawn liquids, always spawns in the front layer, will have fun effects if `entity_type` is not a liquid (only the short version, without velocity etc.).
---Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
---`liquid_flags` - not much known about, 2 - will probably crash the game, 3 - pause_physics, 6-12 is probably agitation, surface_tension etc. set to 0 to ignore
---`amount` - it will spawn amount x amount (so 1 = 1, 2 = 4, 3 = 6 etc.), `blobs_separation` is optional
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@return nil
function spawn_liquid(entity_type, x, y) end
---Spawn liquids, always spawns in the front layer, will have fun effects if `entity_type` is not a liquid (only the short version, without velocity etc.).
---Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
---`liquid_flags` - not much known about, 2 - will probably crash the game, 3 - pause_physics, 6-12 is probably agitation, surface_tension etc. set to 0 to ignore
---`amount` - it will spawn amount x amount (so 1 = 1, 2 = 4, 3 = 6 etc.), `blobs_separation` is optional
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param velocityx number
---@param velocityy number
---@param liquid_flags integer
---@param amount integer
---@param blobs_separation number
---@return nil
function spawn_liquid(entity_type, x, y, velocityx, velocityy, liquid_flags, amount, blobs_separation) end
---Spawn an entity in position with some velocity and return the uid of spawned entity.
---Uses level coordinates with [LAYER.FRONT](#LAYER) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYER(n), where (n) is a player number (1-4).
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@param vx number
---@param vy number
---@return integer
function spawn_entity(entity_type, x, y, layer, vx, vy) end
---Short for [spawn_entity](https://spelunky-fyi.github.io/overlunky/#spawn_entity).
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@param vx number
---@param vy number
---@return integer
function spawn(entity_type, x, y, layer, vx, vy) end
---Spawns an entity directly on the floor below the tile at the given position.
---Use this to avoid the little fall that some entities do when spawned during level gen callbacks.
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function spawn_entity_snapped_to_floor(entity_type, x, y, layer) end
---Short for [spawn_entity_snapped_to_floor](https://spelunky-fyi.github.io/overlunky/#spawn_entity_snapped_to_floor).
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function spawn_on_floor(entity_type, x, y, layer) end
---Spawn a grid entity, such as floor or traps, that snaps to the grid.
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function spawn_grid_entity(entity_type, x, y, layer) end
---Same as `spawn_entity` but does not trigger any pre-entity-spawn callbacks, so it will not be replaced by another script
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@param vx number
---@param vy number
---@return integer
function spawn_entity_nonreplaceable(entity_type, x, y, layer, vx, vy) end
---Short for [spawn_entity_nonreplaceable](https://spelunky-fyi.github.io/overlunky/#spawn_entity_nonreplaceable).
---@param entity_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@param vx number
---@param vy number
---@return integer
function spawn_critical(entity_type, x, y, layer, vx, vy) end
---Spawn a door to another world, level and theme and return the uid of spawned entity.
---Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
---@param x number
---@param y number
---@param layer LAYER
---@param w integer
---@param l integer
---@param t integer
---@return integer
function spawn_door(x, y, layer, w, l, t) end
---Short for [spawn_door](https://spelunky-fyi.github.io/overlunky/#spawn_door).
---@param x number
---@param y number
---@param layer LAYER
---@param w integer
---@param l integer
---@param t integer
---@return integer
function door(x, y, layer, w, l, t) end
---Spawn a door to backlayer.
---@param x number
---@param y number
---@return nil
function spawn_layer_door(x, y) end
---Short for [spawn_layer_door](https://spelunky-fyi.github.io/overlunky/#spawn_layer_door).
---@param x number
---@param y number
---@return nil
function layer_door(x, y) end
---Spawns apep with the choice if it going left or right, if you want the game to choose use regular spawn functions with `ENT_TYPE.MONS_APEP_HEAD`
---@param x number
---@param y number
---@param layer LAYER
---@param right boolean
---@return integer
function spawn_apep(x, y, layer, right) end
---Spawns and grows a tree
---@param x number
---@param y number
---@param layer LAYER
---@param height integer
---@return integer
function spawn_tree(x, y, layer, height) end
---Spawns and grows a tree
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function spawn_tree(x, y, layer) end
---Spawns and grows mushroom, height relates to the trunk, without it, it will roll the game default 3-5 height
---Regardless, if there is not enough space, it will spawn shorter one or if there is no space even for the smallest one, it will just not spawn at all
---Returns uid of the base or -1 if it wasn't able to spawn
---@param x number
---@param y number
---@param l LAYER
---@param height integer
---@return integer
function spawn_mushroom(x, y, l, height) end
---Spawns and grows mushroom, height relates to the trunk, without it, it will roll the game default 3-5 height
---Regardless, if there is not enough space, it will spawn shorter one or if there is no space even for the smallest one, it will just not spawn at all
---Returns uid of the base or -1 if it wasn't able to spawn
---@param x number
---@param y number
---@param l LAYER
---@return integer
function spawn_mushroom(x, y, l) end
---Spawns an already unrolled rope as if created by player
---@param x number
---@param y number
---@param layer LAYER
---@param texture TEXTURE
---@return integer
function spawn_unrolled_player_rope(x, y, layer, texture) end
---Spawns an already unrolled rope as if created by player
---@param x number
---@param y number
---@param layer LAYER
---@param texture TEXTURE
---@param max_length integer
---@return integer
function spawn_unrolled_player_rope(x, y, layer, texture, max_length) end
---Spawn a player in given location, if player of that slot already exist it will spawn clone, the game may crash as this is very unexpected situation
---If you want to respawn a player that is a ghost, set in his Inventory `health` to above 0, and `time_of_death` to 0 and call this function, the ghost entity will be removed automatically
---@param player_slot integer
---@param x number?
---@param y number?
---@param layer LAYER?
---@return integer
function spawn_player(player_slot, x, y, layer) end
---Spawn the PlayerGhost entity, it will not move and not be connected to any player, you can then use [steal_input](https://spelunky-fyi.github.io/overlunky/#steal_input) and send_input to control it
---or change it's `player_inputs` to the `input` of real player so he can control it directly
---@param char_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function spawn_playerghost(char_type, x, y, layer) end
---Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
---This is run before the entity is spawned, spawn your own entity and return its uid to replace the intended spawn.
---In many cases replacing the intended entity won't have the intended effect or will even break the game, so use only if you really know what you're doing.
---The callback signature is optional<int> pre_entity_spawn(ENT_TYPE entity_type, float x, float y, int layer, Entity overlay_entity, SPAWN_TYPE spawn_flags)
---@param cb fun(entity_type: ENT_TYPE, x: number, y: number, layer: integer, overlay_entity: Entity, spawn_flags: SPAWN_TYPE): integer?
---@param flags SPAWN_TYPE
---@param mask integer
---@vararg any
---@return CallbackId
function set_pre_entity_spawn(cb, flags, mask, ...) end
---Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
---This is run right after the entity is spawned but before and particular properties are changed, e.g. owner or velocity.
---The callback signature is nil post_entity_spawn(Entity ent, SPAWN_TYPE spawn_flags)
---@param cb fun(ent: Entity, spawn_flags: SPAWN_TYPE): nil
---@param flags SPAWN_TYPE
---@param mask integer
---@vararg any
---@return CallbackId
function set_post_entity_spawn(cb, flags, mask, ...) end
---Warp to a level immediately.
---@param world integer
---@param level integer
---@param theme integer
---@return nil
function warp(world, level, theme) end
---Set seed and reset run.
---@param seed integer
---@return nil
function set_seed(seed) end
---Enable/disable godmode for players.
---@param g boolean
---@return nil
function god(g) end
---Enable/disable godmode for companions.
---@param g boolean
---@return nil
function god_companions(g) end
---Set the zoom level used in levels and shops. 13.5 is the default, or 12.5 for shops. See zoom_reset.
---@param level number
---@return nil
function zoom(level) end
---Reset the default zoom levels for all areas and sets current zoom level to 13.5.
---@return nil
function zoom_reset() end
---Teleport entity to coordinates with optional velocity
---@param uid integer
---@param x number
---@param y number
---@param vx number
---@param vy number
---@return nil
function move_entity(uid, x, y, vx, vy) end
---Teleport entity to coordinates with optional velocity
---@param uid integer
---@param x number
---@param y number
---@param vx number
---@param vy number
---@param layer LAYER
---@return nil
function move_entity(uid, x, y, vx, vy, layer) end
---Teleport grid entity, the destination should be whole number, this ensures that the collisions will work properly
---@param uid integer
---@param x number
---@param y number
---@param layer LAYER
---@return nil
function move_grid_entity(uid, x, y, layer) end
---Destroy the grid entity (by uid or position), and its item entities, removing them from the grid without dropping particles or gold.
---Will also destroy monsters or items that are standing on a linked activefloor or chain, though excludes MASK.PLAYER to prevent crashes
---@param uid integer
---@return nil
function destroy_grid(uid) end
---Destroy the grid entity (by uid or position), and its item entities, removing them from the grid without dropping particles or gold.
---Will also destroy monsters or items that are standing on a linked activefloor or chain, though excludes MASK.PLAYER to prevent crashes
---@param x number
---@param y number
---@param layer LAYER
---@return nil
function destroy_grid(x, y, layer) end
---Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
---@param uid integer
---@param w integer
---@param l integer
---@param t integer
---@return nil
function set_door_target(uid, w, l, t) end
---Short for [set_door_target](https://spelunky-fyi.github.io/overlunky/#set_door_target).
---@param uid integer
---@param w integer
---@param l integer
---@param t integer
---@return nil
function set_door(uid, w, l, t) end
---Get door target `world`, `level`, `theme`
---@param uid integer
---@return integer, integer, integer
function get_door_target(uid) end
---Set the contents of [Coffin](https://spelunky-fyi.github.io/overlunky/#Coffin), [Present](https://spelunky-fyi.github.io/overlunky/#Present), [Pot](https://spelunky-fyi.github.io/overlunky/#Pot), [Container](https://spelunky-fyi.github.io/overlunky/#Container)
---Check the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md) for what the exact ENT_TYPE's can this function affect
---@param uid integer
---@param item_entity_type ENT_TYPE
---@return nil
function set_contents(uid, item_entity_type) end
---Get the Entity behind an uid, converted to the correct type. To see what type you will get, consult the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md)
---@param uid integer
---@return Entity
function get_entity(uid) end
---Get the [EntityDB](https://spelunky-fyi.github.io/overlunky/#EntityDB) behind an ENT_TYPE...
---@param id ENT_TYPE
---@return EntityDB
function get_type(id) end
---Gets a grid entity, such as floor or spikes, at the given position and layer.
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function get_grid_entity_at(x, y, layer) end
---Get uids of static entities overlapping this grid position (decorations, backgrounds etc.)
---@param x number
---@param y number
---@param layer LAYER
---@return integer[]
function get_entities_overlapping_grid(x, y, layer) end
---Returns a list of all uids in `entities` for which `predicate(get_entity(uid))` returns true
---@param entities integer[]
---@param predicate function
---@return integer[]
function filter_entities(entities, predicate) end
---Get uids of entities by some conditions ([ENT_TYPE](https://spelunky-fyi.github.io/overlunky/#ENT_TYPE), [MASK](https://spelunky-fyi.github.io/overlunky/#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types.
---Recommended to always set the mask, even if you look for one entity type
---@param entity_types ENT_TYPE[]
---@param mask integer
---@param layer LAYER
---@return integer[]
function get_entities_by(entity_types, mask, layer) end
---Get uids of entities by some conditions ([ENT_TYPE](https://spelunky-fyi.github.io/overlunky/#ENT_TYPE), [MASK](https://spelunky-fyi.github.io/overlunky/#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types.
---Recommended to always set the mask, even if you look for one entity type
---@param entity_type ENT_TYPE
---@param mask integer
---@param layer LAYER
---@return integer[]
function get_entities_by(entity_type, mask, layer) end
---Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
---You can even pass a table!
---This function can be slower than the [get_entities_by](https://spelunky-fyi.github.io/overlunky/#get_entities_by) with the mask parameter filled
---@vararg any
---@return integer[]
function get_entities_by_type(...) end
---Get uids of matching entities inside some radius ([ENT_TYPE](https://spelunky-fyi.github.io/overlunky/#ENT_TYPE), [MASK](https://spelunky-fyi.github.io/overlunky/#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
---Recommended to always set the mask, even if you look for one entity type
---@param entity_types ENT_TYPE[]
---@param mask integer
---@param x number
---@param y number
---@param layer LAYER
---@param radius number
---@return integer[]
function get_entities_at(entity_types, mask, x, y, layer, radius) end
---Get uids of matching entities inside some radius ([ENT_TYPE](https://spelunky-fyi.github.io/overlunky/#ENT_TYPE), [MASK](https://spelunky-fyi.github.io/overlunky/#MASK)). Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
---Recommended to always set the mask, even if you look for one entity type
---@param entity_type ENT_TYPE
---@param mask integer
---@param x number
---@param y number
---@param layer LAYER
---@param radius number
---@return integer[]
function get_entities_at(entity_type, mask, x, y, layer, radius) end
---Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
---@param entity_types ENT_TYPE[]
---@param mask integer
---@param hitbox AABB
---@param layer LAYER
---@return integer[]
function get_entities_overlapping_hitbox(entity_types, mask, hitbox, layer) end
---Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
---@param entity_type ENT_TYPE
---@param mask integer
---@param hitbox AABB
---@param layer LAYER
---@return integer[]
function get_entities_overlapping_hitbox(entity_type, mask, hitbox, layer) end
---Attaches `attachee` to `overlay`, similar to setting `get_entity(attachee).overlay = get_entity(overlay)`.
---However this function offsets `attachee` (so you don't have to) and inserts it into `overlay`'s inventory.
---@param overlay_uid integer
---@param attachee_uid integer
---@return nil
function attach_entity(overlay_uid, attachee_uid) end
---Get the `flags` field from entity by uid
---@param uid integer
---@return integer
function get_entity_flags(uid) end
---Set the `flags` field from entity by uid
---@param uid integer
---@param flags integer
---@return nil
function set_entity_flags(uid, flags) end
---Get the `more_flags` field from entity by uid
---@param uid integer
---@return integer
function get_entity_flags2(uid) end
---Set the `more_flags` field from entity by uid
---@param uid integer
---@param flags integer
---@return nil
function set_entity_flags2(uid, flags) end
---Get `state.level_flags`
---@return integer
function get_level_flags() end
---Set `state.level_flags`
---@param flags integer
---@return nil
function set_level_flags(flags) end
---Get the ENT_TYPE... of the entity by uid
---@param uid integer
---@return ENT_TYPE
function get_entity_type(uid) end
---Get the current set zoom level
---@return number
function get_zoom_level() end
---Get the game coordinates at the screen position (`x`, `y`)
---@param x number
---@param y number
---@return number, number
function game_position(x, y) end
---Translate an entity position to screen position to be used in drawing functions
---@param x number
---@param y number
---@return number, number
function screen_position(x, y) end
---Translate a distance of `x` tiles to screen distance to be be used in drawing functions
---@param x number
---@return number
function screen_distance(x) end
---Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
---you're standing on, not real level coordinates.
---@param uid integer
---@return number, number, integer
function get_position(uid) end
---Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...
---@param uid integer
---@return number, number, integer
function get_render_position(uid) end
---Get velocity `vx, vy` of an entity by uid. Use this to get velocity relative to the game world, (the `Entity.velocityx/velocityy` are relative to `Entity.overlay`). Only works for movable or liquid entities
---@param uid integer
---@return number, number
function get_velocity(uid) end
---Remove item by uid from entity. `check_autokill` defaults to true, checks if entity should be killed when missing overlay and kills it if so (can help with avoiding crashes)
---@param uid integer
---@param item_uid integer
---@param check_autokill boolean?
---@return nil
function entity_remove_item(uid, item_uid, check_autokill) end
---Spawns and attaches ball and chain to `uid`, the initial position of the ball is at the entity position plus `off_x`, `off_y`
---@param uid integer
---@param off_x number
---@param off_y number
---@return integer
function attach_ball_and_chain(uid, off_x, off_y) end
---Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`
---@param entity_type ENT_TYPE
---@param over_uid integer
---@param x number
---@param y number
---@return integer
function spawn_entity_over(entity_type, over_uid, x, y) end
---Short for [spawn_entity_over](https://spelunky-fyi.github.io/overlunky/#spawn_entity_over)
---@param entity_type ENT_TYPE
---@param over_uid integer
---@param x number
---@param y number
---@return integer
function spawn_over(entity_type, over_uid, x, y) end
---Check if the entity `uid` has some specific `item_uid` by uid in their inventory
---@param uid integer
---@param item_uid integer
---@return boolean
function entity_has_item_uid(uid, item_uid) end
---Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory, can also use table of entity_types
---@param uid integer
---@param entity_types ENT_TYPE[]
---@return boolean
function entity_has_item_type(uid, entity_types) end
---Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory, can also use table of entity_types
---@param uid integer
---@param entity_type ENT_TYPE
---@return boolean
function entity_has_item_type(uid, entity_type) end
---Gets uids of entities attached to given entity uid. Use `entity_type` and `mask` ([MASK](https://spelunky-fyi.github.io/overlunky/#MASK)) to filter, set them to 0 to return all attached entities.
---@param uid integer
---@param entity_types ENT_TYPE[]
---@param mask integer
---@return integer[]
function entity_get_items_by(uid, entity_types, mask) end
---Gets uids of entities attached to given entity uid. Use `entity_type` and `mask` ([MASK](https://spelunky-fyi.github.io/overlunky/#MASK)) to filter, set them to 0 to return all attached entities.
---@param uid integer
---@param entity_type ENT_TYPE
---@param mask integer
---@return integer[]
function entity_get_items_by(uid, entity_type, mask) end
---Kills an entity by uid. `destroy_corpse` defaults to `true`, if you are killing for example a caveman and want the corpse to stay make sure to pass `false`.
---@param uid integer
---@param destroy_corpse boolean?
---@return nil
function kill_entity(uid, destroy_corpse) end
---Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen.
---@param who_uid integer
---@param what_uid integer
---@return nil
function pick_up(who_uid, what_uid) end
---Drop held entity, `what_uid` optional, if set, it will check if entity is holding that entity first before dropping it
---@param who_uid integer
---@param what_uid integer?
---@return nil
function drop(who_uid, what_uid) end
---Unequips the currently worn backitem
---@param who_uid integer
---@return nil
function unequip_backitem(who_uid) end
---Returns the uid of the currently worn backitem, or -1 if wearing nothing
---@param who_uid integer
---@return integer
function worn_backitem(who_uid) end
---Apply changes made in [get_type](https://spelunky-fyi.github.io/overlunky/#get_type)() to entity instance by uid.
---@param uid integer
---@return nil
function apply_entity_db(uid) end
---Try to lock the exit at coordinates
---@param x number
---@param y number
---@return nil
function lock_door_at(x, y) end
---Try to unlock the exit at coordinates
---@param x number
---@param y number
---@return nil
function unlock_door_at(x, y) end
---Get the frame count from the main game state. You can use this to make some timers yourself, the engine runs at 60fps.
---This counter is paused if the pause is set with flags PAUSE.FADE or PAUSE.ANKH. Rolls back with online rollback etc.
---@return integer
function get_frame() end
---Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps. This counter keeps incrementing with game loop. Never stops.
---@return integer
function get_global_frame() end
---Get the current timestamp in milliseconds since the Unix Epoch.
---@return nil
function get_ms() end
---Make `mount_uid` carry `rider_uid` on their back. Only use this with actual mounts and living things.
---@param mount_uid integer
---@param rider_uid integer
---@return nil
function carry(mount_uid, rider_uid) end
---Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).
---@param threshold integer
---@return nil
function set_kapala_blood_threshold(threshold) end
---Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour).
---If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!
---@param icon_index integer
---@return nil
function set_kapala_hud_icon(icon_index) end
---Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
---Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
---Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!
---@param angle_increment number
---@param distance number
---@return nil
function modify_sparktraps(angle_increment, distance) end
---Activate custom variables for speed and distance in the `ITEM_SPARK`
---note: because those the variables are custom and game does not initiate them, you need to do it yourself for each spark, recommending `set_post_entity_spawn`
---default game values are: speed = -0.015, distance = 3.0
---@param activate boolean
---@return nil
function activate_sparktraps_hack(activate) end
---Set layer to search for storage items on
---@param layer LAYER
---@return nil
function set_storage_layer(layer) end
---Flip entity around by uid. All new entities face right by default.
---@param uid integer
---@return nil
function flip_entity(uid) end
---Sets the Y-level at which Olmec changes phases
---@param phase integer
---@param y number
---@return nil
function set_olmec_phase_y_level(phase, y) end
---Forces Olmec to stay on phase 0 (stomping)
---@param b boolean
---@return nil
function force_olmec_phase_0(b) end
---Determines when the ghost appears, either when the player is cursed or not
---@param normal integer
---@param cursed integer
---@return nil
function set_ghost_spawn_times(normal, cursed) end
---Determines whether the ghost appears when breaking the ghost pot
---@param enable boolean
---@return nil
function set_cursepot_ghost_enabled(enable) end
---Determines whether the time ghost appears, including the showing of the ghost toast
---@param b boolean
---@return nil
function set_time_ghost_enabled(b) end
---Determines whether the time jelly appears in cosmic ocean
---@param b boolean
---@return nil
function set_time_jelly_enabled(b) end
---Enables or disables the journal
---@param b boolean
---@return nil
function set_journal_enabled(b) end
---Enables or disables the default position based camp camera bounds, to set them manually yourself
---@param b boolean
---@return nil
function set_camp_camera_bounds_enabled(b) end
---Sets which entities are affected by a bomb explosion. Default = MASK.PLAYER | MASK.MOUNT | MASK.MONSTER | MASK.ITEM | MASK.ACTIVEFLOOR | MASK.FLOOR
---@param mask integer
---@return nil
function set_explosion_mask(mask) end
---Sets the maximum length of a thrown rope (anchor segment not included). Unfortunately, setting this higher than default (6) creates visual glitches in the rope, even though it is fully functional.
---@param length integer
---@return nil
function set_max_rope_length(length) end
---Checks whether a coordinate is inside a room containing an active shop. This function checks whether the shopkeeper is still alive.
---@param x number
---@param y number
---@param layer LAYER
---@return boolean
function is_inside_active_shop_room(x, y, layer) end
---Checks whether a coordinate is inside a shop zone, the rectangle where the camera zooms in a bit. Does not check if the shop is still active!
---@param x number
---@param y number
---@param layer LAYER
---@return boolean
function is_inside_shop_zone(x, y, layer) end
---Returns how many of a specific entity type Waddler has stored
---@param entity_type ENT_TYPE
---@return integer
function waddler_count_entity(entity_type) end
---Store an entity type in Waddler's storage. Returns the slot number the item was stored in or -1 when storage is full and the item couldn't be stored.
---@param entity_type ENT_TYPE
---@return integer
function waddler_store_entity(entity_type) end
---Removes an entity type from Waddler's storage. Second param determines how many of the item to remove (default = remove all)
---@param entity_type ENT_TYPE
---@param amount_to_remove integer
---@return nil
function waddler_remove_entity(entity_type, amount_to_remove) end
---Gets the 16-bit meta-value associated with the entity type in the associated slot
---@param slot integer
---@return integer
function waddler_get_entity_meta(slot) end
---Sets the 16-bit meta-value associated with the entity type in the associated slot
---@param slot integer
---@param meta integer
---@return nil
function waddler_set_entity_meta(slot, meta) end
---Gets the entity type of the item in the provided slot
---@param slot integer
---@return integer
function waddler_entity_type_in_slot(slot) end
---Spawn a companion (hired hand, player character, eggplant child)
---@param companion_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@return integer
function spawn_companion(companion_type, x, y, layer) end
---Calculate the tile distance of two entities by uid
---@param uid_a integer
---@param uid_b integer
---@return number
function distance(uid_a, uid_b) end
---Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
---inside these boundaries. The order is: left x, top y, right x, bottom y
---@return number, number, number, number
function get_bounds() end
---Same as [get_bounds](https://spelunky-fyi.github.io/overlunky/#get_bounds) but returns AABB struct instead of loose floats
---@return AABB
function get_aabb_bounds() end
---Gets the current camera position in the level
---@return number, number
function get_camera_position() end
---Sets the absolute current camera position without rubberbanding animation. Ignores camera bounds or currently focused uid, but doesn't clear them. Best used in ON.RENDER_PRE_GAME or similar. See Camera for proper camera handling with bounds and rubberbanding.
---@param cx number
---@param cy number
---@return nil
function set_camera_position(cx, cy) end
---Updates the camera focus according to the params set in Camera, i.e. to apply normal camera movement when paused etc.
---@return nil
function update_camera_position() end
---Set the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
---@param flags Flags
---@param bit integer
---@return Flags
function set_flag(flags, bit) end
---Clears the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
---@param flags Flags
---@param bit integer
---@return Flags
function clr_flag(flags, bit) end
---Flips the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
---@param flags Flags
---@param bit integer
---@return Flags
function flip_flag(flags, bit) end
---Returns true if the nth bit is set in the number.
---@param flags Flags
---@param bit integer
---@return boolean
function test_flag(flags, bit) end
---Set a bitmask in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
---@param flags Flags
---@param mask Flags
---@return Flags
function set_mask(flags, mask) end
---Clears a bitmask in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
---@param flags Flags
---@param mask Flags
---@return Flags
function clr_mask(flags, mask) end
---Flips the nth bit in a number. This doesn't actually change the variable you pass, it just returns the new value you can use.
---@param flags Flags
---@param mask Flags
---@return Flags
function flip_mask(flags, mask) end
---Returns true if a bitmask is set in the number.
---@param flags Flags
---@param mask Flags
---@return boolean
function test_mask(flags, mask) end
---Gets the resolution (width and height) of the screen
---@return integer, integer
function get_window_size() end
---Clears a callback that is specific to a screen.
---@param screen_id integer
---@param cb_id CallbackId
---@return nil
function clear_screen_callback(screen_id, cb_id) end
---Returns unique id for the callback to be used in [clear_screen_callback](https://spelunky-fyi.github.io/overlunky/#clear_screen_callback) or `nil` if screen_id is not valid.
---Sets a callback that is called right before the screen is drawn, return `true` to skip the default rendering.
---The callback signature is bool render_screen(Screen self, VanillaRenderContext render_ctx)
---@param screen_id integer
---@param fun fun(self: Screen, render_ctx: VanillaRenderContext): boolean
---@return CallbackId?
function set_pre_render_screen(screen_id, fun) end
---Returns unique id for the callback to be used in [clear_screen_callback](https://spelunky-fyi.github.io/overlunky/#clear_screen_callback) or `nil` if screen_id is not valid.
---Sets a callback that is called right after the screen is drawn.
---The callback signature is nil render_screen(Screen self, VanillaRenderContext render_ctx)
---@param screen_id integer
---@param fun fun(self: Screen, render_ctx: VanillaRenderContext): nil
---@return CallbackId?
function set_post_render_screen(screen_id, fun) end
---Returns unique id for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback) or `nil` if uid is not valid.
---Sets a callback that is called right when an player/hired hand is crushed/insta-gibbed, return `true` to skip the game's crush handling.
---The game's instagib function will be forcibly executed (regardless of whatever you return in the callback) when the entity's health is zero.
---This is so that when the entity dies (from other causes), the death screen still gets shown.
---Use this only when no other approach works, this call can be expensive if overused.
---The callback signature is bool on_player_instagib(Entity self)
---@param uid integer
---@param fun fun(self: Entity): boolean
---@return CallbackId?
function set_on_player_instagib(uid, fun) end
---Raise a signal and probably crash the game
---@return nil
function raise() end
---Convert the hash to stringid
---Check [strings00_hashed.str](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/strings00_hashed.str) for the hash values, or extract assets with modlunky and check those.
---@param hash integer
---@return STRINGID
function hash_to_stringid(hash) end
---Get string behind STRINGID, **don't use stringid directly for vanilla string**, use [hash_to_stringid](https://spelunky-fyi.github.io/overlunky/#hash_to_stringid) first
---Will return the string of currently choosen language
---@param string_id STRINGID
---@return string
function get_string(string_id) end
---Change string at the given id (**don't use stringid directly for vanilla string**, use [hash_to_stringid](https://spelunky-fyi.github.io/overlunky/#hash_to_stringid) first)
---This edits custom string and in game strings but changing the language in settings will reset game strings
---@param id STRINGID
---@param str string
---@return nil
function change_string(id, str) end
---Add custom string, currently can only be used for names of shop items (EntityDB->description)
---Returns STRINGID of the new string
---@param str string
---@return STRINGID
function add_string(str) end
---Get localized name of an entity from the journal, pass `fallback_strategy` as `true` to fall back to the `ENT_TYPE.*` enum name
---if the entity has no localized name
---@param type ENT_TYPE
---@param fallback_strategy boolean?
---@return string
function get_entity_name(type, fallback_strategy) end
---Adds custom name to the item by uid used in the shops
---This is better alternative to `add_string` but instead of changing the name for entity type, it changes it for this particular entity
---@param uid integer
---@param name string
---@return nil
function add_custom_name(uid, name) end
---Clears the name set with [add_custom_name](https://spelunky-fyi.github.io/overlunky/#add_custom_name)
---@param uid integer
---@return nil
function clear_custom_name(uid) end
---Calls the enter door function, position doesn't matter, can also enter closed doors (like COG, EW) without unlocking them
---@param player_uid integer
---@param door_uid integer
---@return nil
function enter_door(player_uid, door_uid) end
---Change ENT_TYPE's spawned by `FLOOR_SUNCHALLENGE_GENERATOR`, by default there are 4:
---{MONS_WITCHDOCTOR, MONS_VAMPIRE, MONS_SORCERESS, MONS_NECROMANCER}
---Use empty table as argument to reset to the game default
---@param ent_types ENT_TYPE[]
---@return nil
function change_sunchallenge_spawns(ent_types) end
---Change ENT_TYPE's spawned in dice shops (Madame Tusk as well), by default there are 25:
---{ITEM_PICKUP_BOMBBAG, ITEM_PICKUP_BOMBBOX, ITEM_PICKUP_ROPEPILE, ITEM_PICKUP_COMPASS, ITEM_PICKUP_PASTE, ITEM_PICKUP_PARACHUTE, ITEM_PURCHASABLE_CAPE, ITEM_PICKUP_SPECTACLES, ITEM_PICKUP_CLIMBINGGLOVES, ITEM_PICKUP_PITCHERSMITT,
---ENT_TYPE_ITEM_PICKUP_SPIKESHOES, ENT_TYPE_ITEM_PICKUP_SPRINGSHOES, ITEM_MACHETE, ITEM_BOOMERANG, ITEM_CROSSBOW, ITEM_SHOTGUN, ITEM_FREEZERAY, ITEM_WEBGUN, ITEM_CAMERA, ITEM_MATTOCK, ITEM_PURCHASABLE_JETPACK, ITEM_PURCHASABLE_HOVERPACK,
---ITEM_TELEPORTER, ITEM_PURCHASABLE_TELEPORTER_BACKPACK, ITEM_PURCHASABLE_POWERPACK}
---Min 6, Max 255, if you want less then 6 you need to write some of them more then once (they will have higher "spawn chance").
---If you use this function in the level with dice shop in it, you have to update `item_ids` in the [ITEM_DICE_PRIZE_DISPENSER](https://spelunky-fyi.github.io/overlunky/#PrizeDispenser).
---Use empty table as argument to reset to the game default
---@param ent_types ENT_TYPE[]
---@return nil
function change_diceshop_prizes(ent_types) end
---Change ENT_TYPE's spawned when you damage the altar, by default there are 6:
---{MONS_BAT, MONS_BEE, MONS_SPIDER, MONS_JIANGSHI, MONS_FEMALE_JIANGSHI, MONS_VAMPIRE}
---Max 255 types.
---Use empty table as argument to reset to the game default
---@param ent_types ENT_TYPE[]
---@return nil
function change_altar_damage_spawns(ent_types) end
---Change ENT_TYPE's spawned when Waddler dies, by default there are 3:
---{ITEM_PICKUP_COMPASS, ITEM_CHEST, ITEM_KEY}
---Max 255 types.
---Use empty table as argument to reset to the game default
---@param ent_types ENT_TYPE[]
---@return nil
function change_waddler_drop(ent_types) end
---Poisons entity, to cure poison set [Movable](https://spelunky-fyi.github.io/overlunky/#Movable).`poison_tick_timer` to -1
---@param entity_uid integer
---@return nil
function poison_entity(entity_uid) end
---Change how much health the ankh gives you after death, with every beat (the heart beat effect) it will add `beat_add_health` to your health,
---`beat_add_health` has to be divisor of `health` and can't be 0, otherwise the function does nothing. Set `health` to 0 to return to the game defaults
---If you set `health` above the game max health it will be forced down to the game max
---@param max_health integer
---@param beat_add_health integer
---@return nil
function modify_ankh_health_gain(max_health, beat_add_health) end
---Adds entity as shop item, has to be of [Purchasable](https://spelunky-fyi.github.io/overlunky/#Purchasable) type, check the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md) to find all the Purchasable entity types.
---Adding other entities will result in not obtainable items or game crash
---@param item_uid integer
---@param shop_owner_uid integer
---@return nil
function add_item_to_shop(item_uid, shop_owner_uid) end
---Change the amount of frames after the damage from poison is applied
---@param frames integer
---@return nil
function change_poison_timer(frames) end
---Creates a new Illumination. Don't forget to continuously call [refresh_illumination](https://spelunky-fyi.github.io/overlunky/#refresh_illumination), otherwise your light emitter fades out! Check out the [illumination.lua](https://github.com/spelunky-fyi/overlunky/blob/main/examples/illumination.lua) script for an example.
---Warning: this is only valid for current level!
---@param pos Vec2
---@param color Color
---@param type LIGHT_TYPE
---@param size number
---@param flags integer
---@param uid integer
---@param layer LAYER
---@return Illumination
function create_illumination(pos, color, type, size, flags, uid, layer) end
---Creates a new Illumination. Don't forget to continuously call [refresh_illumination](https://spelunky-fyi.github.io/overlunky/#refresh_illumination), otherwise your light emitter fades out! Check out the [illumination.lua](https://github.com/spelunky-fyi/overlunky/blob/main/examples/illumination.lua) script for an example.
---Warning: this is only valid for current level!
---@param color Color
---@param size number
---@param x number
---@param y number
---@return Illumination
function create_illumination(color, size, x, y) end
---Creates a new Illumination. Don't forget to continuously call [refresh_illumination](https://spelunky-fyi.github.io/overlunky/#refresh_illumination), otherwise your light emitter fades out! Check out the [illumination.lua](https://github.com/spelunky-fyi/overlunky/blob/main/examples/illumination.lua) script for an example.
---Warning: this is only valid for current level!
---@param color Color
---@param size number
---@param uid integer
---@return Illumination
function create_illumination(color, size, uid) end
---Refreshes an Illumination, keeps it from fading out, short for `illumination.timer = get_frame()`
---@param illumination Illumination
---@return nil
function refresh_illumination(illumination) end
---Removes all liquid that is about to go out of bounds, this would normally crash the game, but playlunky/overlunky patch this bug.
---The patch however does not destroy the liquids that fall pass the level bounds,
---so you may still want to use this function if you spawn a lot of liquid that may fall out of the level
---@return nil
function fix_liquid_out_of_bounds() end
---Return the name of the first matching number in an enum table
---@param enum table
---@param value integer
---@return string
function enum_get_name(enum, value) end
---Return all the names of a number in an enum table
---@param enum table
---@param value integer
---@return table<string>
function enum_get_names(enum, value) end
---Return the matching names for a bitmask in an enum table of masks
---@param enum table
---@param value integer
---@return table<string>
function enum_get_mask_names(enum, value) end
---Gets the specified setting, values might need to be interpreted differently per setting
---@param setting GAME_SETTING
---@return integer?
function get_setting(setting) end
---Sets the specified setting temporarily. These values are not saved and might reset to the users real settings if they visit the options menu. (Check example.) All settings are available in unsafe mode and only a smaller subset SAFE_SETTING by default for Hud and other visuals. Returns false, if setting failed.
---@param setting GAME_SETTING
---@param value integer
---@return boolean
function set_setting(setting, value) end
---Short for print(string.format(...))
---@return nil
function printf() end
---Spawn a Shopkeeper in the coordinates and make the room their shop. Returns the Shopkeeper uid. Also see [spawn_roomowner](https://spelunky-fyi.github.io/overlunky/#spawn_roomowner).
---@param x number
---@param y number
---@param layer LAYER
---@param room_template ROOM_TEMPLATE
---@return integer
function spawn_shopkeeper(x, y, layer, room_template) end
---Spawn a RoomOwner (or a few other like [CavemanShopkeeper](https://spelunky-fyi.github.io/overlunky/#CavemanShopkeeper)) in the coordinates and make them own the room, optionally changing the room template. Returns the RoomOwner uid.
---@param owner_type ENT_TYPE
---@param x number
---@param y number
---@param layer LAYER
---@param room_template ROOM_TEMPLATE
---@return integer
function spawn_roomowner(owner_type, x, y, layer, room_template) end
---Get the current adventure seed pair, or optionally what it was at the start of this run, because it changes every level.
---@param run_start boolean?
---@return integer, integer
function get_adventure_seed(run_start) end
---Set the current adventure seed pair. Use just before resetting a run to recreate an adventure run.
---@param first integer
---@param second integer
---@return nil
function set_adventure_seed(first, second) end
---Updates the floor collisions used by the liquids, set add to false to remove tile of collision, set to true to add one
---optional `layer` parameter to be used when liquid was moved to back layer using [set_liquid_layer](https://spelunky-fyi.github.io/overlunky/#set_liquid_layer)
---@param x number
---@param y number
---@param add boolean
---@param layer LAYER?
---@return nil
function update_liquid_collision_at(x, y, add, layer) end
---Optimized function to check for the amount of liquids at a certain position, by accessing a 2d array of liquids by third of a tile. Try the `liquids.lua` example to know better how it works.
---Returns a pair of water and lava, in that order.
---Water blobs increase the number by 2 on the grid, while lava blobs increase it by 3. The maximum is usually 6.
---Coarse water increase the number by 3, coarse and stagnant lava by 6. Combinations of both normal and coarse can make the number higher than 6.
---@param x number
---@param y number
---@param layer LAYER
---@return integer, integer
function get_liquids_at(x, y, layer) end
---Disable all crust item spawns, returns whether they were already disabled before the call
---@param disable boolean
---@return boolean
function disable_floor_embeds(disable) end
---Get the rva for a pattern name, used for debugging.
---@param address_name string
---@return string
function get_rva(address_name) end
---Get the rva for a vtable offset and index, used for debugging.
---@param offset VTABLE_OFFSET
---@param index integer
---@return string
function get_virtual_rva(offset, index) end
---Get memory address from a lua object
---@param o any
---@return nil
function get_address(o) end
---Log to spelunky.log
---@param message string
---@return nil
function log_print(message) end
---Immediately ends the run with the death screen, also calls the [save_progress](https://spelunky-fyi.github.io/overlunky/#save_progress)
---@return nil
function load_death_screen() end
---Saves the game to savegame.sav, unless game saves are blocked in the settings. Also runs the ON.SAVE callback. Fails and returns false, if you're trying to save too often (2s).
---@return boolean
function save_progress() end
---Runs the ON.SAVE callback. Fails and returns false, if you're trying to save too often (2s).
---@return boolean
function save_script() end
---Set the level number shown in the hud and journal to any string. This is reset to the default "%d-%d" automatically just before PRE_LOAD_SCREEN to a level or main menu, so use in PRE_LOAD_SCREEN, POST_LEVEL_GENERATION or similar for each level.
---Use "%d-%d" to reset to default manually. Does not affect the "...COMPLETED!" message in transitions or lines in "Dear Journal", you need to edit them separately with [change_string](https://spelunky-fyi.github.io/overlunky/#change_string).
---@param str string
---@return nil
function set_level_string(str) end
---Force the character unlocked in either ending to ENT_TYPE. Set to 0 to reset to the default guys. Does not affect the texture of the actual savior. (See example)
---@param type ENT_TYPE
---@return nil
function set_ending_unlock(type) end
---Get the thread-local version of state
---@return StateMemory
function get_local_state() end
---Get the thread-local version of players
---@return Player[]
function get_local_players() end
---List files in directory relative to the script root. Returns table of file/directory names or nil if not found.
---@param dir string?
---@return nil
function list_dir(dir) end
---List files in directory relative to the mods data directory (Mods/Data/...). Returns table of file/directory names or nil if not found.
---@param dir string?
---@return nil
function list_data_dir(dir) end
---List all char_*.png files recursively from Mods/Packs. Returns table of file paths.
---@return nil
function list_char_mods() end
---Approximate bounding box of the player hud element for player index 1..4 based on user settings and player count
---@param index integer
---@return AABB
function get_hud_position(index) end
---Olmec cutscene moves Olmec and destroys the four floor tiles, so those things never happen if the cutscene is disabled, and Olmec will spawn on even ground. More useful for level gen mods, where the cutscene doesn't make sense. You can also set olmec_cutscene.timer to the last frame (809) to skip to the end, with Olmec in the hole.
---@param enable boolean
---@return nil
function set_olmec_cutscene_enabled(enable) end
---Tiamat cutscene is also responsible for locking the exit door, so you may need to close it yourself if you still want Tiamat kill to be required
---@param enable boolean
---@return nil
function set_tiamat_cutscene_enabled(enable) end
---Activate custom variables for position used for detecting the player (normally hardcoded)
---note: because those variables are custom and game does not initiate them, you need to do it yourself for each Tiamat entity, recommending set_post_entity_spawn
---default game values are: attack_x = 17.5 attack_y = 62.5
---@param activate boolean
---@return nil
function activate_tiamat_position_hack(activate) end
---Activate custom variables for speed and y coordinate limit for crushing elevator
---note: because those variables are custom and game does not initiate them, you need to do it yourself for each CrushElevator entity, recommending set_post_entity_spawn
---default game values are: speed = 0.0125, y_limit = 98.5
---@param activate boolean
---@return nil
function activate_crush_elevator_hack(activate) end
---Activate custom variables for y coordinate limit for hundun and spawn of it's heads
---note: because those variables are custom and game does not initiate them, you need to do it yourself for each Hundun entity, recommending set_post_entity_spawn
---default game value are: y_limit = 98.5, rising_speed_x = 0, rising_speed_y = 0.0125, bird_head_spawn_y = 55, snake_head_spawn_y = 71
---@param activate boolean
---@return nil
function activate_hundun_hack(activate) end
---Allows you to disable the control over the door for Hundun and Tiamat
---This will also prevent game crashing when there is no exit door when they are in level
---@param enable boolean
---@return nil
function set_boss_door_control_enabled(enable) end
---Run state update manually, i.e. simulate one logic frame. Use in e.g. POST_UPDATE, but be mindful of infinite loops, this will cause another POST_UPDATE. Can even be called thousands of times to simulate minutes of gameplay in a few seconds.
---@return nil
function update_state() end
---Set engine target frametime (1/framerate, default 1/60). Always capped by your GPU max FPS / VSync. To run the engine faster than rendered FPS, try update_state. Set to 0 to go as fast as possible. Call without arguments to reset. Also see set_speedhack
---@param frametime double?
---@return nil
function set_frametime(frametime) end
---Get engine target frametime (1/framerate, default 1/60).
---@return double
function get_frametime() end
---Set engine target frametime when game is unfocused (1/framerate, default 1/33). Always capped by the engine frametime. Set to 0 to go as fast as possible. Call without arguments to reset.
---@param frametime double?
---@return nil
function set_frametime_unfocused(frametime) end
---Get engine target frametime when game is unfocused (1/framerate, default 1/33).
---@return double
function get_frametime_unfocused() end
---Adds new custom type (group of ENT_TYPE) that can be later used in functions like get_entities_by or set_(pre/post)_entity_spawn
---Use empty array or no parameter to get new unique ENT_TYPE that can be used for custom EntityDB
---@param types ENT_TYPE[]
---@return ENT_TYPE
function add_custom_type(types) end
---Adds new custom type (group of ENT_TYPE) that can be later used in functions like get_entities_by or set_(pre/post)_entity_spawn
---Use empty array or no parameter to get new unique ENT_TYPE that can be used for custom EntityDB
---@return ENT_TYPE
function add_custom_type() end
---Get uids of entities by draw_depth. Can also use table of draw_depths.
---You can later use [filter_entities](https://spelunky-fyi.github.io/overlunky/#filter_entities) if you want specific entity
---@param draw_depths integer[]
---@param l LAYER
---@return integer[]
function get_entities_by_draw_depth(draw_depths, l) end
---Get uids of entities by draw_depth. Can also use table of draw_depths.
---You can later use [filter_entities](https://spelunky-fyi.github.io/overlunky/#filter_entities) if you want specific entity
---@param draw_depth integer
---@param l LAYER
---@return integer[]
function get_entities_by_draw_depth(draw_depth, l) end
---Just convenient way of getting the current amount of money
---short for state->money_shop_total + loop[inventory.money + inventory.collected_money_total]
---@return integer
function get_current_money() end
---Adds money to the state.money_shop_total and displays the effect on the HUD for money change
---Can be negative, default display_time = 60 (about 2s). Returns the current money after the transaction
---@param amount integer
---@param display_time integer?
---@return integer
function add_money(amount, display_time) end
---Adds money to the state.items.player_inventory[player_slot].money and displays the effect on the HUD for money change
---Can be negative, default display_time = 60 (about 2s). Returns the current money after the transaction
---@param amount integer
---@param player_slot integer
---@param display_time integer?
---@return integer
function add_money_slot(amount, player_slot, display_time) end
---Destroys all layers and all entities in the level. Usually a bad idea, unless you also call create_level and spawn the player back in.
---@return nil
function destroy_level() end
---Destroys a layer and all entities in it.
---@param layer integer
---@return nil
function destroy_layer(layer) end
---Initializes an empty front and back layer that don't currently exist. Does nothing(?) if layers already exist.
---@return nil
function create_level() end
---Initializes an empty layer that doesn't currently exist.
---@param layer integer
---@return nil
function create_layer(layer) end
---Setting to false disables all player logic in SCREEN.LEVEL, mainly the death screen from popping up if all players are dead or missing, but also shop camera zoom and some other small things.
---@param enable boolean
---@return nil
function set_level_logic_enabled(enable) end
---Setting to true will stop the state update from unpausing after a screen load, leaving you with state.pause == PAUSE.FADE on the first frame to do what you want.
---@param enable boolean
---@return nil
function set_start_level_paused(enable) end
---Returns true if the level pause hack is enabled
---@return boolean
function get_start_level_paused() end
---Converts INPUTS to (x, y, BUTTON)
---@param inputs INPUTS
---@return number, number, BUTTON
function inputs_to_buttons(inputs) end
---Converts (x, y, BUTTON) to INPUTS
---@param x number
---@param y number
---@param buttons BUTTON
---@return INPUTS
function buttons_to_inputs(x, y, buttons) end
---Disable the Infinite Loop Detection of 420 million instructions per frame, if you know what you're doing and need to perform some serious calculations that hang the game updates for several seconds.
---@param enable boolean
---@return nil
function set_infinite_loop_detection_enabled(enable) end
---This disables the `state.camera_layer` to be forced to the `(leader player).layer` and setting of the `state.layer_transition_timer` & `state.transition_to_layer` when player enters layer door.
---Letting you control those manually.
---Look at the example on how to mimic game layer switching behavior
---@param enable boolean
---@return nil
function set_camera_layer_control_enabled(enable) end
---Set multiplier (default 1.0) for a QueryPerformanceCounter hook based speedhack, similar to the one in Cheat Engine. Call without arguments to reset. Also see [set_frametime](https://spelunky-fyi.github.io/overlunky/#set_frametime)
---@param multiplier number?
---@return nil
function set_speedhack(multiplier) end
---Get the current speedhack multiplier
---@return number
function get_speedhack() end
---Retrieves the current value of the performance counter, which is a high resolution (<1us) time stamp that can be used for time-interval measurements.
---@return integer
function get_performance_counter() end
---Retrieves the frequency of the performance counter. The frequency of the performance counter is fixed at system boot and is consistent across all processors. Therefore, the frequency need only be queried upon application initialization, and the result can be cached.
---@return integer
function get_performance_frequency() end
---Initializes some adventure run related values and loads the character select screen, as if starting a new adventure run from the Play menu. Character select can be skipped by changing `state.screen_next` right after calling this function, maybe with `warp()`. If player isn't already selected, make sure to set `state.items.player_select` and `state.items.player_count` appropriately too.
---@return nil
function play_adventure() end
---Initializes some seeded run related values and loads the character select screen, as if starting a new seeded run after entering the seed.
---@param seed integer?
---@return nil
function play_seeded(seed) end
---Change layer at which the liquid spawns in, THIS FUNCTION NEEDS TO BE CALLED BEFORE THE LEVEL IS BUILD, otherwise collisions and other stuff will be wrong for the newly spawned liquid
---This sadly also makes lavamanders extinct, since the logic for their spawn is hardcoded to front layer with bunch of other unrelated stuff (you can still spawn them with script or place them directly in level files)
---Everything should be working more or less correctly (report on community discord if you find something unusual)
---@param l LAYER
---@return nil
function set_liquid_layer(l) end
---Get the current layer that the liquid is spawn in. Related function [set_liquid_layer](https://spelunky-fyi.github.io/overlunky/#set_liquid_layer)
---@return integer
function get_liquid_layer() end
---Attach liquid collision to entity by uid (this is what the push blocks use)
---Collision is based on the entity's hitbox, collision is removed when the entity is destroyed (bodies of killed entities will still have the collision)
---Use only for entities that can move around, (for static prefer [update_liquid_collision_at](https://spelunky-fyi.github.io/overlunky/#update_liquid_collision_at) )
---If entity is in back layer and liquid in the front, there will be no collision created, also collision is not destroyed when entity changes layers, so you have to handle that yourself
---@param uid integer
---@param add boolean
---@return nil
function add_entity_to_liquid_collision(uid, add) end
---@return boolean
function toast_visible() end
---@return boolean
function speechbubble_visible() end
---@return nil
function cancel_toast() end
---@return nil
function cancel_speechbubble() end
---Save current level state to slot 1..4. These save states are invalid and cleared after you exit the current level, but can be used to rollback to an earlier state in the same level. You probably definitely shouldn't use save state functions during an update, and sync them to the same event outside an update (i.e. GUIFRAME, POST_UPDATE). These slots are already allocated by the game, actually used for online rollback, and use no additional memory. Also see SaveState if you need more.
---@param slot integer
---@return nil
function save_state(slot) end
---Load level state from slot 1..4, if a save_state was made in this level.
---@param slot integer
---@return nil
function load_state(slot) end
---Clear save state from slot 1..4.
---@param slot integer
---@return nil
function clear_state(slot) end
---Get StateMemory from a save_state slot.
---@param slot integer
---@return StateMemory
function get_save_state(slot) end
---Returns RawInput, a game structure for raw keyboard and controller state
---@return RawInput
function get_raw_input() end
---Seed the game prng.
---@param seed integer
---@return nil
function seed_prng(seed) end
---Get the thread-local version of prng
---@return PRNG
function get_local_prng() end
---Same as `Player.get_name`
---@param type_id ENT_TYPE
---@return string
function get_character_name(type_id) end
---Same as `Player.get_short_name`
---@param type_id ENT_TYPE
---@return string
function get_character_short_name(type_id) end
---Same as `Player.get_heart_color`
---@param type_id ENT_TYPE
---@return Color
function get_character_heart_color(type_id) end
---Same as `Player.is_female`
---@param type_id ENT_TYPE
---@return boolean
function is_character_female(type_id) end
---Same as `Player.set_heart_color`
---@param type_id ENT_TYPE
---@param color Color
---@return nil
function set_character_heart_color(type_id, color) end
---Make a `CustomMovableBehavior`, if `base_behavior` is `nil` you will have to set all of the
---behavior functions. If a behavior with `behavior_name` already exists for your script it will
---be returned instead.
---@param behavior_name string
---@param state_id integer
---@param base_behavior VanillaMovableBehavior
---@return CustomMovableBehavior
function make_custom_behavior(behavior_name, state_id, base_behavior) end
---Get the [ParticleDB](https://spelunky-fyi.github.io/overlunky/#ParticleDB) details of the specified ID
---@param id PARTICLEEMITTER
---@return ParticleDB
function get_particle_type(id) end
---Generate particles of the specified type around the specified entity uid (use e.g. `local emitter = generate_world_particles(PARTICLEEMITTER.PETTING_PET, players[1].uid)`). You can then decouple the emitter from the entity with `emitter.entity_uid = -1` and freely move it around. See the `particles.lua` example script for more details.
---@param particle_emitter_id PARTICLEEMITTER
---@param uid integer
---@return ParticleEmitterInfo
function generate_world_particles(particle_emitter_id, uid) end
---Generate particles of the specified type at a certain screen coordinate (use e.g. `local emitter = generate_screen_particles(PARTICLEEMITTER.CHARSELECTOR_TORCHFLAME_FLAMES, 0.0, 0.0)`). See the `particles.lua` example script for more details.
---@param particle_emitter_id PARTICLEEMITTER
---@param x number
---@param y number
---@return ParticleEmitterInfo
function generate_screen_particles(particle_emitter_id, x, y) end
---Advances the state of the screen particle emitter (simulates the next positions, ... of all the particles in the emitter). Only used with screen particle emitters. See the `particles.lua` example script for more details.
---@param particle_emitter ParticleEmitterInfo
---@return nil
function advance_screen_particles(particle_emitter) end
---Renders the particles to the screen. Only used with screen particle emitters. See the `particles.lua` example script for more details.
---@param particle_emitter ParticleEmitterInfo
---@return nil
function render_screen_particles(particle_emitter) end
---Extinguish a particle emitter (use the return value of `generate_world_particles` or `generate_screen_particles` as the parameter in this function)
---@param particle_emitter ParticleEmitterInfo
---@return nil
function extinguish_particles(particle_emitter) end
---Default function in spawn definitions to check whether a spawn is valid or not
---@param x number
---@param y number
---@param layer LAYER
---@return boolean
function default_spawn_is_valid(x, y, layer) end
---Check if position satisfies the given POS_TYPE flags, to be used in a custom is_valid function procedural for spawns.
---@param x number
---@param y number
---@param layer LAYER
---@param flags POS_TYPE
---@return boolean
function position_is_valid(x, y, layer, flags) end
---Add a callback for a specific tile code that is called before the game handles the tile code.
---Return true in order to stop the game or scripts loaded after this script from handling this tile code.
---For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
---The callback signature is bool pre_tile_code(float x, float y, int layer, ROOM_TEMPLATE room_template)
---@param cb fun(x: number, y: number, layer: integer, room_template: ROOM_TEMPLATE): boolean
---@param tile_code string
---@return CallbackId
function set_pre_tile_code_callback(cb, tile_code) end
---Add a callback for a specific tile code that is called after the game handles the tile code.
---Use this to affect what the game or other scripts spawned in this position.
---This is received even if a previous pre-tile-code-callback has returned true
---The callback signature is nil post_tile_code(float x, float y, int layer, ROOM_TEMPLATE room_template)
---@param cb fun(x: number, y: number, layer: integer, room_template: ROOM_TEMPLATE): nil
---@param tile_code string
---@return CallbackId
function set_post_tile_code_callback(cb, tile_code) end
---Define a new tile code, to make this tile code do anything you have to use either [set_pre_tile_code_callback](https://spelunky-fyi.github.io/overlunky/#set_pre_tile_code_callback) or [set_post_tile_code_callback](https://spelunky-fyi.github.io/overlunky/#set_post_tile_code_callback).
---If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.
---@param tile_code string
---@return TILE_CODE
function define_tile_code(tile_code) end
---Gets a short tile code based on definition, returns `nil` if it can't be found
---@param short_tile_code_def ShortTileCodeDef
---@return integer?
function get_short_tile_code(short_tile_code_def) end
---Gets the definition of a short tile code (if available), will vary depending on which file is loaded
---@param short_tile_code SHORT_TILE_CODE
---@return ShortTileCodeDef?
function get_short_tile_code_definition(short_tile_code) end
---Define a new procedural spawn, the function `nil do_spawn(float x, float y, LAYER layer)` contains your code to spawn the thing, whatever it is.
---The function `bool is_valid(float x, float y, LAYER layer)` determines whether the spawn is legal in the given position and layer.
---Use for example when you can spawn only on the ceiling, under water or inside a shop.
---Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
---If a user disables your script but still uses your level mod nothing will be spawned in place of your procedural spawn.
---@param procedural_spawn string
---@param do_spawn fun(x: number, y: number, layer: LAYER): nil
---@param is_valid fun(x: number, y: number, layer: LAYER): boolean
---@return PROCEDURAL_CHANCE
function define_procedural_spawn(procedural_spawn, do_spawn, is_valid) end
---Define a new extra spawn, these are semi-guaranteed level gen spawns with a fixed upper bound.
---The function `nil do_spawn(float x, float y, LAYER layer)` contains your code to spawn the thing, whatever it is.
---The function `bool is_valid(float x, float y, LAYER layer)` determines whether the spawn is legal in the given position and layer.
---Use for example when you can spawn only on the ceiling, under water or inside a shop.
---Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
---To change the number of spawns use `PostRoomGenerationContext:set_num_extra_spawns` during `ON.POST_ROOM_GENERATION`
---No name is attached to the extra spawn since it is not modified from level files, instead every call to this function will return a new unique id.
---@param do_spawn fun(x: number, y: number, layer: LAYER): nil
---@param is_valid fun(x: number, y: number, layer: LAYER): boolean
---@param num_spawns_frontlayer integer
---@param num_spawns_backlayer integer
---@return integer
function define_extra_spawn(do_spawn, is_valid, num_spawns_frontlayer, num_spawns_backlayer) end
---Use to query whether any of the requested spawns could not be made, usually because there were not enough valid spaces in the level.
---Returns missing spawns in the front layer and missing spawns in the back layer in that order.
---The value only makes sense after level generation is complete, aka after `ON.POST_LEVEL_GENERATION` has run.
---@param extra_spawn_chance_id integer
---@return integer, integer
function get_missing_extra_spawns(extra_spawn_chance_id) end
---Transform a position to a room index to be used in `get_room_template` and `PostRoomGenerationContext.set_room_template`
---@param x number
---@param y number
---@return integer, integer
function get_room_index(x, y) end
---Transform a room index into the top left corner position in the room
---@param x integer
---@param y integer
---@return number, number
function get_room_pos(x, y) end
---Get the room template given a certain index, returns `nil` if coordinates are out of bounds
---@param x integer
---@param y integer
---@param layer LAYER
---@return integer?
function get_room_template(x, y, layer) end
---Get whether a room is flipped at the given index, returns `false` if coordinates are out of bounds
---@param x integer
---@param y integer
---@return boolean
function is_room_flipped(x, y) end
---Get whether a room is the origin of a machine room
---@param x integer
---@param y integer
---@return boolean
function is_machine_room_origin(x, y) end
---For debugging only, get the name of a room template, returns `'invalid'` if room template is not defined
---@param room_template integer
---@return string
function get_room_template_name(room_template) end
---Define a new room template to use with `set_room_template`
---@param room_template string
---@param type ROOM_TEMPLATE_TYPE
---@return integer
function define_room_template(room_template, type) end
---Set the size of room template in tiles, the template must be of type `ROOM_TEMPLATE_TYPE.MACHINE_ROOM`.
---@param room_template integer
---@param width integer
---@param height integer
---@return boolean
function set_room_template_size(room_template, width, height) end
---Get the inverse chance of a procedural spawn for the current level.
---A return value of 0 does not mean the chance is infinite, it means the chance is zero.
---@param chance_id PROCEDURAL_CHANCE
---@return integer
function get_procedural_spawn_chance(chance_id) end
---Gets the sub theme of the current cosmic ocean level, returns COSUBTHEME.NONE if the current level is not a CO level.
---@return COSUBTHEME
function get_co_subtheme() end
---Forces the theme of the next cosmic ocean level(s) (use e.g. `force_co_subtheme(COSUBTHEME.JUNGLE)`. Use COSUBTHEME.RESET to reset to default random behaviour)
---@param subtheme COSUBTHEME
---@return nil
function force_co_subtheme(subtheme) end
---Gets the value for the specified config
---@param config LEVEL_CONFIG
---@return integer
function get_level_config(config) end
---Set the value for the specified config
---@param config LEVEL_CONFIG
---@param value integer
---@return nil
function set_level_config(config, value) end
---Grow vines from `GROWABLE_VINE` and `VINE_TREE_TOP` entities in a level, `area` default is whole level, `destroy_broken` default is false
---@param l LAYER
---@param max_length integer
---@param area AABB
---@param destroy_broken boolean
---@return nil
function grow_vines(l, max_length, area, destroy_broken) end
---Grow vines from `GROWABLE_VINE` and `VINE_TREE_TOP` entities in a level, `area` default is whole level, `destroy_broken` default is false
---@param l LAYER
---@param max_length integer
---@return nil
function grow_vines(l, max_length) end
---Grow pole from `GROWABLE_CLIMBING_POLE` entities in a level, `area` default is whole level, `destroy_broken` default is false
---@param l LAYER
---@param max_length integer
---@param area AABB
---@param destroy_broken boolean
---@return nil
function grow_poles(l, max_length, area, destroy_broken) end
---Grow pole from `GROWABLE_CLIMBING_POLE` entities in a level, `area` default is whole level, `destroy_broken` default is false
---@param l LAYER
---@param max_length integer
---@return nil
function grow_poles(l, max_length) end
---Grow chains from `ENT_TYPE_FLOOR_CHAIN_CEILING` and chain with blocks on it from `ENT_TYPE_FLOOR_CHAINANDBLOCKS_CEILING`, it starts looking for the ceilings from the top left corner of a level.
---To limit it use the parameters, so x = 10 will only grow chains from ceilings with x < 10, with y = 10 it's ceilings that have y > (level bound top - 10)
---@return boolean
function grow_chainandblocks() end
---Grow chains from `ENT_TYPE_FLOOR_CHAIN_CEILING` and chain with blocks on it from `ENT_TYPE_FLOOR_CHAINANDBLOCKS_CEILING`, it starts looking for the ceilings from the top left corner of a level.
---To limit it use the parameters, so x = 10 will only grow chains from ceilings with x < 10, with y = 10 it's ceilings that have y > (level bound top - 10)
---@param x integer
---@param y integer
---@return boolean
function grow_chainandblocks(x, y) end
---Immediately load a screen based on [state](https://spelunky-fyi.github.io/overlunky/#state).screen_next and stuff
---@return nil
function load_screen() end
---Force a theme in PRE_LOAD_LEVEL_FILES, POST_ROOM_GENERATION or PRE_LEVEL_GENERATION to change different aspects of the levelgen. You can pass a CustomTheme, ThemeInfo or THEME.
---@param customtheme CustomTheme|ThemeInfo|THEME
---@return nil
function force_custom_theme(customtheme) end
---Force current subtheme used in the CO theme. You can pass a CustomTheme, ThemeInfo or THEME. Not to be confused with force_co_subtheme.
---@param customtheme CustomTheme|ThemeInfo|THEME
---@return nil
function force_custom_subtheme(customtheme) end
---Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
---@param path string
---@return CustomSound?
function create_sound(path) end
---Gets an existing sound, either if a file at the same path was already loaded or if it is already loaded by the game
---@param path_or_vanilla_sound string
---@return CustomSound?
function get_sound(path_or_vanilla_sound) end
---Loads a bank from disk relative to this script, ownership might be shared with other code that loads the same file.
---Returns nil if the file can't be found. Loading a bank file will load the banks metadata, but not non-streaming
---sample data. Once a bank has finished loading, all metadata can be accessed meaning that event descriptions can
---be found with `get_event_by_id()` or using `create_fmod_guid_map()` and calling `FMODguidMap:getEvent()`.
---The banks loading state can be queried using `CustomBank:getLoadingState()` which will return an `FMOD_LOADING_STATE`.
---@param path string
---@param flags FMODStudio::LoadBankFlags
---@return CustomBank?
function load_bank(path, flags) end
---Gets an existing loaded bank if a file at the same path was already loaded
---@param path string
---@return CustomBank?
function get_bank(path) end
---Gets a `CustomEventDescription` if the event description is loaded using an FMOD GUID string. The string representation
---must be formatted as 32 digits seperated by hyphens and enclosed in braces: {00000000-0000-0000-0000-000000000000}.
---@param guid_string string
---@return CustomEventDescription?
function get_event_by_id(guid_string) end
---An `FMODguidMap` can be used to resolve FMOD GUIDs for events and snapshots from paths using the GUIDs.txt exported
---from an FMOD Studio Project. By default FMOD studio uses a strings bank to do this, however the games master bank and
---strings bank cannot be rebuilt to include the names and paths of new events or snapshots. `FMODguidMap` is a
---workaround for this, and allows you to get a `CustomEventDescription` from a path with `FMODguidMap:getEvent()`.
---`FMODguidMap:getEvent()` expects the path to be formatted similarly to event:/UI/Cancel or snapshot:/IngamePause.
---Creates an `FMODguidMap` by parsing a GUIDs.txt exported from FMOD Studio from disk relative to this script. This is useful
---if you want to use a human readable FMOD event path to create a `CustomEventDescription` instead of using an FMOD GUID string.
---@param path string
---@return FMODguidMap?
function create_fmod_guid_map(path) end
---Returns unique id for the callback to be used in [clear_vanilla_sound_callback](https://spelunky-fyi.github.io/overlunky/#clear_vanilla_sound_callback).
---Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
---Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
---If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any properties on the sound. Otherwise you may cause a deadlock.
---The callback signature is nil on_vanilla_sound(PlayingSound sound)
---@param name VANILLA_SOUND
---@param types VANILLA_SOUND_CALLBACK_TYPE
---@param cb fun(sound: PlayingSound): nil
---@return CallbackId
function set_vanilla_sound_callback(name, types, cb) end
---Clears a previously set callback
---@param id CallbackId
---@return nil
function clear_vanilla_sound_callback(id) end
---Use source_uid to make the sound be played at the location of that entity, set it -1 to just play it "everywhere"
---Returns SoundMeta, beware that the sound can't be stopped (`start_over` and `playing` are unavailable). Should only be used for sfx.
---@param sound VANILLA_SOUND
---@param source_uid integer
---@return SoundMeta
function play_sound(sound, source_uid) end
---Use source_uid to make the sound be played at the location of that entity, set it -1 to just play it "everywhere"
---Returns SoundMeta, beware that the sound can't be stopped (`start_over` and `playing` are unavailable). Should only be used for sfx.
---@param sound_id SOUNDID
---@param source_uid integer
---@return SoundMeta
function play_sound(sound_id, source_uid) end
---@param id SOUNDID
---@return VANILLA_SOUND
function convert_sound_id(id) end
---Convert SOUNDID to VANILLA_SOUND and vice versa
---@param sound VANILLA_SOUND
---@return SOUNDID
function convert_sound_id(sound) end
---Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.
---@param size number
---@param text string
---@return number, number
function draw_text_size(size, text) end
---Create image from file. Returns a tuple containing id, width and height.
---Depending on the image size, this can take a moment, preferably don't create them dynamically, rather create all you need in global scope so it will load them as soon as the game starts
---@param path string
---@return IMAGE, integer, integer
function create_image(path) end
---Create image from file, cropped to the geometry provided. Returns a tuple containing id, width and height.
---Depending on the image size, this can take a moment, preferably don't create them dynamically, rather create all you need in global scope so it will load them as soon as the game starts
---@param path string
---@param x integer
---@param y integer
---@param w integer
---@param h integer
---@return IMAGE, integer, integer
function create_image_crop(path, x, y, w, h) end
---Get image size from file. Returns a tuple containing width and height.
---@param path string
---@return integer, integer
function get_image_size(path) end
---Current mouse cursor position in screen coordinates.
---@return number, number
function mouse_position() end
---Returns human readable string from KEY chord (e.g. "Ctrl+X", "Unknown" or "None")
---@return string
function key_name() end
---Returns: [ImGuiIO](https://spelunky-fyi.github.io/overlunky/#ImGuiIO) for raw keyboard, mouse and xinput gamepad stuff.
---@return ImGuiIO
function get_io() end
---Returns unique id >= 0 for the callback to be used in [clear_callback](https://spelunky-fyi.github.io/overlunky/#clear_callback) or -1 if the key could not be registered.
---Add callback function to be called on a hotkey, using Windows hotkey api. These hotkeys will override all game and UI input and can work even when the game is unfocused. They are by design very intrusive and won't let anything else use the same key combo. Can't detect if input is active in another instance, use ImGuiIO if you need Playlunky hotkeys to react to Overlunky input state. Key is a KEY combo (e.g. `KEY.OL_MOD_CTRL | KEY.X`), possibly returned by GuiDrawContext:key_picker. Doesn't work with mouse buttons.
---The callback signature is nil on_hotkey(KEY key)
---@param cb fun(key: KEY): nil
---@param key KEY
---@param flags HOTKEY_TYPE
---@return CallbackId
function set_hotkey(cb, key, flags) end
---Force the LUT texture for the given layer (or both) until it is reset.
---Pass `nil` in the first parameter to reset
---@param texture_id TEXTURE?
---@param layer LAYER
---@return nil
function set_lut(texture_id, layer) end
---Same as `set_lut(nil, layer)`
---@param layer LAYER
---@return nil
function reset_lut(layer) end
---@return HudData
function get_hud() end
---Alters the drop chance for the provided monster-item combination (use e.g. set_drop_chance(DROPCHANCE.MOLE_MATTOCK, 10) for a 1 in 10 chance)
---Use `-1` as dropchance_id to reset all to default
---@param dropchance_id integer
---@param new_drop_chance integer
---@return nil
function set_drop_chance(dropchance_id, new_drop_chance) end
---Changes a particular drop, e.g. what Van Horsing throws at you (use e.g. replace_drop(DROP.VAN_HORSING_DIAMOND, ENT_TYPE.ITEM_PLASMACANNON))
---Use `0` as type to reset this drop to default, use `-1` as drop_id to reset all to default
---Check all the available drops [here](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/drops.cpp)
---@param drop_id integer
---@param new_drop_entity_type ENT_TYPE
---@return nil
function replace_drop(drop_id, new_drop_entity_type) end
---Gets a `TextureDefinition` for equivalent to the one used to define the texture with `id`
---@param texture_id TEXTURE
---@return TextureDefinition
function get_texture_definition(texture_id) end
---Defines a new texture that can be used in Entity::set_texture
---If a texture with the same definition already exists the texture will be reloaded from disk.
---@param texture_data TextureDefinition
---@return TEXTURE
function define_texture(texture_data) end
---Gets a texture with the same definition as the given, if none exists returns `nil`
---@param texture_data TextureDefinition
---@return TEXTURE?
function get_texture(texture_data) end
---Reloads a texture from disk, use this only as a development tool for example in the console
---Note that [define_texture](https://spelunky-fyi.github.io/overlunky/#define_texture) will also reload the texture if it already exists
---@param texture_path string
---@return nil
function reload_texture(texture_path) end
---Replace a vanilla texture definition with a custom texture definition and reload the texture.
---@param vanilla_id TEXTURE
---@param custom_id TEXTURE
---@return boolean
function replace_texture(vanilla_id, custom_id) end
---Reset a replaced vanilla texture to the original and reload the texture.
---@param vanilla_id TEXTURE
---@return nil
function reset_texture(vanilla_id) end
---Replace a vanilla texture definition with a custom texture definition and reload the texture. Set corresponding character heart color to the pixel in the center of the player indicator arrow in that texture. (448,1472)
---@param vanilla_id TEXTURE
---@param custom_id TEXTURE
---@return boolean
function replace_texture_and_heart_color(vanilla_id, custom_id) end
---Clear cache for a file path or the whole directory
---@return nil
function clear_cache() end
---Gets the hitbox of an entity, use `extrude` to make the hitbox bigger/smaller in all directions and `offset` to offset the hitbox in a given direction
---@param uid integer
---@param extrude number?
---@param offsetx number?
---@param offsety number?
---@return AABB
function get_hitbox(uid, extrude, offsetx, offsety) end
---Same as `get_hitbox` but based on `get_render_position`
---@param uid integer
---@param extrude number?
---@param offsetx number?
---@param offsety number?
---@return AABB
function get_render_hitbox(uid, extrude, offsetx, offsety) end
---Convert an `AABB` to a screen `AABB` that can be directly passed to draw functions
---@param box AABB
---@return AABB
function screen_aabb(box) end
---Find intersection point of two lines [A, B] and [C, D], returns INFINITY if the lines don't intersect each other [parallel]
---@param A Vec2
---@param B Vec2
---@param C Vec2
---@param D Vec2
---@return Vec2
function intersection(A, B, C, D) end
---Measures angle between two lines with one common point
---@param A Vec2
---@param common Vec2
---@param B Vec2
---@return number
function two_lines_angle(A, common, B) end
---Gets line1_A, intersection point and line2_B and calls the 3 parameter version of this function
---@param line1_A Vec2
---@param line1_B Vec2
---@param line2_A Vec2
---@param line2_B Vec2
---@return number
function two_lines_angle(line1_A, line1_B, line2_A, line2_B) end
---Force the journal to open on a chapter and entry# when pressing the journal button. Only use even entry numbers. Set chapter to `JOURNALUI_PAGE_SHOWN.JOURNAL` to reset. (This forces the journal toggle to always read from `game_manager.save_related.journal_popup_ui.entry_to_show` etc.)
---@param chapter integer
---@param entry integer
---@return nil
function force_journal(chapter, entry) end
---Open or close the journal as if pressing the journal button. Will respect visible journal popups and [force_journal](https://spelunky-fyi.github.io/overlunky/#force_journal).
---@return nil
function toggle_journal() end
---Open the journal on a chapter and page. The main Journal spread is pages 0..1, so most chapters start at 2. Use even page numbers only.
---@param chapter JOURNALUI_PAGE_SHOWN
---@param page integer
---@return nil
function show_journal(chapter, page) end
---Start an UDP server on specified address and run callback when data arrives. Return a string from the callback to reply. Requires unsafe mode.
---The server will be closed once the handle is released.
---@param host string
---@param port integer
---@param cb function
---@return UdpServer
function udp_listen(host, port, cb) end
---Send data to specified UDP address. Requires unsafe mode.
---@param host string
---@param port integer
---@param msg string
---@return nil
function udp_send(host, port, msg) end
---Hook the sendto and recvfrom functions and start dumping network data to terminal
---@return nil
function dump_network() end
---Send a synchronous HTTP GET request and return response as a string or nil on an error
---@param url string
---@return string?
function http_get(url) end
---Send an asynchronous HTTP GET request and run the callback when done. If there is an error, response will be nil and vice versa.
---The callback signature is nil on_data(string response, string error)
---@param url string
---@param on_data fun(response: string, error: string): nil
---@return nil
function http_get_async(url, on_data) end
---Check if the user has performed a feat (Real Steam achievement or a hooked one). Returns: `bool unlocked, bool hidden, string name, string description`
---@param feat FEAT
---@return boolean, boolean, string, string
function get_feat(feat) end
---Get the visibility of a feat
---@param feat FEAT
---@return boolean
function get_feat_hidden(feat) end
---Set the visibility of a feat
---@param feat FEAT
---@param hidden boolean
---@return nil
function set_feat_hidden(feat, hidden) end
---Helper function to set the title and description strings for a FEAT with change_string, as well as the hidden state.
---@param feat FEAT
---@param hidden boolean
---@param name string
---@param description string
---@return nil
function change_feat(feat, hidden, name, description) end
---Access the PauseAPI, or directly call `pause(true)` to enable current `pause.pause_type`
---@return nil
function pause() end
---Returns the Bucket of data stored in shared memory between Overlunky and Playlunky
---@return Bucket
function get_bucket() end
---Converts a color to int to be used in drawing functions. Use values from `0..255`.
---@param r integer
---@param g integer
---@param b integer
---@param a integer
---@return uColor
function rgba(r, g, b, a) end
---Convert a string to a color, you can use the HTML color names, or even HTML color codes, just prefix them with '#' symbol
---You can also convert hex string into a color, prefix it with '0x', but use it only if you need to since lua allows for hex values directly too.
---Default alpha value will be 0xFF, unless it's specified
---Format: [name], #RRGGBB, #RRGGBBAA, 0xBBGGRR, 0xAABBGGRR
---@param color_name string
---@param alpha integer?
---@return uColor
function get_color(color_name, alpha) end

--## Types
do

---@class Players

---@class SaveContext
    ---@field save fun(self, data: string): boolean

---@class LoadContext
    ---@field load fun(self): string

---@class ArenaConfigArenas
    ---@field list boolean[] @size: 40
    ---@field dwelling_1 boolean
    ---@field dwelling_2 boolean
    ---@field dwelling_3 boolean
    ---@field dwelling_4 boolean
    ---@field dwelling_5 boolean
    ---@field jungle_1 boolean
    ---@field jungle_2 boolean
    ---@field jungle_3 boolean
    ---@field jungle_4 boolean
    ---@field jungle_5 boolean
    ---@field volcana_1 boolean
    ---@field volcana_2 boolean
    ---@field volcana_3 boolean
    ---@field volcana_4 boolean
    ---@field volcana_5 boolean
    ---@field tidepool_1 boolean
    ---@field tidepool_2 boolean
    ---@field tidepool_3 boolean
    ---@field tidepool_4 boolean
    ---@field tidepool_5 boolean
    ---@field temple_1 boolean
    ---@field temple_2 boolean
    ---@field temple_3 boolean
    ---@field temple_4 boolean
    ---@field temple_5 boolean
    ---@field icecaves_1 boolean
    ---@field icecaves_2 boolean
    ---@field icecaves_3 boolean
    ---@field icecaves_4 boolean
    ---@field icecaves_5 boolean
    ---@field neobabylon_1 boolean
    ---@field neobabylon_2 boolean
    ---@field neobabylon_3 boolean
    ---@field neobabylon_4 boolean
    ---@field neobabylon_5 boolean
    ---@field sunkencity_1 boolean
    ---@field sunkencity_2 boolean
    ---@field sunkencity_3 boolean
    ---@field sunkencity_4 boolean
    ---@field sunkencity_5 boolean

---@class ArenaConfigItems
    ---@field rock boolean
    ---@field pot boolean
    ---@field bombbag boolean
    ---@field bombbox boolean
    ---@field ropepile boolean
    ---@field pickup_12bag boolean
    ---@field pickup_24bag boolean
    ---@field cooked_turkey boolean
    ---@field royal_jelly boolean
    ---@field torch boolean
    ---@field boomerang boolean
    ---@field machete boolean
    ---@field mattock boolean
    ---@field crossbow boolean
    ---@field webgun boolean
    ---@field freezeray boolean
    ---@field shotgun boolean
    ---@field camera boolean
    ---@field plasma_cannon boolean
    ---@field wooden_shield boolean
    ---@field metal_shield boolean
    ---@field teleporter boolean
    ---@field mine boolean
    ---@field snaptrap boolean
    ---@field paste boolean
    ---@field climbing_gloves boolean
    ---@field pitchers_mitt boolean
    ---@field spike_shoes boolean
    ---@field spring_shoes boolean
    ---@field parachute boolean
    ---@field cape boolean
    ---@field vlads_cape boolean
    ---@field jetpack boolean
    ---@field hoverpack boolean
    ---@field telepack boolean
    ---@field powerpack boolean
    ---@field excalibur boolean
    ---@field scepter boolean
    ---@field kapala boolean
    ---@field true_crown boolean

---@class ArenaConfigEquippedItems
    ---@field paste boolean
    ---@field climbing_gloves boolean
    ---@field pitchers_mitt boolean
    ---@field spike_shoes boolean
    ---@field spring_shoes boolean
    ---@field parachute boolean
    ---@field kapala boolean
    ---@field scepter boolean

---@class ArenaState
    ---@field current_arena integer
    ---@field player_teams integer[] @size: 4
    ---@field format integer
    ---@field ruleset integer
    ---@field player_lives integer[] @size: 4
    ---@field player_totalwins integer[] @size: 4
    ---@field player_won boolean[] @size: 4
    ---@field timer integer @The menu selection for timer, default values 0..20 where 0 == 30 seconds, 19 == 10 minutes and 20 == infinite. Can go higher, although this will glitch the menu text. Actual time (seconds) = (state.arena.timer + 1) x 30
    ---@field timer_ending integer
    ---@field wins integer
    ---@field lives integer
    ---@field time_to_win integer
    ---@field player_idolheld_countdown integer[] @size: 4
    ---@field health integer
    ---@field bombs integer
    ---@field ropes integer
    ---@field stun_time integer
    ---@field mount integer
    ---@field arena_select integer
    ---@field arenas ArenaConfigArenas
    ---@field dark_level_chance integer
    ---@field crate_frequency integer
    ---@field items_enabled ArenaConfigItems
    ---@field items_in_crate ArenaConfigItems
    ---@field held_item integer
    ---@field equipped_backitem integer
    ---@field equipped_items ArenaConfigEquippedItems
    ---@field whip_damage integer
    ---@field final_ghost boolean
    ---@field breath_cooldown integer
    ---@field punish_ball boolean

---@class JournalProgressStickerSlot
    ---@field theme integer
    ---@field grid_position integer
    ---@field entity_type ENT_TYPE
    ---@field x number
    ---@field y number
    ---@field angle number

---@class JournalProgressStainSlot
    ---@field x number
    ---@field y number
    ---@field angle number
    ---@field scale number
    ---@field texture_column integer
    ---@field texture_row integer
    ---@field texture_range integer

---@class SelectPlayerSlot
    ---@field activated boolean
    ---@field character ENT_TYPE
    ---@field texture TEXTURE

---@class Items
    ---@field player_count integer
    ---@field saved_pets_count integer @Only for the level transition, the actual number is held in player inventory
    ---@field saved_pets ENT_TYPE[] @size: 4 @Pet information for level transition
    ---@field is_pet_cursed boolean[] @size: 4
    ---@field is_pet_poisoned boolean[] @size: 4
    ---@field leader integer @Index of leader player in coop
    ---@field player_select SelectPlayerSlot[] @size: MAX_PLAYERS
    ---@field player_inventory Inventory[] @size: MAX_PLAYERS
    ---@field players Player[] @size: MAX_PLAYERS @Table of players, also keeps the dead body until they are destroyed (necromancer revive also destroys the old body)

---@class LiquidPhysicsEngine
    ---@field pause boolean
    ---@field gravity number
    ---@field cohesion number
    ---@field elasticity number
    ---@field size number
    ---@field weight number
    ---@field count integer

---@class LiquidPhysicsParams
    ---@field gravity number
    ---@field cohesion number
    ---@field elasticity number

---@class LiquidPool
    ---@field default LiquidPhysicsParams
    ---@field engine LiquidPhysicsEngine

---@class LiquidPhysics
    ---@field pools LiquidPool[] @size: 5

---@class StateMemory
    ---@field screen_last SCREEN @Previous SCREEN, used to check where we're coming from when loading another SCREEN
    ---@field screen SCREEN @Current SCREEN, generally read-only or weird things will happen
    ---@field screen_next SCREEN @Next SCREEN, used to load the right screen when loading. Can be changed in PRE_LOAD_SCREEN to go somewhere else instead. Also see `state.loading`.
    ---@field pause PAUSE @8bit flags, multiple might be active at the same time<br/>1: Menu: Pauses the level timer and engine. Can't set, controlled by the menu.<br/>2: Fade/Loading: Pauses all timers and engine.<br/>4: Cutscene: Pauses total/level time but not engine. Used by boss cutscenes.<br/>8: Unknown: Pauses total/level time and engine. Does not pause the global counter so set_global_interval still runs.<br/>16: Unknown: Pauses total/level time and engine. Does not pause the global counter so set_global_interval still runs.<br/>32: Ankh: Pauses all timers, engine, but not camera. Used by the ankh cutscene.
    ---@field width integer @level width in rooms (number of rooms horizontally)
    ---@field height integer @level height in rooms (number of rooms vertically)
    ---@field kali_favor integer
    ---@field kali_status integer
    ---@field kali_altars_destroyed integer @Also affects if the player has punish ball, if the punish ball is destroyed it is set to -1
    ---@field kali_gifts integer @0 - none, 1 - item, 3 - kapala
    ---@field seed integer @Current seed in seeded mode, just set to a funny value and does nothing in adventure mode
    ---@field time_total integer @Total frames of current run, equal to the final game time on win
    ---@field world integer @Current world number, shown in hud and used by some game logic like choosing the next level on transition
    ---@field world_next integer @Next world number, used when loading a new level or transition
    ---@field world_start integer @World number to start new runs in
    ---@field level integer @Current level number, shown in hud and used by some game logic like choosing the next level on transition
    ---@field level_next integer @Next level number, used when loading a new level or transition
    ---@field level_start integer @Level number to start new runs in
    ---@field theme THEME @Current THEME number, used to pick the music and by some game logic like choosing the next level on transition
    ---@field theme_next THEME @Next THEME number, used when loading a new level or transition
    ---@field theme_start THEME @THEME to start new runs in
    ---@field current_theme ThemeInfo @Points to the current ThemeInfo
    ---@field force_current_theme fun(self, t: THEME): nil @This function should only be used in a very specific circumstance (forcing the exiting theme when manually transitioning). Will crash the game if used inappropriately!
    ---@field shoppie_aggro integer @Current shoppie aggro
    ---@field shoppie_aggro_next integer @Shoppie aggro to use in the next level
    ---@field outposts_spawned integer
    ---@field merchant_aggro integer @Tun aggro
    ---@field kills_npc integer
    ---@field level_count integer @Current zero-based level count, or number of levels completed
    ---@field damage_taken integer @Total amount of damage taken, excluding cause of death
    ---@field journal_flags JOURNAL_FLAG
    ---@field time_last_level integer @Level time of previous level in frames, used by game logic to decide dark levels etc
    ---@field time_level integer @Level time of current level in frames, show on the hud
    ---@field level_flags integer
    ---@field quest_flags QUEST_FLAG @32bit flags, can be written to trigger a run reset on next level load etc.
    ---@field presence_flags PRESENCE_FLAG
    ---@field loading FADE @Current loading/fade state. Pauses all updates if > FADE.NONE. Writing FADE.OUT or FADE.LOAD will trigger a screen load to `screen_next`.
    ---@field fade_value number @Current fade-to-black amount (0.0 = all visible; 1.0 = all black). Manipulated by the loading routine when loading > 0.
    ---@field fade_timer integer @Remaining frames for fade-in/fade-out when loading. Counts down to 0.
    ---@field fade_length integer @Total frames for fade-in/fade-out when loading.
    ---@field fade_delay integer @Additional delay after fade_timer reaches 0, before moving to the next fading state. Used after Ouroboros, in credits etc. for longer black screens, but also works after FADE.IN.
    ---@field fade_enabled boolean @Enables the fade effect on FADE.IN, setting to false makes loading skip FADE.IN state instantly
    ---@field fade_circle boolean @Makes loading use circle iris effect instead of fade on FADE.IN
    ---@field saved_dogs integer @Run totals
    ---@field saved_cats integer
    ---@field saved_hamsters integer
    ---@field win_state integer @0 = no win 1 = tiamat win 2 = hundun win 3 = CO win; set this and next doorway leads to victory scene
    ---@field illumination Illumination @The global level illumination, very big and bright.
    ---@field money_last_levels integer
    ---@field money_shop_total integer @Total amount spent in shops and sold idols during the run<br><br/>The total money currently available is `loop (players[].inventory.money + players[].inventory.collected_money_total) + state.money_shop_total`
    ---@field player_inputs PlayerInputs @Access the player inputs even when no player entities are available
    ---@field quests QuestsInfo @NPC quest states
    ---@field camera Camera @Camera bounds and position
    ---@field special_visibility_flags integer
    ---@field cause_of_death CAUSE_OF_DEATH
    ---@field cause_of_death_entity_type ENT_TYPE
    ---@field toast_timer integer
    ---@field speechbubble_timer integer
    ---@field speechbubble_owner integer
    ---@field level_gen LevelGenSystem @Entrance and exit coordinates, shop types and all themes
    ---@field correct_ushabti integer @See `get_correct_ushabti`. == anim_frame - (2 * floor(anim_frame/12))
    ---@field items Items @Has the current player count, player inventories and character selections
    ---@field camera_layer integer
    ---@field layer_transition_timer integer
    ---@field transition_to_layer integer
    ---@field screen_character_select ScreenCharacterSelect
    ---@field screen_team_select ScreenTeamSelect @For the arena
    ---@field screen_camp ScreenStateCamp
    ---@field screen_level ScreenStateLevel
    ---@field screen_transition ScreenTransition
    ---@field screen_death ScreenDeath
    ---@field screen_win ScreenWin
    ---@field screen_credits ScreenCredits @The spaceship minigame
    ---@field screen_scores ScreenScores @Landing on the moon after win
    ---@field screen_constellation ScreenConstellation
    ---@field screen_recap ScreenRecap @Journal after CO win
    ---@field screen_arena_stages_select ScreenArenaStagesSelect
    ---@field screen_arena_intro ScreenArenaIntro
    ---@field screen_arena_level ScreenArenaLevel
    ---@field screen_arena_score ScreenArenaScore
    ---@field screen_arena_menu ScreenArenaMenu
    ---@field screen_arena_items ScreenArenaItems
    ---@field get_correct_ushabti fun(self): integer @Returns animation_frame of the correct ushabti
    ---@field set_correct_ushabti fun(self, animation_frame: integer): nil
    ---@field arena ArenaState
    ---@field speedrun_character ENT_TYPE @Who administers the tutorial speedrun in base camp
    ---@field speedrun_activation_trigger boolean @must transition from true to false to activate it
    ---@field end_spaceship_character ENT_TYPE @Who pops out the spaceship for a tiamat/hundun win, this is set upon the spaceship door open
    ---@field world2_coffin_spawned boolean
    ---@field world4_coffin_spawned boolean
    ---@field world6_coffin_spawned boolean
    ---@field first_damage_cause ENT_TYPE
    ---@field first_damage_world integer
    ---@field first_damage_level integer
    ---@field time_speedrun integer
    ---@field coffin_contents ENT_TYPE @the contents of the special coffin that will be spawned during levelgen
    ---@field screen_change_counter integer
    ---@field time_startup integer @Number of frames since the game was launched
    ---@field storage_uid integer @entity uid of the first floor_storage entity
    ---@field waddler_storage ENT_TYPE[] @size: 99
    ---@field waddler_metadata integer[] @size: 99
    ---@field journal_progress_sticker_count integer
    ---@field journal_progress_sticker_slots JournalProgressStickerSlot[] @size: 40 @stickers for notable items and entities in journal progress page
    ---@field journal_progress_stain_count integer
    ---@field journal_progress_stain_slots JournalProgressStainSlot[] @size: 30 @blood splats and paw prints in journal progress page
    ---@field journal_progress_theme_count integer
    ---@field journal_progress_theme_slots THEME[] @size: 9 @visited themes in journal progress page
    ---@field theme_info ThemeInfo @Points to the current ThemeInfo
    ---@field logic LogicList @Level logic like dice game and cutscenes
    ---@field liquid LiquidPhysics
    ---@field next_entity_uid integer @Next entity spawned will have this uid
    ---@field room_owners RoomOwnersInfo @Holds info about owned rooms and items (shops, challenge rooms, vault etc.)
    ---@field user_data any

---@class LightParams
    ---@field red number
    ---@field green number
    ---@field blue number
    ---@field size number
    ---@field as_color fun(self): Color @Returns LightParams as Color, note that size = alpha

---@class Illumination
    ---@field lights LightParams[] @size: 4 @Table of light1, light2, ... etc.
    ---@field light1 LightParams
    ---@field light2 LightParams
    ---@field light3 LightParams
    ---@field light4 LightParams @It's rendered on objects around, not as an actual bright spot
    ---@field brightness number
    ---@field brightness_multiplier number
    ---@field light_pos_x number
    ---@field light_pos_y number
    ---@field offset_x number
    ---@field offset_y number
    ---@field distortion number
    ---@field entity_uid integer
    ---@field timer integer
    ---@field flags integer @see [flags.hpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/flags.hpp) illumination_flags
    ---@field type_flags LIGHT_TYPE
    ---@field enabled boolean
    ---@field layer integer

---@class Camera
    ---@field bounds_left number
    ---@field bounds_right number
    ---@field bounds_bottom number
    ---@field bounds_top number
    ---@field calculated_focus_x number
    ---@field calculated_focus_y number
    ---@field adjusted_focus_x number
    ---@field adjusted_focus_y number
    ---@field focus_offset_x number
    ---@field focus_offset_y number
    ---@field focus_x number
    ---@field focus_y number
    ---@field vertical_pan number
    ---@field shake_countdown_start integer
    ---@field shake_countdown integer
    ---@field shake_amplitude number
    ---@field shake_multiplier_x number
    ---@field shake_multiplier_y number
    ---@field uniform_shake boolean
    ---@field focused_entity_uid integer @if set to -1, you have free control over camera focus through focus_x, focus_y
    ---@field inertia number @This is a bad name, but it represents the camera tweening speed. [0..5] where 0=still, 1=default (move 20% of distance per frame), 5=max (move 5*20% or 100% aka instantly to destination per frame)
    ---@field peek_timer integer @amount of frames to freeze camera in place and move to the peek_layer<br/>during the peek you can freely set camera position no matter if focused_entity_uid is set to -1 or not
    ---@field peek_layer integer
    ---@field get_bounds fun(self): AABB
    ---@field set_bounds fun(self, bounds: AABB): nil

---@class Online
    ---@field online_players OnlinePlayer[] @size: 4
    ---@field local_player OnlinePlayer
    ---@field lobby OnlineLobby
    ---@field is_active fun(self): boolean

---@class OnlinePlayer
    ---@field game_mode GAME_MODE
    ---@field platform PLATFORM
    ---@field ready_state READY_STATE
    ---@field character integer
    ---@field player_name string

---@class OnlineLobby
    ---@field code integer
    ---@field local_player_slot integer
    ---@field get_code fun(self): string @Gets the string equivalent of the code

---@class RoomOwnersInfo
    ---@field owned_items custom_map<integer, ItemOwnerDetails> @key/index is the uid of an item
    ---@field owned_rooms RoomOwnerDetails[]

---@class ItemOwnerDetails
    ---@field owner_type ENT_TYPE
    ---@field owner_uid integer

---@class RoomOwnerDetails
    ---@field layer integer
    ---@field room_index integer
    ---@field owner_uid integer

---@class SaveState
    ---@field load fun(self): nil @Load a SaveState
    ---@field save fun(self): nil @Save over a previously allocated SaveState
    ---@field clear fun(self): nil @Delete the SaveState and free the memory. The SaveState can't be used after this.
    ---@field get_state fun(self): StateMemory @Access the StateMemory inside a SaveState
    ---@field get_frame fun(self): integer @Get the current frame from the SaveState, equivelent to the [get_frame](#Get_frame) global function that returns the frame from the "loaded in state"
    ---@field get_prng fun(self): PRNG @Access the PRNG inside a SaveState

---@class BackgroundMusic
    ---@field game_startup BackgroundSound
    ---@field main_backgroundtrack BackgroundSound
    ---@field basecamp BackgroundSound
    ---@field win_scene BackgroundSound
    ---@field arena BackgroundSound
    ---@field arena_intro_and_win BackgroundSound
    ---@field level_gameplay BackgroundSound
    ---@field dark_level BackgroundSound
    ---@field level_transition BackgroundSound
    ---@field backlayer BackgroundSound
    ---@field shop BackgroundSound
    ---@field angered_shopkeeper BackgroundSound
    ---@field inside_sunken_city_pipe BackgroundSound
    ---@field pause_menu BackgroundSound
    ---@field death_transition BackgroundSound

---@class GameManager
    ---@field music BackgroundMusic
    ---@field game_props GameProps
    ---@field screen_logo ScreenLogo
    ---@field screen_intro ScreenIntro
    ---@field screen_prologue ScreenPrologue
    ---@field screen_title ScreenTitle
    ---@field screen_menu ScreenMenu
    ---@field screen_options ScreenOptions
    ---@field screen_player_profile Screen @It just opens journal
    ---@field screen_leaderboards Screen @All handled by the Online
    ---@field screen_seed_input ScreenCodeInput
    ---@field screen_camp ScreenCamp
    ---@field screen_level ScreenLevel
    ---@field screen_transition Screen
    ---@field screen_arena_level Screen
    ---@field screen_arena_score Screen
    ---@field screen_online_loading ScreenOnlineLoading
    ---@field screen_online_lobby ScreenOnlineLobby
    ---@field pause_ui PauseUI
    ---@field journal_ui JournalUI
    ---@field save_related SaveRelated
    ---@field main_menu_music BackgroundSound
    ---@field buttons_controls integer[] @size: MAX_PLAYERS @Yet another place to get player inputs, in some format
    ---@field buttons_movement integer[] @size: MAX_PLAYERS @Yet another place to get player inputs, in some format

---@class SaveRelated
    ---@field journal_popup_ui JournalPopupUI
    ---@field places_data table<integer, JournalPageData> @Scale and offset not used in those pages. Can't add more
    ---@field bestiary_data table<ENT_TYPE, JournalBestiaryData>
    ---@field monster_part_to_main table<ENT_TYPE, ENT_TYPE> @used to map stuff like Osiris_Hand -> Osiris_Head, Hundun limbs -> Hundun etc.
    ---@field people_info table<ENT_TYPE, JournalPeopleData>
    ---@field people_part_to_main table<ENT_TYPE, ENT_TYPE> @used to map shopkeeper clone to shopkeeper only
    ---@field item_info table<ENT_TYPE, JournalPageData>
    ---@field trap_info table<ENT_TYPE, JournalPageData>
    ---@field trap_part_to_main table<ENT_TYPE, ENT_TYPE> @used for stuff like upsidedown_spikes -> spikes, skulldrop skulls -> skulldrop trap etc.
    ---@field stickers_data table<ENT_TYPE, StickersData>
    ---@field get_savegame fun(self): SaveData @Gets local version of the SaveData

---@class JournalPageData
    ---@field page_nr integer
    ---@field sprite_id integer
    ---@field name STRINGID
    ---@field description STRINGID
    ---@field scale number
    ---@field offset_x number
    ---@field offset_y number

---@class JournalBestiaryData : JournalPageData
    ---@field texture TEXTURE
    ---@field background_sprite_id integer
    ---@field killed_by_NA boolean
    ---@field defeated_NA boolean

---@class JournalPeopleData : JournalPageData
    ---@field texture TEXTURE
    ---@field background_sprite_id integer
    ---@field killed_by_NA boolean
    ---@field defeated_NA boolean
    ---@field portrait_texture TEXTURE

---@class JournalTrapData : JournalPageData
    ---@field texture TEXTURE
    ---@field background_sprite_id integer

---@class StickersData
    ---@field sprite_id integer
    ---@field texture TEXTURE

---@class JournalPopupUI
    ---@field wiggling_page_icon TextureRenderingInfo
    ---@field black_background TextureRenderingInfo
    ---@field button_icon TextureRenderingInfo
    ---@field wiggling_page_angle number
    ---@field chapter_to_show integer
    ---@field entry_to_show integer
    ---@field timer integer
    ---@field slide_position number

---@class GameProps
    ---@field input integer[] @size: MAX_PLAYERS @Used for player input and might be used for some menu inputs not found in buttons_menu. You can probably capture and edit this in ON.POST_PROCESS_INPUT. These are raw inputs, without things like autorun applied.
    ---@field input_previous integer[] @size: MAX_PLAYERS
    ---@field input_menu MENU_INPUT @Inputs used to control all the menus, separate from player inputs. You can probably capture and edit this in ON.POST_PROCESS_INPUT
    ---@field input_menu_previous MENU_INPUT @Previous state of buttons_menu
    ---@field game_has_focus boolean
    ---@field menu_open boolean
    ---@field input_index integer[] @size: 5 @Input index for players 1-4 and maybe for the menu controls. -1: disabled, 0..3: keyboards, 4..7: Xinput, 8..11: other controllers

---@class RawInput
    ---@field keyboard KeyboardKey[] @size: 112 @State of all keyboard buttons in a random game order as usual, most key indexes can be found in RAW_KEY. Zero-based indexing, i.e. use PlayerSlot.input_mapping_keyboard directly to index this.
    ---@field controller ControllerInput[] @size: 12 @State of controller buttons per controller. Zero-based indexing, i.e. use GameProps.input_index directly to index this.

---@class KeyboardKey
    ---@field down boolean @Key is being held
    ---@field pressed boolean @Key was just pressed down this frame

---@class ControllerInput
    ---@field buttons ControllerButton[] @size: 16 @Zero-based indexing. Use PlayerSlot.input_mapping_controller or RAW_BUTTON (or RAW_DUALSHOCK) to index this.

---@class ControllerButton
    ---@field down boolean @Button is being held
    ---@field pressed boolean @Button was just pressed down this frame

---@class PRNG
    ---@field seed fun(self, seed: integer): nil @Same as `seed_prng`
    ---@field random_float fun(self, type: PRNG_CLASS): number @Generate a random floating point number in the range `[0, 1)`
    ---@field random_chance fun(self, inverse_chance: integer, type: PRNG_CLASS): boolean @Returns true with a chance of `1/inverse_chance`
    ---@field random_index fun(self, i: integer, type: PRNG_CLASS): integer? @Generate a integer number in the range `[1, i]` or `nil` if `i < 1`
    ---@field random_int fun(self, min: integer, max: integer, type: PRNG_CLASS): integer @Generate a integer number in the range `[min, max]`
    ---@field get_pair fun(self, type: PRNG_CLASS): integer, integer
    ---@field set_pair fun(self, type: PRNG_CLASS, first: integer, second: integer): nil
local PRNG = nil
---Drop-in replacement for `math.random()`
---@return number
function PRNG:random() end
---Drop-in replacement for `math.random(i)`
---@param i integer
---@return integer?
function PRNG:random(i) end
---Drop-in replacement for `math.random(min, max)`
---@param min integer
---@param max integer
---@return integer
function PRNG:random(min, max) end

---@class Animation
    ---@field id integer
    ---@field first_tile integer
    ---@field num_tiles integer
    ---@field interval integer
    ---@field repeat_mode REPEAT_TYPE

---@class EntityDB
    ---@field id ENT_TYPE
    ---@field search_flags integer @MASK
    ---@field width number
    ---@field height number
    ---@field draw_depth integer
    ---@field offsetx number @Offset of the hitbox in relation to the entity position
    ---@field offsety number @Offset of the hitbox in relation to the entity position
    ---@field hitboxx number @Half of the width of the hitbox
    ---@field hitboxy number @Half of the height of the hitbox
    ---@field default_shape SHAPE
    ---@field default_hitbox_enabled boolean
    ---@field collision2_mask integer @MASK, will only call collision2 when colliding with entities that match this mask.
    ---@field collision_mask integer @MASK used for collision with floors, walls etc.
    ---@field default_flags integer
    ---@field default_more_flags integer
    ---@field properties_flags integer
    ---@field friction number
    ---@field elasticity number
    ---@field weight number
    ---@field acceleration number
    ---@field max_speed number
    ---@field sprint_factor number
    ---@field jump number
    ---@field default_color Color
    ---@field texture TEXTURE
    ---@field tilex integer
    ---@field tiley integer
    ---@field damage integer
    ---@field life integer
    ---@field sacrifice_value integer @Favor for sacrificing alive. Halved when dead (health == 0).
    ---@field blood_content integer
    ---@field leaves_corpse_behind boolean
    ---@field description STRINGID
    ---@field sound_killed_by_player SOUNDID
    ---@field sound_killed_by_other SOUNDID
    ---@field animations table<integer, Animation>
    ---@field default_special_offsetx number
    ---@field default_special_offsety number

---@class RenderInfo
    ---@field x number
    ---@field y number
    ---@field offset_x number
    ---@field offset_y number
    ---@field shader WORLD_SHADER
    ---@field source Quad
    ---@field destination Quad
    ---@field tilew number
    ---@field tileh number
    ---@field facing_left boolean
    ---@field angle number
    ---@field animation_frame integer
    ---@field render_inactive boolean
    ---@field brightness number
    ---@field texture_num integer
    ---@field get_entity fun(self): Entity
    ---@field set_normal_map_texture fun(self, texture_id: TEXTURE): boolean @Sets second_texture to the texture specified, then sets third_texture to SHINE_0 and texture_num to 3. You still have to change shader to 30 to render with normal map (same as COG normal maps)
    ---@field get_second_texture fun(self): TEXTURE?
    ---@field get_third_texture fun(self): TEXTURE?
    ---@field set_second_texture fun(self, texture_id: TEXTURE): boolean
    ---@field set_third_texture fun(self, texture_id: TEXTURE): boolean
    ---@field set_texture_num fun(self, num: integer): boolean @Set the number of textures that may be used, need to have them set before for it to work
    ---@field set_pre_virtual fun(self, entry: RENDER_INFO_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: RENDER_INFO_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_dtor fun(self, fun: fun(self: RenderInfo): nil): CallbackId @Hooks before the virtual function.<br/>The callback signature is `nil dtor(RenderInfo self)`
    ---@field set_post_dtor fun(self, fun: fun(self: RenderInfo): nil): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil dtor(RenderInfo self)`
    ---@field set_pre_draw fun(self, fun: fun(self: RenderInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool draw(RenderInfo self)`<br/>Virtual function docs:<br/>Called when the entity enters the camera view, using its hitbox with an extra threshold. Handles low-level graphics tasks related to the GPU
    ---@field set_post_draw fun(self, fun: fun(self: RenderInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil draw(RenderInfo self)`<br/>Virtual function docs:<br/>Called when the entity enters the camera view, using its hitbox with an extra threshold. Handles low-level graphics tasks related to the GPU
    ---@field set_pre_render fun(self, fun: fun(self: RenderInfo, offset: Vec2, vanilla_render_context: VanillaRenderContext): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool render(RenderInfo self, Vec2 offset, VanillaRenderContext vanilla_render_context)`<br/>Virtual function docs:<br/>Offset used in CO to draw the fake image of the entity on the other side of a level
    ---@field set_post_render fun(self, fun: fun(self: RenderInfo, offset: Vec2, vanilla_render_context: VanillaRenderContext): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil render(RenderInfo self, Vec2 offset, VanillaRenderContext vanilla_render_context)`<br/>Virtual function docs:<br/>Offset used in CO to draw the fake image of the entity on the other side of a level

---@class Entity
    ---@field type EntityDB @Type of the entity, contains special properties etc. If you want to edit them just for this entity look at the EntityDB
    ---@field overlay Entity
    ---@field flags ENT_FLAG @see [flags.hpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/flags.hpp) entity_flags
    ---@field more_flags ENT_MORE_FLAG @see [flags.hpp](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/flags.hpp) more_flags
    ---@field uid integer @Unique id of the entity, save it to variable to check this entity later (don't use the whole Entity type as it will be replaced with a different one when this is destroyed)
    ---@field animation_frame integer @Number (id) of the sprite in the texture
    ---@field draw_depth integer @Depth level that this entity is drawn on.<br/>Don't edit this directly, use `set_draw_depth` function
    ---@field x number @Position of the entity in the world, or relative to overlay if attached to something. Use `get_absolute_position` to get real position of anything in the game world.
    ---@field y number @Position of the entity in the world, or relative to overlay if attached to something. Use `get_absolute_position` to get real position of anything in the game world.
    ---@field abs_x number @Absolute position in the world, even if overlaid. Might be a frame off since it's updated with `apply_movement` function and so it does not update if game moves the entity in different way after movement is processed.<br/>Use `get_absolute_position` for precise. Read only.
    ---@field abs_y number @Absolute position in the world, even if overlaid. Might be a frame off since it's updated with `apply_movement` function and so it does not update if game moves the entity in different way after movement is processed.<br/>Use `get_absolute_position` for precise. Read only.
    ---@field layer integer @Use `set_layer` to change
    ---@field width number @Width of the sprite
    ---@field height number @Height of the sprite
    ---@field special_offsetx number @Special offset used for entities attached to others (or picked by others) that need to flip to the other side when the parent flips sides
    ---@field special_offsety number @Special offset used for entities attached to others (or picked by others) that need to flip to the other side when the parent flips sides
    ---@field tile_width number @Size of the sprite in the texture
    ---@field tile_height number @Size of the sprite in the texture
    ---@field angle number
    ---@field color Color
    ---@field hitboxx number @Half of the width of the hitbox
    ---@field hitboxy number @Half of the height of the hitbox
    ---@field shape SHAPE
    ---@field hitbox_enabled boolean
    ---@field offsetx number @Offset of the hitbox in relation to the entity position
    ---@field offsety number @Offset of the hitbox in relation to the entity position
    ---@field rendering_info RenderInfo
    ---@field user_data any
    ---@field topmost fun(self): Entity @Returns the top entity in a chain (overlay)
    ---@field get_texture fun(self): TEXTURE
    ---@field set_texture fun(self, texture_id: TEXTURE): boolean @Changes the entity texture, check the [textures.txt](game_data/textures.txt) for available vanilla textures or use [define_texture](#define_texture) to make custom one
    ---@field set_draw_depth fun(self, draw_depth: integer, unknown: integer?): nil @optional unknown - game usually sets it to 0, doesn't appear to have any special effect (needs more reverse engineering) 
    ---@field reset_draw_depth fun(self): nil
    ---@field friction fun(self): number @Friction of this entity, affects it's contact with other entities (how fast it slows down on the floor, how fast it can move but also the other way around for floors/activefloors: how other entities can move on it)
    ---@field liberate_from_shop fun(self, clear_parrent: boolean): nil @`clear_parent` used only for CHAR_* entities, sets the `linked_companion_parent` to -1. It's not called when item is bought
    ---@field get_held_entity fun(self): Entity
    ---@field set_layer fun(self, layer: LAYER): nil @Moves the entity to specified layer with all it's items, nothing else happens, so this does not emulate a door transition
    ---@field apply_layer fun(self): nil @Adds the entity to its own layer, to add it to entity lookup tables without waiting for a state update
    ---@field remove fun(self): nil @Moves the entity to the limbo-layer where it can later be retrieved from again via `respawn`
    ---@field respawn fun(self, layer_to: LAYER): nil @Moves the entity from the limbo-layer (where it was previously put by `remove`) to `layer`
    ---@field kill fun(self, destroy_corpse: boolean, responsible: Entity): nil @Kills the entity, you can set responsible to `nil` to ignore it
    ---@field destroy fun(self): nil @Completely removes the entity from existence
    ---@field activate fun(self, activator: Entity): nil @Activates a button prompt (with the Use door/Buy button), e.g. buy shop item, activate drill, read sign, interact in camp, ... `get_entity(<udjat socket uid>):activate(players[1])` (make sure player 1 has the udjat eye though)
    ---@field perform_teleport fun(self, delta_x: integer, delta_y: integer): nil @Performs a teleport as if the entity had a teleporter and used it. The delta coordinates are where you want the entity to teleport to relative to its current position, in tiles (so integers, not floats). Positive numbers = to the right and up, negative left and down.
    ---@field trigger_action fun(self, user: Entity): boolean @Triggers weapons and other held items like teleporter, mattock etc. You can check the [virtual-availability.md](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md), if entity has `open` in the `on_open` you can use this function, otherwise it does nothing. Returns false if action could not be performed (cooldown is not 0, no arrow loaded in etc. the animation could still be played thou)
    ---@field get_metadata fun(self): integer @e.g. for turkey: stores health, poison/curse state, for mattock: remaining swings (returned value is transferred)
    ---@field apply_metadata fun(self, metadata: integer): nil
    ---@field set_invisible fun(self, value: boolean): nil
    ---@field get_items fun(self): integer[]
    ---@field is_in_liquid fun(self): boolean @Returns true if entity is in water/lava
    ---@field is_cursed fun(self): boolean
    ---@field is_movable fun(self): boolean
    ---@field can_be_pushed fun(self): boolean
    ---@field update fun(self): nil
    ---@field flip fun(self, left: boolean): nil
    ---@field remove_item fun(self, entity: Entity, autokill_check: boolean): nil @Can be called multiple times for the same entity (for example when play throws/drops entity from it's hands)
    ---@field apply_db fun(self): nil @Applies changes made in `entity.type`
    ---@field get_absolute_velocity fun(self): Vec2 @Get's the velocity relative to the game world, only for movable or liquid entities
    ---@field get_absolute_position fun(self): Vec2 @Get the absolute position of an entity in the game world
    ---@field get_hitbox fun(self, use_render_pos: boolean?): AABB @`use_render_pos` default is `false`
    ---@field attach fun(self, new_overlay: Entity): nil @Attach to other entity (at the current relative position to it)
    ---@field detach fun(self, check_autokill: boolean?): nil @Detach from overlay
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_dtor fun(self, fun: fun(self: Entity): nil): CallbackId @Hooks before the virtual function.<br/>The callback signature is `nil dtor(Entity self)`
    ---@field set_post_dtor fun(self, fun: fun(self: Entity): nil): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil dtor(Entity self)`
    ---@field set_pre_create_rendering_info fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool create_rendering_info(Entity self)`
    ---@field set_post_create_rendering_info fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil create_rendering_info(Entity self)`
    ---@field set_pre_update_state_machine fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool update_state_machine(Entity self)`
    ---@field set_post_update_state_machine fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil update_state_machine(Entity self)`
    ---@field set_pre_kill fun(self, fun: fun(self: Entity, destroy_corpse: boolean, responsible: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool kill(Entity self, boolean destroy_corpse, Entity responsible)`<br/>Virtual function docs:<br/>Kills the entity, you can set responsible to `nil` to ignore it
    ---@field set_post_kill fun(self, fun: fun(self: Entity, destroy_corpse: boolean, responsible: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil kill(Entity self, boolean destroy_corpse, Entity responsible)`<br/>Virtual function docs:<br/>Kills the entity, you can set responsible to `nil` to ignore it
    ---@field set_pre_on_collision1 fun(self, fun: fun(self: Entity, other_entity: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool on_collision1(Entity self, Entity other_entity)`<br/>Virtual function docs:<br/>Collisions with stuff that blocks you, like walls, floors, etc. Triggers for entities in it's EntityDB.collision_mask
    ---@field set_post_on_collision1 fun(self, fun: fun(self: Entity, other_entity: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil on_collision1(Entity self, Entity other_entity)`<br/>Virtual function docs:<br/>Collisions with stuff that blocks you, like walls, floors, etc. Triggers for entities in it's EntityDB.collision_mask
    ---@field set_pre_destroy fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool destroy(Entity self)`<br/>Virtual function docs:<br/>Completely removes the entity from existence
    ---@field set_post_destroy fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil destroy(Entity self)`<br/>Virtual function docs:<br/>Completely removes the entity from existence
    ---@field set_pre_generate_damage_particles fun(self, fun: fun(self: Entity, victim: Entity, damage: DAMAGE_TYPE, killing: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool generate_damage_particles(Entity self, Entity victim, DAMAGE_TYPE damage, boolean killing)`
    ---@field set_post_generate_damage_particles fun(self, fun: fun(self: Entity, victim: Entity, damage: DAMAGE_TYPE, killing: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil generate_damage_particles(Entity self, Entity victim, DAMAGE_TYPE damage, boolean killing)`
    ---@field set_pre_can_be_pushed fun(self, fun: fun(self: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> can_be_pushed(Entity self)`
    ---@field set_post_can_be_pushed fun(self, fun: fun(self: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil can_be_pushed(Entity self)`
    ---@field set_pre_is_in_liquid fun(self, fun: fun(self: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> is_in_liquid(Entity self)`<br/>Virtual function docs:<br/>Returns true if entity is in water/lava
    ---@field set_post_is_in_liquid fun(self, fun: fun(self: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil is_in_liquid(Entity self)`<br/>Virtual function docs:<br/>Returns true if entity is in water/lava
    ---@field set_pre_set_invisible fun(self, fun: fun(self: Entity, value: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool set_invisible(Entity self, boolean value)`
    ---@field set_post_set_invisible fun(self, fun: fun(self: Entity, value: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil set_invisible(Entity self, boolean value)`
    ---@field set_pre_flip fun(self, fun: fun(self: Entity, left: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool flip(Entity self, boolean left)`
    ---@field set_post_flip fun(self, fun: fun(self: Entity, left: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil flip(Entity self, boolean left)`
    ---@field set_pre_set_draw_depth fun(self, fun: fun(self: Entity, draw_depth: integer, b3f: integer): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool set_draw_depth(Entity self, integer draw_depth, integer b3f)`
    ---@field set_post_set_draw_depth fun(self, fun: fun(self: Entity, draw_depth: integer, b3f: integer): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil set_draw_depth(Entity self, integer draw_depth, integer b3f)`
    ---@field set_pre_reset_draw_depth fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool reset_draw_depth(Entity self)`
    ---@field set_post_reset_draw_depth fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil reset_draw_depth(Entity self)`
    ---@field set_pre_friction fun(self, fun: fun(self: Entity): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> friction(Entity self)`<br/>Virtual function docs:<br/>Friction of this entity, affects it's contact with other entities (how fast it slows down on the floor, how fast it can move but also the other way around for floors/activefloors: how other entities can move on it)
    ---@field set_post_friction fun(self, fun: fun(self: Entity): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil friction(Entity self)`<br/>Virtual function docs:<br/>Friction of this entity, affects it's contact with other entities (how fast it slows down on the floor, how fast it can move but also the other way around for floors/activefloors: how other entities can move on it)
    ---@field set_pre_set_as_sound_source fun(self, fun: fun(self: Entity, soundmeta: SoundMeta): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool set_as_sound_source(Entity self, SoundMeta soundmeta)`
    ---@field set_post_set_as_sound_source fun(self, fun: fun(self: Entity, soundmeta: SoundMeta): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil set_as_sound_source(Entity self, SoundMeta soundmeta)`
    ---@field set_pre_remove_item fun(self, fun: fun(self: Entity, entity: Entity, autokill_check: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool remove_item(Entity self, Entity entity, boolean autokill_check)`<br/>Virtual function docs:<br/>Can be called multiple times for the same entity (for example when play throws/drops entity from it's hands)
    ---@field set_post_remove_item fun(self, fun: fun(self: Entity, entity: Entity, autokill_check: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil remove_item(Entity self, Entity entity, boolean autokill_check)`<br/>Virtual function docs:<br/>Can be called multiple times for the same entity (for example when play throws/drops entity from it's hands)
    ---@field set_pre_get_held_entity fun(self, fun: fun(self: Entity): Entity?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<Entity> get_held_entity(Entity self)`
    ---@field set_post_get_held_entity fun(self, fun: fun(self: Entity): Entity?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil get_held_entity(Entity self)`
    ---@field set_pre_trigger_action fun(self, fun: fun(self: Entity, user: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> trigger_action(Entity self, Entity user)`<br/>Virtual function docs:<br/>Triggers weapons and other held items like teleporter, mattock etc. You can check the [virtual-availability.md](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md), if entity has `open` in the `on_open` you can use this function, otherwise it does nothing. Returns false if action could not be performed (cooldown is not 0, no arrow loaded in etc. the animation could still be played thou)
    ---@field set_post_trigger_action fun(self, fun: fun(self: Entity, user: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil trigger_action(Entity self, Entity user)`<br/>Virtual function docs:<br/>Triggers weapons and other held items like teleporter, mattock etc. You can check the [virtual-availability.md](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md), if entity has `open` in the `on_open` you can use this function, otherwise it does nothing. Returns false if action could not be performed (cooldown is not 0, no arrow loaded in etc. the animation could still be played thou)
    ---@field set_pre_activate fun(self, fun: fun(self: Entity, activator: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool activate(Entity self, Entity activator)`<br/>Virtual function docs:<br/>Activates a button prompt (with the Use door/Buy button), e.g. buy shop item, activate drill, read sign, interact in camp, ... `get_entity(<udjat socket uid>):activate(players[1])` (make sure player 1 has the udjat eye though)
    ---@field set_post_activate fun(self, fun: fun(self: Entity, activator: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil activate(Entity self, Entity activator)`<br/>Virtual function docs:<br/>Activates a button prompt (with the Use door/Buy button), e.g. buy shop item, activate drill, read sign, interact in camp, ... `get_entity(<udjat socket uid>):activate(players[1])` (make sure player 1 has the udjat eye though)
    ---@field set_pre_on_collision2 fun(self, fun: fun(self: Entity, other_entity: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool on_collision2(Entity self, Entity other_entity)`<br/>Virtual function docs:<br/>More like on_overlap, triggers when entities touch/overlap each other. Triggers for entities in it's EntityDB.collision2_mask
    ---@field set_post_on_collision2 fun(self, fun: fun(self: Entity, other_entity: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil on_collision2(Entity self, Entity other_entity)`<br/>Virtual function docs:<br/>More like on_overlap, triggers when entities touch/overlap each other. Triggers for entities in it's EntityDB.collision2_mask
    ---@field set_pre_get_metadata fun(self, fun: fun(self: Entity): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> get_metadata(Entity self)`<br/>Virtual function docs:<br/>e.g. for turkey: stores health, poison/curse state, for mattock: remaining swings (returned value is transferred)
    ---@field set_post_get_metadata fun(self, fun: fun(self: Entity): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil get_metadata(Entity self)`<br/>Virtual function docs:<br/>e.g. for turkey: stores health, poison/curse state, for mattock: remaining swings (returned value is transferred)
    ---@field set_pre_apply_metadata fun(self, fun: fun(self: Entity, metadata: integer): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool apply_metadata(Entity self, integer metadata)`
    ---@field set_post_apply_metadata fun(self, fun: fun(self: Entity, metadata: integer): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil apply_metadata(Entity self, integer metadata)`
    ---@field set_pre_walked_on fun(self, fun: fun(self: Entity, walker: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool walked_on(Entity self, Entity walker)`
    ---@field set_post_walked_on fun(self, fun: fun(self: Entity, walker: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil walked_on(Entity self, Entity walker)`
    ---@field set_pre_walked_off fun(self, fun: fun(self: Entity, walker: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool walked_off(Entity self, Entity walker)`
    ---@field set_post_walked_off fun(self, fun: fun(self: Entity, walker: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil walked_off(Entity self, Entity walker)`
    ---@field set_pre_ledge_grab fun(self, fun: fun(self: Entity, who: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool ledge_grab(Entity self, Entity who)`
    ---@field set_post_ledge_grab fun(self, fun: fun(self: Entity, who: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ledge_grab(Entity self, Entity who)`
    ---@field set_pre_stood_on fun(self, fun: fun(self: Entity, entity: Entity, Vec2: ): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool stood_on(Entity self, Entity entity, Vec2)`
    ---@field set_post_stood_on fun(self, fun: fun(self: Entity, entity: Entity, Vec2: ): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil stood_on(Entity self, Entity entity, Vec2)`
    ---@field set_pre_liberate_from_shop fun(self, fun: fun(self: Entity, clear_parrent: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool liberate_from_shop(Entity self, boolean clear_parrent)`<br/>Virtual function docs:<br/>`clear_parent` used only for CHAR_* entities, sets the `linked_companion_parent` to -1. It's not called when item is bought
    ---@field set_post_liberate_from_shop fun(self, fun: fun(self: Entity, clear_parrent: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil liberate_from_shop(Entity self, boolean clear_parrent)`<br/>Virtual function docs:<br/>`clear_parent` used only for CHAR_* entities, sets the `linked_companion_parent` to -1. It's not called when item is bought
    ---@field set_pre_init fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool init(Entity self)`<br/>Virtual function docs:<br/>Applies changes made in `entity.type`
    ---@field set_post_init fun(self, fun: fun(self: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil init(Entity self)`<br/>Virtual function docs:<br/>Applies changes made in `entity.type`
local Entity = nil
---@param hitbox AABB
---@return boolean
function Entity:overlaps_with(hitbox) end
---Deprecated
---Use `overlaps_with(AABB hitbox)` instead
---@param rect_left number
---@param rect_bottom number
---@param rect_right number
---@param rect_top number
---@return boolean
function Entity:overlaps_with(rect_left, rect_bottom, rect_right, rect_top) end
---@param other Entity
---@return boolean
function Entity:overlaps_with(other) end
---Kill entity along with all entities attached to it. Be aware that for example killing push block with this function will also kill anything on top of it, any items, players, monsters etc.
---To avoid that, you can inclusively or exclusively limit certain MASK and ENT_TYPE. Note: the function will first check mask, if the entity doesn't match, it will look in the provided ENT_TYPE's
---destroy_corpse and responsible are the standard parameters for the kill function
---@param destroy_corpse boolean
---@param responsible Entity
---@param mask integer?
---@param ent_types ENT_TYPE[]
---@param rec_mode RECURSIVE_MODE
---@return nil
function Entity:kill_recursive(destroy_corpse, responsible, mask, ent_types, rec_mode) end
---Short for using RECURSIVE_MODE.NONE
---@param destroy_corpse boolean
---@param responsible Entity
---@return nil
function Entity:kill_recursive(destroy_corpse, responsible) end
---Destroy entity along with all entities attached to it. Be aware that for example destroying push block with this function will also destroy anything on top of it, any items, players, monsters etc.
---To avoid that, you can inclusively or exclusively limit certain MASK and ENT_TYPE. Note: the function will first check the mask, if the entity doesn't match, it will look in the provided ENT_TYPE's
---@param mask integer?
---@param ent_types ENT_TYPE[]
---@param rec_mode RECURSIVE_MODE
---@return nil
function Entity:destroy_recursive(mask, ent_types, rec_mode) end
---Short for using RECURSIVE_MODE.NONE
---@return nil
function Entity:destroy_recursive() end

---@class Movable : Entity
    ---@field move Vec2 @{movex, movey}
    ---@field movex number @Move directions (-1.0 to 1.0) that represent in whit direction the entity want's to move
    ---@field movey number @Move directions (-1.0 to 1.0) that represent in whit direction the entity want's to move
    ---@field buttons BUTTON
    ---@field buttons_previous BUTTON
    ---@field stand_counter integer
    ---@field jump_height_multiplier number @EntityDB.jump gets multiplied by this to get the jump
    ---@field price integer
    ---@field owner_uid integer
    ---@field last_owner_uid integer
    ---@field current_animation Animation
    ---@field idle_counter integer
    ---@field standing_on_uid integer
    ---@field velocityx number @speed, can be relative to the platform you standing on (pushblocks, elevators), use [get_velocity](#get_velocity) to get accurate speed in the game world
    ---@field velocityy number @speed, can be relative to the platform you standing on (pushblocks, elevators), use [get_velocity](#get_velocity) to get accurate speed in the game world
    ---@field holding_uid integer
    ---@field state integer
    ---@field last_state integer
    ---@field move_state integer
    ---@field health integer
    ---@field stun_timer integer
    ---@field stun_state integer
    ---@field lock_input_timer integer @Related to taking damage, also drops you from ladder/rope, can't be set while on the ground unless you're on a mount
    ---@field wet_effect_timer integer
    ---@field poison_tick_timer integer @Used to apply damage from poison, can be set to -1 to cure poison, to set poison use [poison_entity](#poison_entity)
    ---@field falling_timer integer
    ---@field dark_shadow_timer integer
    ---@field onfire_effect_timer integer
    ---@field exit_invincibility_timer integer
    ---@field invincibility_frames_timer integer
    ---@field frozen_timer integer
    ---@field is_poisoned fun(self): boolean
    ---@field is_button_pressed fun(self, button: BUTTON): boolean
    ---@field is_button_held fun(self, button: BUTTON): boolean
    ---@field is_button_released fun(self, button: BUTTON): boolean
    ---@field stun fun(self, framecount: integer): nil
    ---@field freeze fun(self, framecount: integer, ignore_lava: boolean): nil @Sets the `frozen_timer`, the param `ignore_lava` doesn't do much, just skips the liquid check,<br/>if in lava the game will set `frozen_timer` to 0 immediately most of the time
    ---@field light_on_fire fun(self, time: integer): nil @Does not damage entity
    ---@field set_cursed fun(self, b: boolean, effect: boolean): nil
    ---@field drop fun(self): nil @Called when dropping or throwing
    ---@field pick_up fun(self, entity_to_pick_up: Entity): nil
    ---@field standing_on fun(self): Entity
    ---@field collect_treasure fun(self, value: integer, treasure: ENT_TYPE): boolean @Adds or subtracts the specified amount of money to the movable's (player's) inventory. Shows the calculation animation in the HUD. Adds treasure to the inventory list shown on transition. Use the global add_money to add money without adding specific treasure.
    ---@field can_jump fun(self): boolean @Return true if the entity is allowed to jump, even midair. Return false and can't jump, except from ladders apparently.
    ---@field is_on_fire fun(self): boolean
    ---@field is_powerup_capable fun(self): boolean
    ---@field can_be_picked_up_by fun(self, entity_picking_up: Entity, boolean: ): boolean
    ---@field can_break_block fun(self, horizontal: boolean, block: Entity): boolean
    ---@field break_block fun(self, camera_shake: boolean, block: Entity): nil
    ---@field damage fun(self, damage_dealer: Entity, damage_amount: integer, damage_flags: DAMAGE_TYPE, velocity: Vec2, unknown_damage_phase: integer, stun_amount: integer, iframes: integer, unknown_is_final: boolean): boolean @Damage the movable by the specified amount, stuns and gives it invincibility for the specified amount of frames and applies the velocities. `damage_dealer` can be set to nil.<br/>Returns: true if entity was affected (for stuff like: if pot was thrown into entity, should that pot break after hit), false if the event should be ignored by damage_dealer
    ---@field get_all_behaviors fun(self): integer[] @Get all available behavior ids
    ---@field set_behavior fun(self, behavior_id: integer): boolean @Set behavior, this is more than just state as it's an active function, for example climbing ladder is a behavior and it doesn't actually need ladder/rope entity<br/>Returns false if entity doesn't have this behavior id
    ---@field get_behavior fun(self): integer @Get the current behavior id
    ---@field set_gravity fun(self, gravity: number): nil @Force the gravity for this entity. Will override anything set by special states like swimming too, unless you reset it. Default 1.0
    ---@field reset_gravity fun(self): nil @Remove the gravity hook and reset to defaults
    ---@field set_position fun(self, to_x: number, to_y: number): nil @Set the absolute position of an entity and offset all rendering related things accordingly to teleport without any interpolation or graphical glitches. If the camera is focused on the entity, it is also moved.
    ---@field process_input fun(self): nil
    ---@field calculate_jump_velocity fun(self, dont_ignore_liquid: boolean): number
    ---@field apply_velocity fun(self, velocities: Vec2, ignore_weight: boolean): nil @Mostly used for ragdoll by the game
    ---@field get_damage fun(self): integer @Returns the damage that the entity deals
    ---@field attack fun(self, victim: Entity): boolean @Runs on contact damage, returns false if there wasn't any interaction (called from on_collision2, will be called as long as the hitboxes overlap)
    ---@field thrown_into fun(self, victim: Entity): boolean @Same as above, but for being thrown into something and potentially dealing damage that way
    ---@field get_damage_sound fun(self, damage: DAMAGE_TYPE): SOUNDID @returns sound id for the damage taken, return 0 to make it silence
    ---@field copy_extra_info fun(self, clone: Entity, some_entity_uid: integer): nil @Entities must be of the same type!
    ---@field cutscene CutsceneBehavior
    ---@field clear_cutscene fun(self): nil
    ---@field get_base_behavior fun(self, state_id: integer): VanillaMovableBehavior @Gets a vanilla behavior from this movable, needs to be called before `clear_behaviors`<br/>but the returned values are still valid after a call to `clear_behaviors`
    ---@field add_behavior fun(self, behavior: MovableBehavior): nil @Add a behavior to this movable, can be either a `VanillaMovableBehavior` or a<br/>`CustomMovableBehavior`
    ---@field clear_behavior fun(self, behavior: MovableBehavior): nil @Clear a specific behavior of this movable, can be either a `VanillaMovableBehavior` or a<br/>`CustomMovableBehavior`, a behavior with this behaviors `state_id` may be required to<br/>run this movables statemachine without crashing, so add a new one if you are not sure
    ---@field clear_behaviors fun(self): nil @Clears all behaviors of this movable, need to call `add_behavior` to avoid crashing
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_can_jump fun(self, fun: fun(self: Movable): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> can_jump(Movable self)`<br/>Virtual function docs:<br/>Return true if the entity is allowed to jump, even midair. Return false and can't jump, except from ladders apparently.
    ---@field set_post_can_jump fun(self, fun: fun(self: Movable): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil can_jump(Movable self)`<br/>Virtual function docs:<br/>Return true if the entity is allowed to jump, even midair. Return false and can't jump, except from ladders apparently.
    ---@field set_pre_sprint_factor fun(self, fun: fun(self: Movable): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> sprint_factor(Movable self)`
    ---@field set_post_sprint_factor fun(self, fun: fun(self: Movable): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil sprint_factor(Movable self)`
    ---@field set_pre_calculate_jump_velocity fun(self, fun: fun(self: Movable, dont_ignore_liquid: boolean): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> calculate_jump_velocity(Movable self, boolean dont_ignore_liquid)`
    ---@field set_post_calculate_jump_velocity fun(self, fun: fun(self: Movable, dont_ignore_liquid: boolean): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil calculate_jump_velocity(Movable self, boolean dont_ignore_liquid)`
    ---@field set_pre_apply_velocity fun(self, fun: fun(self: Movable, velocities: Vec2, ignore_weight: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool apply_velocity(Movable self, Vec2 velocities, boolean ignore_weight)`<br/>Virtual function docs:<br/>Mostly used for ragdoll by the game
    ---@field set_post_apply_velocity fun(self, fun: fun(self: Movable, velocities: Vec2, ignore_weight: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil apply_velocity(Movable self, Vec2 velocities, boolean ignore_weight)`<br/>Virtual function docs:<br/>Mostly used for ragdoll by the game
    ---@field set_pre_get_damage fun(self, fun: fun(self: Movable): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> get_damage(Movable self)`<br/>Virtual function docs:<br/>Returns the damage that the entity deals
    ---@field set_post_get_damage fun(self, fun: fun(self: Movable): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil get_damage(Movable self)`<br/>Virtual function docs:<br/>Returns the damage that the entity deals
    ---@field set_pre_is_on_fire fun(self, fun: fun(self: Movable): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> is_on_fire(Movable self)`
    ---@field set_post_is_on_fire fun(self, fun: fun(self: Movable): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil is_on_fire(Movable self)`
    ---@field set_pre_attack fun(self, fun: fun(self: Movable, victim: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> attack(Movable self, Entity victim)`<br/>Virtual function docs:<br/>Runs on contact damage, returns false if there wasn't any interaction (called from on_collision2, will be called as long as the hitboxes overlap)
    ---@field set_post_attack fun(self, fun: fun(self: Movable, victim: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil attack(Movable self, Entity victim)`<br/>Virtual function docs:<br/>Runs on contact damage, returns false if there wasn't any interaction (called from on_collision2, will be called as long as the hitboxes overlap)
    ---@field set_pre_thrown_into fun(self, fun: fun(self: Movable, victim: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> thrown_into(Movable self, Entity victim)`<br/>Virtual function docs:<br/>Same as above, but for being thrown into something and potentially dealing damage that way
    ---@field set_post_thrown_into fun(self, fun: fun(self: Movable, victim: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil thrown_into(Movable self, Entity victim)`<br/>Virtual function docs:<br/>Same as above, but for being thrown into something and potentially dealing damage that way
    ---@field set_pre_damage fun(self, fun: fun(self: Movable, damage_dealer: Entity, damage_amount: integer, damage_flags: DAMAGE_TYPE, velocity: Vec2, unknown_damage_phase: integer, stun_amount: integer, iframes: integer, unknown_is_final: boolean): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> damage(Movable self, Entity damage_dealer, integer damage_amount, DAMAGE_TYPE damage_flags, Vec2 velocity, integer unknown_damage_phase, integer stun_amount, integer iframes, boolean unknown_is_final)`<br/>Virtual function docs:<br/>Damage the movable by the specified amount, stuns and gives it invincibility for the specified amount of frames and applies the velocities. `damage_dealer` can be set to nil.<br/>Returns: true if entity was affected (for stuff like: if pot was thrown into entity, should that pot break after hit), false if the event should be ignored by damage_dealer
    ---@field set_post_damage fun(self, fun: fun(self: Movable, damage_dealer: Entity, damage_amount: integer, damage_flags: DAMAGE_TYPE, velocity: Vec2, unknown_damage_phase: integer, stun_amount: integer, iframes: integer, unknown_is_final: boolean): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil damage(Movable self, Entity damage_dealer, integer damage_amount, DAMAGE_TYPE damage_flags, Vec2 velocity, integer unknown_damage_phase, integer stun_amount, integer iframes, boolean unknown_is_final)`<br/>Virtual function docs:<br/>Damage the movable by the specified amount, stuns and gives it invincibility for the specified amount of frames and applies the velocities. `damage_dealer` can be set to nil.<br/>Returns: true if entity was affected (for stuff like: if pot was thrown into entity, should that pot break after hit), false if the event should be ignored by damage_dealer
    ---@field set_pre_on_hit fun(self, fun: fun(self: Movable, damage_dealer: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool on_hit(Movable self, Entity damage_dealer)`<br/>Virtual function docs:<br/>Hit by broken arrows etc that don't deal damage, calls damage with 0 damage.
    ---@field set_post_on_hit fun(self, fun: fun(self: Movable, damage_dealer: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil on_hit(Movable self, Entity damage_dealer)`<br/>Virtual function docs:<br/>Hit by broken arrows etc that don't deal damage, calls damage with 0 damage.
    ---@field set_pre_get_damage_sound fun(self, fun: fun(self: Movable, damage: DAMAGE_TYPE): SOUNDID?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<SOUNDID> get_damage_sound(Movable self, DAMAGE_TYPE damage)`<br/>Virtual function docs:<br/>returns sound id for the damage taken, return 0 to make it silence
    ---@field set_post_get_damage_sound fun(self, fun: fun(self: Movable, damage: DAMAGE_TYPE): SOUNDID?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil get_damage_sound(Movable self, DAMAGE_TYPE damage)`<br/>Virtual function docs:<br/>returns sound id for the damage taken, return 0 to make it silence
    ---@field set_pre_stun fun(self, fun: fun(self: Movable, framecount: integer): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool stun(Movable self, integer framecount)`
    ---@field set_post_stun fun(self, fun: fun(self: Movable, framecount: integer): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil stun(Movable self, integer framecount)`
    ---@field set_pre_freeze fun(self, fun: fun(self: Movable, framecount: integer, ignore_lava: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool freeze(Movable self, integer framecount, boolean ignore_lava)`<br/>Virtual function docs:<br/>Sets the `frozen_timer`, the param `ignore_lava` doesn't do much, just skips the liquid check,<br/>if in lava the game will set `frozen_timer` to 0 immediately most of the time
    ---@field set_post_freeze fun(self, fun: fun(self: Movable, framecount: integer, ignore_lava: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil freeze(Movable self, integer framecount, boolean ignore_lava)`<br/>Virtual function docs:<br/>Sets the `frozen_timer`, the param `ignore_lava` doesn't do much, just skips the liquid check,<br/>if in lava the game will set `frozen_timer` to 0 immediately most of the time
    ---@field set_pre_light_on_fire fun(self, fun: fun(self: Movable, time: integer): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool light_on_fire(Movable self, integer time)`<br/>Virtual function docs:<br/>Does not damage entity
    ---@field set_post_light_on_fire fun(self, fun: fun(self: Movable, time: integer): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil light_on_fire(Movable self, integer time)`<br/>Virtual function docs:<br/>Does not damage entity
    ---@field set_pre_set_cursed fun(self, fun: fun(self: Movable, b: boolean, effect: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool set_cursed(Movable self, boolean b, boolean effect)`
    ---@field set_post_set_cursed fun(self, fun: fun(self: Movable, b: boolean, effect: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil set_cursed(Movable self, boolean b, boolean effect)`
    ---@field set_pre_web_collision fun(self, fun: fun(self: Movable, boolean: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool web_collision(Movable self, boolean boolean)`
    ---@field set_post_web_collision fun(self, fun: fun(self: Movable, boolean: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil web_collision(Movable self, boolean boolean)`
    ---@field set_pre_check_out_of_bounds fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool check_out_of_bounds(Movable self)`<br/>Virtual function docs:<br/>Disable to not get killed outside level bounds.
    ---@field set_post_check_out_of_bounds fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil check_out_of_bounds(Movable self)`<br/>Virtual function docs:<br/>Disable to not get killed outside level bounds.
    ---@field set_pre_set_standing_on fun(self, fun: fun(self: Movable, entity_uid: integer): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool set_standing_on(Movable self, integer entity_uid)`
    ---@field set_post_set_standing_on fun(self, fun: fun(self: Movable, entity_uid: integer): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil set_standing_on(Movable self, integer entity_uid)`
    ---@field set_pre_standing_on fun(self, fun: fun(self: Movable): Entity?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<Entity> standing_on(Movable self)`
    ---@field set_post_standing_on fun(self, fun: fun(self: Movable): Entity?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil standing_on(Movable self)`
    ---@field set_pre_stomped_by fun(self, fun: fun(self: Movable, stomper: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> stomped_by(Movable self, Entity stomper)`
    ---@field set_post_stomped_by fun(self, fun: fun(self: Movable, stomper: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil stomped_by(Movable self, Entity stomper)`
    ---@field set_pre_thrown_by fun(self, fun: fun(self: Movable, thrower: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool thrown_by(Movable self, Entity thrower)`
    ---@field set_post_thrown_by fun(self, fun: fun(self: Movable, thrower: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil thrown_by(Movable self, Entity thrower)`
    ---@field set_pre_cloned_to fun(self, fun: fun(self: Movable, clone: Entity, some_entity_uid: integer): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool cloned_to(Movable self, Entity clone, integer some_entity_uid)`<br/>Virtual function docs:<br/>Entities must be of the same type!
    ---@field set_post_cloned_to fun(self, fun: fun(self: Movable, clone: Entity, some_entity_uid: integer): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil cloned_to(Movable self, Entity clone, integer some_entity_uid)`<br/>Virtual function docs:<br/>Entities must be of the same type!
    ---@field set_pre_pick_up fun(self, fun: fun(self: Movable, entity_to_pick_up: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool pick_up(Movable self, Entity entity_to_pick_up)`
    ---@field set_post_pick_up fun(self, fun: fun(self: Movable, entity_to_pick_up: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil pick_up(Movable self, Entity entity_to_pick_up)`
    ---@field set_pre_can_be_picked_up_by fun(self, fun: fun(self: Movable, entity_picking_up: Entity, boolean: ): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> can_be_picked_up_by(Movable self, Entity entity_picking_up, boolean)`
    ---@field set_post_can_be_picked_up_by fun(self, fun: fun(self: Movable, entity_picking_up: Entity, boolean: ): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil can_be_picked_up_by(Movable self, Entity entity_picking_up, boolean)`
    ---@field set_pre_drop fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool drop(Movable self)`<br/>Virtual function docs:<br/>Called when dropping or throwing
    ---@field set_post_drop fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil drop(Movable self)`<br/>Virtual function docs:<br/>Called when dropping or throwing
    ---@field set_pre_collect_treasure fun(self, fun: fun(self: Movable, value: integer, treasure: ENT_TYPE): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> collect_treasure(Movable self, integer value, ENT_TYPE treasure)`<br/>Virtual function docs:<br/>Adds or subtracts the specified amount of money to the movable's (player's) inventory. Shows the calculation animation in the HUD. Adds treasure to the inventory list shown on transition. Use the global add_money to add money without adding specific treasure.
    ---@field set_post_collect_treasure fun(self, fun: fun(self: Movable, value: integer, treasure: ENT_TYPE): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil collect_treasure(Movable self, integer value, ENT_TYPE treasure)`<br/>Virtual function docs:<br/>Adds or subtracts the specified amount of money to the movable's (player's) inventory. Shows the calculation animation in the HUD. Adds treasure to the inventory list shown on transition. Use the global add_money to add money without adding specific treasure.
    ---@field set_pre_apply_movement fun(self, fun: fun(self: Movable, integer: , integer: , integer: ): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> apply_movement(Movable self, integer, integer, integer)`
    ---@field set_post_apply_movement fun(self, fun: fun(self: Movable, integer: , integer: , integer: ): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil apply_movement(Movable self, integer, integer, integer)`
    ---@field set_pre_is_powerup_capable fun(self, fun: fun(self: Movable): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> is_powerup_capable(Movable self)`
    ---@field set_post_is_powerup_capable fun(self, fun: fun(self: Movable): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil is_powerup_capable(Movable self)`
    ---@field set_pre_initialize fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool initialize(Movable self)`
    ---@field set_post_initialize fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil initialize(Movable self)`
    ---@field set_pre_process_input fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool process_input(Movable self)`
    ---@field set_post_process_input fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil process_input(Movable self)`
    ---@field set_pre_picked_up fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool picked_up(Movable self)`<br/>Virtual function docs:<br/>Called for entity that just has been picked up
    ---@field set_post_picked_up fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil picked_up(Movable self)`<br/>Virtual function docs:<br/>Called for entity that just has been picked up
    ---@field set_pre_release fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool release(Movable self)`<br/>Virtual function docs:<br/>Called for entity that just has been thrown/dropped
    ---@field set_post_release fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil release(Movable self)`<br/>Virtual function docs:<br/>Called for entity that just has been thrown/dropped
    ---@field set_pre_generate_landing_effects fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool generate_landing_effects(Movable self)`<br/>Virtual function docs:<br/>Only for landing on the floor or activefloor, generates "poof" particle and plays sfx (note: when stunned, sfx is played by the damage function)
    ---@field set_post_generate_landing_effects fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil generate_landing_effects(Movable self)`<br/>Virtual function docs:<br/>Only for landing on the floor or activefloor, generates "poof" particle and plays sfx (note: when stunned, sfx is played by the damage function)
    ---@field set_pre_fall fun(self, fun: fun(self: Movable, number: number): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool fall(Movable self, number number)`<br/>Virtual function docs:<br/>Applies gravity to entity. Disable to float like on hoverpack.
    ---@field set_post_fall fun(self, fun: fun(self: Movable, number: number): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil fall(Movable self, number number)`<br/>Virtual function docs:<br/>Applies gravity to entity. Disable to float like on hoverpack.
    ---@field set_pre_apply_friction fun(self, fun: fun(self: Movable, number: , vertical: boolean, number: ): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool apply_friction(Movable self, number, boolean vertical, number)`
    ---@field set_post_apply_friction fun(self, fun: fun(self: Movable, number: , vertical: boolean, number: ): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil apply_friction(Movable self, number, boolean vertical, number)`
    ---@field set_pre_can_break_block fun(self, fun: fun(self: Movable, horizontal: boolean, block: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> can_break_block(Movable self, boolean horizontal, Entity block)`
    ---@field set_post_can_break_block fun(self, fun: fun(self: Movable, horizontal: boolean, block: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil can_break_block(Movable self, boolean horizontal, Entity block)`
    ---@field set_pre_break_block fun(self, fun: fun(self: Movable, camera_shake: boolean, block: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool break_block(Movable self, boolean camera_shake, Entity block)`
    ---@field set_post_break_block fun(self, fun: fun(self: Movable, camera_shake: boolean, block: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil break_block(Movable self, boolean camera_shake, Entity block)`
    ---@field set_pre_crush fun(self, fun: fun(self: Movable, entity: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool crush(Movable self, Entity entity)`
    ---@field set_post_crush fun(self, fun: fun(self: Movable, entity: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil crush(Movable self, Entity entity)`
    ---@field set_pre_body_destruction fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool body_destruction(Movable self)`
    ---@field set_post_body_destruction fun(self, fun: fun(self: Movable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil body_destruction(Movable self)`
local Movable = nil
---Move a movable according to its velocity, can disable gravity
---Will also update `movable.animation_frame` and various timers and counters
---@param disable_gravity boolean
---@return nil
function Movable:generic_update_world(disable_gravity) end
---Move a movable according to its velocity, update physics, gravity, etc.
---Will also update `movable.animation_frame` and various timers and counters
---@return nil
function Movable:generic_update_world() end
---Move a movable according to its velocity and `move`, if the movables `BUTTON.RUN` is
---held apply `sprint_factor` on `move.x`, can disable gravity or lock its horizontal
---movement via `on_rope`. Use this for example to update a custom enemy type.
---Will also update `movable.animation_frame` and various timers and counters
---@param move Vec2
---@param sprint_factor number
---@param disable_gravity boolean
---@param on_rope boolean
---@return nil
function Movable:generic_update_world(move, sprint_factor, disable_gravity, on_rope) end

---@class CutsceneBehavior

---@class PowerupCapable : Movable
    ---@field remove_powerup fun(self, powerup_type: ENT_TYPE): nil @Removes a currently applied powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Removing the Eggplant crown does not seem to undo the throwing of eggplants, the other powerups seem to work.
    ---@field give_powerup fun(self, powerup_type: ENT_TYPE): nil @Gives the player/monster the specified powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Giving true crown to a monster crashes the game.
    ---@field has_powerup fun(self, powerup_type: ENT_TYPE): boolean @Checks whether the player/monster has a certain powerup
    ---@field get_powerups fun(self): ENT_TYPE[] @Return all powerups that the entity has
    ---@field unequip_backitem fun(self): nil @Unequips the currently worn backitem
    ---@field worn_backitem fun(self): integer @Returns the uid of the currently worn backitem, or -1 if wearing nothing
    ---@field set_jetpack_fuel fun(self, fuel: integer): nil
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_blood_collision fun(self, fun: fun(self: PowerupCapable): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> blood_collision(PowerupCapable self)`<br/>Virtual function docs:<br/>only triggers when entity has kapala
    ---@field set_post_blood_collision fun(self, fun: fun(self: PowerupCapable): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil blood_collision(PowerupCapable self)`<br/>Virtual function docs:<br/>only triggers when entity has kapala

---@class Inventory
    ---@field money integer @Sum of the money collected in current level
    ---@field bombs integer
    ---@field ropes integer
    ---@field player_slot integer
    ---@field poison_tick_timer integer @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field cursed boolean @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field elixir_buff boolean @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field health integer @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field kapala_blood_amount integer @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field time_of_death integer @Is set to state.time_total when player dies in coop (to determinate who should be first to re-spawn from coffin)
    ---@field held_item ENT_TYPE @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field held_item_metadata integer @Metadata of the held item (health, is cursed etc.)<br/>Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field saved_pets_count integer
    ---@field mount_type ENT_TYPE @Used to transfer information to transition/next level (player riding a mount). Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field mount_metadata integer @Metadata of the mount (health, is cursed etc.)<br/>Used to transfer information to transition/next level (player riding a mount). Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field kills_level integer
    ---@field kills_total integer
    ---@field collected_money_total integer @Total money collected during previous levels (so excluding the current one)
    ---@field collected_money_count integer @Count/size for the `collected_money` arrays
    ---@field collected_money ENT_TYPE[] @size: 512 @Types of gold/gems collected during this level, used later to display during the transition
    ---@field collected_money_values integer[] @size: 512 @Values of gold/gems collected during this level, used later to display during the transition
    ---@field killed_enemies ENT_TYPE[] @size: 256 @Types of enemies killed during this level, used later to display during the transition
    ---@field companion_count integer @Number of companions, it will determinate how many companions will be transferred to next level<br/>Increments when player acquires new companion, decrements when one of them dies
    ---@field companions ENT_TYPE[] @size: 8 @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field companion_held_items ENT_TYPE[] @size: 8 @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field companion_held_item_metadatas integer[] @size: 8 @Metadata of items held by companions (health, is cursed etc.)<br/>Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field companion_trust integer[] @size: 8 @(0..3) Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field companion_health integer[] @size: 8 @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field companion_poison_tick_timers integer[] @size: 8 @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field is_companion_cursed boolean[] @size: 8 @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ---@field acquired_powerups ENT_TYPE[] @size: 30 @Used to transfer information to transition/next level. Is not updated during a level<br/>You can use `ON.PRE_LEVEL_GENERATION` to access/edit this

---@class Ai
    ---@field target Entity
    ---@field target_uid integer
    ---@field timer integer
    ---@field state integer @AI state (patrol, sleep, attack, aggro...)
    ---@field last_state integer
    ---@field trust integer @Levels completed with, 0..3
    ---@field whipped integer @Number of times whipped by player
    ---@field walk_pause_timer integer @positive: walking, negative: waiting/idle

---@class Player : PowerupCapable
    ---@field inventory Inventory
    ---@field emitted_light Illumination
    ---@field linked_companion_parent integer @entity uid
    ---@field linked_companion_child integer @entity uid
    ---@field ai Ai
    ---@field input PlayerSlot
    ---@field basecamp_button_entity Entity @Used in base camp to talk with the NPC's
    ---@field special_sound SoundMeta @For Lise System walking and looking up sounds
    ---@field jump_lock_timer integer @Increases when holding jump button in the air, set to max while jumping. If this isn't 0, a jump will only be<br/>registered if the jump button was not held on the previous frame.
    ---@field coyote_timer integer @can jump while airborne if greater than 0
    ---@field swim_timer integer @Timer between strokes when holding jump button in water.
    ---@field hired_hand_name integer @0-25 alphabetical index of hired hand names.
    ---@field kapala_blood_amount fun(self): integer
    ---@field get_name fun(self): string @Get the full name of the character, this will be the modded name not only the vanilla name.
    ---@field get_short_name fun(self): string @Get the short name of the character, this will be the modded name not only the vanilla name.
    ---@field get_heart_color fun(self): Color @Get the heart color of the character, this will be the modded heart color not only the vanilla heart color.
    ---@field is_female fun(self): boolean @Check whether the character is female, will be `true` if the character was modded to be female as well.
    ---@field set_heart_color fun(self, hcolor: Color): nil @Set the heart color for the character.
    ---@field let_go fun(self): nil @Drops from ladders, ropes and ledge grabs

---@class Floor : Entity
    ---@field deco_top integer
    ---@field deco_bottom integer
    ---@field deco_left integer
    ---@field deco_right integer
    ---@field fix_border_tile_animation fun(self): nil @Sets `animation_frame` of the floor for types `FLOOR_BORDERTILE`, `FLOOR_BORDERTILE_METAL` and `FLOOR_BORDERTILE_OCTOPUS`.
    ---@field fix_decorations fun(self, fix_also_neighbors: boolean, fix_styled_floor: boolean): nil @Used to add decoration to a floor entity after it was spawned outside of level gen, is not necessary when spawning during level gen.<br/>Set `fix_also_neighbors` to `true` to fix the neighboring floor tile decorations on the border of the two tiles.<br/>Set `fix_styled_floor` to `true` to fix decorations on `FLOORSTYLED_*` entities, those usually only have decorations when broken.
    ---@field add_decoration fun(self, side: FLOOR_SIDE): nil @Explicitly add a decoration on the given side. Corner decorations only exist for `FLOOR_BORDERTILE` and `FLOOR_BORDERTILE_OCTOPUS`.
    ---@field remove_decoration fun(self, side: FLOOR_SIDE): nil @Explicitly remove a decoration on the given side. Corner decorations only exist for `FLOOR_BORDERTILE` and `FLOOR_BORDERTILE_OCTOPUS`.
    ---@field decorate_internal fun(self): nil
    ---@field get_floor_type fun(self): ENT_TYPE @Returns it's ENT_TYPE except for FLOOR_PEN (returns FLOORSTYLED_MINEWOOD) and FLOOR_QUICKSAND, FLOOR_TOMB, FLOOR_EMPRESS_GRAVE which return FLOOR_GENERIC
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_floor_update fun(self, fun: fun(self: Floor): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool floor_update(Floor self)`
    ---@field set_post_floor_update fun(self, fun: fun(self: Floor): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil floor_update(Floor self)`

---@class Door : Floor
    ---@field counter integer
    ---@field fx_button Entity
    ---@field enter fun(self, who: Entity): integer @Returns the entity state / behavior id to set the entity to after the entering animation.
    ---@field light_level fun(self): number @Returns the darkest light level used to fade the entity when entering or exiting. 0 = black, 1 = no change
    ---@field is_unlocked fun(self): boolean @Should we display the button prompt when collided by player. Will always return `true` for exits, layers and others that the game never locks, even if you lock it with `unlock` function
    ---@field can_enter fun(self, player: Entity): boolean @Can the door actually be entered by player. Overrides the button prompt too if false.
    ---@field unlock fun(self, unlock: boolean): nil @Lock/Unlock doors
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_enter_attempt fun(self, fun: fun(self: Door, who: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool enter_attempt(Door self, Entity who)`
    ---@field set_post_enter_attempt fun(self, fun: fun(self: Door, who: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil enter_attempt(Door self, Entity who)`
    ---@field set_pre_hide_hud fun(self, fun: fun(self: Door, who: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool hide_hud(Door self, Entity who)`
    ---@field set_post_hide_hud fun(self, fun: fun(self: Door, who: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil hide_hud(Door self, Entity who)`
    ---@field set_pre_enter fun(self, fun: fun(self: Door, who: Entity): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> enter(Door self, Entity who)`<br/>Virtual function docs:<br/>Returns the entity state / behavior id to set the entity to after the entering animation.
    ---@field set_post_enter fun(self, fun: fun(self: Door, who: Entity): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil enter(Door self, Entity who)`<br/>Virtual function docs:<br/>Returns the entity state / behavior id to set the entity to after the entering animation.
    ---@field set_pre_light_level fun(self, fun: fun(self: Door): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> light_level(Door self)`<br/>Virtual function docs:<br/>Returns the darkest light level used to fade the entity when entering or exiting. 0 = black, 1 = no change
    ---@field set_post_light_level fun(self, fun: fun(self: Door): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil light_level(Door self)`<br/>Virtual function docs:<br/>Returns the darkest light level used to fade the entity when entering or exiting. 0 = black, 1 = no change
    ---@field set_pre_is_unlocked fun(self, fun: fun(self: Door): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> is_unlocked(Door self)`<br/>Virtual function docs:<br/>Should we display the button prompt when collided by player. Will always return `true` for exits, layers and others that the game never locks, even if you lock it with `unlock` function
    ---@field set_post_is_unlocked fun(self, fun: fun(self: Door): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil is_unlocked(Door self)`<br/>Virtual function docs:<br/>Should we display the button prompt when collided by player. Will always return `true` for exits, layers and others that the game never locks, even if you lock it with `unlock` function
    ---@field set_pre_can_enter fun(self, fun: fun(self: Door, player: Entity): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> can_enter(Door self, Entity player)`<br/>Virtual function docs:<br/>Can the door actually be entered by player. Overrides the button prompt too if false.
    ---@field set_post_can_enter fun(self, fun: fun(self: Door, player: Entity): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil can_enter(Door self, Entity player)`<br/>Virtual function docs:<br/>Can the door actually be entered by player. Overrides the button prompt too if false.

---@class ExitDoor : Door
    ---@field entered boolean @if true entering it does not load the transition
    ---@field special_door boolean @use provided world/level/theme
    ---@field level integer
    ---@field timer integer
    ---@field world integer
    ---@field theme integer

---@class DecoratedDoor : ExitDoor
    ---@field special_bg Entity

---@class LockedDoor : Door
    ---@field unlocked boolean

---@class CityOfGoldDoor : DecoratedDoor
    ---@field unlocked boolean

---@class MainExit : ExitDoor
    ---@field sound SoundMeta
    ---@field door_blocker Entity @Normally `FX_MAIN_EXIT_DOOR` but setting any entity here will block the door

---@class EggShipDoor : Door
    ---@field timer integer

---@class EggShipDoorS : EggShipDoor
    ---@field entered boolean

---@class Arrowtrap : Floor
    ---@field arrow_shot boolean
    ---@field rearm fun(self): nil
    ---@field trigger fun(self, who_uid: integer): nil @The uid must be movable entity for ownership transfers

---@class TotemTrap : Floor
    ---@field spawn_entity_type ENT_TYPE
    ---@field first_sound_id SOUNDID
    ---@field trigger fun(self, who_uid: integer, left: boolean): nil @The uid must be movable entity for ownership transfers

---@class LaserTrap : Floor
    ---@field emitted_light Illumination
    ---@field timer integer @counts up from 0 after triggering, cannot shoot again until 360
    ---@field trigger fun(self, who_uid: integer): nil @The uid must be movable entity for ownership transfers

---@class SparkTrap : Floor
    ---@field emitted_light Illumination
    ---@field spark_uid integer

---@class Altar : Floor
    ---@field timer integer @for normal altar: counts from 0 to 20 then 0, then 1 then 0 and sacrifice happens

---@class SpikeballTrap : Floor
    ---@field sound SoundMeta
    ---@field chain Entity
    ---@field end_piece Entity
    ---@field state integer @0 - none, 1 - start, 2 - going_down, 3 - going_up, 4 - pause; going_up is only right when timer is 0, otherwise it just sits at the bottom
    ---@field timer integer @for the start and retract

---@class TransferFloor : Floor
    ---@field transferred_entities table<integer, integer> @Index is the uid, value is frame the entity entered the floor (time_level), use `pairs` to loop thru

---@class ConveyorBelt : TransferFloor
    ---@field timer integer

---@class Pipe : Floor
    ---@field direction_type integer @3 - straight_horizontal, 4 - blocked, 5 - down_left_turn, 6 - down_right_turn, 8 - blocked, 9 - up_left_turn, 10 - up_right_turn, 12 - straight_vertical
    ---@field end_pipe boolean

---@class Generator : Floor
    ---@field spawned_uid integer
    ---@field set_timer integer
    ---@field timer integer
    ---@field start_counter integer @Applicable only for ENT_TYPE`.FLOOR_SUNCHALLENGE_GENERATOR`
    ---@field on_off boolean @Applicable only for ENT_TYPE`.FLOOR_SUNCHALLENGE_GENERATOR`

---@class SlidingWallCeiling : Floor
    ---@field attached_piece Entity
    ---@field active_floor_part_uid integer
    ---@field state integer @1 - going up / is at the top, 2 - pause
    ---@field ball_rise SoundMeta
    ---@field ball_drop SoundMeta

---@class QuickSand : Floor

---@class BigSpearTrap : Floor
    ---@field spear_uid integer
    ---@field left_part boolean @setting the left part to 0 or right part to 1 destroys the trap
    ---@field trigger fun(self, who_uid: integer, left: boolean): nil @The uid must be movable entity for ownership transfers, has to be called on the left part of the trap,

---@class StickyTrap : Floor
    ---@field sound SoundMeta
    ---@field attached_piece_uid integer
    ---@field ball_uid integer
    ---@field state integer @0 - none, 1 - start, 2 - going down, 3 - is at the bottom, 4 - going up, 5 - pause
    ---@field timer integer

---@class MotherStatue : Floor
    ---@field players_standing boolean[] @size: 4 @Table of player1_standing, player2_standing, ... etc.
    ---@field player1_standing boolean
    ---@field player2_standing boolean
    ---@field player3_standing boolean
    ---@field player4_standing boolean
    ---@field players_health_received boolean[] @size: 4 @Table of player1_health_received, player2_health_received, ... etc.
    ---@field player1_health_received boolean
    ---@field player2_health_received boolean
    ---@field player3_health_received boolean
    ---@field player4_health_received boolean
    ---@field players_health_timer integer[] @size: 4 @Table of player1_health_timer, player2_health_timer, ... etc.
    ---@field player1_health_timer integer
    ---@field player2_health_timer integer
    ---@field player3_health_timer integer
    ---@field player4_health_timer integer
    ---@field eggplantchild_timer integer
    ---@field eggplantchild_detected boolean

---@class TeleportingBorder : Floor
    ---@field direction integer @0 - right, 1 - left, 2 - bottom, 3 - top, 4 - disable

---@class ForceField : Floor
    ---@field first_item_beam Entity
    ---@field fx Entity
    ---@field sound SoundMeta
    ---@field emitted_light Illumination
    ---@field is_on boolean
    ---@field activate_laserbeam fun(self, turn_on: boolean): nil

---@class TimedForceField : ForceField
    ---@field timer integer
    ---@field pause boolean

---@class HorizontalForceField : Floor
    ---@field first_item_beam Entity
    ---@field fx Entity
    ---@field sound SoundMeta
    ---@field timer integer
    ---@field is_on boolean

---@class TentacleBottom : Floor
    ---@field attached_piece_uid integer
    ---@field tentacle_uid integer
    ---@field state integer @0 - none, 1 - start, 2 - moving up, 3 - at the top, 4 - moving down 5 - pause

---@class PoleDeco : Floor
    ---@field deco_up integer
    ---@field deco_down integer

---@class JungleSpearTrap : Floor
    ---@field trigger fun(self, who_uid: integer, direction: integer): nil @The uid must be movable entity for ownership transfers, direction: 1 = left, 2 = right, 3 = up, 4 = down

---@class Crushtrap : Movable
    ---@field dirx number
    ---@field diry number
    ---@field timer integer @counts from 30 to 0 before moving, after it stops, counts from 60 to 0 before it can be triggered again
    ---@field bounce_back_timer integer @counts from 7 to 0 after it hits the wall and moves away until the timer hits 0, then moves back and counts from 255 until it hits the wall again, if needed it will start the counter again for another bounce

---@class Olmec : Movable
    ---@field sound SoundMeta
    ---@field target_uid integer
    ---@field attack_phase integer @0 = stomp, 1 = bombs, 2 = stomp+ufos, 3 = in lava
    ---@field attack_timer integer @in phase 0/2: time spent looking for player, in phase 1: time between bomb salvo
    ---@field ai_timer integer @general timer that counts down whenever olmec is active
    ---@field move_direction integer @-1 = left, 0 = down, 1 = right, phase 0/2: depends on target, phase 1: travel direction
    ---@field jump_timer integer
    ---@field phase1_amount_of_bomb_salvos integer
    ---@field unknown_attack_state integer
    ---@field broken_floaters fun(self): integer

---@class WoodenlogTrap : Movable
    ---@field ceiling_1_uid integer
    ---@field ceiling_2_uid integer
    ---@field falling_speed number

---@class Boulder : Movable
    ---@field is_rolling integer @is set to 1 when the boulder first hits the ground

---@class PushBlock : Movable
    ---@field sound SoundMeta
    ---@field dust_particle ParticleEmitterInfo
    ---@field dest_pos_x number

---@class BoneBlock : Movable

---@class ChainedPushBlock : PushBlock
    ---@field is_chained boolean

---@class LightArrowPlatform : Movable
    ---@field emitted_light Illumination

---@class FallingPlatform : Movable
    ---@field timer integer
    ---@field shaking_factor number
    ---@field y_pos number

---@class UnchainedSpikeBall : Movable
    ---@field bounce boolean

---@class Drill : Movable
    ---@field sound1 SoundMeta
    ---@field sound2 SoundMeta
    ---@field top_chain_piece Entity
    ---@field trigger fun(self, play_sound_effect: boolean?): nil

---@class ThinIce : Movable
    ---@field strength integer @counts down when standing on, maximum is 134 as based of this value it changes animation_frame, and above that value it changes to wrong sprite

---@class Elevator : Movable
    ---@field emitted_light Illumination
    ---@field timer integer @pause timer, counts down 60 to 0
    ---@field moving_up boolean

---@class ClamBase : Movable
    ---@field treasure_type ENT_TYPE
    ---@field treasure_uid integer
    ---@field treasure_x_pos number
    ---@field treasure_y_pos number
    ---@field top_part_uid integer

---@class RegenBlock : Movable
    ---@field on_breaking boolean

---@class TimedPowderkeg : PushBlock
    ---@field timer integer @timer till explosion, -1 = pause, counts down

---@class CrushElevator : Movable
    ---@field y_limit number @This is custom variable, you need [activate_crush_elevator_hack](#activate_crush_elevator_hack) to use it
    ---@field speed number @This is custom variable, you need [activate_crush_elevator_hack](#activate_crush_elevator_hack) to use it

---@class Mount : PowerupCapable
    ---@field rider_uid integer
    ---@field sound SoundMeta
    ---@field can_doublejump boolean
    ---@field tamed boolean
    ---@field walk_pause_timer integer
    ---@field taming_timer integer
    ---@field used_double_jump fun(self): boolean
    ---@field remove_rider fun(self): nil
    ---@field carry fun(self, rider: Movable): nil
    ---@field tame fun(self, value: boolean): nil
    ---@field get_rider_offset fun(self, offset: Vec2): Vec2
    ---@field get_rider_offset_crouching fun(self, value: Vec2): Vec2
    ---@field get_jump_sound fun(self, double_jump: boolean): SOUNDID
    ---@field get_attack_sound fun(self): SOUNDID
    ---@field get_mounting_sound fun(self): SOUNDID
    ---@field get_walking_sound fun(self): SOUNDID
    ---@field get_untamed_loop_sound fun(self): SOUNDID

---@class Rockdog : Mount
    ---@field attack_cooldown integer

---@class Axolotl : Mount
    ---@field attack_cooldown integer
    ---@field can_teleport boolean

---@class Mech : Mount
    ---@field crouch_walk_sound SoundMeta
    ---@field explosion_sound SoundMeta
    ---@field gun_cooldown integer
    ---@field walking boolean
    ---@field breaking_wall boolean

---@class Qilin : Mount
    ---@field fly_gallop_sound SoundMeta
    ---@field attack_cooldown integer

---@class Monster : PowerupCapable
    ---@field chased_target_uid integer
    ---@field target_selection_timer integer

---@class RoomOwner : Monster
    ---@field room_index integer
    ---@field climb_y_direction number
    ---@field ai_state integer
    ---@field patrol_timer integer
    ---@field lose_interest_timer integer
    ---@field countdown_timer integer @can't shot when the timer is running
    ---@field is_patrolling boolean
    ---@field aggro_trigger boolean @setting this makes him angry, if it's shopkeeper you get 2 aggro points
    ---@field was_hurt boolean @also is set true if you set aggro to true, get's trigger even when whipping
    ---@field should_attack_on_sight fun(self): boolean
    ---@field is_angry_flag_set fun(self): boolean
    ---@field weapon_type fun(self): ENT_TYPE

---@class WalkingMonster : Monster
    ---@field chatting_to_uid integer
    ---@field walk_pause_timer integer @alternates between walking and pausing every time it reaches zero
    ---@field cooldown_timer integer @used for chatting with other monsters, attack cooldowns etc.

---@class NPC : Monster
    ---@field climb_direction number
    ---@field target_in_sight_timer integer
    ---@field ai_state integer
    ---@field aggro boolean @for bodyguard and shopkeeperclone it spawns a weapon as well
    ---@field should_attack_on_sight fun(self): boolean
    ---@field weapon_type fun(self): ENT_TYPE

---@class Ghost : Monster
    ---@field split_timer integer @for SMALL_HAPPY this is also the sequence timer of its various states
    ---@field wobble_timer integer
    ---@field pace_timer integer @Controls ghost pacing when all players are dead.
    ---@field velocity_multiplier number
    ---@field ghost_behaviour GHOST_BEHAVIOR
    ---@field emitted_light Illumination
    ---@field linked_ghost Entity
    ---@field sound SoundMeta
    ---@field blown_by_player boolean
    ---@field happy_dancing_clockwise boolean
    ---@field target_dist_visibility_factor number
    ---@field target_layer_visibility_factor number

---@class Bat : Monster
    ---@field spawn_x number
    ---@field spawn_y number

---@class Jiangshi : Monster
    ---@field wait_timer integer @wait time between jumps
    ---@field jump_counter integer @only female aka assassin: when 0 will jump up into ceiling
    ---@field on_ceiling boolean @only female aka assassin

---@class Monkey : Monster
    ---@field sound SoundMeta
    ---@field jump_timer integer
    ---@field on_vine boolean

---@class GoldMonkey : Monster
    ---@field jump_timer integer
    ---@field poop_timer integer
    ---@field poop_count integer

---@class Mole : Monster
    ---@field burrowing_sound SoundMeta
    ---@field nonburrowing_sound SoundMeta
    ---@field burrowing_particle ParticleEmitterInfo
    ---@field burrow_dir_x number
    ---@field burrow_dir_y number
    ---@field burrowing_in_uid integer @stores the last uid as well
    ---@field counter_burrowing integer
    ---@field counter_nonburrowing integer
    ---@field countdown_for_appearing integer
    ---@field digging_state integer @0 - non_burrowed, 1 - unknown, 2 - burrowed, 3 - state_change

---@class Spider : Monster
    ---@field ceiling_pos_x number
    ---@field ceiling_pos_y number
    ---@field jump_timer integer @For the giant spider, some times he shot web instead of jumping
    ---@field trigger_distance number @only in the x coordinate
    ---@field on_ceiling fun(self): boolean

---@class HangSpider : Monster
    ---@field dangle_jump_timer integer
    ---@field ceiling_pos_x number
    ---@field ceiling_pos_y number

---@class Shopkeeper : RoomOwner
    ---@field name integer @0 - Ali, 1 - Bob, 2 - Comso ... and so one, anything above 28 is just random string, can crash the game
    ---@field shotgun_attack_delay integer @can't shot when the timer is running
    ---@field has_key boolean @will drop key after stun/kill
    ---@field is_ear boolean
    ---@field shop_owner boolean

---@class Yang : RoomOwner
    ---@field turkeys_in_den integer[] @Table of uids of the turkeys, goes only up to 3, is nil when yang is angry
    ---@field first_message_shown boolean @I'm looking for turkeys, wanna help?
    ---@field quest_incomplete boolean @Is set to false when the quest is over (Yang dead or second turkey delivered)
    ---@field special_message_shown boolean @Tusk palace/black market/one way door - message shown

---@class Tun : RoomOwner
    ---@field arrows_left integer
    ---@field reload_timer integer @when 0, a new arrow is loaded into the bow; resets when she finds an arrow on the ground
    ---@field challenge_fee_paid boolean @affect only the speech bubble
    ---@field congrats_challenge boolean @congrats message shown after exiting a challenge
    ---@field murdered boolean
    ---@field shop_entered boolean
    ---@field tiamat_encounter boolean @if set to false, greets you with 'you've done well to reach this place'

---@class Pet : Monster
    ---@field fx_button Entity
    ---@field petting_by_uid integer @person whos petting it, only in the camp
    ---@field yell_counter integer @counts up to 400 (6.6 sec), when 0 the pet yells out
    ---@field func_timer integer @used when free running in the camp
    ---@field active_state integer @-1 = sitting and yelling, 0 = either running, dead or picked up
    ---@field petted_counter integer @number of times petted in the camp

---@class Caveman : WalkingMonster
    ---@field wake_up_timer integer
    ---@field can_pick_up_timer integer @0 = can pick something up, when holding forced to 179, after tripping and regaining consciousness counts down to 0
    ---@field aggro_timer integer @keeps resetting when angry and a player is nearby

---@class CavemanShopkeeper : WalkingMonster
    ---@field tripping boolean
    ---@field shop_entered boolean

---@class HornedLizard : Monster
    ---@field eaten_uid integer @dungbeetle being eaten
    ---@field walk_pause_timer integer @alternates between walking and pausing when timer reaches zero
    ---@field attack_cooldown_timer integer @won't attack until timer reaches zero
    ---@field blood_squirt_timer integer
    ---@field sound SoundMeta
    ---@field particle ParticleEmitterInfo

---@class Mosquito : Monster
    ---@field direction_x number
    ---@field direction_y number
    ---@field stuck_rel_pos_x number
    ---@field stuck_rel_pos_y number
    ---@field sound SoundMeta
    ---@field timer integer

---@class Mantrap : Monster
    ---@field walk_pause_timer integer @alternates between walking and pausing every time it reaches zero
    ---@field eaten_uid integer @the uid of the entity the mantrap has eaten, in case it can break out, like a shopkeeper

---@class Skeleton : Monster
    ---@field explosion_timer integer @-1 = never explodes

---@class Scarab : Monster
    ---@field sound SoundMeta
    ---@field emitted_light Illumination
    ---@field timer integer @how long to stay in current position

---@class Imp : Monster
    ---@field carrying_uid integer
    ---@field patrol_y_level number

---@class Lavamander : Monster
    ---@field emitted_light Illumination
    ---@field shoot_lava_timer integer @when this timer reaches zero, it appears on the surface/shoots lava, triggers on player proximity
    ---@field jump_pause_timer integer
    ---@field lava_detection_timer integer
    ---@field is_hot boolean
    ---@field player_detect_state integer @0 - didn't see player, 1 - saw player, 2 - spitted lava; probably used so he won't spit imminently after seeing the player

---@class Firebug : Monster
    ---@field sound SoundMeta
    ---@field fire_timer integer
    ---@field going_up boolean
    ---@field detached_from_chain boolean

---@class FirebugUnchained : Monster
    ---@field sound SoundMeta
    ---@field max_flight_height number
    ---@field ai_timer integer
    ---@field walking_timer integer

---@class Robot : WalkingMonster
    ---@field sound SoundMeta
    ---@field emitted_light_explosion Illumination

---@class Quillback : WalkingMonster
    ---@field sound SoundMeta
    ---@field particle ParticleEmitterInfo
    ---@field seen_player boolean

---@class Leprechaun : WalkingMonster
    ---@field sound SoundMeta
    ---@field hump_timer integer
    ---@field target_in_sight_timer integer
    ---@field gold integer @amount of gold he picked up, will be drooped on death
    ---@field timer_after_humping integer
    ---@field collected_treasure ENT_TYPE[]

---@class Crocman : WalkingMonster
    ---@field teleport_cooldown integer

---@class Mummy : Monster
    ---@field walk_pause_timer integer

---@class VanHorsing : NPC
    ---@field show_text boolean @if set to true, he will say "I've been hunting this fiend a long time!" when on screen
    ---@field special_message_shown boolean @one way door message has been shown

---@class WitchDoctor : WalkingMonster
    ---@field sound SoundMeta
    ---@field skull_regen_timer integer

---@class WitchDoctorSkull : Monster
    ---@field witch_doctor_uid integer
    ---@field emitted_light Illumination
    ---@field sound SoundMeta
    ---@field rotation_angle number

---@class ForestSister : NPC
    ---@field walk_pause_timer integer

---@class Vampire : Monster
    ---@field jump_trigger_distance_x number
    ---@field jump_trigger_distance_y number
    ---@field sleep_pos_x number
    ---@field sleep_pos_y number
    ---@field walk_pause_timer integer

---@class Vlad : Vampire
    ---@field teleport_timer integer @triggers when Vlad teleports, when timer running he can't teleport and will stun when hit
    ---@field aggro boolean @or is awake

---@class Waddler : RoomOwner
    ---@field player_detected boolean
    ---@field on_the_ground boolean
    ---@field air_timer integer

---@class Octopus : WalkingMonster

---@class Bodyguard : NPC
    ---@field position_state integer @0 - none, 1 - Tusk dice shop, 2 - Entrance to pleasure palace, 3 - Basement entrance to pleasure palace
    ---@field message_shown boolean

---@class Fish : Monster
    ---@field change_direction_timer integer

---@class GiantFish : Monster
    ---@field sound SoundMeta
    ---@field change_direction_timer integer @when bouncing into a wall
    ---@field lose_interest_timer integer @delay in-between attacks

---@class Crabman : Monster
    ---@field walk_pause_timer integer
    ---@field invincibility_timer integer
    ---@field poison_attack_timer integer
    ---@field attacking_claw_uid integer
    ---@field at_maximum_attack boolean

---@class Kingu : Monster
    ---@field sound1 SoundMeta @initialized when breaking the shell (sliding down sound maybe?)
    ---@field sound2 SoundMeta @Turning into stone sound
    ---@field climb_direction_x number
    ---@field climb_direction_y number
    ---@field climb_pause_timer integer
    ---@field shell_invincibility_timer integer
    ---@field monster_spawn_timer integer
    ---@field initial_shell_health integer @Excalibur wipes out immediately, bombs take off 11 points, when 0 vulnerable to whip
    ---@field player_seen_by_kingu boolean

---@class Anubis : Monster
    ---@field spawn_x number
    ---@field spawn_y number
    ---@field attack_proximity_y number
    ---@field attack_proximity_x number
    ---@field ai_timer integer
    ---@field next_attack_timer integer
    ---@field psychic_orbs_counter integer
    ---@field awake boolean

---@class Cobra : Monster
    ---@field spit_timer integer

---@class CatMummy : Monster
    ---@field ai_state integer
    ---@field attack_timer integer

---@class Sorceress : WalkingMonster
    ---@field inbetween_attack_timer integer
    ---@field in_air_timer number
    ---@field halo_emitted_light Illumination
    ---@field fx_entity Entity
    ---@field sound SoundMeta
    ---@field hover_timer integer

---@class MagmaMan : Monster
    ---@field emitted_light Illumination
    ---@field sound SoundMeta
    ---@field particle ParticleEmitterInfo
    ---@field jump_timer integer
    ---@field alive_timer integer

---@class Bee : Monster
    ---@field can_rest boolean
    ---@field sound SoundMeta
    ---@field fly_hang_timer integer
    ---@field targeting_timer integer
    ---@field walk_start_time integer
    ---@field walk_end_time integer
    ---@field wobble_x number
    ---@field wobble_y number

---@class Ammit : Monster
    ---@field walk_pause_timer integer
    ---@field particle ParticleEmitterInfo

---@class ApepPart : Monster
    ---@field y_pos number
    ---@field sine_angle number
    ---@field sync_timer integer @or pause timer, used to sync the body parts moving up and down

---@class ApepHead : ApepPart
    ---@field sound1 SoundMeta
    ---@field sound2 SoundMeta
    ---@field distance_traveled number
    ---@field tail_uid integer
    ---@field fx_mouthpiece1_uid integer
    ---@field fx_mouthpiece2_uid integer

---@class OsirisHead : Monster
    ---@field right_hand_uid integer @right from his perspective
    ---@field left_hand_uid integer
    ---@field moving_left boolean
    ---@field targeting_timer integer
    ---@field invincibility_timer integer

---@class OsirisHand : Monster
    ---@field attack_cooldown_timer integer

---@class Alien : Monster
    ---@field jump_timer integer

---@class UFO : Monster
    ---@field sound SoundMeta
    ---@field patrol_distance integer
    ---@field attack_cooldown_timer integer
    ---@field is_falling boolean

---@class Lahamu : Monster
    ---@field sound SoundMeta
    ---@field eyeball Entity
    ---@field attack_cooldown_timer integer

---@class YetiQueen : Monster
    ---@field walk_pause_timer integer

---@class YetiKing : Monster
    ---@field walk_pause_timer integer
    ---@field emitted_light Illumination
    ---@field particle_fog ParticleEmitterInfo
    ---@field particle_dust ParticleEmitterInfo
    ---@field particle_sparkles ParticleEmitterInfo

---@class Lamassu : Monster
    ---@field sound SoundMeta
    ---@field attack_effect_entity Entity
    ---@field particle ParticleEmitterInfo
    ---@field emitted_light Illumination
    ---@field walk_pause_timer integer
    ---@field flight_timer integer
    ---@field attack_timer integer
    ---@field attack_angle number

---@class Olmite : WalkingMonster
    ---@field armor_on boolean
    ---@field in_stack boolean @disables the attack, stun, lock's looking left flag between stack
    ---@field in_stack2 boolean @is set to false couple frame after being detached from stack
    ---@field on_top_uid integer
    ---@field y_offset number
    ---@field attack_cooldown_timer integer

---@class Tiamat : Monster
    ---@field sound SoundMeta @Turning into stone sound
    ---@field fx_tiamat_head integer
    ---@field fx_tiamat_arm_right1 integer
    ---@field fx_tiamat_arm_right2 integer
    ---@field frown_timer integer
    ---@field damage_timer integer
    ---@field attack_timer integer
    ---@field tail_angle number
    ---@field tail_radian number
    ---@field tail_move_speed number
    ---@field right_arm_angle number
    ---@field attack_x number @This is custom variable, you need [activate_tiamat_position_hack](#activate_tiamat_position_hack) to use it
    ---@field attack_y number @This is custom variable, you need [activate_tiamat_position_hack](#activate_tiamat_position_hack) to use it

---@class GiantFrog : Monster
    ---@field door_front_layer Entity
    ---@field door_back_layer Entity
    ---@field platform Entity
    ---@field attack_timer integer
    ---@field frogs_ejected_in_cycle integer
    ---@field invincibility_timer integer
    ---@field mouth_close_timer integer
    ---@field mouth_open_trigger boolean @opens the mouth and starts mouth_close_timer, used when detecting grub in the mouth area

---@class Frog : Monster
    ---@field grub_being_eaten_uid integer
    ---@field jump_timer integer
    ---@field pause boolean

---@class FireFrog : Frog
    ---@field sound SoundMeta

---@class Grub : Monster
    ---@field rotation_delta number
    ---@field drop boolean
    ---@field looking_for_new_direction_timer integer @used when he touches floor/wall/ceiling
    ---@field walk_pause_timer integer
    ---@field turn_into_fly_timer integer
    ---@field particle ParticleEmitterInfo
    ---@field sound SoundMeta

---@class Tadpole : Monster
    ---@field acceleration_timer integer
    ---@field player_spotted boolean

---@class GiantFly : Monster
    ---@field head_entity Entity
    ---@field sound SoundMeta
    ---@field particle ParticleEmitterInfo
    ---@field sine_amplitude number
    ---@field sine_frequency number
    ---@field delta_y_angle number
    ---@field sine_counter integer

---@class Ghist : Monster
    ---@field body_uid integer
    ---@field idle_timer integer
    ---@field sound SoundMeta
    ---@field transparency integer
    ---@field fadeout integer

---@class JumpDog : Monster
    ---@field walk_pause_timer integer
    ---@field squish_timer integer

---@class EggplantMinister : Monster
    ---@field sound SoundMeta
    ---@field walk_pause_timer integer
    ---@field squish_timer integer

---@class Yama : Monster
    ---@field message_shown boolean

---@class Hundun : Monster
    ---@field applied_hor_velocity number
    ---@field applied_ver_velocity number
    ---@field birdhead_entity_uid integer
    ---@field snakehead_entity_uid integer
    ---@field y_level number @current floor level
    ---@field bounce_timer integer
    ---@field fireball_timer integer
    ---@field birdhead_defeated boolean
    ---@field snakehead_defeated boolean
    ---@field hundun_flags integer @1:  Will move to the left, 2: Birdhead emerged, 3: Snakehead emerged, 4: Top level arena reached, 5: Birdhead shot last - to alternate the heads shooting fireballs
    ---@field y_limit number @This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    ---@field rising_speed_x number @This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    ---@field rising_speed_y number @This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    ---@field bird_head_spawn_y number @This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it
    ---@field snake_head_spawn_y number @This is custom variable, you need [activate_hundun_hack](#activate_hundun_hack) to use it

---@class HundunHead : Monster
    ---@field attack_position_x number @Position where the head will move on attack
    ---@field attack_position_y number
    ---@field egg_crack_effect_uid integer
    ---@field targeted_player_uid integer
    ---@field looking_for_target_timer integer @also cooldown before attack
    ---@field invincibility_timer integer

---@class MegaJellyfish : Monster
    ---@field flipper1 Entity
    ---@field flipper2 Entity
    ---@field sound SoundMeta
    ---@field orb_uid integer @game checks if this uid, and two following exist, if not, the Jellyfish starts chasing player
    ---@field tail_bg_uid integer
    ---@field applied_velocity number
    ---@field wagging_tail_counter number
    ---@field flipper_distance integer @only applies to door-blocking one
    ---@field velocity_application_timer integer

---@class Scorpion : Monster
    ---@field walk_pause_timer integer
    ---@field jump_cooldown_timer integer

---@class Hermitcrab : Monster
    ---@field carried_entity_type ENT_TYPE
    ---@field carried_entity_uid integer
    ---@field walk_spit_timer integer
    ---@field is_active boolean @whether it is hidden behind the carried block or not, if true you can damage him
    ---@field is_inactive boolean
    ---@field spawn_new_carried_item boolean @defaults to true, when toggled to false, a new carried item spawns

---@class Necromancer : WalkingMonster
    ---@field sound SoundMeta
    ---@field red_skeleton_spawn_x number
    ---@field red_skeleton_spawn_y number
    ---@field resurrection_uid integer
    ---@field resurrection_timer integer

---@class ProtoShopkeeper : Monster
    ---@field movement_state integer @1: "Headpulse/explosion related, 2: Walking, 3: Headpulse/explosion related, 4: Crawling, 6: Headpulse/explosion related
    ---@field walk_pause_explode_timer integer
    ---@field walking_speed integer @0 = slow, 4 = fast

---@class Beg : NPC
    ---@field walk_pause_timer integer
    ---@field disappear_timer integer

---@class Terra : Monster
    ---@field fx_button Entity
    ---@field x_pos number
    ---@field abuse_speechbubble_timer integer

---@class Critter : Monster
    ---@field last_picked_up_by_uid integer
    ---@field holding_state integer

---@class CritterBeetle : Critter
    ---@field pause boolean @used when he's getting eaten

---@class CritterCrab : Critter
    ---@field walk_pause_timer integer
    ---@field walking_left boolean
    ---@field unfriendly boolean @moves away from its target instead of towards it

---@class CritterButterfly : Critter
    ---@field change_direction_timer integer
    ---@field vertical_flight_direction integer

---@class CritterLocust : Critter
    ---@field sound SoundMeta
    ---@field jump_timer integer

---@class CritterSnail : Critter
    ---@field x_direction number
    ---@field y_direction number
    ---@field pos_x number
    ---@field pos_y number
    ---@field rotation_center_x number
    ---@field rotation_center_y number
    ---@field rotation_angle number
    ---@field rotation_speed number

---@class CritterFish : Critter
    ---@field swim_pause_timer integer
    ---@field player_in_proximity boolean

---@class CritterPenguin : Critter
    ---@field walk_pause_timer integer
    ---@field jump_timer integer

---@class CritterFirefly : Critter
    ---@field sine_amplitude number
    ---@field sine_frequency number
    ---@field sine_angle number
    ---@field change_direction_timer integer
    ---@field sit_timer integer
    ---@field sit_cooldown_timer integer

---@class CritterDrone : Critter
    ---@field emitted_light Illumination
    ---@field sound SoundMeta
    ---@field applied_hor_momentum number
    ---@field applied_ver_momentum number
    ---@field unfriendly boolean @moves away from its target instead of towards it
    ---@field move_timer integer

---@class CritterSlime : Critter
    ---@field x_direction number
    ---@field y_direction number
    ---@field pos_x number
    ---@field pos_y number
    ---@field rotation_center_x number
    ---@field rotation_center_y number
    ---@field rotation_angle number
    ---@field rotation_speed number
    ---@field walk_pause_timer integer

---@class Bomb : Movable
    ---@field sound SoundMeta
    ---@field scale_hor number @1.25 = default regular bomb, 1.875 = default giant bomb, > 1.25 generates ENT_TYPE_FX_POWEREDEXPLOSION
    ---@field scale_ver number
    ---@field is_big_bomb boolean @is bomb from powerpack

---@class Backpack : Powerup
    ---@field explosion_trigger boolean @More like on fire trigger, the explosion happens when the timer reaches > 29
    ---@field explosion_timer integer
    ---@field trigger_explosion fun(self): nil
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_trigger_explosion fun(self, fun: fun(self: Backpack): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool trigger_explosion(Backpack self)`
    ---@field set_post_trigger_explosion fun(self, fun: fun(self: Backpack): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil trigger_explosion(Backpack self)`

---@class Projectile : Movable

---@class Purchasable : Movable
    ---@field equip fun(self, who: Entity): nil @Is called after purchase, changes the DummyPurchasableEntity into the real entity plus tries to equip it, or pick it up (for stuff like weapons), or give the powerup.<br/>Nothing else happens, by itself it does not remove item from shop etc.
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_equip fun(self, fun: fun(self: Purchasable, who: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool equip(Purchasable self, Entity who)`<br/>Virtual function docs:<br/>Is called after purchase, changes the DummyPurchasableEntity into the real entity plus tries to equip it, or pick it up (for stuff like weapons), or give the powerup.<br/>Nothing else happens, by itself it does not remove item from shop etc.
    ---@field set_post_equip fun(self, fun: fun(self: Purchasable, who: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil equip(Purchasable self, Entity who)`<br/>Virtual function docs:<br/>Is called after purchase, changes the DummyPurchasableEntity into the real entity plus tries to equip it, or pick it up (for stuff like weapons), or give the powerup.<br/>Nothing else happens, by itself it does not remove item from shop etc.

---@class DummyPurchasableEntity : Purchasable
    ---@field replace_entity Entity
    ---@field exploding boolean
    ---@field explosion_timer integer @Explodes when timer reaches 30
    ---@field trigger_explosion fun(self, who: Entity): nil @Transfers ownership etc. for who to blame, sets the exploding bool
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_trigger_explosion fun(self, fun: fun(self: DummyPurchasableEntity, who: Entity): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool trigger_explosion(DummyPurchasableEntity self, Entity who)`<br/>Virtual function docs:<br/>Transfers ownership etc. for who to blame, sets the exploding bool
    ---@field set_post_trigger_explosion fun(self, fun: fun(self: DummyPurchasableEntity, who: Entity): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil trigger_explosion(DummyPurchasableEntity self, Entity who)`<br/>Virtual function docs:<br/>Transfers ownership etc. for who to blame, sets the exploding bool

---@class Bow : Purchasable
    ---@field get_arrow_special_offset fun(self): number @When laying on the ground

---@class Present : Purchasable
    ---@field inside ENT_TYPE

---@class Jetpack : Backpack
    ---@field flame_on boolean
    ---@field fuel integer
    ---@field acceleration fun(self): number
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_acceleration fun(self, fun: fun(self: Jetpack): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> acceleration(Jetpack self)`
    ---@field set_post_acceleration fun(self, fun: fun(self: Jetpack): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil acceleration(Jetpack self)`

---@class TeleporterBackpack : Backpack
    ---@field teleport_number integer

---@class Hoverpack : Backpack
    ---@field sound SoundMeta
    ---@field is_on boolean

---@class Cape : Backpack
    ---@field floating_down boolean

---@class VladsCape : Cape
    ---@field can_double_jump boolean

---@class Mattock : Purchasable
    ---@field remaining integer

---@class Gun : Purchasable
    ---@field cooldown integer
    ---@field shots integer @used only for webgun
    ---@field shots2 integer @used only for clonegun
    ---@field in_chamber integer @Only for webgun, uid of the webshot entity

---@class Flame : Movable
    ---@field sound SoundMeta
    ---@field emitted_light Illumination

---@class FlameSize : Flame
    ---@field flame_size number @if changed, gradually goes down (0.03 per frame) to the default size, it's the base value for `entity.width` and `entity.height`

---@class ClimbableRope : Movable
    ---@field segment_nr_inverse integer
    ---@field burn_timer integer @entity is killed after 20
    ---@field above_part Entity
    ---@field below_part Entity
    ---@field segment_nr integer

---@class Idol : Movable
    ---@field trap_triggered boolean @if you set it to true for the ice caves or volcano idol, the trap won't trigger
    ---@field touch integer @changes to 0 when first picked up by player and back to -1 if HH picks it up
    ---@field spawn_x number
    ---@field spawn_y number

---@class Spear : Movable
    ---@field sound_id SOUNDID

---@class JungleSpearCosmetic : Movable
    ---@field move_x number
    ---@field move_y number

---@class WebShot : Projectile
    ---@field shot boolean @if false, it's attached to the gun

---@class HangStrand : Movable
    ---@field start_pos_y number

---@class HangAnchor : Movable
    ---@field spider_uid integer

---@class Arrow : Purchasable
    ---@field flame_uid integer
    ---@field is_on_fire boolean
    ---@field is_poisoned boolean
    ---@field shot_from_trap boolean
    ---@field poison_arrow fun(self, poisoned: boolean): nil
    ---@field light_up fun(self, lit: boolean): nil

---@class LightArrow : Arrow
    ---@field emitted_light Illumination

---@class LightShot : Projectile
    ---@field emitted_light Illumination

---@class LightEmitter : Movable
    ---@field emitted_light Illumination

---@class ScepterShot : LightEmitter
    ---@field sound SoundMeta
    ---@field speed number
    ---@field idle_timer integer @short timer before it goes after target

---@class SpecialShot : LightEmitter
    ---@field target_x number
    ---@field target_y number

---@class SoundShot : LightShot
    ---@field sound SoundMeta

---@class Spark : Flame
    ---@field particle ParticleEmitterInfo
    ---@field fx_entity Entity
    ---@field rotation_center_x number
    ---@field rotation_center_y number
    ---@field rotation_angle number
    ---@field size number @slowly goes down to default 1.0, is 0.0 when not on screen
    ---@field size_multiply number @0.0 when not on screen
    ---@field next_size number @width and height will be set to `next_size * size_multiply` next frame
    ---@field size_change_timer integer @very short timer before next size change, giving a pulsing effect
    ---@field speed number @This is custom variable, you need [activate_sparktraps_hack](#activate_sparktraps_hack) to use it
    ---@field distance number @This is custom variable, you need [activate_sparktraps_hack](#activate_sparktraps_hack) to use it

---@class TiamatShot : LightEmitter
    ---@field sound SoundMeta

---@class Fireball : SoundShot
    ---@field particle ParticleEmitterInfo

---@class Leaf : Movable
    ---@field fade_away_counter number @counts to 100.0 then the leaf fades away
    ---@field swing_direction integer
    ---@field fade_away_trigger boolean

---@class AcidBubble : Movable
    ---@field speed_x number
    ---@field speed_y number
    ---@field float_counter number

---@class Claw : Movable
    ---@field crabman_uid integer
    ---@field spawn_x number
    ---@field spawn_y number

---@class StretchChain : Movable
    ---@field at_end_of_chain_uid integer
    ---@field dot_offset number
    ---@field position_in_chain integer
    ---@field inverse_doubled_position_in_chain integer
    ---@field is_dot_hidden boolean

---@class Chest : Movable
    ---@field leprechaun boolean
    ---@field bomb boolean @size of the bomb is random, if set both true only leprechaun spawns

---@class Treasure : Movable
    ---@field cashed boolean @spawns a dust effect and adds money for the total

---@class HundunChest : Treasure
    ---@field timer integer

---@class Boombox : Movable
    ---@field fx_button Entity
    ---@field music_note1 ParticleEmitterInfo
    ---@field music_note2 ParticleEmitterInfo
    ---@field spawn_y number
    ---@field station integer
    ---@field station_change_delay integer
    ---@field jump_timer integer
    ---@field jump_state integer

---@class TV : Movable
    ---@field sound SoundMeta
    ---@field fx_button Entity
    ---@field emitted_light Illumination
    ---@field station integer

---@class Telescope : Movable
    ---@field fx_button Entity
    ---@field camera_anchor Entity
    ---@field looked_through_by_uid integer

---@class Torch : Movable
    ---@field flame_uid integer
    ---@field is_lit boolean @It's used just to check, to light/extinguish use `light_up` function
    ---@field light_up fun(self, lit: boolean): nil
    ---@field get_flame_type fun(self): ENT_TYPE

---@class WallTorch : Torch
    ---@field dropped_gold boolean @if false, it will drop gold when light up

---@class TorchFlame : Flame
    ---@field smoke_particle ParticleEmitterInfo
    ---@field flame_particle ParticleEmitterInfo
    ---@field warp_particle ParticleEmitterInfo
    ---@field flame_size number

---@class LampFlame : Flame
    ---@field flame_particle ParticleEmitterInfo

---@class Bullet : Projectile

---@class TimedShot : LightShot
    ---@field timer integer

---@class CloneGunShot : LightShot
    ---@field timer integer
    ---@field spawn_y number

---@class PunishBall : Movable
    ---@field attached_to_uid integer
    ---@field timer integer @counts down from 20 while the ball is eligible to break a floor and tries to break it at 0
    ---@field x_pos number
    ---@field y_pos number

---@class Chain : Movable
    ---@field attached_to_uid integer
    ---@field timer integer

---@class Container : Movable
    ---@field inside ENT_TYPE

---@class Coffin : Movable
    ---@field inside ENT_TYPE
    ---@field timer integer
    ---@field player_respawn boolean

---@class Fly : Movable
    ---@field timer integer

---@class OlmecCannon : Movable
    ---@field timer integer
    ---@field bombs_left integer
    ---@field spawn_projectile fun(self): nil
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_spawn_projectile fun(self, fun: fun(self: OlmecCannon): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_projectile(OlmecCannon self)`
    ---@field set_post_spawn_projectile fun(self, fun: fun(self: OlmecCannon): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_projectile(OlmecCannon self)`

---@class Landmine : LightEmitter
    ---@field timer integer @explodes at 57, if you set it to 58 will count to overflow

---@class UdjatSocket : Movable
    ---@field fx_button Entity

---@class Ushabti : Movable
    ---@field wiggle_timer integer
    ---@field shine_timer integer

---@class Honey : Movable
    ---@field wiggle_timer integer

---@class GiantClamTop : Movable
    ---@field close_timer integer
    ---@field open_timer integer

---@class PlayerGhost : LightEmitter
    ---@field sparkles_particle ParticleEmitterInfo
    ---@field player_inputs PlayerSlot
    ---@field inventory Inventory
    ---@field sound SoundMeta
    ---@field body_uid integer @Is not set to -1 when crushed
    ---@field shake_timer integer
    ---@field boost_timer integer

---@class GhostBreath : Projectile
    ---@field timer integer
    ---@field big_cloud boolean

---@class LaserBeam : Movable
    ---@field sparks ParticleEmitterInfo
    ---@field emitted_light Illumination

---@class TreasureHook : Movable
    ---@field sound SoundMeta

---@class AxolotlShot : Projectile
    ---@field trapped_uid integer
    ---@field size number
    ---@field swing number
    ---@field swing_periodicity number
    ---@field distance_after_capture number

---@class TrapPart : Movable
    ---@field ceiling Entity

---@class SkullDropTrap : Movable
    ---@field sound SoundMeta
    ---@field left_skull_uid integer
    ---@field middle_skull_uid integer
    ---@field right_skull_uid integer
    ---@field left_skull_drop_time integer
    ---@field middle_skull_drop_time integer
    ---@field right_skull_drop_time integer
    ---@field timer integer @counts from 60 to 0, 3 times, the last time dropping the skulls, then random longer timer for reset

---@class FrozenLiquid : Movable

---@class Switch : Movable
    ---@field timer integer

---@class FlyHead : Movable
    ---@field vored_entity_uid integer

---@class SnapTrap : Movable
    ---@field bait_uid integer
    ---@field reload_timer integer

---@class EmpressGrave : Movable
    ---@field fx_button Entity
    ---@field ghost Entity

---@class Tentacle : Chain
    ---@field bottom Entity

---@class MiniGameShip : Movable
    ---@field sound SoundMeta
    ---@field velocity_x number
    ---@field velocity_y number
    ---@field swing number
    ---@field up_down_normal number @0.0 - down, 1.0 - up, 0.5 - idle

---@class OlmecShip : Movable
    ---@field sound SoundMeta
    ---@field door_fx Entity
    ---@field smoke ParticleEmitterInfo
    ---@field flight_time integer
    ---@field has_spawned_jetflames boolean

---@class MiniGameAsteroid : Movable
    ---@field spin_speed number

---@class Pot : Purchasable
    ---@field inside ENT_TYPE
    ---@field dont_transfer_dmg boolean

---@class CursedPot : Movable
    ---@field smoke ParticleEmitterInfo
    ---@field smoke2 ParticleEmitterInfo

---@class CookFire : Torch
    ---@field emitted_light Illumination
    ---@field particles_smoke ParticleEmitterInfo
    ---@field particles_flames ParticleEmitterInfo
    ---@field particles_warp ParticleEmitterInfo
    ---@field sound SoundMeta

---@class Orb : Movable
    ---@field sound SoundMeta
    ---@field timer integer

---@class EggSac : Movable
    ---@field timer integer

---@class Goldbar : Movable

---@class Coin : Movable
    ---@field nominal_price integer

---@class RollingItem : Purchasable
    ---@field roll_speed number
    ---@field give_powerup fun(self, who: Entity, play_sfx: boolean): nil @Skip this function for item to be unpickable
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_give_powerup fun(self, fun: fun(self: RollingItem, who: Entity, play_sfx: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool give_powerup(RollingItem self, Entity who, boolean play_sfx)`<br/>Virtual function docs:<br/>Skip this function for item to be unpickable
    ---@field set_post_give_powerup fun(self, fun: fun(self: RollingItem, who: Entity, play_sfx: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil give_powerup(RollingItem self, Entity who, boolean play_sfx)`<br/>Virtual function docs:<br/>Skip this function for item to be unpickable

---@class PlayerBag : Movable
    ---@field bombs integer
    ---@field ropes integer

---@class Powerup : Movable
    ---@field set_pre_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: ENTITY_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_putting_on fun(self, fun: fun(self: Powerup, who: PowerupCapable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool putting_on(Powerup self, PowerupCapable who)`<br/>Virtual function docs:<br/>only for backpacks
    ---@field set_post_putting_on fun(self, fun: fun(self: Powerup, who: PowerupCapable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil putting_on(Powerup self, PowerupCapable who)`<br/>Virtual function docs:<br/>only for backpacks
    ---@field set_pre_putting_off fun(self, fun: fun(self: Powerup, who: PowerupCapable): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool putting_off(Powerup self, PowerupCapable who)`<br/>Virtual function docs:<br/>only for backpacks
    ---@field set_post_putting_off fun(self, fun: fun(self: Powerup, who: PowerupCapable): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil putting_off(Powerup self, PowerupCapable who)`<br/>Virtual function docs:<br/>only for backpacks

---@class KapalaPowerup : Powerup
    ---@field amount_of_blood integer

---@class ParachutePowerup : Powerup
    ---@field falltime_deploy integer @this gets compared with entity's falling_timer
    ---@field deployed boolean
    ---@field deploy fun(self): nil
    ---@field gold_timer integer @Timer for spawning a single gold nugget.
    ---@field gold_spawning_time integer @Time until gold nuggets stop spawning.

---@class TrueCrownPowerup : Powerup
    ---@field timer integer

---@class AnkhPowerup : Powerup
    ---@field sound SoundMeta
    ---@field player Entity
    ---@field fx_glow Entity
    ---@field timer1 integer
    ---@field timer2 integer
    ---@field timer3 integer
    ---@field music_on_off boolean

---@class YellowCape : Cape
    ---@field sound SoundMeta

---@class Teleporter : Purchasable
    ---@field teleport_number integer

---@class Boomerang : Purchasable
    ---@field sound SoundMeta
    ---@field trail ParticleEmitterInfo
    ---@field distance number
    ---@field rotation number
    ---@field returns_to_uid integer

---@class Excalibur : Movable
    ---@field in_stone boolean

---@class Shield : Purchasable
    ---@field shake number

---@class PrizeDispenser : Movable
    ---@field item_ids integer[] @size: 6 @Id's of the items (not types), by default 0-24, look at [change_diceshop_prizes](#change_diceshop_prizes) for the list of default prizes<br/>so for example: id 0 equals ITEM_PICKUP_BOMBBAG, id 1 equals ITEM_PICKUP_BOMBBOX etc. Game generates 6 but uses max 5 for Tusk dice shop
    ---@field prizes_spawned integer

---@class Web : Movable
    ---@field decay_rate number @Is subtracted from the color alpha every frame after the `stand_counter` is more than 300.<br/>Entity automatically dies when the alpha is less than 0.1

---@class Scepter : Movable
    ---@field cooldown integer

---@class LiquidSurface : Movable
    ---@field glow_radius number
    ---@field sine_pos number
    ---@field sine_pos_increment number

---@class OlmecFloater : Movable
    ---@field both_floaters_intact boolean
    ---@field on_breaking boolean

---@class EggshipCenterJetFlame : Movable
    ---@field sound SoundMeta
    ---@field emitted_light Illumination
    ---@field particle ParticleEmitterInfo
    ---@field smoke_on boolean

---@class MiniGameShipOffset : Movable
    ---@field offset_x number
    ---@field offset_y number
    ---@field normal_y_offset number @Is added to offset_y

---@class Button : Movable
    ---@field button_sprite integer @Only one can be set:<br/>1 - pad: A, key: Z<br/>2 - pad: X, key: X<br/>4 - pad: B, key: C<br/>8 - pad: Y, key: D<br/>16 - pad: LB, key: L Shift<br/>32 - pad: RB, key: A<br/>64 - pad: menu?, key: (none)<br/>128 - pad: copy?, key: Tab
    ---@field visibility number
    ---@field is_visible boolean @It's false for selldialog used in shops
    ---@field player_trigger boolean @It's set true even if player does not see the button, like the drill or COG door
    ---@field seen integer @<br/>-1 - hasn't been seen<br/>0 - last seen by player 1<br/>1 - last seen by player 2<br/>2 - last seen by player 3<br/>3 - last seen by player 4

---@class FxTornJournalPage : Movable
    ---@field page_number integer @Only in tutorial

---@class FxMainExitDoor : Movable
    ---@field emitted_light Illumination
    ---@field timer integer @When breaking open in tutorial

---@class Birdies : Movable

---@class Explosion : Movable
    ---@field emitted_light Illumination

---@class FxOuroboroOccluder : Movable

---@class FxOuroboroDragonPart : Movable
    ---@field speed number
    ---@field timer integer
    ---@field particle ParticleEmitterInfo

---@class Rubble : Movable

---@class FxCompass : Movable
    ---@field sine_angle number @Counts form 0 to 2*pi, responsible for moving back and forth
    ---@field visibility number
    ---@field is_active boolean @Player has compass

---@class SleepBubble : Movable
    ---@field show_hide_timer integer

---@class MovingIcon : Movable
    ---@field movement_timer integer @Used to move it up and down in sync with others

---@class FxSaleContainer : Movable
    ---@field fx_value Entity
    ---@field fx_icon Entity
    ---@field fx_button Entity
    ---@field shake_amplitude number @For effect when you don't have enough money
    ---@field sound_trigger boolean @Also sound_played, keeps re-triggering from time to time
    ---@field pop_in_out_procentage integer

---@class FxPickupEffect : Movable
    ---@field spawn_y number
    ---@field visibility number

---@class FxShotgunBlast : Movable
    ---@field illumination Illumination

---@class FxJetpackFlame : Movable
    ---@field particle_smoke ParticleEmitterInfo
    ---@field particle_flame ParticleEmitterInfo
    ---@field sound SoundMeta
    ---@field illumination Illumination

---@class FxPlayerIndicator : Movable
    ---@field attached_to integer
    ---@field pos_x number
    ---@field pos_y number

---@class FxSpringtrapRing : Movable
    ---@field timer integer
    ---@field illumination Illumination

---@class FxWitchdoctorHint : Movable

---@class FxNecromancerANKH : Movable

---@class FxWebbedEffect : Movable
    ---@field visible boolean

---@class FxUnderwaterBubble : Movable
    ---@field bubble_source_uid integer
    ---@field direction integer @1 / -1
    ---@field pop boolean @Setting it true makes it disappear/fade away
    ---@field inverted boolean

---@class FxWaterDrop : Movable
    ---@field inverted boolean
    ---@field droplet_source_uid integer

---@class FxKinguSliding : Movable
    ---@field particle ParticleEmitterInfo

---@class FxAlienBlast : Movable

---@class FxSparkSmall : Movable
    ---@field timer integer

---@class FxTiamatHead : Movable
    ---@field timer integer

---@class FxTiamatTorso : Movable
    ---@field timer integer
    ---@field torso_target_size number @Slowly increases/decreases to the given value

---@class FxTiamatTail : Movable
    ---@field angle_two number @Added _two just to not shadow angle in entity, it's angle but the pivot point is at the edge
    ---@field x_pos number
    ---@field y_pos number

---@class FxVatBubble : Movable
    ---@field max_y number

---@class FxHundunNeckPiece : Movable
    ---@field kill_timer integer @Short timer after the head is dead

---@class FxJellyfishStar : Movable
    ---@field rotation_angle number
    ---@field radius number
    ---@field speed number

---@class FxQuickSand : Movable

---@class FxSorceressAttack : Movable
    ---@field size number

---@class FxLamassuAttack : Movable
    ---@field attack_angle number

---@class FxFireflyLight : Movable
    ---@field illumination Illumination
    ---@field light_timer integer
    ---@field cooldown_timer integer @Timer between light flashes

---@class FxEmpress : Movable
    ---@field sine_angle number

---@class FxAnkhRotatingSpark : Movable
    ---@field radius number
    ---@field inclination number
    ---@field speed number @0 - 1.0
    ---@field sine_angle number
    ---@field size number

---@class FxAnkhBrokenPiece : Movable

---@class MegaJellyfishEye : Movable
    ---@field timer integer

---@class Liquid : Entity
    ---@field fx_surface Entity
    ---@field get_liquid_flags fun(self): integer
    ---@field set_liquid_flags fun(self, flags: integer): nil

---@class Lava : Liquid
    ---@field emitted_light Illumination

---@class BGBackLayerDoor : Entity
    ---@field illumination1 Illumination
    ---@field illumination2 Illumination

---@class BGSurfaceStar : Entity
    ---@field blink_timer integer
    ---@field relative_x number
    ---@field relative_y number

---@class BGRelativeElement : Entity
    ---@field relative_x number
    ---@field relative_y number

---@class BGSurfaceLayer : BGRelativeElement
    ---@field relative_offset_x number
    ---@field relative_offset_y number

---@class BGEggshipRoom : Entity
    ---@field sound SoundMeta
    ---@field fx_shell Entity
    ---@field fx_door Entity
    ---@field platform_left Entity
    ---@field platform_middle Entity
    ---@field platform_right Entity
    ---@field player_in boolean

---@class BGMovingStar : BGSurfaceStar
    ---@field falling_speed number @Can make it rise if set to negative

---@class BGTutorialSign : Entity
    ---@field is_shown boolean

---@class BGShootingStar : BGRelativeElement
    ---@field x_increment number
    ---@field y_increment number
    ---@field timer integer
    ---@field max_timer integer
    ---@field size number @Gets smaller as the timer gets close to the max_timer

---@class BGShopEntrance : Entity
    ---@field on_entering boolean

---@class BGFloatingDebris : BGSurfaceLayer
    ---@field distance number @Distance it travels up and down from spawn position
    ---@field speed number
    ---@field sine_angle number

---@class BGShopKeeperPrime : Entity
    ---@field normal_y number
    ---@field sine_pos number
    ---@field bubbles_timer integer
    ---@field bubble_spawn_trigger boolean
    ---@field bubble_spawn_delay integer

---@class CrossBeam : Entity
    ---@field attached_to_side_uid integer
    ---@field attached_to_top_uid integer

---@class DestructibleBG : Entity

---@class PalaceSign : Entity
    ---@field sound SoundMeta @The neon buzz sound
    ---@field illumination Illumination
    ---@field arrow_illumination Illumination
    ---@field arrow_change_timer integer

---@class DecoRegeneratingBlock : Entity

---@class Portal : Entity
    ---@field emitted_light Illumination
    ---@field transition_timer integer
    ---@field level integer
    ---@field world integer
    ---@field theme integer
    ---@field timer integer

---@class ShootingStarSpawner : Entity
    ---@field timer integer

---@class LogicalDoor : Entity
    ---@field door_type ENT_TYPE @Spawns this entity when not covered by floor. Must be initialized to valid ENT_TYPE before revealed, or crashes the game.
    ---@field platform_type ENT_TYPE @Spawns this entity below when tile below is uncovered. Doesn't spawn anything if it was never covered by floor, unless platform_spawned is set to false. Must be initialized to valid ENT_TYPE before revealed, or crashes the game.
    ---@field visible boolean @Set automatically when not covered by floor.
    ---@field platform_spawned boolean @Set automatically when tile below is not covered by floor. Unset to force the platform to spawn if it was never covered in the first place.

---@class LogicalSound : Entity
    ---@field sound SoundMeta

---@class LogicalStaticSound : LogicalSound

---@class LogicalLiquidStreamSound : LogicalStaticSound

---@class LogicalTrapTrigger : Entity
    ---@field min_empty_distance integer @Used in BigSpearTrap when it has to have minimum 2 free spaces to be able to trigger, value in tiles
    ---@field trigger_distance integer @Value in tiles
    ---@field vertical boolean

---@class JungleTrapTrigger : LogicalTrapTrigger

---@class WetEffect : Entity
    ---@field particle ParticleEmitterInfo

---@class OnFireEffect : Entity
    ---@field particle_smoke ParticleEmitterInfo
    ---@field particle_flame ParticleEmitterInfo
    ---@field illumination Illumination

---@class PoisonedEffect : Entity
    ---@field particle_burst ParticleEmitterInfo
    ---@field particle_base ParticleEmitterInfo
    ---@field burst_timer integer
    ---@field burst_active boolean @If forced to false, it will not play the sound or spawn burst particles

---@class CursedEffect : Entity
    ---@field particle ParticleEmitterInfo
    ---@field sound SoundMeta

---@class OuroboroCameraAnchor : Entity
    ---@field target_x number
    ---@field target_y number
    ---@field velocity_x number
    ---@field velocity_y number

---@class OuroboroCameraZoomin : Entity
    ---@field zoomin_level number @Can be set to negative, seams to trigger the warp at some value

---@class CinematicAnchor : Entity
    ---@field blackbar_top Entity
    ---@field blackbar_bottom Entity
    ---@field roll_in number @0.0 to 1.0

---@class BurningRopeEffect : Entity
    ---@field illumination Illumination
    ---@field sound SoundMeta

---@class DustWallApep : Entity
    ---@field particle ParticleEmitterInfo

---@class CameraFlash : Entity
    ---@field illumination1 Illumination
    ---@field illumination2 Illumination
    ---@field timer integer

---@class RoomLight : Entity
    ---@field illumination Illumination

---@class LimbAnchor : Entity
    ---@field move_timer integer
    ---@field flip_vertical boolean

---@class LogicalConveyorbeltSound : LogicalSound

---@class LogicalAnchovyFlock : Entity
    ---@field current_speed number @Increases until max_speed reached
    ---@field max_speed number
    ---@field timer integer

---@class MummyFliesSound : LogicalSound
    ---@field mummy_uid integer
    ---@field flies integer @Numbers of flies spawned

---@class QuickSandSound : LogicalSound

---@class IceSlidingSound : LogicalSound

---@class FrostBreathEffect : Entity
    ---@field timer integer

---@class BoulderSpawner : Entity
    ---@field timer integer @Can be set negative for longer time period, spawns boulder at 150, setting it higher with count to overflow
    ---@field sound SoundMeta

---@class PipeTravelerSound : LogicalSound
    ---@field enter_exit boolean

---@class LogicalDrain : Entity
    ---@field timer integer @Little delay between pulling blob of liquid thru

---@class LogicalRegeneratingBlock : Entity
    ---@field timer integer

---@class SplashBubbleGenerator : Entity
    ---@field timer integer

---@class EggplantThrower : Entity

---@class LogicalMiniGame : Entity
    ---@field timer integer @Delay between spawning ufo

---@class DMSpawning : Entity
    ---@field spawn_x number
    ---@field spawn_y number
    ---@field sine_pos number
    ---@field timer integer

---@class DMAlienBlast : Entity
    ---@field owner_uid integer
    ---@field timer integer
    ---@field sound SoundMeta
    ---@field reticule_internal Entity
    ---@field reticule_external Entity

---@class MovableBehavior
local MovableBehavior = nil
---@return integer
function MovableBehavior:get_state_id() end
---Get the `state_id` of a behavior, this is the id that needs to be returned from a behavior's
---`get_next_state_id` to enter this state, given that the behavior is added to the movable.
---@return integer
function MovableBehavior:get_state_id() end

---@class VanillaMovableBehavior : MovableBehavior

---@class CustomMovableBehavior : MovableBehavior
    ---@field base_behavior VanillaMovableBehavior
    ---@field set_force_state fun(self, force_state: fun(movable: Movable, base_fun: function): boolean): nil @Set the `force_state` function of a `CustomMovableBehavior`, this will be called every frame when<br/>the movable is updated. If an `force_state` is already set it will be overridden. The signature<br/>of the function is `bool force_state(Movable movable, function base_fun)`, when the function returns `true` the movable will<br/>enter this behavior. If no base behavior is set `base_fun` will be `nil`.
    ---@field set_on_enter fun(self, on_enter: fun(movable: Movable, base_fun: function): nil): nil @Set the `on_enter` function of a `CustomMovableBehavior`, this will be called when the movable<br/>enters the state. If an `on_enter` is already set it will be overridden. The signature of the<br/>function is `nil on_enter(Movable movable, function base_fun))`. If no base behavior is set `base_fun` will be `nil`.
    ---@field set_on_exit fun(self, on_exit: fun(movable: Movable, base_fun: function): nil): nil @Set the `on_exit` function of a `CustomMovableBehavior`, this will be called when the movable<br/>leaves the state. If an `on_exit` is already set it will be overridden. The signature of the<br/>function is `nil on_exit(Movable movable, function base_fun))`. If no base behavior is set `base_fun` will be `nil`.
    ---@field set_update_logic fun(self, update_logic: fun(movable: Movable, base_fun: function): nil): nil @Set the `update_logic` function of a `CustomMovableBehavior`, this will be called every frame when<br/>the movable is updated. If an `update_logic` is already set it will be overridden. The signature<br/>of the function is `nil update_logic(Movable movable, function base_fun))`, use it to change the color, texture,<br/>some timers, etc. of the movable. If no base behavior is set `base_fun` will be `nil`.
    ---@field set_update_world fun(self, update_world: fun(movable: Movable, base_fun: function): nil): nil @Set the `update_world` function of a `CustomMovableBehavior`, this will be called every frame when<br/>the movable is updated. If an `update_world` is already set it will be overridden. The signature<br/>of the function is `nil update_world(Movable movable, function base_fun))`, use this to update the move, velocity,<br/>current_animation, etc. of the movable, then call `mov:generic_update_world` to update the movable. If no<br/>base behavior is set `base_fun` will be `nil`.
    ---@field set_get_next_state_id fun(self, get_next_state_id: fun(movable: Movable, base_fun: function): integer): nil @Set the `get_next_state_id` function of a `CustomMovableBehavior`, this will be called every frame when<br/>the movable is updated. If an `get_next_state_id` is already set it will be overridden. The signature<br/>of the function is `int get_next_state_id(Movable movable, function base_fun))`, use this to move to another state, return `nil`.<br/>or this behaviors `state_id` to remain in this behavior. If no base behavior is set `base_fun` will be `nil`.

---@class ParticleDB
    ---@field id PARTICLEEMITTER
    ---@field spawn_count_min integer
    ---@field spawn_count integer
    ---@field lifespan_min integer
    ---@field lifespan integer
    ---@field sheet_id integer
    ---@field animation_sequence_length integer
    ---@field spawn_interval number
    ---@field shrink_growth_factor number
    ---@field rotation_speed number
    ---@field opacity number
    ---@field hor_scattering number
    ---@field ver_scattering number
    ---@field scale_x_min number
    ---@field scale_x number
    ---@field scale_y_min number
    ---@field scale_y number
    ---@field hor_deflection_1 number
    ---@field ver_deflection_1 number
    ---@field hor_deflection_2 number
    ---@field ver_deflection_2 number
    ---@field hor_velocity number
    ---@field ver_velocity number
    ---@field red integer
    ---@field green integer
    ---@field blue integer
    ---@field permanent boolean
    ---@field invisible boolean
    ---@field get_texture fun(self): TEXTURE
    ---@field set_texture fun(self, texture_id: TEXTURE): boolean

---@class ParticleEmitterInfo
    ---@field particle_type ParticleDB
    ---@field particle_type2 ParticleDB
    ---@field particle_count integer
    ---@field particle_count_back_layer integer
    ---@field entity_uid integer
    ---@field x number
    ---@field y number
    ---@field offset_x number
    ---@field offset_y number
    ---@field layer integer
    ---@field draw_depth integer
    ---@field emitted_particles Particle[]
    ---@field emitted_particles_back_layer Particle[]

---@class Particle
    ---@field x number
    ---@field y number
    ---@field velocityx number
    ---@field velocityy number
    ---@field color uColor
    ---@field width number
    ---@field height number
    ---@field lifetime integer
    ---@field max_lifetime integer

---@class ThemeInfo
    ---@field unknown3 integer
    ---@field unknown4 integer
    ---@field theme integer
    ---@field allow_beehive boolean
    ---@field allow_leprechaun boolean
    ---@field sub_theme ThemeInfo
    ---@field reset_theme_flags fun(self): nil @Sets the beehive and leprechaun flags
    ---@field init_flags fun(self): nil @Initializes some flags in the LevelGenSystem, also dark level flag in state.level_flags.
    ---@field init_level fun(self): nil @Adds the entrance room and sets spawn_room_x/y. Sets the level size and toast for echoes feeling. Sets some other level_flags, shop related flags and shop_type.
    ---@field init_rooms fun(self): nil
    ---@field generate_path fun(self, reset: boolean): nil @Generates and adds the path rooms and exit room<br/>Params: reset to start over from the beginning if other rooms didn't fit
    ---@field add_special_rooms fun(self): nil @Adds rooms related to udjat, black market, castle etc
    ---@field add_player_coffin fun(self): nil @Adds a player revival coffin room
    ---@field add_dirk_coffin fun(self): nil @Adds a Dirk coffin room
    ---@field add_idol fun(self): nil @Adds an idol room
    ---@field add_vault fun(self): nil @Adds a vault room
    ---@field add_coffin fun(self): nil @Adds a character unlock coffin room
    ---@field add_special_feeling fun(self): nil @Adds the metal clanking or oppression rooms
    ---@field spawn_level fun(self): nil @Calls many other theme functions to spawn the floor, enemies, items etc, but not background and players. (Disable this to only spawn background and players.)
    ---@field spawn_border fun(self): nil @Spawns the border entities (some floor or teleportingborder)
    ---@field post_process_level fun(self): nil @Theme specific specialties like randomizing ushabti and Coco coffin location, spawns impostor lakes
    ---@field spawn_traps fun(self): nil @Spawns theme specific random traps, pushblocks and critters. Sets special exit doors.
    ---@field post_process_entities fun(self): nil @Fixes textures on pleasure palace ladders, adds some decorations
    ---@field spawn_procedural fun(self): nil @Adds legs under platforms, random pots, goldbars, monsters, compass indicator, initialises quests, random shadows...
    ---@field spawn_background fun(self): nil @Adds the main level background , e.g. CO stars / Duat moon / Plain backwall for other themes
    ---@field spawn_lights fun(self): nil @Adds room lights to udjat chest room or black market
    ---@field spawn_transition fun(self): nil @Spawns the transition tunnel and players in it
    ---@field post_transition fun(self): nil @Handles loading the next level screen from a transition screen
    ---@field spawn_players fun(self): nil @Spawns the players with inventory at `state.level_gen.spawn_x/y`. Also shop and kali background and probably other stuff for some stupid reason.
    ---@field spawn_effects fun(self): nil @Sets the camera bounds and position. Spawns jelly and orbs and the flag in coop. Sets timers/conditions for more jellies and ghosts. Enables the special fog/ember/ice etc particle effects. Spawns beg and handles it's quest flags
    ---@field get_level_file fun(self): string @Returns: The .lvl file to load (e.g. dwelling = dwellingarea.lvl except when level == 4 (cavebossarea.lvl))
    ---@field get_theme_id fun(self): integer @Returns: THEME, or subtheme in CO
    ---@field get_base_id fun(self): integer @Returns: THEME, or logical base THEME for special levels (Abzu->Tide Pool etc)
    ---@field get_floor_spreading_type fun(self): ENT_TYPE @Returns: ENT_TYPE used for floor spreading (generic or one of the styled floors)
    ---@field get_floor_spreading_type2 fun(self): ENT_TYPE @Returns: ENT_TYPE used for floor spreading (stone or one of the styled floors)
    ---@field get_transition_styled_floor fun(self): boolean @Returns: true if transition should use styled floor
    ---@field get_transition_floor_modifier fun(self): integer @Determines the types of FLOOR_TUNNEL_NEXT/CURRENT (depending on where you are transitioning from/to)<br/>Returns: 85 by default, except for: olmec: 15, cog: 23
    ---@field get_transition_styled_floor_type fun(self): ENT_TYPE @Returns: ENT_TYPE used for the transition floor
    ---@field get_backwall_type fun(self): ENT_TYPE @Returns: ENT_TYPE used for the backwall (BG_LEVEL_BACKWALL by default)
    ---@field get_border_type fun(self): ENT_TYPE @Returns: ENT_TYPE to use for the border tiles
    ---@field get_critter_type fun(self): ENT_TYPE @Returns: ENT_TYPE for theme specific critter
    ---@field get_liquid_gravity fun(self): number @Returns: gravity used to initialize liquid pools (-1..1)
    ---@field get_player_damage fun(self): boolean @Returns: false to disable most player damage and the usage of bombs and ropes. Enabled in parts of base camp.
    ---@field get_explosion_soot fun(self): boolean @Returns: true if explosions should spawn background soot
    ---@field get_backlayer_lut fun(self): integer @Returns: TEXTURE for the LUT to be applied to the special back layer, e.g. vlad's castle
    ---@field get_backlayer_light_level fun(self): number @Returns: dynamic backlayer light level (0..1)
    ---@field get_loop fun(self): boolean @Returns: true if the loop rendering should be enabled (Combine with the right get_border_type)
    ---@field get_vault_level fun(self): integer @Returns: highest y-level a vault can spawn
    ---@field get_theme_flag fun(self, index: integer): boolean @Returns: allow_beehive or allow_leprechaun flag<br/>Params: index: 0 or 1
    ---@field get_dynamic_texture fun(self, texture_id: DYNAMIC_TEXTURE): TEXTURE @Returns: TEXTURE based on texture_id<br/>Params: DYNAMIC_TEXTURE texture_id
    ---@field pre_transition fun(self): nil @Sets state.level_next, world_next and theme_next (or state.win_state) based on level number. Runs when exiting a level.
    ---@field get_exit_room_y_level fun(self): integer @Returns: usually state.height - 1. For special levels fixed heights are returned.
    ---@field get_shop_chance fun(self): integer @Returns: inverse shop chance
    ---@field spawn_decoration fun(self): nil @Spawns some specific decoration, e.g. Vlad's big banner
    ---@field spawn_decoration2 fun(self): nil @Spawns some other specific decorations, e.g. grass, flowers, udjat room decal
    ---@field spawn_extra fun(self): nil @Spawns specific extra entities and decorations, like gold key, seaweed, lanterns, banners, signs, wires...
    ---@field do_procedural_spawn fun(self, info: SpawnInfo): nil @Spawns a single procedural entity, used in spawn_procedural (mostly monsters, scarab in dark levels etc.)
    ---@field set_pre_virtual fun(self, entry: THEME_OVERRIDE, fun: function): CallbackId @Hooks before the virtual function at index `entry`.
    ---@field set_post_virtual fun(self, entry: THEME_OVERRIDE, fun: function): CallbackId @Hooks after the virtual function at index `entry`.
    ---@field clear_virtual fun(self, callback_id: CallbackId): nil @Clears the hook given by `callback_id`, alternatively use `clear_callback()` inside the hook.
    ---@field set_pre_dtor fun(self, fun: fun(self: ThemeInfo): nil): CallbackId @Hooks before the virtual function.<br/>The callback signature is `nil dtor(ThemeInfo self)`
    ---@field set_post_dtor fun(self, fun: fun(self: ThemeInfo): nil): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil dtor(ThemeInfo self)`
    ---@field set_pre_reset_theme_flags fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool reset_theme_flags(ThemeInfo self)`<br/>Virtual function docs:<br/>Sets the beehive and leprechaun flags
    ---@field set_post_reset_theme_flags fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil reset_theme_flags(ThemeInfo self)`<br/>Virtual function docs:<br/>Sets the beehive and leprechaun flags
    ---@field set_pre_init_flags fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool init_flags(ThemeInfo self)`<br/>Virtual function docs:<br/>Initializes some flags in the LevelGenSystem, also dark level flag in state.level_flags.
    ---@field set_post_init_flags fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil init_flags(ThemeInfo self)`<br/>Virtual function docs:<br/>Initializes some flags in the LevelGenSystem, also dark level flag in state.level_flags.
    ---@field set_pre_init_level fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool init_level(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds the entrance room and sets spawn_room_x/y. Sets the level size and toast for echoes feeling. Sets some other level_flags, shop related flags and shop_type.
    ---@field set_post_init_level fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil init_level(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds the entrance room and sets spawn_room_x/y. Sets the level size and toast for echoes feeling. Sets some other level_flags, shop related flags and shop_type.
    ---@field set_pre_init_rooms fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool init_rooms(ThemeInfo self)`
    ---@field set_post_init_rooms fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil init_rooms(ThemeInfo self)`
    ---@field set_pre_generate_path fun(self, fun: fun(self: ThemeInfo, reset: boolean): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool generate_path(ThemeInfo self, boolean reset)`<br/>Virtual function docs:<br/>Generates and adds the path rooms and exit room<br/>Params: reset to start over from the beginning if other rooms didn't fit
    ---@field set_post_generate_path fun(self, fun: fun(self: ThemeInfo, reset: boolean): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil generate_path(ThemeInfo self, boolean reset)`<br/>Virtual function docs:<br/>Generates and adds the path rooms and exit room<br/>Params: reset to start over from the beginning if other rooms didn't fit
    ---@field set_pre_special_rooms fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool special_rooms(ThemeInfo self)`
    ---@field set_post_special_rooms fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil special_rooms(ThemeInfo self)`
    ---@field set_pre_player_coffin fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool player_coffin(ThemeInfo self)`
    ---@field set_post_player_coffin fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil player_coffin(ThemeInfo self)`
    ---@field set_pre_dirk_coffin fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool dirk_coffin(ThemeInfo self)`
    ---@field set_post_dirk_coffin fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil dirk_coffin(ThemeInfo self)`
    ---@field set_pre_idol fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool idol(ThemeInfo self)`
    ---@field set_post_idol fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil idol(ThemeInfo self)`
    ---@field set_pre_vault fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool vault(ThemeInfo self)`
    ---@field set_post_vault fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil vault(ThemeInfo self)`
    ---@field set_pre_coffin fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool coffin(ThemeInfo self)`
    ---@field set_post_coffin fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil coffin(ThemeInfo self)`
    ---@field set_pre_feeling fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool feeling(ThemeInfo self)`
    ---@field set_post_feeling fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil feeling(ThemeInfo self)`
    ---@field set_pre_spawn_level fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_level(ThemeInfo self)`<br/>Virtual function docs:<br/>Calls many other theme functions to spawn the floor, enemies, items etc, but not background and players. (Disable this to only spawn background and players.)
    ---@field set_post_spawn_level fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_level(ThemeInfo self)`<br/>Virtual function docs:<br/>Calls many other theme functions to spawn the floor, enemies, items etc, but not background and players. (Disable this to only spawn background and players.)
    ---@field set_pre_spawn_border fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_border(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns the border entities (some floor or teleportingborder)
    ---@field set_post_spawn_border fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_border(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns the border entities (some floor or teleportingborder)
    ---@field set_pre_post_process_level fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool post_process_level(ThemeInfo self)`<br/>Virtual function docs:<br/>Theme specific specialties like randomizing ushabti and Coco coffin location, spawns impostor lakes
    ---@field set_post_post_process_level fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil post_process_level(ThemeInfo self)`<br/>Virtual function docs:<br/>Theme specific specialties like randomizing ushabti and Coco coffin location, spawns impostor lakes
    ---@field set_pre_spawn_traps fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_traps(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns theme specific random traps, pushblocks and critters. Sets special exit doors.
    ---@field set_post_spawn_traps fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_traps(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns theme specific random traps, pushblocks and critters. Sets special exit doors.
    ---@field set_pre_post_process_entities fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool post_process_entities(ThemeInfo self)`<br/>Virtual function docs:<br/>Fixes textures on pleasure palace ladders, adds some decorations
    ---@field set_post_post_process_entities fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil post_process_entities(ThemeInfo self)`<br/>Virtual function docs:<br/>Fixes textures on pleasure palace ladders, adds some decorations
    ---@field set_pre_spawn_procedural fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_procedural(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds legs under platforms, random pots, goldbars, monsters, compass indicator, initialises quests, random shadows...
    ---@field set_post_spawn_procedural fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_procedural(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds legs under platforms, random pots, goldbars, monsters, compass indicator, initialises quests, random shadows...
    ---@field set_pre_spawn_background fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_background(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds the main level background , e.g. CO stars / Duat moon / Plain backwall for other themes
    ---@field set_post_spawn_background fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_background(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds the main level background , e.g. CO stars / Duat moon / Plain backwall for other themes
    ---@field set_pre_spawn_lights fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_lights(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds room lights to udjat chest room or black market
    ---@field set_post_spawn_lights fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_lights(ThemeInfo self)`<br/>Virtual function docs:<br/>Adds room lights to udjat chest room or black market
    ---@field set_pre_spawn_transition fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_transition(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns the transition tunnel and players in it
    ---@field set_post_spawn_transition fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_transition(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns the transition tunnel and players in it
    ---@field set_pre_post_transition fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool post_transition(ThemeInfo self)`<br/>Virtual function docs:<br/>Handles loading the next level screen from a transition screen
    ---@field set_post_post_transition fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil post_transition(ThemeInfo self)`<br/>Virtual function docs:<br/>Handles loading the next level screen from a transition screen
    ---@field set_pre_spawn_players fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_players(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns the players with inventory at `state.level_gen.spawn_x/y`. Also shop and kali background and probably other stuff for some stupid reason.
    ---@field set_post_spawn_players fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_players(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns the players with inventory at `state.level_gen.spawn_x/y`. Also shop and kali background and probably other stuff for some stupid reason.
    ---@field set_pre_spawn_effects fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_effects(ThemeInfo self)`<br/>Virtual function docs:<br/>Sets the camera bounds and position. Spawns jelly and orbs and the flag in coop. Sets timers/conditions for more jellies and ghosts. Enables the special fog/ember/ice etc particle effects. Spawns beg and handles it's quest flags
    ---@field set_post_spawn_effects fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_effects(ThemeInfo self)`<br/>Virtual function docs:<br/>Sets the camera bounds and position. Spawns jelly and orbs and the flag in coop. Sets timers/conditions for more jellies and ghosts. Enables the special fog/ember/ice etc particle effects. Spawns beg and handles it's quest flags
    ---@field set_pre_theme_id fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> theme_id(ThemeInfo self)`
    ---@field set_post_theme_id fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil theme_id(ThemeInfo self)`
    ---@field set_pre_base_id fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> base_id(ThemeInfo self)`
    ---@field set_post_base_id fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil base_id(ThemeInfo self)`
    ---@field set_pre_ent_floor_spreading fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> ent_floor_spreading(ThemeInfo self)`
    ---@field set_post_ent_floor_spreading fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ent_floor_spreading(ThemeInfo self)`
    ---@field set_pre_ent_floor_spreading2 fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> ent_floor_spreading2(ThemeInfo self)`
    ---@field set_post_ent_floor_spreading2 fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ent_floor_spreading2(ThemeInfo self)`
    ---@field set_pre_transition_styled_floor fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> transition_styled_floor(ThemeInfo self)`
    ---@field set_post_transition_styled_floor fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil transition_styled_floor(ThemeInfo self)`
    ---@field set_pre_transition_modifier fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> transition_modifier(ThemeInfo self)`
    ---@field set_post_transition_modifier fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil transition_modifier(ThemeInfo self)`
    ---@field set_pre_ent_transition_styled_floor fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> ent_transition_styled_floor(ThemeInfo self)`
    ---@field set_post_ent_transition_styled_floor fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ent_transition_styled_floor(ThemeInfo self)`
    ---@field set_pre_ent_backwall fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> ent_backwall(ThemeInfo self)`
    ---@field set_post_ent_backwall fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ent_backwall(ThemeInfo self)`
    ---@field set_pre_ent_border fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> ent_border(ThemeInfo self)`
    ---@field set_post_ent_border fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ent_border(ThemeInfo self)`
    ---@field set_pre_ent_critter fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> ent_critter(ThemeInfo self)`
    ---@field set_post_ent_critter fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil ent_critter(ThemeInfo self)`
    ---@field set_pre_gravity fun(self, fun: fun(self: ThemeInfo): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> gravity(ThemeInfo self)`
    ---@field set_post_gravity fun(self, fun: fun(self: ThemeInfo): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil gravity(ThemeInfo self)`
    ---@field set_pre_player_damage fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> player_damage(ThemeInfo self)`
    ---@field set_post_player_damage fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil player_damage(ThemeInfo self)`
    ---@field set_pre_soot fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> soot(ThemeInfo self)`
    ---@field set_post_soot fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil soot(ThemeInfo self)`
    ---@field set_pre_texture_backlayer_lut fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> texture_backlayer_lut(ThemeInfo self)`
    ---@field set_post_texture_backlayer_lut fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil texture_backlayer_lut(ThemeInfo self)`
    ---@field set_pre_backlayer_light_level fun(self, fun: fun(self: ThemeInfo): number?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<number> backlayer_light_level(ThemeInfo self)`
    ---@field set_post_backlayer_light_level fun(self, fun: fun(self: ThemeInfo): number?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil backlayer_light_level(ThemeInfo self)`
    ---@field set_pre_loop fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> loop(ThemeInfo self)`
    ---@field set_post_loop fun(self, fun: fun(self: ThemeInfo): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil loop(ThemeInfo self)`
    ---@field set_pre_vault_level fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> vault_level(ThemeInfo self)`
    ---@field set_post_vault_level fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil vault_level(ThemeInfo self)`
    ---@field set_pre_theme_flag fun(self, fun: fun(self: ThemeInfo, integer: ): boolean?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<boolean> theme_flag(ThemeInfo self, integer)`
    ---@field set_post_theme_flag fun(self, fun: fun(self: ThemeInfo, integer: ): boolean?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil theme_flag(ThemeInfo self, integer)`
    ---@field set_pre_texture_dynamic fun(self, fun: fun(self: ThemeInfo, integer: ): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> texture_dynamic(ThemeInfo self, integer)`
    ---@field set_post_texture_dynamic fun(self, fun: fun(self: ThemeInfo, integer: ): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil texture_dynamic(ThemeInfo self, integer)`
    ---@field set_pre_pre_transition fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool pre_transition(ThemeInfo self)`<br/>Virtual function docs:<br/>Sets state.level_next, world_next and theme_next (or state.win_state) based on level number. Runs when exiting a level.
    ---@field set_post_pre_transition fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil pre_transition(ThemeInfo self)`<br/>Virtual function docs:<br/>Sets state.level_next, world_next and theme_next (or state.win_state) based on level number. Runs when exiting a level.
    ---@field set_pre_exit_room_y_level fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> exit_room_y_level(ThemeInfo self)`
    ---@field set_post_exit_room_y_level fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil exit_room_y_level(ThemeInfo self)`
    ---@field set_pre_shop_chance fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks before the virtual function.<br/>The callback signature is `optional<integer> shop_chance(ThemeInfo self)`
    ---@field set_post_shop_chance fun(self, fun: fun(self: ThemeInfo): integer?): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil shop_chance(ThemeInfo self)`
    ---@field set_pre_spawn_decoration fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_decoration(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns some specific decoration, e.g. Vlad's big banner
    ---@field set_post_spawn_decoration fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_decoration(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns some specific decoration, e.g. Vlad's big banner
    ---@field set_pre_spawn_decoration2 fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_decoration2(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns some other specific decorations, e.g. grass, flowers, udjat room decal
    ---@field set_post_spawn_decoration2 fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_decoration2(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns some other specific decorations, e.g. grass, flowers, udjat room decal
    ---@field set_pre_spawn_extra fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool spawn_extra(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns specific extra entities and decorations, like gold key, seaweed, lanterns, banners, signs, wires...
    ---@field set_post_spawn_extra fun(self, fun: fun(self: ThemeInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil spawn_extra(ThemeInfo self)`<br/>Virtual function docs:<br/>Spawns specific extra entities and decorations, like gold key, seaweed, lanterns, banners, signs, wires...
    ---@field set_pre_do_procedural_spawn fun(self, fun: fun(self: ThemeInfo, info: SpawnInfo): boolean): CallbackId @Hooks before the virtual function.<br/>The callback signature is `bool do_procedural_spawn(ThemeInfo self, SpawnInfo info)`<br/>Virtual function docs:<br/>Spawns a single procedural entity, used in spawn_procedural (mostly monsters, scarab in dark levels etc.)
    ---@field set_post_do_procedural_spawn fun(self, fun: fun(self: ThemeInfo, info: SpawnInfo): boolean): CallbackId @Hooks after the virtual function.<br/>The callback signature is `nil do_procedural_spawn(ThemeInfo self, SpawnInfo info)`<br/>Virtual function docs:<br/>Spawns a single procedural entity, used in spawn_procedural (mostly monsters, scarab in dark levels etc.)

---@class CustomTheme : ThemeInfo
    ---@field level_file string @Level file to load. Probably doesn't do much in custom themes, especially if you're forcing them in PRE_LOAD_LEVEL_FILES.
    ---@field theme integer @Theme index. Probably shouldn't collide with the vanilla ones. Purpose unknown.
    ---@field textures table<DYNAMIC_TEXTURE, TEXTURE> @Add TEXTUREs here to override different dynamic textures.
    ---@field pre fun(self, index: THEME_OVERRIDE, func_: function): nil @Set a callback to be called before this theme function.
    ---@field post fun(self, index: THEME_OVERRIDE, func_: function): nil @Set a callback to be called after this theme function, to fix some changes it did for example.
    ---@field base_theme integer @Base THEME to load enabled functions from, when no other theme is specified.
    ---@field reset_theme_flags fun(self): nil
    ---@field init_flags fun(self): nil
    ---@field init_level fun(self): nil
    ---@field init_rooms fun(self): nil
    ---@field generate_path fun(self, reset: boolean): nil
    ---@field add_special_rooms fun(self): nil
    ---@field add_player_coffin fun(self): nil
    ---@field add_dirk_coffin fun(self): nil
    ---@field add_idol fun(self): nil
    ---@field add_vault fun(self): nil
    ---@field add_coffin fun(self): nil
    ---@field add_special_feeling fun(self): nil
    ---@field spawn_level fun(self): nil
    ---@field spawn_border fun(self): nil
    ---@field post_process_level fun(self): nil
    ---@field spawn_traps fun(self): nil
    ---@field post_process_entities fun(self): nil
    ---@field spawn_procedural fun(self): nil
    ---@field spawn_background fun(self): nil
    ---@field spawn_lights fun(self): nil
    ---@field spawn_transition fun(self): nil
    ---@field post_transition fun(self): nil
    ---@field spawn_players fun(self): nil
    ---@field spawn_effects fun(self): nil
    ---@field get_level_file fun(self): string
    ---@field get_theme_id fun(self): integer
    ---@field get_base_id fun(self): integer
    ---@field get_floor_spreading_type fun(self): integer
    ---@field get_floor_spreading_type2 fun(self): integer
    ---@field get_transition_styled_floor fun(self): boolean
    ---@field get_transition_floor_modifier fun(self): integer
    ---@field get_transition_styled_floor_type fun(self): integer
    ---@field get_backwall_type fun(self): integer
    ---@field get_border_type fun(self): integer
    ---@field get_critter_type fun(self): integer
    ---@field get_liquid_gravity fun(self): number
    ---@field get_player_damage fun(self): boolean
    ---@field get_explosion_soot fun(self): boolean
    ---@field get_backlayer_lut fun(self): integer
    ---@field get_backlayer_light_level fun(self): number
    ---@field get_loop fun(self): boolean
    ---@field get_vault_level fun(self): integer
    ---@field get_theme_flag fun(self, index: integer): boolean
    ---@field get_dynamic_texture fun(self, texture_id: DYNAMIC_TEXTURE): TEXTURE @Add TEXTURE s to `textures` map of the CustomTheme to override different dynamic textures easily.
    ---@field pre_transition fun(self): nil
    ---@field get_exit_room_y_level fun(self): integer
    ---@field get_shop_chance fun(self): integer
    ---@field spawn_decoration fun(self): nil
    ---@field spawn_decoration2 fun(self): nil
    ---@field spawn_extra fun(self): nil
    ---@field do_procedural_spawn fun(self, info: SpawnInfo): nil
local CustomTheme = nil
---To disable or enable theme functions using the base_theme.
---@param index THEME_OVERRIDE
---@param enabled_ boolean
---@return nil
function CustomTheme:override(index, enabled_) end
---To override a theme function with another theme.
---@param index THEME_OVERRIDE
---@param theme_ integer
---@return nil
function CustomTheme:override(index, theme_) end
---To override a theme function with a lua function.
---@param index THEME_OVERRIDE
---@param func_ function
---@return nil
function CustomTheme:override(index, func_) end

---@class PreLoadLevelFilesContext
    ---@field override_level_files fun(self, levels: string[]): nil @Block all loading `.lvl` files and instead load the specified `.lvl` files. This includes `generic.lvl` so if you need it specify it here.<br/>All `.lvl` files are loaded relative to `Data/Levels`, but they can be completely custom `.lvl` files that ship with your mod so long as they are in said folder.<br/>Use at your own risk, some themes/levels expect a certain level file to be loaded.
    ---@field add_level_files fun(self, levels: string[]): nil @Load additional levels files other than the ones that would usually be loaded. Stacks with `override_level_files` if that was called first.<br/>All `.lvl` files are loaded relative to `Data/Levels`, but they can be completely custom `.lvl` files that ship with your mod so long as they are in said folder.

---@class DoorCoords
    ---@field door1_x number
    ---@field door1_y number
    ---@field door2_x number @door2 only valid when there are two in the level, like Volcana drill, Olmec, ...
    ---@field door2_y number

---@class LevelGenSystem
    ---@field shop_type SHOP_TYPE
    ---@field backlayer_shop_type SHOP_TYPE
    ---@field shop_music integer
    ---@field backlayer_shop_music integer
    ---@field spawn_x number
    ---@field spawn_y number
    ---@field spawn_room_x integer
    ---@field spawn_room_y integer
    ---@field exit_doors Vec2[]
    ---@field themes ThemeInfo[] @size: 18
    ---@field flags integer
    ---@field flags2 integer
    ---@field flags3 integer
    ---@field level_config integer[] @size: 17

---@class PostRoomGenerationContext
    ---@field set_room_template fun(self, x: integer, y: integer, layer: LAYER, room_template: ROOM_TEMPLATE): boolean @Set the room template at the given index and layer, returns `false` if the index is outside of the level.
    ---@field mark_as_machine_room_origin fun(self, x: integer, y: integer, layer: LAYER): boolean @Marks the room as the origin of a machine room, should be the top-left corner of the machine room<br/>Run this after setting the room template for the room, otherwise the machine room will not spawn correctly
    ---@field mark_as_set_room fun(self, x: integer, y: integer, layer: LAYER): boolean @Marks the room as a set-room, a corresponding `setroomy-x` template must be loaded, else the game will crash
    ---@field unmark_as_set_room fun(self, x: integer, y: integer, layer: LAYER): boolean @Unmarks the room as a set-room
    ---@field set_shop_type fun(self, x: integer, y: integer, layer: LAYER, shop_type: integer): boolean @Set the shop type for a specific room, does nothing if the room is not a shop
    ---@field set_procedural_spawn_chance fun(self, chance_id: PROCEDURAL_CHANCE, inverse_chance: integer): boolean @Force a spawn chance for this level, has the same restrictions as specifying the spawn chance in the .lvl file.<br/>Note that the actual chance to spawn is `1/inverse_chance` and that is also slightly skewed because of technical reasons.<br/>Returns `false` if the given chance is not defined.
    ---@field set_num_extra_spawns fun(self, extra_spawn_id: integer, num_spawns_front_layer: integer, num_spawns_back_layer: integer): nil @Change the amount of extra spawns for the given `extra_spawn_id`.
    ---@field define_short_tile_code fun(self, short_tile_code_def: ShortTileCodeDef): SHORT_TILE_CODE? @Defines a new short tile code, automatically picks an unused character or returns a used one in case of an exact match<br/>Returns `nil` if all possible short tile codes are already in use
    ---@field change_short_tile_code fun(self, short_tile_code: SHORT_TILE_CODE, short_tile_code_def: ShortTileCodeDef): nil @Overrides a specific short tile code, this means it will change for the whole level

---@class PreHandleRoomTilesContext
    ---@field get_short_tile_code fun(self, tx: integer, ty: integer, layer: LAYER): SHORT_TILE_CODE? @Gets the tile code at the specified tile coordinate<br/>Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH`, `0 <= ty < CONST.ROOM_HEIGHT` and `layer` in `{LAYER.FRONT, LAYER.BACK}`<br/>Also returns `nil` if `layer == LAYER.BACK` and the room does not have a back layer
    ---@field set_short_tile_code fun(self, tx: integer, ty: integer, layer: LAYER, short_tile_code: SHORT_TILE_CODE): boolean @Sets the tile code at the specified tile coordinate<br/>Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH`, `0 <= ty < CONST.ROOM_HEIGHT` and `layer` in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`<br/>Also returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
    ---@field find_all_short_tile_codes fun(self, layer: LAYER, short_tile_code: SHORT_TILE_CODE): integer[][] @Finds all places a short tile code is used in the room, `layer` must be in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`<br/>Returns an empty list if `layer == LAYER.BACK` and the room does not have a back layer
    ---@field replace_short_tile_code fun(self, layer: LAYER, short_tile_code: SHORT_TILE_CODE, replacement_short_tile_code: SHORT_TILE_CODE): boolean @Replaces all instances of `short_tile_code` in the given layer with `replacement_short_tile_code`, `layer` must be in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`<br/>Returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
    ---@field has_back_layer fun(self): boolean @Check whether the room has a back layer
    ---@field add_empty_back_layer fun(self): nil @Add a back layer filled with all `0` if there is no back layer yet<br/>Does nothing if there already is a backlayer
    ---@field add_copied_back_layer fun(self): nil @Add a back layer that is a copy of the front layer<br/>Does nothing if there already is a backlayer

---@class ShortTileCodeDef
    ---@field tile_code TILE_CODE @Tile code that is used by default when this short tile code is encountered. Defaults to 0.
    ---@field chance integer @Chance in percent to pick `tile_code` over `alt_tile_code`, ignored if `chance == 0`. Defaults to 100.
    ---@field alt_tile_code TILE_CODE @Alternative tile code, ignored if `chance == 100`. Defaults to 0.

---@class QuestsInfo
    ---@field yang_state integer
    ---@field jungle_sisters_flags integer
    ---@field van_horsing_state integer
    ---@field sparrow_state integer
    ---@field madame_tusk_state integer
    ---@field beg_state integer

---@class SaveData
    ---@field places boolean[] @size: 16
    ---@field bestiary boolean[] @size: 78
    ---@field people boolean[] @size: 38
    ---@field items boolean[] @size: 54
    ---@field traps boolean[] @size: 24
    ---@field last_daily string
    ---@field characters integer @20bit bitmask of unlocked characters
    ---@field tutorial_state integer @Tutorial state 0..4. Changes the camp layout, camera and lighting. (0=nothing, 1=journal got, 2=key spawned, 3=door unlocked, 4=complete)
    ---@field shortcuts integer @Terra quest state 0..10 (0=not met ... 10=complete)
    ---@field bestiary_killed integer[] @size: 78
    ---@field bestiary_killed_by integer[] @size: 78
    ---@field people_killed integer[] @size: 38
    ---@field people_killed_by integer[] @size: 38
    ---@field plays integer
    ---@field deaths integer
    ---@field wins_normal integer
    ---@field wins_hard integer
    ---@field wins_special integer
    ---@field score_total integer
    ---@field score_top integer
    ---@field deepest_area integer
    ---@field deepest_level integer
    ---@field time_best integer
    ---@field time_total integer
    ---@field time_tutorial integer
    ---@field character_deaths integer[] @size: 20
    ---@field pets_rescued integer[] @size: 3
    ---@field completed_normal boolean
    ---@field completed_ironman boolean
    ---@field completed_hard boolean
    ---@field profile_seen boolean
    ---@field seeded_unlocked boolean
    ---@field world_last integer
    ---@field level_last integer
    ---@field theme_last integer
    ---@field score_last integer
    ---@field time_last integer
    ---@field stickers ENT_TYPE[] @size: 20
    ---@field players integer[] @size: 4
    ---@field constellation Constellation

---@class Constellation
    ---@field star_count integer
    ---@field stars ConstellationStar[] @size: 45
    ---@field scale number
    ---@field line_count integer
    ---@field lines ConstellationLine[] @size: 90
    ---@field line_red_intensity number

---@class ConstellationStar
    ---@field type integer
    ---@field x number
    ---@field y number
    ---@field size number
    ---@field red number
    ---@field green number
    ---@field blue number
    ---@field alpha number
    ---@field halo_red number
    ---@field halo_green number
    ---@field halo_blue number
    ---@field halo_alpha number
    ---@field canis_ring boolean
    ---@field fidelis_ring boolean

---@class ConstellationLine
    ---@field from integer
    ---@field to integer

---@class SpawnInfo
    ---@field room_template ROOM_TEMPLATE
    ---@field grid_entity Entity @Grid entity at this position, will only try to spawn procedural if this is nil
    ---@field x number
    ---@field y number

---@class ParameterId
    ---@field data1 any @FMODStudio::ParameterId::data1
    ---@field data2 any @FMODStudio::ParameterId::data2

---@class ParameterDescription
    ---@field name any @FMODStudio::ParameterDescription::name
    ---@field id any @FMODStudio::ParameterDescription::id
    ---@field minimum any @FMODStudio::ParameterDescription::minimum
    ---@field maximum any @FMODStudio::ParameterDescription::maximum
    ---@field defaultvalue any @FMODStudio::ParameterDescription::defaultvalue
    ---@field type any @FMODStudio::ParameterDescription::type
    ---@field flags any @FMODStudio::ParameterDescription::flags

---@class CustomBank
    ---@field getLoadingState fun(self): FMODStudio::LoadingState?
    ---@field loadSampleData fun(self): boolean
    ---@field unloadSampleData fun(self): boolean
    ---@field getSampleLoadingState fun(self): FMODStudio::LoadingState?
    ---@field unload fun(self): boolean
    ---@field isValid fun(self): boolean

---@class CustomEventDescription
    ---@field createInstance fun(self): CustomEventInstance
    ---@field releaseAllInstances fun(self): boolean
    ---@field loadSampleData fun(self): boolean
    ---@field unloadSampleData fun(self): boolean
    ---@field getSampleLoadingState fun(self): FMODStudio::LoadingState?
    ---@field getParameterDescriptionCount fun(self): integer?
    ---@field getParameterDescriptionByName fun(self, name: string): FMODStudio::ParameterDescription?
    ---@field getParameterDescriptionByIndex fun(self, index: integer): FMODStudio::ParameterDescription?
    ---@field getParameterIDByName fun(self, name: string): FMODStudio::ParameterId?
    ---@field isValid fun(self): boolean

---@class CustomEventInstance
    ---@field start fun(self): boolean
    ---@field getPlaybackState fun(self): FMODStudio::PlaybackState?
    ---@field setPause fun(self, pause: boolean): boolean
    ---@field getPause fun(self): boolean?
    ---@field keyOff fun(self): boolean
    ---@field setPitch fun(self, pitch: number): boolean
    ---@field getPitch fun(self): number?
    ---@field setTimelinePosition fun(self, position: integer): boolean
    ---@field getTimelinePosition fun(self): integer?
    ---@field setVolume fun(self, volume: number): boolean
    ---@field getVolume fun(self): number?
    ---@field release fun(self): boolean
    ---@field isValid fun(self): boolean
    ---@field getParameterByName fun(self, name: string): number?
    ---@field release fun(self): boolean
    ---@field isValid fun(self): boolean
local CustomEventInstance = nil
---@return boolean
function CustomEventInstance:stop() end
---@param mode FMODStudio::StopMode
---@return boolean
function CustomEventInstance:stop(mode) end
---@param name string
---@param value number
---@return boolean
function CustomEventInstance:setParameterByName(name, value) end
---@param name string
---@param value number
---@param ignoreseekspeed boolean
---@return boolean
function CustomEventInstance:setParameterByName(name, value, ignoreseekspeed) end
---@param name string
---@param label string
---@return boolean
function CustomEventInstance:setParameterByNameWithLabel(name, label) end
---@param name string
---@param label string
---@param ignoreseekspeed boolean
---@return boolean
function CustomEventInstance:setParameterByNameWithLabel(name, label, ignoreseekspeed) end
---@param id FMODStudio::ParameterId
---@param value number
---@return boolean
function CustomEventInstance:setParameterByID(id, value) end
---@param id FMODStudio::ParameterId
---@param value number
---@param ignoreseekspeed boolean
---@return boolean
function CustomEventInstance:setParameterByID(id, value, ignoreseekspeed) end
---@param id FMODStudio::ParameterId
---@param label string
---@return boolean
function CustomEventInstance:setParameterByIDWithLabel(id, label) end
---@param id FMODStudio::ParameterId
---@param label string
---@param ignoreseekspeed boolean
---@return boolean
function CustomEventInstance:setParameterByIDWithLabel(id, label, ignoreseekspeed) end

---@class FMODguidMap
    ---@field getEvent fun(self, path: string): CustomEventDescription?

---@class CustomSound
    ---@field get_parameters fun(self): table<VANILLA_SOUND_PARAM, string>
local CustomSound = nil
---@return PlayingSound
function CustomSound:play() end
---@param paused boolean
---@return PlayingSound
function CustomSound:play(paused) end
---@param paused boolean
---@param sound_type SOUND_TYPE
---@return PlayingSound
function CustomSound:play(paused, sound_type) end

---@class PlayingSound
    ---@field is_playing fun(self): boolean
    ---@field stop fun(self): boolean
    ---@field set_pause fun(self, pause: boolean): boolean
    ---@field set_mute fun(self, mute: boolean): boolean
    ---@field set_pitch fun(self, pitch: number): boolean
    ---@field set_pan fun(self, pan: number): boolean
    ---@field set_volume fun(self, volume: number): boolean
    ---@field set_looping fun(self, loop_mode: SOUND_LOOP_MODE): boolean
    ---@field set_callback fun(self, callback: SoundCallbackFunction): boolean
    ---@field get_parameters fun(self): table<VANILLA_SOUND_PARAM, string>
    ---@field get_parameter fun(self, parameter_index: VANILLA_SOUND_PARAM): number?
    ---@field set_parameter fun(self, parameter_index: VANILLA_SOUND_PARAM, value: number): boolean

---@class SoundMeta
    ---@field x number
    ---@field y number
    ---@field sound_info SoundInfo
    ---@field left_channel number[] @size: 38 @Use VANILLA_SOUND_PARAM as index, warning: special case with first index at 0, loop using pairs will get you all results but the key/index will be wrong, ipairs will have correct key/index but will skip the first element
    ---@field right_channel number[] @size: 38 @Use VANILLA_SOUND_PARAM as index warning: special case with first index at 0, loop using pairs will get you all results but the key/index will be wrong, ipairs will have correct key/index but will skip the first element
    ---@field start_over boolean @when false, current track starts from the beginning, is immediately set back to true
    ---@field playing boolean @set to false to turn off
    ---@field start fun(self): nil

---@class BackgroundSound : SoundMeta

---@class SoundInfo
    ---@field sound_id SOUNDID
    ---@field sound_name VANILLA_SOUND

---@class PlayerSlotSettings
    ---@field controller_vibration boolean
    ---@field auto_run_enabled boolean
    ---@field controller_right_stick boolean

---@class PlayerSlot
    ---@field buttons_gameplay INPUTS
    ---@field buttons INPUTS
    ---@field input_mapping_keyboard InputMapping
    ---@field input_mapping_controller InputMapping
    ---@field player_slot integer
    ---@field is_participating boolean

---@class InputMapping
    ---@field jump integer
    ---@field attack integer
    ---@field bomb integer
    ---@field rope integer
    ---@field walk_run integer
    ---@field use_door_buy integer
    ---@field pause_menu integer
    ---@field journal integer
    ---@field left integer
    ---@field right integer
    ---@field up integer
    ---@field down integer
    ---@field mapping RAW_KEY[] @size: 12 @Can be indexed with INPUT_FLAG. Keyboard uses RAW_KEY values, controller uses RAW_BUTTON values.

---@class PlayerInputs
    ---@field player_slots PlayerSlot[] @size: MAX_PLAYERS
    ---@field player_slot_1 PlayerSlot
    ---@field player_slot_2 PlayerSlot
    ---@field player_slot_3 PlayerSlot
    ---@field player_slot_4 PlayerSlot
    ---@field player_settings PlayerSlotSettings[] @size: MAX_PLAYERS
    ---@field player_slot_1_settings PlayerSlotSettings
    ---@field player_slot_2_settings PlayerSlotSettings
    ---@field player_slot_3_settings PlayerSlotSettings
    ---@field player_slot_4_settings PlayerSlotSettings

---@class GuiDrawContext
    ---@field draw_line fun(self, x1: number, y1: number, x2: number, y2: number, thickness: number, color: uColor): nil @Draws a line on screen
    ---@field draw_triangle fun(self, p1: Vec2, p2: Vec2, p3: Vec2, thickness: number, color: uColor): nil @Draws a triangle on screen.
    ---@field draw_triangle_filled fun(self, p1: Vec2, p2: Vec2, p3: Vec2, color: uColor): nil @Draws a filled triangle on screen.
    ---@field draw_poly fun(self, points: Vec2[], thickness: number, color: uColor): nil @Draws a polyline on screen.
    ---@field draw_poly_filled fun(self, points: Vec2[], color: uColor): nil @Draws a filled convex polyline on screen.
    ---@field draw_bezier_cubic fun(self, p1: Vec2, p2: Vec2, p3: Vec2, p4: Vec2, thickness: number, color: uColor): nil @Draws a cubic bezier curve on screen.
    ---@field draw_bezier_quadratic fun(self, p1: Vec2, p2: Vec2, p3: Vec2, thickness: number, color: uColor): nil @Draws a quadratic bezier curve on screen.
    ---@field draw_circle fun(self, x: number, y: number, radius: number, thickness: number, color: uColor): nil @Draws a circle on screen
    ---@field draw_circle_filled fun(self, x: number, y: number, radius: number, color: uColor): nil @Draws a filled circle on screen
    ---@field draw_text fun(self, x: number, y: number, size: number, text: string, color: uColor): nil @Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
    ---@field draw_layer fun(self, layer: DRAW_LAYER): nil @Draw on top of UI windows, including platform windows that may be outside the game area, or only in current widget window. Defaults to main viewport background.
    ---@field window fun(self, title: string, x: number, y: number, w: number, h: number, movable: boolean, callback: fun(ctx: GuiDrawContext, pos: Vec2, size: Vec2): nil): boolean @Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the<br/>[ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to<br/>autosize in center. Use just a `##Label` as title to hide titlebar.<br/>**Important: Keep all your labels unique!** If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to<br/>give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text...<br/>Returns false if the window was closed from the X.<br/><br/>The callback signature is nil win(GuiDrawContext ctx, Vec2 pos, Vec2 size)
    ---@field win_text fun(self, text: string): nil @Add some text to window, automatically wrapped
    ---@field win_separator fun(self): nil @Add a separator line to window
    ---@field win_separator_text fun(self, text: string): nil @Add a separator text line to window
    ---@field win_inline fun(self): nil @Add next thing on the same line. This is same as `win_sameline(0, -1)`
    ---@field win_sameline fun(self, offset: number, spacing: number): nil @Add next thing on the same line, with an offset
    ---@field win_button fun(self, text: string): boolean @Add a button
    ---@field win_input_text fun(self, label: string, value: string): string @Add a text field
    ---@field win_input_int fun(self, label: string, value: integer): integer @Add an integer field
    ---@field win_input_float fun(self, label: string, value: number): number @Add a float field
    ---@field win_slider_int fun(self, label: string, value: integer, min: integer, max: integer): integer @Add an integer slider
    ---@field win_drag_int fun(self, label: string, value: integer, min: integer, max: integer): integer @Add an integer dragfield
    ---@field win_slider_float fun(self, label: string, value: number, min: number, max: number): number @Add an float slider
    ---@field win_drag_float fun(self, label: string, value: number, min: number, max: number): number @Add an float dragfield
    ---@field win_check fun(self, label: string, value: boolean): boolean @Add a checkbox
    ---@field win_combo fun(self, label: string, selected: integer, opts: string): integer @Add a combo box
    ---@field win_popid fun(self): nil @Pop unique identifier from the stack. Put after the input.
    ---@field win_image fun(self, image: IMAGE, width: number, height: number): nil @Draw image to window.
    ---@field win_imagebutton fun(self, label: string, image: IMAGE, width: number, height: number, uvx1: number, uvy1: number, uvx2: number, uvy2: number): boolean @Draw imagebutton to window.
    ---@field win_section fun(self, title: string, callback: function): nil @Add a collapsing accordion section, put contents in the callback function.
    ---@field win_indent fun(self, width: number): nil @Indent contents, or unindent if negative
    ---@field win_width fun(self, width: number): nil @Sets next item width (width>1: width in pixels, width<0: to the right of window, -1<width<1: fractional, multiply by available window width)
    ---@field key_picker fun(self, message: string, flags: KEY_TYPE): integer @Returns KEY flags including held OL_MOD modifiers, or -1 before any key has been pressed and released, or mouse button pressed. Also returns -1 before all initially held keys are released before the picker was opened, or if another key picker is already waiting for keys. If a modifier is released, that modifier is returned as an actual keycode (e.g. `KEY.LSHIFT`) while the other held modifiers are returned as `KEY.OL_MOD_...` flags.<br/>Shows a fullscreen key picker with a message, with the accepted input type (keyboard/mouse) filtered by flags. The picker won't show before all previously held keys have been released and other key pickers have returned a valid key.
local GuiDrawContext = nil
---Draws a rectangle on screen from top-left to bottom-right.
---@param left number
---@param top number
---@param right number
---@param bottom number
---@param thickness number
---@param rounding number
---@param color uColor
---@return nil
function GuiDrawContext:draw_rect(left, top, right, bottom, thickness, rounding, color) end
---Draws a rectangle on screen from top-left to bottom-right.
---@param rect AABB
---@param thickness number
---@param rounding number
---@param color uColor
---@return nil
function GuiDrawContext:draw_rect(rect, thickness, rounding, color) end
---Draws a filled rectangle on screen from top-left to bottom-right.
---@param left number
---@param top number
---@param right number
---@param bottom number
---@param rounding number
---@param color uColor
---@return nil
function GuiDrawContext:draw_rect_filled(left, top, right, bottom, rounding, color) end
---Draws a filled rectangle on screen from top-left to bottom-right.
---@param rect AABB
---@param rounding number
---@param color uColor
---@return nil
function GuiDrawContext:draw_rect_filled(rect, rounding, color) end
---Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
---@param image IMAGE
---@param left number
---@param top number
---@param right number
---@param bottom number
---@param uvx1 number
---@param uvy1 number
---@param uvx2 number
---@param uvy2 number
---@param color uColor
---@return nil
function GuiDrawContext:draw_image(image, left, top, right, bottom, uvx1, uvy1, uvx2, uvy2, color) end
---Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
---@param image IMAGE
---@param rect AABB
---@param uv_rect AABB
---@param color uColor
---@return nil
function GuiDrawContext:draw_image(image, rect, uv_rect, color) end
---Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
---@param image IMAGE
---@param left number
---@param top number
---@param right number
---@param bottom number
---@param uvx1 number
---@param uvy1 number
---@param uvx2 number
---@param uvy2 number
---@param color uColor
---@param angle number
---@param px number
---@param py number
---@return nil
function GuiDrawContext:draw_image_rotated(image, left, top, right, bottom, uvx1, uvy1, uvx2, uvy2, color, angle, px, py) end
---Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
---@param image IMAGE
---@param rect AABB
---@param uv_rect AABB
---@param color uColor
---@param angle number
---@param px number
---@param py number
---@return nil
function GuiDrawContext:draw_image_rotated(image, rect, uv_rect, color, angle, px, py) end
---Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
---@param id integer
---@return nil
function GuiDrawContext:win_pushid(id) end
---Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
---@param id string
---@return nil
function GuiDrawContext:win_pushid(id) end

---@class Gamepad
    ---@field enabled boolean
    ---@field buttons GAMEPAD
    ---@field lt number
    ---@field rt number
    ---@field lx number
    ---@field ly number
    ---@field rx number
    ---@field ry number

---@class ImGuiIO
    ---@field displaysize Vec2
    ---@field framerate number
    ---@field wantkeyboard boolean
    ---@field keys boolean[] @size: 652
    ---@field keydown fun(key: number | string): boolean
    ---@field keypressed fun(key: number | string, repeat?: boolean ): boolean
    ---@field keyreleased fun(key: number | string): boolean
    ---@field keyctrl boolean
    ---@field keyshift boolean
    ---@field keyalt boolean
    ---@field keysuper boolean
    ---@field modifierdown fun(self, chord: integer): boolean
    ---@field wantmouse boolean
    ---@field mousepos Vec2
    ---@field mousedown boolean[] @size: 5
    ---@field mouseclicked boolean[] @size: 5
    ---@field mousedoubleclicked boolean[] @size: 5
    ---@field mousereleased boolean[] @size: 5
    ---@field mousewheel number
    ---@field gamepad Gamepad
    ---@field gamepads fun(self, index: integer): Gamepad
    ---@field showcursor boolean

---@class VanillaRenderContext
    ---@field draw_text_size fun(self, text: string, scale_x: number, scale_y: number, fontstyle: integer): number, number @Measure the provided text using the built-in renderer<br/>If you can, consider creating your own TextRenderingInfo instead<br/>You can then use `:text_size()` and `draw_text` with that one object<br/>`draw_text_size` works by creating new TextRenderingInfo just to call `:text_size()`, which is not very optimal
    ---@field set_corner_finish fun(self, c: CORNER_FINISH): nil @Set the preferred way of drawing corners for the non filled shapes
    ---@field draw_screen_line fun(self, A: Vec2, B: Vec2, thickness: number, color: Color): nil @Draws a line on screen using the built-in renderer from point `A` to point `B`.<br/>Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    ---@field draw_screen_rect fun(self, rect: AABB, thickness: number, color: Color, angle: number?, px: number?, py: number?): nil @Draw rectangle in screen coordinates from top-left to bottom-right using the built-in renderer with optional `angle`.<br/>`px`/`py` is pivot for the rotation where 0,0 is center 1,1 is top right corner etc. (corner from the AABB, not the visible one from adding the `thickness`)<br/>Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    ---@field draw_screen_rect_filled fun(self, rect: AABB, color: Color, angle: number?, px: number?, py: number?): nil @Draw filled rectangle in screen coordinates from top-left to bottom-right using the built-in renderer with optional `angle`.<br/>`px`/`py` is pivot for the rotation where 0,0 is center 1,1 is top right corner etc.<br/>Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    ---@field draw_screen_triangle fun(self, triangle: Triangle, thickness: number, color: Color): nil @Draw triangle in screen coordinates using the built-in renderer.<br/>Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    ---@field draw_screen_triangle_filled fun(self, triangle: Triangle, color: Color): nil @Draw filled triangle in screen coordinates using the built-in renderer.<br/>Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    ---@field draw_world_line fun(self, A: Vec2, B: Vec2, thickness: number, color: Color): nil @Draws a line in world coordinates using the built-in renderer from point `A` to point `B`.<br/>Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    ---@field draw_world_rect fun(self, rect: AABB, thickness: number, color: Color, angle: number?, px: number?, py: number?): nil @Draw rectangle in world coordinates from top-left to bottom-right using the built-in renderer with optional `angle`.<br/>`px`/`py` is pivot for the rotation where 0,0 is center 1,1 is top right corner etc. (corner from the AABB, not the visible one from adding the `thickness`)<br/>Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    ---@field draw_world_rect_filled fun(self, rect: AABB, color: Color, angle: number?, px: number?, py: number?): nil @Draw rectangle in world coordinates from top-left to bottom-right using the built-in renderer with optional `angle`.<br/>`px`/`py` is pivot for the rotation where 0,0 is center 1,1 is top right corner etc.<br/>Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    ---@field draw_world_triangle fun(self, triangle: Triangle, thickness: number, color: Color): nil @Draw triangle in world coordinates using the built-in renderer.<br/>Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    ---@field draw_world_triangle_filled fun(self, triangle: Triangle, color: Color): nil @Draw filled triangle in world coordinates using the built-in renderer.<br/>Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    ---@field bounding_box AABB
    ---@field render_draw_depth any @render_draw_depth_lua
local VanillaRenderContext = nil
---Draw text using the built-in renderer
---Use in combination with ON.RENDER_✱ events. See vanilla_rendering.lua in the example scripts.
---@param text string
---@param x number
---@param y number
---@param scale_x number
---@param scale_y number
---@param color Color
---@param alignment VANILLA_TEXT_ALIGNMENT
---@param fontstyle VANILLA_FONT_STYLE
---@return nil
function VanillaRenderContext:draw_text(text, x, y, scale_x, scale_y, color, alignment, fontstyle) end
---@param tri TextRenderingInfo
---@param color Color
---@return nil
function VanillaRenderContext:draw_text(tri, color) end
---Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param left number
---@param top number
---@param right number
---@param bottom number
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_texture(texture_id, row, column, left, top, right, bottom, color) end
---Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param rect AABB
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_texture(texture_id, row, column, rect, color) end
---Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer with angle, px/py is pivot for the rotation where 0,0 is center 1,1 is top right corner etc.
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param rect AABB
---@param color Color
---@param angle number
---@param px number
---@param py number
---@return nil
function VanillaRenderContext:draw_screen_texture(texture_id, row, column, rect, color, angle, px, py) end
---Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param dest Quad
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_texture(texture_id, row, column, dest, color) end
---Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer. `source` - the coordinates in the texture, `dest` - the coordinates on the screen
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param texture_id TEXTURE
---@param source Quad
---@param dest Quad
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_texture(texture_id, source, dest, color) end
---Draw a texture in screen coordinates using TextureRenderingInfo
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param texture_id TEXTURE
---@param tri TextureRenderingInfo
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_texture(texture_id, tri, color) end
---Draw a polyline on screen from points using the built-in renderer
---Draws from the first to the last point, use `closed` to connect first and last as well
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param points Vec2[]
---@param thickness number
---@param color Color
---@param closed boolean
---@return nil
function VanillaRenderContext:draw_screen_poly(points, thickness, color, closed) end
---Draw quadrilateral in screen coordinates from top-left to bottom-right using the built-in renderer.
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param points Quad
---@param thickness number
---@param color Color
---@param closed boolean
---@return nil
function VanillaRenderContext:draw_screen_poly(points, thickness, color, closed) end
---Draw a convex polygon on screen from points using the built-in renderer
---Can probably draw almost any polygon, but the convex one is guaranteed to look correct
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param points Vec2[]
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_poly_filled(points, color) end
---Draw filled quadrilateral in screen coordinates from top-left to bottom-right using the built-in renderer.
---Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
---@param points Quad
---@param color Color
---@return nil
function VanillaRenderContext:draw_screen_poly_filled(points, color) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---For more control use the version taking a Quad instead
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param left number
---@param top number
---@param right number
---@param bottom number
---@param color Color
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, row, column, left, top, right, bottom, color) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---For more control use the version taking a Quad instead
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param dest AABB
---@param color Color
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, row, column, dest, color) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer with angle, px/py is pivot for the rotation where 0,0 is center 1,1 is top right corner etc.
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---For more control use the version taking a Quad instead
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param dest AABB
---@param color Color
---@param angle number
---@param px number
---@param py number
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, row, column, dest, color, angle, px, py) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
---The `shader` parameter controls how to render the texture
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param dest Quad
---@param color Color
---@param shader WORLD_SHADER
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, row, column, dest, color, shader) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param texture_id TEXTURE
---@param row integer
---@param column integer
---@param dest Quad
---@param color Color
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, row, column, dest, color) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer.  `source` - the coordinates in the texture, `dest` - the coordinates on the screen
---The `shader` parameter controls how to render the texture
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param texture_id TEXTURE
---@param source Quad
---@param dest Quad
---@param color Color
---@param shader WORLD_SHADER
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, source, dest, color, shader) end
---Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer.  `source` - the coordinates in the texture, `dest` - the coordinates on the screen
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param texture_id TEXTURE
---@param source Quad
---@param dest Quad
---@param color Color
---@return nil
function VanillaRenderContext:draw_world_texture(texture_id, source, dest, color) end
---Draw a polyline in world coordinates from points using the built-in renderer
---Draws from the first to the last point, use `closed` to connect first and last as well
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param points Vec2[]
---@param thickness number
---@param color Color
---@param closed boolean
---@return nil
function VanillaRenderContext:draw_world_poly(points, thickness, color, closed) end
---Draw quadrilateral in world coordinates from top-left to bottom-right using the built-in renderer.
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param points Quad
---@param thickness number
---@param color Color
---@param closed boolean
---@return nil
function VanillaRenderContext:draw_world_poly(points, thickness, color, closed) end
---Draw a convex polygon in world coordinates from points using the built-in renderer
---Can probably draw almost any polygon, but the convex one is guaranteed to look correct
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param points Vec2[]
---@param color Color
---@return nil
function VanillaRenderContext:draw_world_poly_filled(points, color) end
---Draw filled quadrilateral in world coordinates from top-left to bottom-right using the built-in renderer.
---Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
---@param points Quad
---@param color Color
---@return nil
function VanillaRenderContext:draw_world_poly_filled(points, color) end

---@class TextureRenderingInfo
    ---@field new any @constructors<TextureRenderingInfo(), TextureRenderingInfo(TextureRenderingInfo)>{}
    ---@field x number
    ---@field y number
    ---@field destination_bottom_left_x number @destination is relative to the x,y center point
    ---@field destination_bottom_left_y number
    ---@field destination_bottom_right_x number
    ---@field destination_bottom_right_y number
    ---@field destination_top_left_x number
    ---@field destination_top_left_y number
    ---@field destination_top_right_x number
    ---@field destination_top_right_y number
    ---@field set_destination fun(self, bbox: AABB): nil
    ---@field dest_get_quad fun(self): Quad
    ---@field dest_set_quad fun(self, quad: Quad): nil
    ---@field source_bottom_left_x number
    ---@field source_bottom_left_y number
    ---@field source_bottom_right_x number
    ---@field source_bottom_right_y number
    ---@field source_top_left_x number
    ---@field source_top_left_y number
    ---@field source_top_right_x number
    ---@field source_top_right_y number
    ---@field source_get_quad fun(self): Quad
    ---@field source_set_quad fun(self, quad: Quad): nil

---@class Letter
    ---@field bottom Triangle
    ---@field top Triangle
    ---@field get_quad fun(self): Quad @Get the Quad of a letter (easier to work with compared to the two triangles)<br/>This assumes that the triangles are in the correct 'touching each other' position<br/>if the positions were altered the results may not end up as expected
    ---@field set_quad fun(self, quad: Quad): nil @Inverse of the get_quad
    ---@field center fun(self): Vec2 @Get's approximated center of a letter by finding the highest and lowest values, then finding the center of a rectangle build from those values

---@class TextRenderingInfo
    ---@field new any @initializers(TextRenderingInfo_ctor, TextRenderingInfo_ctor2)
    ---@field x number
    ---@field y number
    ---@field text_length integer @You can also just use `#` operator on the whole TextRenderingInfo to get the text length
    ---@field width number
    ---@field height number
    ---@field special_texture_id TEXTURE @Used to draw buttons and stuff, default is -1 which uses the buttons texture
    ---@field get_dest fun(self): Letter[] @Returns reference to the letter coordinates relative to the x,y position
    ---@field get_source fun(self): Letter[] @Returns reference to the letter coordinates in the texture
    ---@field text_size fun(self): number, number @{width, height}, is only updated when you set/change the text. This is equivalent to draw_text_size
    ---@field rotate fun(self, angle: number, px: number?, py: number?): nil @Rotates the text around the pivot point (default 0), pivot is relative to the text position (x, y), use px and py to offset it
    ---@field set_text fun(self, text: string, scale_x: number, scale_y: number, alignment: VANILLA_TEXT_ALIGNMENT, fontstyle: VANILLA_FONT_STYLE): nil @Changes the text, only position stays the same, everything else (like rotation) is reset or set according to the parameters
    ---@field get_font fun(self): TEXTURE
    ---@field set_font fun(self, id: TEXTURE): boolean

---@class HudInventory
    ---@field enabled boolean
    ---@field health integer
    ---@field bombs integer
    ---@field ropes integer
    ---@field ankh boolean
    ---@field kapala boolean
    ---@field kapala_sprite SpritePosition
    ---@field poison boolean
    ---@field curse boolean
    ---@field elixir boolean
    ---@field crown ENT_TYPE @Powerup type or 0
    ---@field powerup_sprites SpritePosition[] @size: 18
    ---@field item_count integer @Amount of generic pickup items at the bottom. Set to 0 to not draw them.

---@class HudElement
    ---@field dim boolean @Hide background and dim if using the auto adjust setting.
    ---@field opacity number @Background will be drawn if this is not 0.5
    ---@field time_dim integer @Level time when element should dim again after highlighted, INT_MAX if dimmed on auto adjust. 0 on opaque.

---@class HudPlayer : HudElement
    ---@field health integer
    ---@field bombs integer
    ---@field ropes integer

---@class HudMoney : HudElement
    ---@field total integer
    ---@field counter integer
    ---@field timer integer

---@class HudData
    ---@field inventory HudInventory[] @size: MAX_PLAYERS
    ---@field udjat boolean
    ---@field money_total integer
    ---@field money_counter integer
    ---@field time_total integer @in ms
    ---@field time_level integer @in ms
    ---@field world_num integer
    ---@field level_num integer
    ---@field angry_shopkeeper boolean
    ---@field seed_shown boolean
    ---@field seed integer
    ---@field opacity number
    ---@field roll_in number
    ---@field players HudPlayer[] @size: MAX_PLAYERS
    ---@field money HudMoney
    ---@field money_increase_sparkles ParticleEmitterInfo
    ---@field timer HudElement
    ---@field level HudElement
    ---@field clover_falling_apart_timer number
    ---@field player_cursed_particles ParticleEmitterInfo[] @size: MAX_PLAYERS
    ---@field player_poisoned_particles ParticleEmitterInfo[] @size: MAX_PLAYERS
    ---@field player_highlight TextureRenderingInfo @For player related icons, they use the same TextureRendering, just offset while drawing
    ---@field player_heart TextureRenderingInfo
    ---@field player_ankh TextureRenderingInfo
    ---@field kapala_icon TextureRenderingInfo
    ---@field player_crown TextureRenderingInfo
    ---@field player_bomb TextureRenderingInfo
    ---@field player_rope TextureRenderingInfo
    ---@field udjat_icon TextureRenderingInfo
    ---@field money_and_time_highlight TextureRenderingInfo @Money and time use the same TextureRendering, just offset while drawing
    ---@field dollar_icon TextureRenderingInfo
    ---@field hourglass_icon TextureRenderingInfo
    ---@field clover_icon TextureRenderingInfo
    ---@field level_highlight TextureRenderingInfo
    ---@field level_icon TextureRenderingInfo
    ---@field seed_background TextureRenderingInfo

---@class Hud
    ---@field y number
    ---@field opacity number
    ---@field data HudData

---@class TextureDefinition
    ---@field texture_path string
    ---@field width integer
    ---@field height integer
    ---@field tile_width integer
    ---@field tile_height integer
    ---@field sub_image_offset_x integer
    ---@field sub_image_offset_y integer
    ---@field sub_image_width integer
    ---@field sub_image_height integer

---@class Vec2
    ---@field x number
    ---@field y number
    ---@field distance_to fun(self, other: Vec2): number @Just simple Pythagoras theorem
    ---@field set fun(self, other: Vec2): Vec2
    ---@field split fun(self): number, number
local Vec2 = nil
---@param angle number
---@param px number
---@param py number
---@return Vec2
function Vec2:rotate(angle, px, py) end
---@param angle number
---@param p Vec2
---@return Vec2
function Vec2:rotate(angle, p) end

---@class AABB
    ---@field left number
    ---@field bottom number
    ---@field right number
    ---@field top number
    ---@field overlaps_with fun(self, other: AABB): boolean
    ---@field abs fun(self): AABB @Fixes the AABB if any of the sides have negative length
    ---@field offset fun(self, off_x: number, off_y: number): AABB @Offsets the AABB by the given offset.
    ---@field area fun(self): number @Compute area of the AABB, can be zero if one dimension is zero or negative if one dimension is inverted.
    ---@field center fun(self): number, number @Short for `(aabb.left + aabb.right) / 2.0f, (aabb.top + aabb.bottom) / 2.0f`.
    ---@field width fun(self): number @Short for `aabb.right - aabb.left`.
    ---@field height fun(self): number @Short for `aabb.top - aabb.bottom`.
    ---@field set fun(self, other: AABB): AABB
    ---@field split fun(self): number, number, number, number
local AABB = nil
---Grows or shrinks the AABB by the given amount in all directions.
---If `amount < 0` and `abs(amount) > right/top - left/bottom` the respective dimension of the AABB will become `0`.
---@param amount number
---@return AABB
function AABB:extrude(amount) end
---Grows or shrinks the AABB by the given amount in each direction.
---If `amount_x/y < 0` and `abs(amount_x/y) > right/top - left/bottom` the respective dimension of the AABB will become `0`.
---@param amount_x number
---@param amount_y number
---@return AABB
function AABB:extrude(amount_x, amount_y) end
---Checks if point lies between left/right and top/bottom
---@param p Vec2
---@return boolean
function AABB:is_point_inside(p) end
---@param x number
---@param y number
---@return boolean
function AABB:is_point_inside(x, y) end

---@class Triangle
    ---@field A Vec2
    ---@field B Vec2
    ---@field C Vec2
    ---@field rotate fun(self, angle: number, px: number, py: number): Triangle @Rotate triangle by an angle, the px/py are just coordinates, not offset from the center
    ---@field center fun(self): Vec2 @Also known as centroid
    ---@field get_angles fun(self): number, number, number @Returns ABC, BCA, CAB angles in radians
    ---@field scale fun(self, scale: number): Triangle
    ---@field area fun(self): number
    ---@field set fun(self, other: Triangle): Triangle
    ---@field split fun(self): Vec2, Vec2, Vec2 @Returns the corner points
local Triangle = nil
---@param off Vec2
---@return Triangle
function Triangle:offset(off) end
---@param x number
---@param y number
---@return Triangle
function Triangle:offset(x, y) end
---Check if point lies inside of triangle
---Because of the imprecise nature of floating point values, the `epsilon` value is needed to compare the floats, the default value is `0.0001`
---@param p Vec2
---@param epsilon number?
---@return boolean
function Triangle:is_point_inside(p, epsilon) end
---@param x number
---@param y number
---@param epsilon number?
---@return boolean
function Triangle:is_point_inside(x, y, epsilon) end

---@class Quad
    ---@field bottom_left_x number
    ---@field bottom_left_y number
    ---@field bottom_right_x number
    ---@field bottom_right_y number
    ---@field top_right_x number
    ---@field top_right_y number
    ---@field top_left_x number
    ---@field top_left_y number
    ---@field get_AABB fun(self): AABB @Returns the max/min values of the Quad
    ---@field rotate fun(self, angle: number, px: number, py: number): Quad @Rotates a Quad by an angle, px/py are not offsets, use `:get_AABB():center()` to get approximated center for symmetrical quadrangle
    ---@field flip_horizontally fun(self): Quad
    ---@field flip_vertically fun(self): Quad
    ---@field set fun(self, other: Quad): Quad
    ---@field split fun(self): Vec2, Vec2, Vec2, Vec2 @Returns the corners in order: bottom_left, bottom_right, top_right, top_left
local Quad = nil
---@param off Vec2
---@return Quad
function Quad:offset(off) end
---@param off_x number
---@param off_y number
---@return Quad
function Quad:offset(off_x, off_y) end
---Check if point lies inside of triangle
---Because of the imprecise nature of floating point values, the `epsilon` value is needed to compare the floats, the default value is `0.00001`
---@param p Vec2
---@param epsilon number?
---@return boolean
function Quad:is_point_inside(p, epsilon) end
---@param x number
---@param y number
---@param epsilon number?
---@return boolean
function Quad:is_point_inside(x, y, epsilon) end

---@class MenuScreenPanels
    ---@field woodpanels_velocity number
    ---@field woodpanels_progress number
    ---@field scroll_unfurl_progress number
    ---@field bottom_woodpanel_speed_multiplayer number
    ---@field bottom_woodpanel_y_offset number
    ---@field bottom_woodpanel TextureRenderingInfo
    ---@field top_woodpanel TextureRenderingInfo
    ---@field scroll TextureRenderingInfo
    ---@field top_woodpanel_left_scrollhandle TextureRenderingInfo
    ---@field top_woodpanel_right_scrollhandle TextureRenderingInfo
    ---@field scroll_text STRINGID
    ---@field bottom_left_text STRINGID
    ---@field bottom_right_text STRINGID
    ---@field bottom_middle_text STRINGID
    ---@field top_woodpanel_visible boolean
    ---@field bottom_woodpanel_visible boolean
    ---@field toggle_woodpanel_slidein_animation boolean
    ---@field capitalize_scroll_text boolean

---@class ScreenControls
    ---@field up boolean
    ---@field down boolean
    ---@field left boolean
    ---@field right boolean
    ---@field direction_input integer @-1 - none, 0 - UP, 1 - DOWN, 2 - LEFT, 3 - RIGHT
    ---@field hold_down_timer integer @Delay after which fast scroll activates (can stop at different value, only matters when you hold down the direction button)
    ---@field fast_scroll_timer integer

---@class Screen
    ---@field render_timer number
    ---@field init fun(self): nil @Initializes the screen.

---@class ScreenLogo : Screen
    ---@field logo_mossmouth TextureRenderingInfo
    ---@field logo_blitworks TextureRenderingInfo
    ---@field logo_fmod TextureRenderingInfo
    ---@field state integer @0 - mossmouth, 1 - blitworks, 2 - fmod, 3 - end (next screen)
    ---@field timer integer

---@class ScreenIntro : Screen
    ---@field blackout_background TextureRenderingInfo
    ---@field blackout_alpha number
    ---@field active boolean @ends the intro immediately if set to false
    ---@field skip_prologue boolean @skips prologue and goes straight to the title screen after the intro

---@class ScreenPrologue : Screen
    ---@field line1 STRINGID
    ---@field line2 STRINGID
    ---@field line3 STRINGID
    ---@field line1_alpha number
    ---@field line2_alpha number
    ---@field line3_alpha number

---@class ScreenTitle : Screen
    ---@field logo_spelunky2 TextureRenderingInfo
    ---@field ana TextureRenderingInfo
    ---@field ana_right_eyeball_torch_reflection TextureRenderingInfo
    ---@field ana_left_eyeball_torch_reflection TextureRenderingInfo
    ---@field particle_torchflame_smoke ParticleEmitterInfo
    ---@field particle_torchflame_backflames ParticleEmitterInfo
    ---@field particle_torchflame_flames ParticleEmitterInfo
    ---@field particle_torchflame_backflames_animated ParticleEmitterInfo
    ---@field particle_torchflame_flames_animated ParticleEmitterInfo
    ---@field particle_torchflame_ash ParticleEmitterInfo
    ---@field brightness number
    ---@field music SoundMeta
    ---@field torch_sound SoundMeta

---@class SpritePosition
    ---@field column integer
    ---@field row integer

---@class ScreenMenu : Screen
    ---@field state integer @0: "cthulhu_pre_movement",<br/>1: "cthulhu_rotating",<br/>2: "cthulhu_separating",<br/>3: "cthulhu_lowering",<br/>4: "cthulhu_transition_to_menu",<br/>5: "return_from_backlayer",<br/>6: "highlight_selection",<br/>7: "idle",<br/>8: "to_submenu",<br/>9: "to_backlayer",<br/>10: "backlayer_idle"
    ---@field tunnel_background TextureRenderingInfo
    ---@field cthulhu_disc TextureRenderingInfo
    ---@field tunnel_ring_darkbrown TextureRenderingInfo
    ---@field cthulhu_body TextureRenderingInfo
    ---@field tunnel_ring_lightbrown TextureRenderingInfo
    ---@field vine_left TextureRenderingInfo
    ---@field vine_right TextureRenderingInfo
    ---@field skull_left TextureRenderingInfo
    ---@field salamander_right TextureRenderingInfo
    ---@field left_spear TextureRenderingInfo
    ---@field right_spear TextureRenderingInfo
    ---@field spear_dangler_related TextureRenderingInfo
    ---@field play_scroll TextureRenderingInfo
    ---@field info_toast TextureRenderingInfo
    ---@field cthulhu_sound SoundMeta
    ---@field particle_smoke ParticleEmitterInfo
    ---@field particle_rubble ParticleEmitterInfo
    ---@field cthulhu_disc_ring_angle number
    ---@field cthulhu_disc_split_progress number
    ---@field cthulhu_disc_y number
    ---@field cthulhu_timer number
    ---@field controls ScreenControls
    ---@field selected_menu_index integer
    ---@field sides_hold_down_timer integer
    ---@field sides_fast_scroll_timer integer
    ---@field loop boolean @Allow going up from first to last option
    ---@field menu_id integer @0 = main menu, 1 = play, 2 = online
    ---@field transfer_to_menu_id integer
    ---@field menu_text_opacity number
    ---@field spear_position number[] @size: 6
    ---@field spear_dangler SpritePosition[] @size: 6
    ---@field spear_dangle_momentum integer[] @size: 6
    ---@field spear_dangle_angle integer[] @size: 6
    ---@field play_scroll_descend_timer number
    ---@field scroll_text STRINGID
    ---@field shake_offset_x number
    ---@field shake_offset_y number
    ---@field loaded_once boolean @Set to true when going from title to menu screen for the first time, makes sure the animation play once

---@class ScreenOptions : Screen
    ---@field down boolean
    ---@field up boolean
    ---@field direction_input integer @-1 = none, 0 = down, 1 = up
    ---@field hold_down_timer integer
    ---@field fast_scroll_timer integer
    ---@field selected_menu_index integer
    ---@field sides_hold_down_timer integer
    ---@field sides_fast_scroll_timer integer
    ---@field loop boolean @Allow going up from first to last option
    ---@field screen_panels MenuScreenPanels
    ---@field menu_id integer
    ---@field transfer_to_menu_id integer
    ---@field show_apply_button boolean
    ---@field topleft_woodpanel_esc TextureRenderingInfo
    ---@field brick_background TextureRenderingInfo
    ---@field brick_middlelayer TextureRenderingInfo
    ---@field brick_foreground TextureRenderingInfo
    ---@field selected_item_rounded_rect TextureRenderingInfo
    ---@field selected_item_scarab TextureRenderingInfo
    ---@field item_option_arrow_left TextureRenderingInfo
    ---@field item_option_arrow_right TextureRenderingInfo
    ---@field tooltip_background TextureRenderingInfo
    ---@field progressbar_background TextureRenderingInfo
    ---@field volume_progressbar_foreground TextureRenderingInfo
    ---@field progressbar_foreground TextureRenderingInfo
    ---@field volume_progressbar_position_indicator TextureRenderingInfo
    ---@field sectionheader_background TextureRenderingInfo
    ---@field pet_icons TextureRenderingInfo @In "Gameplay" menu
    ---@field bottom_scroll TextureRenderingInfo @For the code in the sync menu
    ---@field bottom_left_scrollhandle TextureRenderingInfo
    ---@field bottom_right_scrollhandle TextureRenderingInfo
    ---@field topleft_woodpanel_esc_slidein number
    ---@field text_fadein number
    ---@field vertical_scroll_effect number
    ---@field options_visiable boolean
    ---@field show_highlight boolean @Shows the red background behind the option, the scarab on the left and left/right arrows
    ---@field tooltip_text STRINGID[]
    ---@field disable_controls boolean @Used for sync progress
    ---@field sync_progress_state integer @0 - none, 1 - waiting for the code, 2 - code acquired, 3 - sync in progress, 4 - sync completed
    ---@field credits_progression number

---@class ScreenCodeInput : Screen
    ---@field screen_panels MenuScreenPanels
    ---@field allow_random boolean @needs to be set before opening the screen to show the correct text at the bottom
    ---@field selected_button_index integer
    ---@field pressed_select boolean
    ---@field topleft_woodpanel_esc_slidein number
    ---@field scroll_text_id STRINGID
    ---@field start_text_id STRINGID
    ---@field main_woodpanel_left_border TextureRenderingInfo
    ---@field main_woodpanel_center TextureRenderingInfo
    ---@field main_woodpanel_right_border TextureRenderingInfo
    ---@field top_scroll TextureRenderingInfo
    ---@field letter_cutouts TextureRenderingInfo
    ---@field hand_pointer TextureRenderingInfo
    ---@field key_background TextureRenderingInfo
    ---@field topleft_woodpanel_esc TextureRenderingInfo
    ---@field start_sidepanel TextureRenderingInfo
    ---@field start_sidepanel_slidein number
    ---@field seed_length integer @Current input length (0-8). You probably shouldn't write to this, except to set it to 0.
    ---@field get_seed fun(self): integer? @Get the seed currently entered in the seed dialog or nil if nothing is entered. Will also return incomplete seeds, check seed_length to verify it's ready.
    ---@field set_seed fun(self, seed: integer?, length: integer?): nil @Set the seed entered in the seed dialog. Call without arguments to clear entered seed. Optionally enter a length to set partial seed.

---@class FlyingThing
    ---@field texture_info TextureRenderingInfo
    ---@field entity_type ENT_TYPE
    ---@field spritesheet_column number
    ---@field spritesheet_row number
    ---@field spritesheet_animation_length number
    ---@field velocity_x number
    ---@field amplitude number
    ---@field frequency number
    ---@field sinewave_angle number

---@class ScreenCharacterSelect : Screen
    ---@field main_background_zoom_progress number
    ---@field main_background_zoom_target number
    ---@field blurred_border_zoom_progress number
    ---@field blurred_border_zoom_target number
    ---@field screen_panels MenuScreenPanels
    ---@field mine_entrance_background TextureRenderingInfo
    ---@field character TextureRenderingInfo
    ---@field character_shadow TextureRenderingInfo
    ---@field character_flag TextureRenderingInfo
    ---@field character_left_arrow TextureRenderingInfo
    ---@field character_right_arrow TextureRenderingInfo
    ---@field mine_entrance_border TextureRenderingInfo
    ---@field mine_entrance_shutter TextureRenderingInfo
    ---@field background TextureRenderingInfo
    ---@field blurred_border TextureRenderingInfo
    ---@field blurred_border2 TextureRenderingInfo
    ---@field topleft_woodpanel_esc TextureRenderingInfo
    ---@field start_sidepanel TextureRenderingInfo
    ---@field quick_select_panel TextureRenderingInfo
    ---@field quick_select_selected_char_background TextureRenderingInfo
    ---@field quick_select_panel_related TextureRenderingInfo
    ---@field player_shutter_timer number[] @size: MAX_PLAYERS
    ---@field player_x number[] @size: MAX_PLAYERS
    ---@field player_y number[] @size: MAX_PLAYERS
    ---@field player_arrow_slidein_timer number[][] @size: MAX_PLAYERS
    ---@field player_facing_left boolean[] @size: MAX_PLAYERS
    ---@field player_quickselect_shown boolean[] @size: MAX_PLAYERS
    ---@field player_quickselect_fadein_timer number[] @size: MAX_PLAYERS
    ---@field player_quickselect_coords number[][] @size: MAX_PLAYERS
    ---@field player_quickselect_wiggle_angle number[] @size: MAX_PLAYERS
    ---@field topleft_woodpanel_esc_slidein number
    ---@field start_panel_slidein number
    ---@field action_buttons_keycap_size number
    ---@field screen_loading boolean
    ---@field seeded_run boolean
    ---@field daily_challenge boolean
    ---@field arena boolean @Short for `screen->next_screen_to_load == SCREEN.TEAM_SELECT and not screen->seeded_run and not screen->daily_challenge` 
    ---@field next_screen_to_load integer
    ---@field not_ready_to_start_yet boolean
    ---@field available_mine_entrances integer
    ---@field amount_of_mine_entrances_activated integer
    ---@field screen_blackout TextureRenderingInfo
    ---@field blackout_transparency number
    ---@field start_pressed boolean
    ---@field transition_to_game_started boolean
    ---@field disable_controls boolean
    ---@field flying_things FlyingThing[] @size: 6
    ---@field flying_thing_countdown integer
    ---@field particle_ceilingdust_smoke ParticleEmitterInfo
    ---@field particle_ceilingdust_rubble ParticleEmitterInfo
    ---@field particle_mist ParticleEmitterInfo
    ---@field particle_torchflame_smoke1 ParticleEmitterInfo
    ---@field particle_torchflame_flames1 ParticleEmitterInfo
    ---@field particle_torchflame_smoke2 ParticleEmitterInfo
    ---@field particle_torchflame_flames2 ParticleEmitterInfo
    ---@field particle_torchflame_smoke3 ParticleEmitterInfo
    ---@field particle_torchflame_flames3 ParticleEmitterInfo
    ---@field particle_torchflame_smoke4 ParticleEmitterInfo
    ---@field particle_torchflame_flames4 ParticleEmitterInfo
    ---@field torch_sound SoundMeta[] @size: 4
    ---@field buttons integer[] @size: MAX_PLAYERS

---@class ScreenTeamSelect : Screen
    ---@field player_portrait TextureRenderingInfo
    ---@field scroll_bottom_left TextureRenderingInfo
    ---@field scrollend_bottom_left TextureRenderingInfo
    ---@field four_ropes TextureRenderingInfo
    ---@field gems_above_the_ropes TextureRenderingInfo
    ---@field four_characters TextureRenderingInfo
    ---@field left_arrow TextureRenderingInfo
    ---@field right_arrow TextureRenderingInfo
    ---@field start_panel TextureRenderingInfo
    ---@field go_back_wooden_panel TextureRenderingInfo
    ---@field start_panel_slide number
    ---@field go_back_wooden_panel_slide number
    ---@field pulsating_arrows_timer number
    ---@field selected_player integer
    ---@field buttons integer
    ---@field ready boolean

---@class ScreenCamp : Screen
    ---@field buttons integer

---@class ScreenStateCamp : Screen
    ---@field time_till_reset integer @Delay after player death to reset camp

---@class ScreenLevel : Screen
    ---@field buttons integer

---@class ScreenStateLevel : Screen
    ---@field buttons integer
    ---@field time_till_death_screen integer @Delay after player death to open the death screen

---@class ScreenTransition : Screen
    ---@field woodpanel_pos number
    ---@field stats_scroll_horizontal_posaa number
    ---@field stats_scroll_vertical_pos number
    ---@field level_completed_pos number
    ---@field stats_scroll_unfurl_actualvalue number
    ---@field stats_scroll_unfurl_targetvalue number
    ---@field woodpanel1 TextureRenderingInfo
    ---@field woodpanel2 TextureRenderingInfo
    ---@field woodpanel3 TextureRenderingInfo
    ---@field woodpanel_cutout1 TextureRenderingInfo
    ---@field woodpanel_cutout2 TextureRenderingInfo
    ---@field woodpanel_cutout3 TextureRenderingInfo
    ---@field woodplank TextureRenderingInfo
    ---@field woodpanel_bottomcutout1 TextureRenderingInfo
    ---@field woodpanel_bottomcutout2 TextureRenderingInfo
    ---@field woodpanel_bottomcutout3 TextureRenderingInfo
    ---@field scroll TextureRenderingInfo
    ---@field stats_scroll_top_bottom TextureRenderingInfo
    ---@field killcount_rounded_rect TextureRenderingInfo
    ---@field level_completed_panel TextureRenderingInfo
    ---@field stats_scroll_state_1 integer
    ---@field stats_scroll_state_2 integer
    ---@field hide_press_to_go_next_level boolean
    ---@field mama_tunnel TextureRenderingInfo
    ---@field speechbubble TextureRenderingInfo
    ---@field speechbubble_arrow TextureRenderingInfo
    ---@field mama_tunnel_fade_targetvalue number
    ---@field mama_tunnel_fade_targetvalue number
    ---@field mama_tunnel_text_id STRINGID
    ---@field mama_tunnel_choice_visible boolean
    ---@field mama_tunnel_agree_with_gift boolean
    ---@field mama_tunnel_face_invisible boolean
    ---@field mama_tunnel_face_transparency number
    ---@field mama_tunnel_agree_panel TextureRenderingInfo
    ---@field mama_tunnel_agree_panel_indicator TextureRenderingInfo
    ---@field woodpanel_cutout_big_money1 TextureRenderingInfo
    ---@field woodpanel_cutout_big_money2 TextureRenderingInfo
    ---@field woodpanel_cutout_big_money3 TextureRenderingInfo
    ---@field big_dollar_sign TextureRenderingInfo
    ---@field stats_scroll_unfurl_sequence integer
    ---@field player_stats_scroll_numeric_value integer[] @size: MAX_PLAYERS
    ---@field player_secondary_icon TextureRenderingInfo[] @size: MAX_PLAYERS
    ---@field player_icon TextureRenderingInfo[] @size: MAX_PLAYERS
    ---@field player_secondary_icon_type integer[] @size: MAX_PLAYERS
    ---@field player_icon_index integer[] @size: MAX_PLAYERS
    ---@field hourglasses TextureRenderingInfo
    ---@field small_dollar_signs TextureRenderingInfo
    ---@field this_level_money_color Color
    ---@field buttons integer[] @size: MAX_PLAYERS

---@class ScreenDeath : Screen

---@class ScreenWin : Screen
    ---@field sequence_timer integer
    ---@field frame_timer integer
    ---@field animation_state integer
    ---@field rescuing_ship_entity Entity

---@class ScreenCredits : Screen
    ---@field credits_progression number
    ---@field bg_music_info SoundMeta

---@class ScreenScores : Screen
    ---@field animation_state integer
    ---@field woodpanel1 TextureRenderingInfo
    ---@field woodpanel2 TextureRenderingInfo
    ---@field woodpanel3 TextureRenderingInfo
    ---@field woodpanel_cutout TextureRenderingInfo
    ---@field dollarsign TextureRenderingInfo
    ---@field hourglass TextureRenderingInfo
    ---@field animation_timer integer
    ---@field woodpanel_slidedown_timer number

---@class ScreenConstellation : Screen
    ---@field sequence_state integer
    ---@field animation_timer integer
    ---@field constellation_text_opacity number
    ---@field constellation_text string
    ---@field explosion_and_loop SoundMeta
    ---@field music SoundMeta

---@class ScreenRecap : Screen

---@class ScreenOnlineLoading : Screen
    ---@field ouroboros TextureRenderingInfo
    ---@field ouroboros_angle number

---@class OnlineLobbyScreenPlayer
    ---@field platform_icon integer @16 = PC, 17 = Discord, 18 = Steam, 19 = Xbox, 32 = switch, 48 = PS, 49 = PS again?
    ---@field character integer @0 - Ana Spelunky, 1 - Margaret Tunnel, 2 - Colin Northward, 3 - Roffy D. Sloth.. and so on. Same order as in ENT_TYPE
    ---@field ready boolean
    ---@field searching boolean

---@class ScreenOnlineLobby : Screen
    ---@field screen_panels MenuScreenPanels
    ---@field players OnlineLobbyScreenPlayer[] @size: 4
    ---@field background_image TextureRenderingInfo
    ---@field topleft_woodpanel_esc TextureRenderingInfo
    ---@field topleft_woodpanel_esc_slidein number
    ---@field character_walk_offset number
    ---@field character_facing_left boolean
    ---@field move_direction integer
    ---@field character TextureRenderingInfo
    ---@field player_ready_icon TextureRenderingInfo
    ---@field arrow_left TextureRenderingInfo
    ---@field arrow_right TextureRenderingInfo
    ---@field arrow_left_hor_offset number
    ---@field arrow_right_hor_offset number
    ---@field platform_icon TextureRenderingInfo
    ---@field player_count integer
    ---@field searching_for_players boolean
    ---@field show_code_panel boolean
    ---@field screen_code_input ScreenEnterOnlineCode

---@class ScreenEnterOnlineCode : ScreenCodeInput
    ---@field enter_code_your_code_scroll TextureRenderingInfo
    ---@field enter_code_your_code_scroll_left_handle TextureRenderingInfo
    ---@field enter_code_your_code_scroll_right_handle TextureRenderingInfo

---@class PauseUI
    ---@field menu_slidein_progress number
    ---@field blurred_background TextureRenderingInfo
    ---@field woodpanel_left TextureRenderingInfo
    ---@field woodpanel_middle TextureRenderingInfo
    ---@field woodpanel_right TextureRenderingInfo
    ---@field woodpanel_top TextureRenderingInfo
    ---@field scroll TextureRenderingInfo
    ---@field confirmation_panel TextureRenderingInfo @Prompt background
    ---@field selected_option integer @It's set wh game displays the prompt
    ---@field prompt_visible boolean
    ---@field buttons_actions integer[] @size: MAX_PLAYERS
    ---@field buttons_movement integer[] @size: MAX_PLAYERS
    ---@field visibility integer @0 - Invisible, 1 - Sliding down, 2 - Visible, 3 - Sliding up

---@class JournalUI
    ---@field state integer
    ---@field chapter_shown JOURNALUI_PAGE_SHOWN
    ---@field current_page integer
    ---@field flipping_to_page integer
    ---@field max_page_count integer
    ---@field book_background TextureRenderingInfo
    ---@field arrow_left TextureRenderingInfo
    ---@field arrow_right TextureRenderingInfo
    ---@field entire_book TextureRenderingInfo
    ---@field fade_timer integer
    ---@field page_timer integer
    ---@field opacity number
    ---@field pages JournalPage[] @Stores pages loaded into memory. It's not cleared after the journal is closed or when you go back to the main (menu) page.<br/>Use `:get_type()` to check page type and cast it correctly (see ON.[RENDER_PRE_JOURNAL_PAGE](#ON-RENDER_PRE_JOURNAL_PAGE))

---@class JournalPage
    ---@field background TextureRenderingInfo
    ---@field page_number integer
    ---@field is_right_side_page fun(self): boolean @background.x < 0
    ---@field get_type fun(self): JOURNAL_PAGE_TYPE

---@class JournalPageProgress : JournalPage
    ---@field coffeestain_top TextureRenderingInfo

---@class JournalPageJournalMenu : JournalPage
    ---@field journal_text_info TextRenderingInfo
    ---@field completion_badge TextureRenderingInfo

---@class JournalPageDiscoverable : JournalPage
    ---@field show_main_image boolean
    ---@field title_text_info TextRenderingInfo
    ---@field entry_text_info TextRenderingInfo
    ---@field chapter_title_text_info TextRenderingInfo

---@class JournalPagePlaces : JournalPageDiscoverable
    ---@field main_image TextureRenderingInfo

---@class JournalPagePeople : JournalPageDiscoverable
    ---@field character_background TextureRenderingInfo
    ---@field character_icon TextureRenderingInfo
    ---@field character_drawing TextureRenderingInfo

---@class JournalPageBestiary : JournalPageDiscoverable
    ---@field monster_background TextureRenderingInfo
    ---@field monster_icon TextureRenderingInfo
    ---@field defeated_killedby_black_bars TextureRenderingInfo
    ---@field defeated_text_info TextRenderingInfo
    ---@field defeated_value_text_info TextRenderingInfo
    ---@field killedby_text_info TextRenderingInfo
    ---@field killedby_value_text_info TextRenderingInfo

---@class JournalPageItems : JournalPageDiscoverable
    ---@field item_icon TextureRenderingInfo
    ---@field item_background TextureRenderingInfo

---@class JournalPageTraps : JournalPageDiscoverable
    ---@field trap_icon TextureRenderingInfo
    ---@field trap_background TextureRenderingInfo

---@class JournalPageFeats : JournalPage
    ---@field chapter_title_text_info TextRenderingInfo
    ---@field feat_icons TextureRenderingInfo

---@class JournalPageDeathCause : JournalPage
    ---@field death_cause_text_info TextRenderingInfo

---@class JournalPageDeathMenu : JournalPage
    ---@field game_over_text_info TextRenderingInfo
    ---@field level_text_info TextRenderingInfo
    ---@field level_value_text_info TextRenderingInfo
    ---@field money_text_info TextRenderingInfo
    ---@field money_value_text_info TextRenderingInfo
    ---@field time_text_info TextRenderingInfo
    ---@field time_value_text_info TextRenderingInfo

---@class JournalPagePlayerProfile : JournalPage
    ---@field player_icon TextureRenderingInfo
    ---@field player_icon_id integer
    ---@field player_profile_text_info TextRenderingInfo
    ---@field plays_text_info TextRenderingInfo
    ---@field plays_value_text_info TextRenderingInfo
    ---@field wins_text_info TextRenderingInfo
    ---@field wins_value_text_info TextRenderingInfo
    ---@field deaths_text_info TextRenderingInfo
    ---@field deaths_value_text_info TextRenderingInfo
    ---@field win_pct_text_info TextRenderingInfo
    ---@field win_pct_value_text_info TextRenderingInfo
    ---@field average_score_text_info TextRenderingInfo
    ---@field average_score_value_text_info TextRenderingInfo
    ---@field top_score_text_info TextRenderingInfo
    ---@field top_score_value_text_info TextRenderingInfo
    ---@field deepest_level_text_info TextRenderingInfo
    ---@field deepest_level_value_text_info TextRenderingInfo
    ---@field deadliest_level_text_info TextRenderingInfo
    ---@field deadliest_level_value_text_info TextRenderingInfo
    ---@field average_time_text_info TextRenderingInfo
    ---@field average_time_value_text_info TextRenderingInfo
    ---@field best_time_text_info TextRenderingInfo
    ---@field best_time_value_text_info TextRenderingInfo

---@class JournalPageLastGamePlayed : JournalPage
    ---@field main_image TextureRenderingInfo
    ---@field last_game_played_text_info TextRenderingInfo
    ---@field level_text_info TextRenderingInfo
    ---@field level_value_text_info TextRenderingInfo
    ---@field money_text_info TextRenderingInfo
    ---@field money_value_text_info TextRenderingInfo
    ---@field time_text_info TextRenderingInfo
    ---@field time_value_text_info TextRenderingInfo
    ---@field sticker_count integer
    ---@field stickers TextureRenderingInfo[] @size: 20

---@class ScreenArenaMenu : Screen
    ---@field screen_panels MenuScreenPanels
    ---@field brick_background TextureRenderingInfo
    ---@field blurry_border TextureRenderingInfo
    ---@field blurry_border2 TextureRenderingInfo
    ---@field characters_drawing TextureRenderingInfo
    ---@field info_black_background TextureRenderingInfo
    ---@field main_panel_top_left_corner TextureRenderingInfo
    ---@field main_panel_top TextureRenderingInfo
    ---@field main_panel_top_right_corner TextureRenderingInfo
    ---@field main_panel_left TextureRenderingInfo
    ---@field main_panel_center TextureRenderingInfo
    ---@field main_panel_right TextureRenderingInfo
    ---@field main_panel_bottom_left_corner TextureRenderingInfo
    ---@field main_panel_bottom TextureRenderingInfo
    ---@field main_panel_bottom_right_corner TextureRenderingInfo
    ---@field rules_scroll TextureRenderingInfo
    ---@field black_option_boxes_left TextureRenderingInfo
    ---@field black_option_boxes_center TextureRenderingInfo
    ---@field black_option_boxes_right TextureRenderingInfo
    ---@field gold_option_outline TextureRenderingInfo
    ---@field option_icons TextureRenderingInfo
    ---@field option_left_arrow TextureRenderingInfo
    ---@field option_right_arrow TextureRenderingInfo
    ---@field bottom_left_bricks TextureRenderingInfo
    ---@field top_left_esc_panel TextureRenderingInfo
    ---@field next_panel TextureRenderingInfo
    ---@field center_panels_hor_slide_position number
    ---@field esc_next_panels_slide_timer number
    ---@field main_panel_vertical_scroll_position number
    ---@field selected_option_index integer
    ---@field controls ScreenControls

---@class ScreenArenaStagesSelect : Screen
    ---@field screen_panels MenuScreenPanels
    ---@field buttons integer
    ---@field brick_background TextureRenderingInfo
    ---@field info_black_background TextureRenderingInfo
    ---@field woodenpanel_center TextureRenderingInfo
    ---@field blocky_level_representation TextureRenderingInfo
    ---@field theme_indicator TextureRenderingInfo
    ---@field bricks_bottom_left TextureRenderingInfo
    ---@field grid_background_row_0 TextureRenderingInfo
    ---@field grid_background_row_1 TextureRenderingInfo
    ---@field grid_background_row_2 TextureRenderingInfo
    ---@field grid_background_row_3 TextureRenderingInfo
    ---@field grid_background_row_4 TextureRenderingInfo
    ---@field grid_background_row_5 TextureRenderingInfo
    ---@field grid_background_row_6 TextureRenderingInfo
    ---@field grid_background_row_7 TextureRenderingInfo
    ---@field grid_background_disabled_cross TextureRenderingInfo
    ---@field grid_background_manipulators TextureRenderingInfo
    ---@field unknown21 TextureRenderingInfo
    ---@field grid_disabled_cross TextureRenderingInfo
    ---@field grid_yellow_highlighter TextureRenderingInfo
    ---@field woodpanel_esc TextureRenderingInfo
    ---@field woodpanel_fight TextureRenderingInfo
    ---@field big_player_drawing TextureRenderingInfo
    ---@field players_turn_scroll TextureRenderingInfo
    ---@field players_turn_scroll_handle TextureRenderingInfo
    ---@field grid_player_icon TextureRenderingInfo
    ---@field stages_to_gray_out table<integer, number>
    ---@field panels_slide_from_both_sides number
    ---@field visibility_all_stages number
    ---@field selected_stage_index integer
    ---@field controls ScreenControls

---@class ScreenArenaItems : Screen
    ---@field screen_panels MenuScreenPanels
    ---@field brick_background TextureRenderingInfo
    ---@field black_background_bottom_right TextureRenderingInfo
    ---@field woodpanel_bottom TextureRenderingInfo
    ---@field scroll_bottom TextureRenderingInfo
    ---@field scroll_right_handle_bottom TextureRenderingInfo
    ---@field held_item_crate_on_scroll TextureRenderingInfo
    ---@field held_item_on_scroll TextureRenderingInfo
    ---@field item_background TextureRenderingInfo
    ---@field toggles_background TextureRenderingInfo
    ---@field item_selection_gold_outline TextureRenderingInfo
    ---@field item_icons TextureRenderingInfo
    ---@field item_held_badge TextureRenderingInfo
    ---@field item_equipped_badge TextureRenderingInfo
    ---@field item_off_gray_overlay TextureRenderingInfo
    ---@field esc_woodpanel TextureRenderingInfo
    ---@field items_to_gray_out table<integer, number>
    ---@field center_panels_horizontal_slide_position number
    ---@field esc_panel_slide_timer number
    ---@field selected_item_index integer
    ---@field controls ScreenControls

---@class ScreenArenaIntro : Screen
    ---@field players TextureRenderingInfo
    ---@field background_colors TextureRenderingInfo
    ---@field vertical_lines TextureRenderingInfo
    ---@field vertical_line_electricity_effect TextureRenderingInfo
    ---@field left_scroll TextureRenderingInfo
    ---@field right_scroll TextureRenderingInfo
    ---@field scroll_unfurl number
    ---@field waiting boolean
    ---@field names_opacity number
    ---@field line_electricity_effect_timer number
    ---@field state integer
    ---@field countdown integer
    ---@field particles ParticleEmitterInfo[] @size: 9

---@class ScreenArenaLevel : Screen
    ---@field get_ready TextureRenderingInfo
    ---@field get_ready_gray_background TextureRenderingInfo
    ---@field get_ready_outline TextureRenderingInfo
    ---@field particles ParticleEmitterInfo[] @size: 11

---@class ScreenArenaScoreLavaBubble
    ---@field x number
    ---@field y number
    ---@field timer1 integer
    ---@field timer2 integer
    ---@field visible boolean

---@class ScreenArenaScore : Screen
    ---@field woodpanel_slide number
    ---@field scroll_unfurl number
    ---@field woodpanel TextureRenderingInfo
    ---@field woodpanel_left_scroll TextureRenderingInfo
    ---@field woodpanel_right_scroll TextureRenderingInfo
    ---@field text_id_1 STRINGID
    ---@field text_id_2 STRINGID
    ---@field woodpanel_visible boolean
    ---@field woodpanel_slide_toggle boolean
    ---@field animation_sequence integer
    ---@field background TextureRenderingInfo
    ---@field ok_panel TextureRenderingInfo
    ---@field ready_panel TextureRenderingInfo
    ---@field ready_speechbubble_indicator TextureRenderingInfo
    ---@field pillars TextureRenderingInfo
    ---@field bottom_lava TextureRenderingInfo
    ---@field players TextureRenderingInfo
    ---@field player_shadows TextureRenderingInfo
    ---@field score_counter TextureRenderingInfo
    ---@field lava_bubbles TextureRenderingInfo
    ---@field player_won boolean[] @size: MAX_PLAYERS
    ---@field victory_jump_y_pos number
    ---@field victory_jump_velocity number
    ---@field squash_and_celebrate boolean
    ---@field player_ready boolean[] @size: MAX_PLAYERS
    ---@field next_transition_timer integer
    ---@field player_bottom_pillar_offset number[] @size: MAX_PLAYERS
    ---@field player_crushing_pillar_height number[] @size: MAX_PLAYERS
    ---@field player_create_giblets boolean[] @size: MAX_PLAYERS
    ---@field next_sidepanel_slidein_timer number
    ---@field particles ParticleEmitterInfo[] @size: 13
    ---@field lava_bubbles_positions ScreenArenaScoreLavaBubble[] @size: 15

---@class UdpServer

---@class LogicList
    ---@field tutorial LogicTutorial @Handles dropping of the torch and rope in intro routine (first time play)
    ---@field ouroboros LogicOuroboros
    ---@field basecamp_speedrun LogicBasecampSpeedrun @Keep track of time, player position passing official
    ---@field ghost_trigger Logic @It's absence is the only reason why ghost doesn't spawn at boss levels or CO
    ---@field ghost_toast_trigger LogicGhostToast
    ---@field tun_aggro Logic @Spawns tun at the door at 30s mark
    ---@field diceshop LogicDiceShop
    ---@field tun_pre_challenge LogicTunPreChallenge
    ---@field tun_moon_challenge LogicMoonChallenge
    ---@field tun_star_challenge LogicStarChallenge
    ---@field tun_sun_challenge LogicSunChallenge
    ---@field magmaman_spawn LogicMagmamanSpawn
    ---@field water_bubbles LogicUnderwaterBubbles @Only the bubbles that spawn from the floor (no border tiles, checks decoration flag), also spawn droplets falling from ceiling<br/>Even without it, entities moving in water still spawn bubbles
    ---@field olmec_cutscene LogicOlmecCutscene
    ---@field tiamat_cutscene LogicTiamatCutscene
    ---@field apep_spawner LogicApepTrigger @Triggers and spawns Apep only in rooms set as ROOM_TEMPLATE.APEP
    ---@field city_of_gold_ankh_sacrifice LogicCOGAnkhSacrifice @All it does is it runs transition to Duat after time delay (sets the state next theme etc. and state.items for proper player respawn)
    ---@field duat_bosses_spawner Logic
    ---@field bubbler LogicTiamatBubbles @Spawn rising bubbles at Tiamat (position hardcoded)
    ---@field tusk_pleasure_palace LogicTuskPleasurePalace @Triggers aggro on everyone when non-high roller enters door
    ---@field discovery_info Logic @black market, vlad, wet fur discovery, logic shows the toast
    ---@field black_market Logic @Changes the camera bounds when you reach black market
    ---@field jellyfish_trigger Logic
    ---@field arena_1 LogicArena1 @Handles create spawns and more, is cleared as soon as the winner is decided (on last player alive)
    ---@field arena_2 Logic
    ---@field arena_3 Logic @Handles time end death
    ---@field arena_alien_blast LogicArenaAlienBlast
    ---@field arena_loose_bombs LogicArenaLooseBombs
    ---@field start_logic fun(self, idx: LOGIC): Logic @This only properly constructs the base class<br/>you may still need to initialise the parameters correctly
local LogicList = nil
---@param idx LOGIC
---@return nil
function LogicList:stop_logic(idx) end
---@param log Logic
---@return nil
function LogicList:stop_logic(log) end

---@class Logic
    ---@field logic_index LOGIC

---@class LogicTutorial : Logic
    ---@field pet_tutorial Entity
    ---@field timer integer

---@class LogicOuroboros : Logic
    ---@field sound SoundMeta
    ---@field timer integer

---@class LogicBasecampSpeedrun : Logic
    ---@field administrator integer @entity uid of the character that keeps the time
    ---@field crate integer @entity uid. you must break this crate for the run to be valid, otherwise you're cheating

---@class LogicGhostToast : Logic
    ---@field toast_timer integer @ default 90

---@class LogicDiceShop : Logic
    ---@field boss_uid integer
    ---@field boss_type ENT_TYPE
    ---@field bet_machine integer @entity uid
    ---@field die1 integer @entity uid
    ---@field die2 integer @entity uid
    ---@field die_1_value integer
    ---@field die_2_value integer
    ---@field prize_dispenser integer @entity uid
    ---@field prize integer @entity uid
    ---@field forcefield integer @entity uid
    ---@field bet_active boolean
    ---@field forcefield_deactivated boolean
    ---@field result_announcement_timer integer @the time the boss waits after your second die throw to announce the results
    ---@field won_prizes_count integer
    ---@field balance integer @cash balance of all the games

---@class LogicTunPreChallenge : Logic
    ---@field tun_uid integer

---@class LogicChallenge : Logic
    ---@field floor_challenge_entrance_uid integer
    ---@field floor_challenge_waitroom_uid integer
    ---@field challenge_active boolean
    ---@field forcefield_countdown integer

---@class LogicMoonChallenge : LogicChallenge
    ---@field mattock_uid integer @entity uid

---@class LogicStarChallenge : LogicChallenge
    ---@field torches Entity[]
    ---@field start_countdown integer

---@class LogicSunChallenge : LogicChallenge
    ---@field start_countdown integer

---@class LogicMagmamanSpawn : Logic
    ---@field magmaman_positions MagmamanSpawnPosition[]
local LogicMagmamanSpawn = nil
---@param x integer
---@param y integer
---@return nil
function LogicMagmamanSpawn:add_spawn(x, y) end
---@param ms MagmamanSpawnPosition
---@return nil
function LogicMagmamanSpawn:add_spawn(ms) end
---@param x integer
---@param y integer
---@return nil
function LogicMagmamanSpawn:remove_spawn(x, y) end
---@param ms MagmamanSpawnPosition
---@return nil
function LogicMagmamanSpawn:remove_spawn(ms) end

---@class LogicUnderwaterBubbles : Logic
    ---@field gravity_direction number @1.0 = normal, -1.0 = inversed, other values have undefined behavior<br/>this value basically have to be the same as return from `ThemeInfo:get_liquid_gravity()`
    ---@field droplets_spawn_chance integer @It's inverse chance, so the lower the number the higher the chance, values below 10 may crash the game
    ---@field droplets_enabled boolean @Enable/disable spawn of ENT_TYPE.FX_WATER_DROP from ceiling (or ground if liquid gravity is inverse)

---@class LogicOlmecCutscene : Logic
    ---@field fx_olmecpart_large Entity
    ---@field olmec Entity
    ---@field player Entity
    ---@field cinematic_anchor Entity
    ---@field timer integer

---@class LogicTiamatCutscene : Logic
    ---@field tiamat Entity
    ---@field player Entity
    ---@field cinematic_anchor Entity
    ---@field timer integer

---@class LogicApepTrigger : Logic
    ---@field spawn_cooldown integer
    ---@field cooling_down boolean
    ---@field apep_journal_entry_logged boolean

---@class LogicCOGAnkhSacrifice : Logic
    ---@field timer integer

---@class LogicTiamatBubbles : Logic
    ---@field bubble_spawn_timer integer

---@class LogicTuskPleasurePalace : Logic
    ---@field locked_door integer

---@class LogicArena1 : Logic
    ---@field crate_spawn_timer integer

---@class LogicArenaAlienBlast : Logic
    ---@field timer integer

---@class LogicArenaLooseBombs : Logic
    ---@field timer integer

---@class MagmamanSpawnPosition
    ---@field x integer
    ---@field y integer
    ---@field timer integer

---@class Overlunky
    ---@field options table<string, any> @Current Overlunky options. Read only.
    ---@field set_options table<string, any> @Write [some select options](https://github.com/search?q=repo%3Aspelunky-fyi%2Foverlunky+legal_options+language%3AC%2B%2B&type=code&l=C%2B%2B) here to change Overlunky options.
    ---@field keys table<string, KEY> @Current Overlunky key bindings. Read only. You can use this to bind some custom feature to the same unknown key as currently bound by the user.
    ---@field ignore_keys unordered_Array<string> @Disable some key bindings in Overlunky, whatever key they are actually bound to. Remember this might not be bound to the default any more, so only use this if you also plan on overriding the current keybinding, or just need to disable some feature and don't care what key it is bound on.
    ---@field ignore_keycodes unordered_Array<KEY> @Disable keys that may or may not be used by Overlunky. You should probably write the keycodes you need here just in case if you think using OL will interfere with your keybinds.
    ---@field ignore_features unordered_Array<string> @TODO: Disable Overlunky features. Doesn't do anything yet.
    ---@field selected_uid integer @Currently selected uid in the entity picker or -1 if nothing is selected.
    ---@field selected_uids integer[] @Currently selected uids in the entity finder.
    ---@field hovered_uid integer @Currently hovered entity uid or -1 if nothing is hovered.
    ---@field set_selected_uid integer? @Set currently selected uid in the entity picker or -1 to clear selection.
    ---@field set_selected_uids integer[] @size: ? @Set currently selected uids in the entity finder.

---@class PauseAPI
    ---@field pause PAUSE_TYPE @Current pause state bitmask. Use custom PAUSE_TYPE.PRE_✱ (or multiple) to freeze the game at the specified callbacks automatically. Checked after the matching ON update callbacks, so can be set on the same callback you want to block at the latest. Vanilla PAUSE flags will be forwarded to state.pause, but use of vanilla PAUSE flags is discouraged and might not work with other PauseAPI features.
    ---@field pause_type PAUSE_TYPE @Pause mask to toggle when using the PauseAPI methods to set or get pause state.
    ---@field pause_trigger PAUSE_TRIGGER @Bitmask for conditions when the current `pause_type` should be automatically enabled in `pause`, can have multiple conditions.
    ---@field pause_screen PAUSE_SCREEN @Bitmask to only enable PAUSE_TRIGGER.SCREEN during specific SCREEN, or any screen when NONE.
    ---@field unpause_trigger PAUSE_TRIGGER @Bitmask for conditions when the current `pause_type` should be automatically disabled in `pause`, can have multiple conditions.
    ---@field unpause_screen PAUSE_SCREEN @Bitmask to only enable PAUSE_TRIGGER.SCREEN during specific SCREEN, or any screen when NONE.
    ---@field ignore_screen PAUSE_SCREEN @Bitmask for game SCREEN where the PRE_✱ pause types are ignored, even though enabled in `pause`. Can also use the special cases [FADE, EXIT] to unfreeze temporarily during fades (or other screen transitions where player input is probably impossible) or the level exit walk of shame.
    ---@field ignore_screen_trigger PAUSE_SCREEN @Bitmask for game SCREEN where the triggers are ignored.
    ---@field skip boolean @Set to true to unfreeze the game for one update cycle. Sets back to false after ON.POST_GAME_LOOP, so it can be used to check if current frame is a frame advance frame.
    ---@field update_camera boolean @Set to true to enable normal camera movement when the game is paused or frozen on a callback by PauseAPI.
    ---@field blocked boolean @Is true when PauseAPI is freezing the game.
    ---@field skip_fade boolean @Set to true to skip all fade transitions, forcing fade_timer and fade_value to 0 on every update.
    ---@field last_instance boolean @Set to true to run pause logic and triggers only in the last API instance in the chain (Playlunky) when multiple instances of the API (Overlunky and Playlunky) are injected.
    ---@field last_trigger_frame integer @Global frame stamp when one of the triggers was last triggered, used to prevent running them again on the same frame on unpause.
    ---@field last_fade_timer integer @Fade timer stamp when fade triggers were last checked.
    ---@field frame_advance fun(self): nil @Sets skip
    ---@field get_pause fun(self): PAUSE_TYPE @Get the current pause flags
    ---@field set_pause fun(self, flags: PAUSE_TYPE): nil @Set the current pause flags
    ---@field set_paused fun(self, true: boolean enable =): boolean @Enable/disable the current pause_type flags in pause state
    ---@field paused fun(self): boolean @Is the game currently paused and that pause state matches any of the current the pause_type
    ---@field toggle fun(self): boolean @Toggles pause state
    ---@field loading fun(self): boolean @Is the game currently loading and PAUSE_SCREEN.LOADING would be triggered, based on state.loading and some arbitrary checks.
    ---@field modifiers_down integer @Bitmask of modifier KEYs that are currently held
    ---@field modifiers_block integer @Bitmask of modifier KEYs that will block all game input
    ---@field modifiers_clear_input boolean @Enable to clear affected input when modifiers are held, disable to ignore all input events, i.e. keep held button state as it was before pressing the modifier key

---@class SharedIO
    ---@field wantkeyboard boolean?
    ---@field wantmouse boolean?

---@class Bucket
    ---@field data table<string, any> @You can store arbitrary simple values here in Playlunky to be read in on Overlunky script for example.
    ---@field overlunky Overlunky @Access Overlunky options here, nil if Overlunky is not loaded.
    ---@field pause PauseAPI @PauseAPI is used by Overlunky and can be used to control the Overlunky pause options from scripts. Can be accessed from the global `pause` more easily.
    ---@field io SharedIO @Shared part of ImGuiIO to block keyboard/mouse input across API instances.
    ---@field count integer @Number of API instances present
    ---@field custom_strings table<STRINGID, string> @Holds all the custom strings added thru [add_string](#add_string), read only 

---@class Color
    ---@field r number
    ---@field g number
    ---@field b number
    ---@field a number
    ---@field get_rgba fun(self): integer, integer, integer, integer @Returns RGBA colors in 0..255 range
    ---@field set_rgba fun(self, red: integer, green: integer, blue: integer, alpha: integer): Color @Changes color based on given RGBA colors in 0..255 range
    ---@field get_ucolor fun(self): uColor @Returns the `uColor` used in `GuiDrawContext` drawing functions
    ---@field set_ucolor fun(self, color: uColor): Color @Changes color based on given uColor
    ---@field set fun(self, other: Color): Color @Copies the values of different Color to this one

end
--## Static class functions

SaveState = nil
---Get the pre-allocated by the game save slot 1-4. Call as `SaveState.get(slot)`
---@param save_slot integer
---@return SaveState
function SaveState:get(save_slot) end

Color = nil
---@return Color
function Color:white() end
---@return Color
function Color:silver() end
---@return Color
function Color:gray() end
---@return Color
function Color:black() end
---@return Color
function Color:red() end
---@return Color
function Color:maroon() end
---@return Color
function Color:yellow() end
---@return Color
function Color:olive() end
---@return Color
function Color:lime() end
---@return Color
function Color:green() end
---@return Color
function Color:aqua() end
---@return Color
function Color:teal() end
---@return Color
function Color:blue() end
---@return Color
function Color:navy() end
---@return Color
function Color:fuchsia() end
---@return Color
function Color:purple() end

--## Constructors
---Create a new temporary SaveState/clone of the main level state. Unlike save_state slots that are preallocated by the game anyway, these will use 32MiB a pop and aren't freed automatically, so make sure to clear them or reuse the same one to save memory. The garbage collector will eventually clear the SaveStates you don't have a handle to any more though.
---@return SaveState
function SaveState:new() end

EntityDB = nil
---@param other EntityDB
---@return EntityDB
function EntityDB:new(other) end
---@param other ENT_TYPE
---@return EntityDB
function EntityDB:new(other) end

ParticleDB = nil
---@param other ParticleDB
---@return ParticleDB
function ParticleDB:new(other) end
---@param particle_id PARTICLEEMITTER
---@return ParticleDB
function ParticleDB:new(particle_id) end

CustomTheme = nil
---Create a new theme with an id and base theme, overriding defaults. Check [theme functions that are default enabled here](https://github.com/spelunky-fyi/overlunky/blob/main/src/game_api/script/usertypes/level_lua.cpp).
---@param theme_id_ integer
---@param base_theme_ integer
---@param defaults boolean
---@return CustomTheme
function CustomTheme:new(theme_id_, base_theme_, defaults) end
---Create a new theme with defaults.
---@param theme_id_ integer
---@param base_theme_ integer
---@return CustomTheme
function CustomTheme:new(theme_id_, base_theme_) end
---Create a new theme with base dwelling and id 100.
---@return CustomTheme
function CustomTheme:new() end

Vec2 = nil
---@return Vec2
function Vec2:new() end
---@param other Vec2
---@return Vec2
function Vec2:new(other) end
---@param x_ number
---@param y_ number
---@return Vec2
function Vec2:new(x_, y_) end

AABB = nil
---Create a new axis aligned bounding box - defaults to all zeroes
---@return AABB
function AABB:new() end
---Copy an axis aligned bounding box
---@param other AABB
---@return AABB
function AABB:new(other) end
---@param top_left Vec2
---@param bottom_right Vec2
---@return AABB
function AABB:new(top_left, bottom_right) end
---Create a new axis aligned bounding box by specifying its values
---@param left_ number
---@param top_ number
---@param right_ number
---@param bottom_ number
---@return AABB
function AABB:new(left_, top_, right_, bottom_) end

Triangle = nil
---@return Triangle
function Triangle:new() end
---@param other Triangle
---@return Triangle
function Triangle:new(other) end
---@param _a Vec2
---@param _b Vec2
---@param _c Vec2
---@return Triangle
function Triangle:new(_a, _b, _c) end
---@param ax number
---@param ay number
---@param bx number
---@param by number
---@param cx number
---@param cy number
---@return Triangle
function Triangle:new(ax, ay, bx, by, cx, cy) end

Quad = nil
---@return Quad
function Quad:new() end
---@param other Quad
---@return Quad
function Quad:new(other) end
---@param bottom_left_ Vec2
---@param bottom_right_ Vec2
---@param top_right_ Vec2
---@param top_left_ Vec2
---@return Quad
function Quad:new(bottom_left_, bottom_right_, top_right_, top_left_) end
---@param _bottom_left_x number
---@param _bottom_left_y number
---@param _bottom_right_x number
---@param _bottom_right_y number
---@param _top_right_x number
---@param _top_right_y number
---@param _top_left_x number
---@param _top_left_y number
---@return Quad
function Quad:new(_bottom_left_x, _bottom_left_y, _bottom_right_x, _bottom_right_y, _top_right_x, _top_right_y, _top_left_x, _top_left_y) end
---@param aabb AABB
---@return Quad
function Quad:new(aabb) end

MagmamanSpawnPosition = nil
---@param x_ integer
---@param y_ integer
---@return MagmamanSpawnPosition
function MagmamanSpawnPosition:new(x_, y_) end
---Create a new color - defaults to black
---@return Color
function Color:new() end
---@param other Color
---@return Color
function Color:new(other) end
---@param color Color
---@return Color
function Color:new(color) end
---Create a new color by specifying its values
---@param r_ number
---@param g_ number
---@param b_ number
---@param a_ number
---@return Color
function Color:new(r_, g_, b_, a_) end
---Create a color from an array of 4 floats
---@param (c number
---@return Color
function Color:new((c) end
---@param color_name string
---@param alpha integer?
---@return Color
function Color:new(color_name, alpha) end

--## Enums


BEG = {
  ALTAR_DESTROYED = 1,
  BOMBBAG_THROWN = 3,
  QUEST_NOT_STARTED = 0,
  SPAWNED_WITH_BOMBBAG = 2,
  SPAWNED_WITH_TRUECROWN = 4,
  TRUECROWN_THROWN = 5
}
---@alias BEG integer
BUTTON = {
  BOMB = 4,
  DOOR = 32,
  JUMP = 1,
  ROPE = 8,
  RUN = 16,
  WHIP = 2
}
---@alias BUTTON integer
CAUSE_OF_DEATH = {
  DEATH = 0,
  ENTITY = 1,
  LONG_FALL = 2,
  MISSED = 4,
  POISONED = 5,
  STILL_FALLING = 3
}
---@alias CAUSE_OF_DEATH integer
CHAR_STATE = {
  ATTACKING = 12,
  CLIMBING = 6,
  DROPPING = 10,
  DUCKING = 5,
  DYING = 22,
  ENTERING = 19,
  EXITING = 21,
  FALLING = 9,
  FLAILING = 0,
  HANGING = 4,
  JUMPING = 8,
  LOADING = 20,
  PUSHING = 7,
  SITTING = 2,
  STANDING = 1,
  STUNNED = 18,
  THROWING = 17
}
---@alias CHAR_STATE integer
CONST = {
  ENGINE_FPS = 60,
  MAX_PLAYERS = 4,
  MAX_TILES_HORIZ = 86,
  MAX_TILES_VERT = 126,
  NOF_DRAW_DEPTHS = 53,
  ROOM_HEIGHT = 8,
  ROOM_WIDTH = 10
}
---@alias CONST integer
CORNER_FINISH = {
  ADAPTIVE = 1,
  CUT = 0,
  NONE = 3,
  REAL = 2
}
---@alias CORNER_FINISH integer
COSUBTHEME = {
  DWELLING = 0,
  ICE_CAVES = 5,
  JUNGLE = 1,
  NEO_BABYLON = 6,
  NONE = -1,
  RESET = -1,
  SUNKEN_CITY = 7,
  TEMPLE = 4,
  TIDE_POOL = 3,
  VOLCANA = 2
}
---@alias COSUBTHEME integer
DAMAGE_TYPE = {
  ARROW = 8,
  CURSE = 256,
  EXPLOSION = 8192,
  FALL = 512,
  FIRE = 32,
  GENERIC = 1,
  ICE_BREAK = 2048,
  LASER = 1024,
  POISON = 64,
  POISON_TICK = 128,
  STOMP = 4096,
  SWORD = 16,
  THROW = 4,
  VOODOO = 16384,
  WHIP = 2
}
---@alias DAMAGE_TYPE integer
DRAW_LAYER = {
  BACKGROUND = 0,
  FOREGROUND = 1,
  WINDOW = 2
}
---@alias DRAW_LAYER integer
DROP = {
  ALIENQUEEN_ALIENBLAST = 187,
  ALIENQUEEN_ALIENBLAST_RE = 189,
  ALIENQUEEN_ALIENBLAST_RI = 188,
  ALTAR_DICE_CLIMBINGGLOVES = 0,
  ALTAR_DICE_COOKEDTURKEY = 1,
  ALTAR_DICE_DIAMOND = 2,
  ALTAR_DICE_HIREDHAND = 10,
  ALTAR_DICE_MACHETE = 3,
  ALTAR_DICE_ROPEPILE = 4,
  ALTAR_DICE_SNAKE = 6,
  ALTAR_DICE_SPECTACLES = 5,
  ALTAR_DICE_TELEPACK = 7,
  ALTAR_DICE_VAMPIRE = 8,
  ALTAR_DICE_WEBGUN = 9,
  ALTAR_GIFT_BOMBBAG = 19,
  ALTAR_GIFT_CAPE = 20,
  ALTAR_HIREDHAND_SHOTGUN = 18,
  ALTAR_IDOL_GOLDEN_MONKEY = 11,
  ALTAR_KAPALA = 21,
  ALTAR_PRESENT_EGGPLANT = 12,
  ALTAR_ROCK_WOODENARROW = 13,
  ALTAR_ROYAL_JELLY = 22,
  ALTAR_USHABTI_CAVEMAN = 14,
  ALTAR_USHABTI_HIREDHAND = 17,
  ALTAR_USHABTI_TURKEY = 15,
  ALTAR_USHABTI_VAMPIRE = 16,
  ANUBIS2_ANUBIS_COFFIN = 162,
  ANUBIS2_JETPACK = 23,
  ANUBIS2_SPECIALSHOT_R = 192,
  ANUBIS_COFFIN_SORCERESS = 111,
  ANUBIS_COFFIN_VAMPIRE = 110,
  ANUBIS_COFFIN_WITCHDOCTOR = 112,
  ANUBIS_SCEPTER = 24,
  ANUBIS_SPECIALSHOT_R = 191,
  ARROWTRAP_WOODENARROW = 159,
  AXOLOTL_BUBBLE = 180,
  BEG_BOMBBAG = 25,
  BEG_TELEPACK = 27,
  BEG_TRUECROWN = 26,
  BONEBLOCK_BONES = 197,
  BONEBLOCK_SKELETON = 195,
  BONEBLOCK_SKULL = 196,
  BONEPILE_SKELETONKEY = 28,
  BONEPILE_SKULL = 29,
  CANDLE_NUGGET = 150,
  CATMUMMY_CURSINGCLOUD = 190,
  CATMUMMY_DIAMOND = 122,
  CHALLENGESTAR_CLONEGUN = 156,
  CHALLENGESTAR_ELIXIR = 157,
  CHALLENGESUN_PLAYERBAG = 158,
  CHEST_BOMB = 141,
  CHEST_EMERALD = 136,
  CHEST_LEPRECHAUN = 140,
  CHEST_RUBY = 138,
  CHEST_SAPPHIRE = 137,
  CHEST_SMALLEMERALD = 135,
  CHEST_SMALLRUBY = 139,
  CLONEGUN_SHOT = 172,
  COBRA_ACIDSPIT = 183,
  COFFIN_SKULL = 153,
  COOKEDTURKEY_HEALTH = 210,
  COOKFIRE_CAVEMAN_1 = 193,
  COOKFIRE_CAVEMAN_2 = 194,
  COOKFIRE_TORCH = 151,
  CROCMAN_TELEPACK = 30,
  CROCMAN_TELEPORTER = 31,
  CRUSHTRAP_NUGGET = 132,
  CUTSCENE_GOLDCOIN = 148,
  DOOR_COG_SCEPTER = 208,
  DUATALTAR_BOMBBAG = 128,
  DUATALTAR_BOMBBOX = 129,
  DUATALTAR_COOKEDTURKEY = 130,
  EGGSAC_GRUB_1 = 115,
  EGGSAC_GRUB_2 = 116,
  EGGSAC_GRUB_3 = 117,
  EMBED_NUGGET = 134,
  FACTORY_GENERATOR_SCRAP = 94,
  FIREBUG_FIREBALL = 182,
  FLOORSTYLEDCOG_NUGGET = 131,
  FLOOR_DIAMOND = 204,
  FLOOR_EMBED_GOLD = 206,
  FLOOR_EMBED_GOLD_BIG = 207,
  FLOOR_EMERALD = 202,
  FLOOR_RUBY = 205,
  FLOOR_SAPPHIRE = 203,
  FREEZERAY_SHOT = 171,
  GHIST_GOLDCOIN = 64,
  GHOSTJAR_DIAMOND = 32,
  GHOST_DIAMOND = 33,
  GIANTFOOD_HEALTH = 211,
  GIANTFROG_FROG = 113,
  GIANTFROG_TADPOLE = 114,
  GIANTSPIDER_PASTE = 34,
  GIANTSPIDER_WEBSHOT = 164,
  GIANTSPIDER_WEB_LEFT = 198,
  GIANTSPIDER_WEB_RIGHT = 199,
  GOLDENMONKEY_NUGGET = 38,
  GOLDENMONKEY_SMALLEMERALD = 35,
  GOLDENMONKEY_SMALLRUBY = 37,
  GOLDENMONKEY_SMALLSAPPHIRE = 36,
  HANGINGSPIDER_WEBGUN = 39,
  HERMITCRAB_ACIDBUBBLE = 185,
  HUMPHEAD_HIREDHAND = 123,
  HUNDUN_FIREBALL = 181,
  ICECAVE_BOULDER = 40,
  ICE_ALIVE_EMBEDDED_ON_ICE = 201,
  IMP_LAVAPOT = 200,
  JIANGSHIASSASSIN_SPIKESHOES = 41,
  JIANGSHI_SPRINGSHOES = 42,
  KAPALA_HEALTH = 213,
  KINGU_FEMALE_JIANGSHI = 46,
  KINGU_JIANGSHI = 45,
  KINGU_OCTOPUS = 44,
  KINGU_TABLETOFDESTINY = 43,
  LAMASSU_DIAMOND = 127,
  LAMASSU_EMERALD = 126,
  LAMASSU_LASERSHOT = 175,
  LAMASSU_RUBY = 125,
  LAMASSU_SAPPHIRE = 124,
  LASERTRAP_SHOT = 161,
  LAVAMANDER_RUBY = 121,
  LAVAPOT_MAGMAMAN = 120,
  LEPRECHAUN_CLOVER = 47,
  LOCKEDCHEST_UDJATEYE = 145,
  MADAME_TUSK_KEY = 109,
  MATTOCK_BROKENMATTOCK = 48,
  MOLE_MATTOCK = 49,
  MOSQUITO_HOVERPACK = 50,
  MOTHERSTATUE_HEALTH = 209,
  MUMMY_DIAMOND = 51,
  MUMMY_FLY = 163,
  NECROMANCER_RUBY = 52,
  OCTOPUS_INKSPIT = 184,
  OLMEC_BOMB = 165,
  OLMEC_CAVEMAN_1 = 53,
  OLMEC_CAVEMAN_2 = 54,
  OLMEC_CAVEMAN_3 = 55,
  OLMEC_SISTERS_BOMBBOX = 155,
  OLMEC_SISTERS_ROPEPILE = 154,
  OLMEC_UFO = 166,
  OSIRIS_EMERALDS = 56,
  OSIRIS_PORTAL = 58,
  OSIRIS_TABLETOFDESTINY = 57,
  PANGXIE_ACIDBUBBLE = 186,
  PANGXIE_WOODENSHIELD = 59,
  PLASMACANNON_SHOT = 170,
  POISONEDARROWTRAP_WOODENARROW = 160,
  POTOFGOLD_GOLDCOIN = 147,
  QILIN_FIREBALL = 179,
  QUEENBEE_ROYALJELLY = 60,
  QUILLBACK_BOMBBAG = 118,
  QUILLBACK_COOKEDTURKEY = 119,
  REDLANTERN_SMALLNUGGET = 149,
  ROBOT_METALSHIELD = 61,
  ROCKDOG_FIREBALL = 178,
  ROYALJELLY_HEALTH = 212,
  SACRIFICE_EGGPLANT = 105,
  SACRIFICE_IDOL = 102,
  SACRIFICE_PRESENT = 103,
  SACRIFICE_ROCK = 104,
  SCEPTER_ANUBISSPECIALSHOT = 168,
  SCEPTER_PLAYERSHOT = 169,
  SCRAP_ALIEN = 100,
  SCRAP_COBRA = 98,
  SCRAP_SCORPION = 99,
  SCRAP_SNAKE = 97,
  SCRAP_SPIDER = 96,
  SHOPKEEPER_GENERATOR_1 = 95,
  SHOPKEEPER_GOLDBAR = 63,
  SHOPKEEPER_GOLDCOIN = 62,
  SHOTGUN_BULLET = 173,
  SKELETON_SKELETONKEY = 65,
  SKELETON_SKULL = 66,
  SKULLDROPTRAP_SKULL = 152,
  SLIDINGWALL_NUGGET = 133,
  SORCERESS_DAGGERSHOT = 176,
  SORCERESS_RUBY = 67,
  SPARROW_ROPEPILE = 68,
  SPARROW_SKELETONKEY = 69,
  TIAMAT_BAT = 70,
  TIAMAT_BEE = 71,
  TIAMAT_CAVEMAN = 72,
  TIAMAT_COBRA = 73,
  TIAMAT_HERMITCRAB = 74,
  TIAMAT_MONKEY = 75,
  TIAMAT_MOSQUITO = 76,
  TIAMAT_OCTOPUS = 77,
  TIAMAT_OLMITE = 78,
  TIAMAT_SCORPION = 79,
  TIAMAT_SHOT = 80,
  TIAMAT_SNAKE = 81,
  TIAMAT_TIAMATSHOT = 177,
  TIAMAT_UFO = 82,
  TIAMAT_YETI = 83,
  TORCH_SMALLNUGGET = 84,
  TURKEY_COOKEDTURKEY = 85,
  UFO_ALIEN = 101,
  UFO_LASERSHOT = 174,
  UFO_PARACHUTE = 86,
  USHABTI_QILIN = 146,
  VAMPIRE_CAPE = 87,
  VAN_HORSING_COMPASS = 88,
  VAN_HORSING_DIAMOND = 89,
  VAULTCHEST_DIAMOND = 143,
  VAULTCHEST_EMERALD = 142,
  VAULTCHEST_RUBY = 144,
  VLAD_VLADSCAPE = 90,
  YAMA_EGGPLANTCROWN = 106,
  YAMA_GIANTFOOD = 107,
  YANG_KEY = 108,
  YETIKING_FREEZERAY = 91,
  YETIKING_ICESPIRE = 167,
  YETIQUEEN_POWERPACK = 92,
  YETI_PITCHERSMITT = 93
}
---@alias DROP integer
DROPCHANCE = {
  BONEBLOCK_SKELETONKEY = 0,
  CROCMAN_TELEPACK = 1,
  HANGINGSPIDER_WEBGUN = 2,
  JIANGSHIASSASSIN_SPIKESHOES = 3,
  JIANGSHI_SPRINGSHOES = 4,
  MOLE_MATTOCK = 5,
  MOSQUITO_HOVERPACK = 6,
  ROBOT_METALSHIELD = 7,
  SKELETON_SKELETONKEY = 8,
  UFO_PARACHUTE = 9,
  YETI_PITCHERSMITT = 10
}
---@alias DROPCHANCE integer
DYNAMIC_TEXTURE = {
  BACKGROUND = -4,
  BACKGROUND_DECORATION = -8,
  COFFIN = -10,
  DOOR = -6,
  DOOR_LAYER = -7,
  FLOOR = -5,
  INVISIBLE = -2,
  KALI_STATUE = -9
}
---@alias DYNAMIC_TEXTURE integer
ENTITY_OVERRIDE = {
  ACCELERATION = 100,
  ACTIVATE = 25,
  APPLY_FRICTION = 84,
  APPLY_METADATA = 28,
  APPLY_MOVEMENT = 71,
  APPLY_VELOCITY = 42,
  ATTACK = 46,
  BLOOD_COLLISION = 93,
  BODY_DESTRUCTION = 92,
  BREAK_BLOCK = 86,
  CALCULATE_JUMP_VELOCITY = 40,
  CAN_BE_PICKED_UP_BY = 68,
  CAN_BE_PUSHED = 10,
  CAN_BREAK_BLOCK = 85,
  CAN_ENTER = 46,
  CAN_JUMP = 37,
  CHECK_OUT_OF_BOUNDS = 58,
  CLONED_TO = 63,
  COLLECT_TREASURE = 70,
  CREATE_RENDERING_INFO = 1,
  CRUSH = 90,
  DAMAGE = 48,
  DESTROY = 5,
  DROP = 69,
  DTOR = 0,
  ENTER = 42,
  ENTER_ATTEMPT = 40,
  EQUIP = 93,
  FALL = 83,
  FLIP = 16,
  FLOOR_UPDATE = 38,
  FREEZE = 52,
  FRICTION = 19,
  GENERATE_DAMAGE_PARTICLES = 8,
  GENERATE_LANDING_EFFECTS = 82,
  GET_DAMAGE = 43,
  GET_DAMAGE_SOUND = 50,
  GET_HELD_ENTITY = 22,
  GET_METADATA = 27,
  GIVE_POWERUP = 94,
  HIDE_HUD = 41,
  INIT = 36,
  INITIALIZE = 75,
  IS_IN_LIQUID = 12,
  IS_ON_FIRE = 45,
  IS_POWERUP_CAPABLE = 74,
  IS_UNLOCKED = 45,
  KILL = 3,
  LEDGE_GRAB = 31,
  LIBERATE_FROM_SHOP = 35,
  LIGHT_LEVEL = 44,
  LIGHT_ON_FIRE = 53,
  ON_COLLISION1 = 4,
  ON_COLLISION2 = 26,
  ON_HIT = 49,
  PICKED_UP = 80,
  PICKED_UP_BY = 68,
  PICK_UP = 67,
  PROCESS_INPUT = 78,
  PUTTING_OFF = 97,
  PUTTING_ON = 96,
  RELEASE = 81,
  REMOVE_ITEM = 21,
  RESET_DRAW_DEPTH = 18,
  SET_AS_SOUND_SOURCE = 20,
  SET_CURSED = 54,
  SET_DRAW_DEPTH = 17,
  SET_INVISIBLE = 15,
  SET_STANDING_ON = 59,
  SPAWN_PROJECTILE = 93,
  SPRINT_FACTOR = 39,
  STANDING_ON = 60,
  STOMPED_BY = 61,
  STOMP_DAMAGE = 43,
  STOOD_ON = 32,
  STUN = 51,
  THROWN_BY = 62,
  THROWN_INTO = 47,
  TRIGGER_ACTION = 24,
  TRIGGER_EXPLOSION = 94,
  UPDATE_STATE_MACHINE = 2,
  WALKED_OFF = 30,
  WALKED_ON = 29,
  WEB_COLLISION = 55
}
---@alias ENTITY_OVERRIDE integer
ENT_FLAG = {
  CAN_BE_STOMPED = 15,
  CLIMBABLE = 9,
  COLLIDES_WALLS = 13,
  DEAD = 29,
  ENABLE_BUTTON_PROMPT = 20,
  FACING_LEFT = 17,
  HAS_BACKITEM = 32,
  INDESTRUCTIBLE_OR_SPECIAL_FLOOR = 2,
  INTERACT_WITH_SEMISOLIDS = 14,
  INTERACT_WITH_WATER = 11,
  INTERACT_WITH_WEBS = 21,
  INVISIBLE = 1,
  IS_PLATFORM = 8,
  LOCKED = 22,
  NO_GRAVITY = 10,
  PASSES_THROUGH_EVERYTHING = 5,
  PASSES_THROUGH_OBJECTS = 4,
  PASSES_THROUGH_PLAYER = 25,
  PAUSE_AI_AND_PHYSICS = 28,
  PICKUPABLE = 18,
  POWER_STOMPS = 16,
  SHOP_FLOOR = 24,
  SHOP_ITEM = 23,
  SOLID = 3,
  STUNNABLE = 12,
  TAKE_NO_DAMAGE = 6,
  THROWABLE_OR_KNOCKBACKABLE = 7,
  USABLE_ITEM = 19
}
---@alias ENT_FLAG integer
ENT_MORE_FLAG = {
  CURSED_EFFECT = 15,
  DISABLE_INPUT = 17,
  ELIXIR_BUFF = 16,
  FALLING = 14,
  HIRED_HAND_REVIVED = 2,
  HIT_GROUND = 12,
  HIT_WALL = 13,
  SWIMMING = 11
}
---@alias ENT_MORE_FLAG integer
ENT_TYPE = {
  ACIDBUBBLE = 1000,
  ACTIVEFLOOR_BONEBLOCK = 601,
  ACTIVEFLOOR_BOULDER = 599,
  ACTIVEFLOOR_BUBBLE_PLATFORM = 622,
  ACTIVEFLOOR_BUSHBLOCK = 602,
  ACTIVEFLOOR_CHAINEDPUSHBLOCK = 604,
  ACTIVEFLOOR_CHAINED_SPIKEBALL = 608,
  ACTIVEFLOOR_CRUSHING_ELEVATOR = 623,
  ACTIVEFLOOR_CRUSH_TRAP = 611,
  ACTIVEFLOOR_CRUSH_TRAP_LARGE = 612,
  ACTIVEFLOOR_DRILL = 610,
  ACTIVEFLOOR_EGGSHIPBLOCKER = 597,
  ACTIVEFLOOR_EGGSHIPPLATFORM = 596,
  ACTIVEFLOOR_ELEVATOR = 617,
  ACTIVEFLOOR_FALLING_PLATFORM = 607,
  ACTIVEFLOOR_GIANTCLAM_BASE = 618,
  ACTIVEFLOOR_KINGU_PLATFORM = 619,
  ACTIVEFLOOR_LIGHTARROWPLATFORM = 606,
  ACTIVEFLOOR_METALARROWPLATFORM = 605,
  ACTIVEFLOOR_OLMEC = 613,
  ACTIVEFLOOR_POWDERKEG = 603,
  ACTIVEFLOOR_PUSHBLOCK = 600,
  ACTIVEFLOOR_REGENERATINGBLOCK = 625,
  ACTIVEFLOOR_SHIELD = 624,
  ACTIVEFLOOR_SLIDINGWALL = 615,
  ACTIVEFLOOR_THINICE = 616,
  ACTIVEFLOOR_TIAMAT_PLATFORM = 620,
  ACTIVEFLOOR_TIAMAT_SHOULDERPLATFORM = 621,
  ACTIVEFLOOR_TIMEDPOWDERKEG = 614,
  ACTIVEFLOOR_UNCHAINED_SPIKEBALL = 609,
  ACTIVEFLOOR_WOODENLOG_TRAP = 598,
  ALIEN = 1001,
  ALTAR = 1002,
  AMMIT = 1003,
  ANKHPOWERUP = 1004,
  ANUBIS = 1005,
  APEPHEAD = 1006,
  APEPPART = 1007,
  ARROW = 1008,
  ARROWTRAP = 1009,
  AXOLOTL = 1010,
  AXOLOTLSHOT = 1011,
  BACKPACK = 1012,
  BAT = 1013,
  BEE = 1014,
  BEG = 1015,
  BGBACKLAYERDOOR = 1016,
  BGEGGSHIPROOM = 1017,
  BGFLOATINGDEBRIS = 1018,
  BGMOVINGSTAR = 1019,
  BGRELATIVEELEMENT = 1020,
  BGSHOOTINGSTAR = 1021,
  BGSHOPENTRENCE = 1022,
  BGSHOPKEEPERPRIME = 1023,
  BGSURFACELAYER = 1024,
  BGSURFACESTAR = 1025,
  BGTUTORIALSIGN = 1026,
  BG_ANUBIS_THRONE = 819,
  BG_BASECAMP_BUNKBED = 799,
  BG_BASECAMP_DININGTABLE_DISHES = 801,
  BG_BASECAMP_DRESSER = 798,
  BG_BASECAMP_SHORTCUTSTATIONBANNER = 802,
  BG_BASECAMP_SIDETABLE = 800,
  BG_BOULDER_STATUE = 828,
  BG_CONSTELLATION_CONNECTION = 776,
  BG_CONSTELLATION_FLASH = 772,
  BG_CONSTELLATION_GLOW = 775,
  BG_CONSTELLATION_HALO = 774,
  BG_CONSTELLATION_STAR = 773,
  BG_COSMIC_FARFLOATINGDEBRIS = 840,
  BG_COSMIC_FLOATINGDEBRIS = 839,
  BG_CROWN_STATUE = 818,
  BG_DOOR = 786,
  BG_DOORGEM = 797,
  BG_DOOR_BACK_LAYER = 788,
  BG_DOOR_BLACK_MARKET = 790,
  BG_DOOR_COG = 791,
  BG_DOOR_EGGPLANT_WORLD = 793,
  BG_DOOR_FRONT_LAYER = 787,
  BG_DOOR_GHIST_SHOP = 789,
  BG_DOOR_LARGE = 785,
  BG_DOOR_OLMEC_SHIP = 792,
  BG_DRILL_INDICATOR = 816,
  BG_DUAT_BLOODMOON = 825,
  BG_DUAT_FARFLOATINGDEBRIS = 827,
  BG_DUAT_FLOATINGDEBRIS = 826,
  BG_DUAT_LAYER = 822,
  BG_DUAT_PYRAMID_LAYER = 824,
  BG_DUAT_SIDE_DECORATION = 823,
  BG_EGGSAC_STAINS = 841,
  BG_EGGSHIP_ROOM = 777,
  BG_ENDINGTREASURE_HUNDUN_GOLD = 779,
  BG_ICE_CRYSTAL = 821,
  BG_KALI_STATUE = 809,
  BG_LEVEL_BACKWALL = 780,
  BG_LEVEL_BOMB_SOOT = 783,
  BG_LEVEL_COSMIC = 838,
  BG_LEVEL_DECO = 781,
  BG_LEVEL_POWEREDBOMB_SOOT = 784,
  BG_LEVEL_SHADOW = 782,
  BG_MOAI_STATUE = 829,
  BG_MOTHER_STATUE = 835,
  BG_OLMEC_PILLAR = 820,
  BG_OUROBORO = 796,
  BG_PALACE_CANDLE = 833,
  BG_PALACE_DISHES = 834,
  BG_PARENTSHIP_LANDINGLEG = 778,
  BG_SHOP = 803,
  BG_SHOPWANTEDPORTRAIT = 807,
  BG_SHOPWANTEDPOSTER = 806,
  BG_SHOP_BACKDOOR = 805,
  BG_SHOP_DICEPOSTER = 808,
  BG_SHOP_ENTRANCEDOOR = 804,
  BG_SPACE = 759,
  BG_SURFACE_BACKGROUNDSEAM = 771,
  BG_SURFACE_ENTITY = 769,
  BG_SURFACE_LAYER = 766,
  BG_SURFACE_LAYER_HOLE = 768,
  BG_SURFACE_LAYER_OCCLUDER = 767,
  BG_SURFACE_MOVING_STAR = 764,
  BG_SURFACE_NEBULA = 765,
  BG_SURFACE_OLMEC_LAYER = 770,
  BG_SURFACE_SHOOTING_STAR = 761,
  BG_SURFACE_SHOOTING_STAR_TRAIL = 762,
  BG_SURFACE_SHOOTING_STAR_TRAIL_PARTICLE = 763,
  BG_SURFACE_STAR = 760,
  BG_TUTORIAL_SIGN_BACK = 794,
  BG_TUTORIAL_SIGN_FRONT = 795,
  BG_UDJATSOCKET_DECORATION = 815,
  BG_VAT_BACK = 830,
  BG_VAT_FRONT = 832,
  BG_VAT_SHOPKEEPER_PRIME = 831,
  BG_VLAD_WINDOW = 817,
  BG_WATER_FOUNTAIN = 836,
  BG_YAMA_BODY = 837,
  BIGSPEARTRAP = 1027,
  BIRDIES = 1028,
  BODYGUARD = 1029,
  BOMB = 1030,
  BONEBLOCK = 1031,
  BOOMBOX = 1032,
  BOOMERANG = 1033,
  BOULDER = 1034,
  BOULDERSPAWNER = 1035,
  BULLET = 1036,
  BURNINGROPEEFFECT = 1037,
  BUTTON = 1038,
  CAMERAFLASH = 1039,
  CAPE = 1040,
  CATMUMMY = 1041,
  CAVEMAN = 1042,
  CAVEMANSHOPKEEPER = 1043,
  CHAIN = 1044,
  CHAINEDPUSHBLOCK = 1045,
  CHAR_AMAZON = 200,
  CHAR_ANA_SPELUNKY = 194,
  CHAR_AU = 207,
  CHAR_BANDA = 198,
  CHAR_CLASSIC_GUY = 213,
  CHAR_COCO_VON_DIAMONDS = 202,
  CHAR_COLIN_NORTHWARD = 196,
  CHAR_DEMI_VON_DIAMONDS = 208,
  CHAR_DIRK_YAMAOKA = 211,
  CHAR_EGGPLANT_CHILD = 216,
  CHAR_GREEN_GIRL = 199,
  CHAR_GUY_SPELUNKY = 212,
  CHAR_HIREDHAND = 215,
  CHAR_LISE_SYSTEM = 201,
  CHAR_MANFRED_TUNNEL = 203,
  CHAR_MARGARET_TUNNEL = 195,
  CHAR_OTAKU = 204,
  CHAR_PILOT = 209,
  CHAR_PRINCESS_AIRYN = 210,
  CHAR_ROFFY_D_SLOTH = 197,
  CHAR_TINA_FLAN = 205,
  CHAR_VALERIE_CRUMP = 206,
  CHEST = 1046,
  CINEMATICANCHOR = 1047,
  CITYOFGOLDDOOR = 1048,
  CLAMBASE = 1049,
  CLAW = 1050,
  CLIMBABLEROPE = 1051,
  CLONEGUNSHOT = 1052,
  COBRA = 1053,
  COFFIN = 1054,
  COIN = 1055,
  CONTAINER = 1056,
  CONVEYORBELT = 1057,
  COOKFIRE = 1058,
  CRABMAN = 1059,
  CRITTER = 1060,
  CRITTERBEETLE = 1061,
  CRITTERBUTTERFLY = 1062,
  CRITTERCRAB = 1063,
  CRITTERDRONE = 1064,
  CRITTERFIREFLY = 1065,
  CRITTERFISH = 1066,
  CRITTERLOCUST = 1067,
  CRITTERPENGUIN = 1068,
  CRITTERSLIME = 1069,
  CRITTERSNAIL = 1070,
  CROCMAN = 1071,
  CROSSBEAM = 1072,
  CRUSHTRAP = 1073,
  CURSEDEFFECT = 1074,
  CURSEDPOT = 1075,
  DECORATEDDOOR = 1076,
  DECORATION_BABYLON = 127,
  DECORATION_BABYLONBUSH = 138,
  DECORATION_BABYLON_FLOWER = 141,
  DECORATION_BABYLON_HANGING_FLOWER = 144,
  DECORATION_BABYLON_NEON_SIGN = 145,
  DECORATION_BASECAMPDOGSIGN = 152,
  DECORATION_BASECAMPSIGN = 151,
  DECORATION_BEEHIVE = 162,
  DECORATION_BG_TRANSITIONCOVER = 128,
  DECORATION_BONEBLOCK = 121,
  DECORATION_BORDER = 115,
  DECORATION_BRANCH = 146,
  DECORATION_BUSHBLOCK = 122,
  DECORATION_CHAINANDBLOCKS_CHAINDECORATION = 163,
  DECORATION_COG = 169,
  DECORATION_CONVEYORBELT_RAILING = 164,
  DECORATION_CROSS_BEAM = 131,
  DECORATION_DUAT = 171,
  DECORATION_DUAT_DARKSAND = 173,
  DECORATION_DUAT_DESTRUCTIBLE_BG = 174,
  DECORATION_DUAT_SAND = 172,
  DECORATION_DWELLINGBUSH = 136,
  DECORATION_EGGPLANT_ALTAR = 180,
  DECORATION_GENERIC = 116,
  DECORATION_GUTS = 179,
  DECORATION_HANGING_BANNER = 134,
  DECORATION_HANGING_HIDE = 132,
  DECORATION_HANGING_SEAWEED = 133,
  DECORATION_HANGING_WIRES = 135,
  DECORATION_JUNGLE = 119,
  DECORATION_JUNGLEBUSH = 137,
  DECORATION_JUNGLE_FLOWER = 140,
  DECORATION_JUNGLE_HANGING_FLOWER = 143,
  DECORATION_KELP = 166,
  DECORATION_LARGETOMB = 185,
  DECORATION_MINEWOOD = 120,
  DECORATION_MINEWOOD_POLE = 129,
  DECORATION_MOTHERSHIP = 170,
  DECORATION_MOTHER_STATUE_HAND = 181,
  DECORATION_MUSHROOM_HAT = 160,
  DECORATION_PAGODA = 125,
  DECORATION_PAGODA_POLE = 130,
  DECORATION_PALACE = 175,
  DECORATION_PALACE_CHANDELIER = 177,
  DECORATION_PALACE_PORTRAIT = 178,
  DECORATION_PALACE_SIGN = 176,
  DECORATION_PIPE = 182,
  DECORATION_POTOFGOLD_RAINBOW = 189,
  DECORATION_REGENERATING_BORDER = 187,
  DECORATION_REGENERATING_SMALL_BLOCK = 186,
  DECORATION_SHOPFORE = 148,
  DECORATION_SHOPSIGN = 149,
  DECORATION_SHOPSIGNICON = 150,
  DECORATION_SKULLDROP_TRAP = 188,
  DECORATION_SLIDINGWALL_CHAINDECORATION = 167,
  DECORATION_SPIKES_BLOOD = 147,
  DECORATION_STONE = 123,
  DECORATION_SUNKEN = 126,
  DECORATION_SUNKEN_BRIDGE = 183,
  DECORATION_SURFACE = 117,
  DECORATION_SURFACE_COVER = 118,
  DECORATION_TEMPLE = 124,
  DECORATION_TEMPLE_SAND = 168,
  DECORATION_THORN_VINE = 161,
  DECORATION_TIDEPOOLBUSH = 139,
  DECORATION_TIDEPOOL_CORAL = 142,
  DECORATION_TOMB = 184,
  DECORATION_TREE = 153,
  DECORATION_TREETRUNK_BROKEN = 157,
  DECORATION_TREETRUNK_CLIMBINGHINT = 154,
  DECORATION_TREETRUNK_TOPBACK = 156,
  DECORATION_TREETRUNK_TOPFRONT = 155,
  DECORATION_TREE_VINE = 159,
  DECORATION_TREE_VINE_TOP = 158,
  DECORATION_VLAD = 165,
  DECOREGENERATINGBLOCK = 1077,
  DESTRUCTIBLEBG = 1078,
  DMALIENBLAST = 1079,
  DMSPAWNING = 1080,
  DOOR = 1081,
  DRILL = 1082,
  DUSTWALLAPEP = 1083,
  EGGPLANTMINISTER = 1084,
  EGGPLANTTHROWER = 1085,
  EGGSAC = 1086,
  EGGSHIPCENTERJETFLAME = 1087,
  EGGSHIPDOOR = 1088,
  EGGSHIPDOORS = 1089,
  ELEVATOR = 1090,
  EMBED_GOLD = 190,
  EMBED_GOLD_BIG = 191,
  EMPRESSGRAVE = 1091,
  ENTITY = 1092,
  EXCALIBUR = 1093,
  EXITDOOR = 1094,
  EXPLOSION = 1095,
  FALLINGPLATFORM = 1096,
  FIREBALL = 1097,
  FIREBUG = 1098,
  FIREBUGUNCHAINED = 1099,
  FIREFROG = 1100,
  FISH = 1101,
  FLAME = 1102,
  FLAMESIZE = 1103,
  FLOOR = 1104,
  FLOORSTYLED_BABYLON = 106,
  FLOORSTYLED_BEEHIVE = 108,
  FLOORSTYLED_COG = 110,
  FLOORSTYLED_DUAT = 112,
  FLOORSTYLED_GUTS = 114,
  FLOORSTYLED_MINEWOOD = 102,
  FLOORSTYLED_MOTHERSHIP = 111,
  FLOORSTYLED_PAGODA = 105,
  FLOORSTYLED_PALACE = 113,
  FLOORSTYLED_STONE = 103,
  FLOORSTYLED_SUNKEN = 107,
  FLOORSTYLED_TEMPLE = 104,
  FLOORSTYLED_VLAD = 109,
  FLOOR_ALTAR = 47,
  FLOOR_ARROW_TRAP = 40,
  FLOOR_BASECAMP_DININGTABLE = 8,
  FLOOR_BASECAMP_LONGTABLE = 9,
  FLOOR_BASECAMP_SINGLEBED = 7,
  FLOOR_BIGSPEAR_TRAP = 79,
  FLOOR_BORDERTILE = 1,
  FLOOR_BORDERTILE_METAL = 2,
  FLOOR_BORDERTILE_OCTOPUS = 3,
  FLOOR_CHAINANDBLOCKS_CEILING = 61,
  FLOOR_CHAINANDBLOCKS_CHAIN = 62,
  FLOOR_CHAIN_CEILING = 63,
  FLOOR_CHALLENGE_ENTRANCE = 87,
  FLOOR_CHALLENGE_WAITROOM = 88,
  FLOOR_CLIMBING_POLE = 20,
  FLOOR_CONVEYORBELT_LEFT = 64,
  FLOOR_CONVEYORBELT_RIGHT = 65,
  FLOOR_DICE_FORCEFIELD = 86,
  FLOOR_DOOR_COG = 31,
  FLOOR_DOOR_EGGPLANT_WORLD = 36,
  FLOOR_DOOR_EGGSHIP = 33,
  FLOOR_DOOR_EGGSHIP_ATREZZO = 34,
  FLOOR_DOOR_EGGSHIP_ROOM = 35,
  FLOOR_DOOR_ENTRANCE = 22,
  FLOOR_DOOR_EXIT = 23,
  FLOOR_DOOR_GHISTSHOP = 28,
  FLOOR_DOOR_LAYER = 26,
  FLOOR_DOOR_LAYER_DROP_HELD = 27,
  FLOOR_DOOR_LOCKED = 29,
  FLOOR_DOOR_LOCKED_PEN = 30,
  FLOOR_DOOR_MAIN_EXIT = 24,
  FLOOR_DOOR_MOAI_STATUE = 32,
  FLOOR_DOOR_PLATFORM = 37,
  FLOOR_DOOR_STARTING_EXIT = 25,
  FLOOR_DUAT_ALTAR = 71,
  FLOOR_DUSTWALL = 70,
  FLOOR_EGGPLANT_ALTAR = 74,
  FLOOR_EMPRESS_GRAVE = 96,
  FLOOR_EXCALIBUR_STONE = 69,
  FLOOR_FACTORY_GENERATOR = 66,
  FLOOR_FORCEFIELD = 85,
  FLOOR_FORCEFIELD_TOP = 90,
  FLOOR_GENERIC = 4,
  FLOOR_GIANTFROG_PLATFORM = 83,
  FLOOR_GROWABLE_CLIMBING_POLE = 21,
  FLOOR_GROWABLE_VINE = 19,
  FLOOR_HORIZONTAL_FORCEFIELD = 91,
  FLOOR_HORIZONTAL_FORCEFIELD_TOP = 92,
  FLOOR_ICE = 72,
  FLOOR_IDOL_BLOCK = 48,
  FLOOR_IDOL_TRAP_CEILING = 49,
  FLOOR_JUNGLE = 10,
  FLOOR_JUNGLE_SPEAR_TRAP = 43,
  FLOOR_LADDER = 15,
  FLOOR_LADDER_PLATFORM = 16,
  FLOOR_LASER_TRAP = 45,
  FLOOR_LION_TRAP = 44,
  FLOOR_MOAI_PLATFORM = 75,
  FLOOR_MOTHER_STATUE = 81,
  FLOOR_MOTHER_STATUE_PLATFORM = 82,
  FLOOR_MUSHROOM_BASE = 55,
  FLOOR_MUSHROOM_HAT_PLATFORM = 58,
  FLOOR_MUSHROOM_TOP = 57,
  FLOOR_MUSHROOM_TRUNK = 56,
  FLOOR_PAGODA_PLATFORM = 14,
  FLOOR_PALACE_BOOKCASE_PLATFORM = 100,
  FLOOR_PALACE_CHANDELIER_PLATFORM = 99,
  FLOOR_PALACE_TABLE_PLATFORM = 97,
  FLOOR_PALACE_TRAY_PLATFORM = 98,
  FLOOR_PEN = 93,
  FLOOR_PIPE = 78,
  FLOOR_PLATFORM = 13,
  FLOOR_POISONED_ARROW_TRAP = 41,
  FLOOR_QUICKSAND = 68,
  FLOOR_SHOPKEEPER_GENERATOR = 76,
  FLOOR_SLIDINGWALL_CEILING = 67,
  FLOOR_SPARK_TRAP = 46,
  FLOOR_SPIKEBALL_CEILING = 60,
  FLOOR_SPIKES = 38,
  FLOOR_SPIKES_UPSIDEDOWN = 39,
  FLOOR_SPRING_TRAP = 73,
  FLOOR_STICKYTRAP_CEILING = 80,
  FLOOR_STORAGE = 50,
  FLOOR_SUNCHALLENGE_GENERATOR = 77,
  FLOOR_SURFACE = 5,
  FLOOR_SURFACE_HIDDEN = 6,
  FLOOR_TELEPORTINGBORDER = 84,
  FLOOR_TENTACLE_BOTTOM = 101,
  FLOOR_THORN_VINE = 59,
  FLOOR_TIMED_FORCEFIELD = 89,
  FLOOR_TOMB = 94,
  FLOOR_TOTEM_TRAP = 42,
  FLOOR_TREE_BASE = 51,
  FLOOR_TREE_BRANCH = 54,
  FLOOR_TREE_TOP = 53,
  FLOOR_TREE_TRUNK = 52,
  FLOOR_TUNNEL_CURRENT = 11,
  FLOOR_TUNNEL_NEXT = 12,
  FLOOR_VINE = 17,
  FLOOR_VINE_TREE_TOP = 18,
  FLOOR_YAMA_PLATFORM = 95,
  FLY = 1105,
  FLYHEAD = 1106,
  FORCEFIELD = 1107,
  FORESTSISTER = 1108,
  FROG = 1109,
  FROSTBREATHEFFECT = 1110,
  FROZENLIQUID = 1111,
  FXALIENBLAST = 1112,
  FXANKHBROKENPIECE = 1113,
  FXANKHROTATINGSPARK = 1114,
  FXCOMPASS = 1115,
  FXEMPRESS = 1116,
  FXFIREFLYLIGHT = 1117,
  FXHUNDUNNECKPIECE = 1118,
  FXJELLYFISHSTAR = 1119,
  FXJETPACKFLAME = 1120,
  FXKINGUSLIDING = 1121,
  FXLAMASSUATTACK = 1122,
  FXMAINEXITDOOR = 1123,
  FXNECROMANCERANKH = 1124,
  FXOUROBORODRAGONPART = 1125,
  FXOUROBOROOCCLUDER = 1126,
  FXPICKUPEFFECT = 1127,
  FXPLAYERINDICATOR = 1128,
  FXQUICKSAND = 1129,
  FXSALECONTAINER = 1130,
  FXSHOTGUNBLAST = 1131,
  FXSORCERESSATTACK = 1132,
  FXSPARKSMALL = 1133,
  FXSPRINGTRAPRING = 1134,
  FXTIAMATHEAD = 1135,
  FXTIAMATTAIL = 1136,
  FXTIAMATTORSO = 1137,
  FXTORNJOURNALPAGE = 1138,
  FXUNDERWATERBUBBLE = 1139,
  FXVATBUBBLE = 1140,
  FXWATERDROP = 1141,
  FXWEBBEDEFFECT = 1142,
  FXWITCHDOCTORHINT = 1143,
  FX_ALIENBLAST = 694,
  FX_ALIENBLAST_RETICULE_EXTERNAL = 693,
  FX_ALIENBLAST_RETICULE_INTERNAL = 692,
  FX_ALIENQUEEN_EYE = 691,
  FX_ALIENQUEEN_EYEBALL = 690,
  FX_ANKH_BACKGLOW = 752,
  FX_ANKH_BROKENPIECE = 755,
  FX_ANKH_FALLINGSPARK = 751,
  FX_ANKH_FRONTGLOW = 753,
  FX_ANKH_LIGHTBEAM = 754,
  FX_ANKH_ROTATINGSPARK = 750,
  FX_ANUBIS_SPECIAL_SHOT_RETICULE = 735,
  FX_APEP_FIRE = 681,
  FX_APEP_MOUTHPIECE = 682,
  FX_AXOLOTL_HEAD_ENTERING_DOOR = 742,
  FX_BASECAMP_COUCH_ARM = 743,
  FX_BIRDIES = 636,
  FX_BUTTON = 670,
  FX_BUTTON_DIALOG = 671,
  FX_CINEMATIC_BLACKBAR = 663,
  FX_COMPASS = 646,
  FX_CRITTERFIREFLY_LIGHT = 741,
  FX_CRUSHINGELEVATOR_DECO = 717,
  FX_CRUSHINGELEVATOR_FILL = 716,
  FX_DIEINDICATOR = 711,
  FX_DRILL_TURNING = 683,
  FX_EGGSHIP_CENTERJETFLAME = 630,
  FX_EGGSHIP_DOOR = 629,
  FX_EGGSHIP_HOOK_CHAIN = 745,
  FX_EGGSHIP_JETFLAME = 631,
  FX_EGGSHIP_SHADOW = 632,
  FX_EGGSHIP_SHELL = 628,
  FX_EMPRESS = 744,
  FX_EXPLOSION = 637,
  FX_HORIZONTALLASERBEAM = 714,
  FX_HUNDUN_EGG_CRACK = 721,
  FX_HUNDUN_EYE = 726,
  FX_HUNDUN_EYEBALL = 725,
  FX_HUNDUN_EYELID = 724,
  FX_HUNDUN_LIMB_CALF = 719,
  FX_HUNDUN_LIMB_FOOT = 720,
  FX_HUNDUN_LIMB_THIGH = 718,
  FX_HUNDUN_NECK_PIECE = 722,
  FX_HUNDUN_WING = 723,
  FX_INK_BLINDNESS = 673,
  FX_INK_SPLAT = 674,
  FX_JETPACKFLAME = 657,
  FX_KINGU_HEAD = 685,
  FX_KINGU_LIMB = 688,
  FX_KINGU_PLATFORM = 687,
  FX_KINGU_SHADOW = 686,
  FX_KINGU_SLIDING = 689,
  FX_LAMASSU_ATTACK = 740,
  FX_LASERBEAM = 713,
  FX_LAVA_BUBBLE = 675,
  FX_LAVA_GLOW = 676,
  FX_LEADER_FLAG = 684,
  FX_MAIN_EXIT_DOOR = 635,
  FX_MECH_COLLAR = 736,
  FX_MEGAJELLYFISH_BOTTOM = 732,
  FX_MEGAJELLYFISH_CROWN = 728,
  FX_MEGAJELLYFISH_EYE = 729,
  FX_MEGAJELLYFISH_FLIPPER = 731,
  FX_MEGAJELLYFISH_STAR = 730,
  FX_MEGAJELLYFISH_TAIL = 733,
  FX_MEGAJELLYFISH_TAIL_BG = 734,
  FX_MINIGAME_SHIP_CENTERJETFLAME = 747,
  FX_MINIGAME_SHIP_DOOR = 746,
  FX_MINIGAME_SHIP_JETFLAME = 748,
  FX_MODERNEXPLOSION = 639,
  FX_NECROMANCER_ANKH = 661,
  FX_OLMECPART_FLOATER = 664,
  FX_OLMECPART_LARGE = 665,
  FX_OLMECPART_MEDIUM = 666,
  FX_OLMECPART_SMALL = 667,
  FX_OLMECPART_SMALLEST = 668,
  FX_OUROBORO_HEAD = 641,
  FX_OUROBORO_OCCLUDER = 640,
  FX_OUROBORO_TAIL = 642,
  FX_OUROBORO_TEXT = 643,
  FX_OUROBORO_TRAIL = 644,
  FX_PICKUPEFFECT = 655,
  FX_PLAYERINDICATOR = 648,
  FX_PLAYERINDICATORPORTRAIT = 649,
  FX_PORTAL = 727,
  FX_POWEREDEXPLOSION = 638,
  FX_QUICKSAND_DUST = 737,
  FX_QUICKSAND_RUBBLE = 738,
  FX_SALEDIALOG_CONTAINER = 651,
  FX_SALEDIALOG_ICON = 654,
  FX_SALEDIALOG_TITLE = 652,
  FX_SALEDIALOG_VALUE = 653,
  FX_SALEICON = 650,
  FX_SHADOW = 633,
  FX_SHOTGUNBLAST = 656,
  FX_SLEEP_BUBBLE = 672,
  FX_SMALLFLAME = 658,
  FX_SORCERESS_ATTACK = 739,
  FX_SPARK = 695,
  FX_SPARK_SMALL = 696,
  FX_SPECIALCOMPASS = 647,
  FX_SPRINGTRAP_RING = 659,
  FX_STORAGE_INDICATOR = 712,
  FX_TELEPORTSHADOW = 662,
  FX_TIAMAT_ARM_LEFT1 = 704,
  FX_TIAMAT_ARM_LEFT2 = 705,
  FX_TIAMAT_ARM_LEFT3 = 706,
  FX_TIAMAT_ARM_RIGHT1 = 707,
  FX_TIAMAT_ARM_RIGHT2 = 708,
  FX_TIAMAT_HEAD = 710,
  FX_TIAMAT_NECK = 709,
  FX_TIAMAT_TAIL = 699,
  FX_TIAMAT_TAIL_DECO1 = 700,
  FX_TIAMAT_TAIL_DECO2 = 701,
  FX_TIAMAT_TAIL_DECO3 = 702,
  FX_TIAMAT_THRONE = 697,
  FX_TIAMAT_TORSO = 703,
  FX_TIAMAT_WAIST = 698,
  FX_TORNJOURNALPAGE = 634,
  FX_UNDERWATER_BUBBLE = 677,
  FX_VAT_BUBBLE = 715,
  FX_WATER_DROP = 678,
  FX_WATER_SPLASH = 679,
  FX_WATER_SURFACE = 680,
  FX_WEBBEDEFFECT = 669,
  FX_WITCHDOCTOR_HINT = 660,
  GENERATOR = 1144,
  GHIST = 1145,
  GHOST = 1146,
  GHOSTBREATH = 1147,
  GIANTCLAMTOP = 1148,
  GIANTFISH = 1149,
  GIANTFLY = 1150,
  GIANTFROG = 1151,
  GOLDBAR = 1152,
  GOLDMONKEY = 1153,
  GRUB = 1154,
  GUN = 1155,
  HANGANCHOR = 1156,
  HANGSPIDER = 1157,
  HANGSTRAND = 1158,
  HERMITCRAB = 1159,
  HONEY = 1160,
  HORIZONTALFORCEFIELD = 1161,
  HORNEDLIZARD = 1162,
  HOVERPACK = 1163,
  HUNDUN = 1164,
  HUNDUNCHEST = 1165,
  HUNDUNHEAD = 1166,
  ICESLIDINGSOUND = 1167,
  IDOL = 1168,
  IMP = 1169,
  ITEM_ACIDBUBBLE = 391,
  ITEM_ACIDSPIT = 389,
  ITEM_ALIVE_EMBEDDED_ON_ICE = 463,
  ITEM_ANUBIS_COFFIN = 453,
  ITEM_AUTOWALLTORCH = 415,
  ITEM_AXOLOTL_BUBBLESHOT = 456,
  ITEM_BASECAMP_TUTORIAL_SIGN = 408,
  ITEM_BIG_SPEAR = 364,
  ITEM_BLOOD = 352,
  ITEM_BOMB = 347,
  ITEM_BONES = 483,
  ITEM_BOOMBOX = 409,
  ITEM_BOOMERANG = 583,
  ITEM_BROKENEXCALIBUR = 586,
  ITEM_BROKEN_ARROW = 372,
  ITEM_BROKEN_MATTOCK = 428,
  ITEM_BULLET = 424,
  ITEM_CAMERA = 580,
  ITEM_CAPE = 564,
  ITEM_CHAIN = 431,
  ITEM_CHAIN_LASTPIECE = 432,
  ITEM_CHEST = 395,
  ITEM_CLIMBABLE_ROPE = 350,
  ITEM_CLONEGUN = 589,
  ITEM_CLONEGUNSHOT = 426,
  ITEM_COFFIN = 435,
  ITEM_CONSTRUCTION_SIGN = 405,
  ITEM_COOKFIRE = 484,
  ITEM_CRABMAN_ACIDBUBBLE = 392,
  ITEM_CRABMAN_CLAW = 393,
  ITEM_CRABMAN_CLAWCHAIN = 394,
  ITEM_CRATE = 402,
  ITEM_CROSSBOW = 579,
  ITEM_CURSEDPOT = 481,
  ITEM_CURSING_CLOUD = 440,
  ITEM_DEPLOYED_PARACHUTE = 464,
  ITEM_DIAMOND = 497,
  ITEM_DICE_BET = 449,
  ITEM_DICE_PRIZE_DISPENSER = 450,
  ITEM_DIE = 448,
  ITEM_DMCRATE = 403,
  ITEM_EGGPLANT = 487,
  ITEM_EGGSAC = 492,
  ITEM_EGGSHIP = 353,
  ITEM_EGGSHIP_HOOK = 455,
  ITEM_EMERALD = 498,
  ITEM_EMERALD_SMALL = 503,
  ITEM_EMPRESS_GRAVE = 470,
  ITEM_ENDINGTREASURE_HUNDUN = 398,
  ITEM_ENDINGTREASURE_TIAMAT = 397,
  ITEM_EXCALIBUR = 585,
  ITEM_FIREBALL = 385,
  ITEM_FLAMETHROWER_FIREBALL = 387,
  ITEM_FLOATING_ORB = 491,
  ITEM_FLY = 436,
  ITEM_FREEZERAY = 578,
  ITEM_FREEZERAYSHOT = 425,
  ITEM_FROZEN_LIQUID = 462,
  ITEM_GHIST_PRESENT = 423,
  ITEM_GIANTCLAM_TOP = 445,
  ITEM_GIANTFLY_HEAD = 467,
  ITEM_GIANTSPIDER_WEBSHOT = 368,
  ITEM_GOLDBAR = 495,
  ITEM_GOLDBARS = 496,
  ITEM_GOLDCOIN = 502,
  ITEM_HANGANCHOR = 370,
  ITEM_HANGSTRAND = 369,
  ITEM_HOLDTHEIDOL = 359,
  ITEM_HONEY = 444,
  ITEM_HORIZONTALLASERBEAM = 452,
  ITEM_HOUYIBOW = 590,
  ITEM_HOVERPACK = 572,
  ITEM_HUNDUN_FIREBALL = 386,
  ITEM_ICECAGE = 427,
  ITEM_ICESPIRE = 488,
  ITEM_IDOL = 356,
  ITEM_INKSPIT = 390,
  ITEM_JETPACK = 567,
  ITEM_JETPACK_MECH = 568,
  ITEM_JUNGLE_SPEAR_COSMETIC = 361,
  ITEM_JUNGLE_SPEAR_DAMAGING = 362,
  ITEM_KEY = 399,
  ITEM_LAMASSU_LASER_SHOT = 380,
  ITEM_LAMP = 418,
  ITEM_LAMPFLAME = 419,
  ITEM_LANDMINE = 439,
  ITEM_LASERBEAM = 451,
  ITEM_LASERTRAP_SHOT = 382,
  ITEM_LAVAPOT = 485,
  ITEM_LEAF = 388,
  ITEM_LIGHT_ARROW = 374,
  ITEM_LION_SPEAR = 363,
  ITEM_LITWALLTORCH = 414,
  ITEM_LOCKEDCHEST = 400,
  ITEM_LOCKEDCHEST_KEY = 401,
  ITEM_MACHETE = 584,
  ITEM_MADAMETUSK_IDOL = 357,
  ITEM_MADAMETUSK_IDOLNOTE = 358,
  ITEM_MATTOCK = 582,
  ITEM_METAL_ARROW = 373,
  ITEM_METAL_SHIELD = 592,
  ITEM_MINIGAME_ASTEROID = 477,
  ITEM_MINIGAME_ASTEROID_BG = 476,
  ITEM_MINIGAME_BROKEN_ASTEROID = 478,
  ITEM_MINIGAME_SHIP = 474,
  ITEM_MINIGAME_UFO = 475,
  ITEM_NUGGET = 501,
  ITEM_NUGGET_SMALL = 506,
  ITEM_OLMECCANNON_BOMBS = 437,
  ITEM_OLMECCANNON_UFO = 438,
  ITEM_OLMECSHIP = 355,
  ITEM_PALACE_CANDLE = 489,
  ITEM_PALACE_CANDLE_FLAME = 468,
  ITEM_PARENTSSHIP = 354,
  ITEM_PASTEBOMB = 348,
  ITEM_PICKUP_12BAG = 515,
  ITEM_PICKUP_24BAG = 516,
  ITEM_PICKUP_ANKH = 539,
  ITEM_PICKUP_BOMBBAG = 513,
  ITEM_PICKUP_BOMBBOX = 514,
  ITEM_PICKUP_CLIMBINGGLOVES = 525,
  ITEM_PICKUP_CLOVER = 521,
  ITEM_PICKUP_COMPASS = 530,
  ITEM_PICKUP_COOKEDTURKEY = 518,
  ITEM_PICKUP_CROWN = 536,
  ITEM_PICKUP_EGGPLANTCROWN = 537,
  ITEM_PICKUP_ELIXIR = 520,
  ITEM_PICKUP_GIANTFOOD = 519,
  ITEM_PICKUP_HEDJET = 535,
  ITEM_PICKUP_JOURNAL = 510,
  ITEM_PICKUP_KAPALA = 534,
  ITEM_PICKUP_PARACHUTE = 532,
  ITEM_PICKUP_PASTE = 529,
  ITEM_PICKUP_PITCHERSMITT = 526,
  ITEM_PICKUP_PLAYERBAG = 543,
  ITEM_PICKUP_ROPE = 511,
  ITEM_PICKUP_ROPEPILE = 512,
  ITEM_PICKUP_ROYALJELLY = 517,
  ITEM_PICKUP_SEEDEDRUNSUNLOCKER = 522,
  ITEM_PICKUP_SKELETON_KEY = 541,
  ITEM_PICKUP_SPECIALCOMPASS = 531,
  ITEM_PICKUP_SPECTACLES = 524,
  ITEM_PICKUP_SPIKESHOES = 528,
  ITEM_PICKUP_SPRINGSHOES = 527,
  ITEM_PICKUP_TABLETOFDESTINY = 540,
  ITEM_PICKUP_TORNJOURNALPAGE = 509,
  ITEM_PICKUP_TRUECROWN = 538,
  ITEM_PICKUP_UDJATEYE = 533,
  ITEM_PLASMACANNON = 587,
  ITEM_PLASMACANNON_SHOT = 375,
  ITEM_PLAYERGHOST = 446,
  ITEM_PLAYERGHOST_BREATH = 447,
  ITEM_POT = 480,
  ITEM_POTOFGOLD = 457,
  ITEM_POWERPACK = 574,
  ITEM_POWERUP_ANKH = 560,
  ITEM_POWERUP_CLIMBING_GLOVES = 546,
  ITEM_POWERUP_COMPASS = 554,
  ITEM_POWERUP_CROWN = 557,
  ITEM_POWERUP_EGGPLANTCROWN = 558,
  ITEM_POWERUP_HEDJET = 556,
  ITEM_POWERUP_KAPALA = 549,
  ITEM_POWERUP_PARACHUTE = 553,
  ITEM_POWERUP_PASTE = 545,
  ITEM_POWERUP_PITCHERSMITT = 551,
  ITEM_POWERUP_SKELETON_KEY = 562,
  ITEM_POWERUP_SPECIALCOMPASS = 555,
  ITEM_POWERUP_SPECTACLES = 550,
  ITEM_POWERUP_SPIKE_SHOES = 547,
  ITEM_POWERUP_SPRING_SHOES = 548,
  ITEM_POWERUP_TABLETOFDESTINY = 561,
  ITEM_POWERUP_TRUECROWN = 559,
  ITEM_POWERUP_UDJATEYE = 552,
  ITEM_PRESENT = 422,
  ITEM_PUNISHBALL = 429,
  ITEM_PUNISHCHAIN = 430,
  ITEM_PURCHASABLE_CAPE = 566,
  ITEM_PURCHASABLE_HOVERPACK = 573,
  ITEM_PURCHASABLE_JETPACK = 569,
  ITEM_PURCHASABLE_POWERPACK = 575,
  ITEM_PURCHASABLE_TELEPORTER_BACKPACK = 571,
  ITEM_REDLANTERN = 420,
  ITEM_REDLANTERNFLAME = 421,
  ITEM_ROCK = 365,
  ITEM_ROPE = 349,
  ITEM_RUBBLE = 645,
  ITEM_RUBY = 500,
  ITEM_RUBY_SMALL = 505,
  ITEM_SAPPHIRE = 499,
  ITEM_SAPPHIRE_SMALL = 504,
  ITEM_SCEPTER = 588,
  ITEM_SCEPTER_ANUBISSHOT = 376,
  ITEM_SCEPTER_ANUBISSPECIALSHOT = 377,
  ITEM_SCEPTER_PLAYERSHOT = 378,
  ITEM_SCRAP = 486,
  ITEM_SHORTCUT_SIGN = 406,
  ITEM_SHOTGUN = 577,
  ITEM_SKULL = 482,
  ITEM_SKULLDROPTRAP = 461,
  ITEM_SKULLDROPTRAP_SKULL = 490,
  ITEM_SLIDINGWALL_CHAIN = 433,
  ITEM_SLIDINGWALL_CHAIN_LASTPIECE = 434,
  ITEM_SLIDINGWALL_SWITCH = 465,
  ITEM_SLIDINGWALL_SWITCH_REWARD = 466,
  ITEM_SNAP_TRAP = 469,
  ITEM_SORCERESS_DAGGER_SHOT = 381,
  ITEM_SPARK = 383,
  ITEM_SPEEDRUN_SIGN = 407,
  ITEM_SPIKES = 454,
  ITEM_STICKYTRAP_BALL = 460,
  ITEM_STICKYTRAP_LASTPIECE = 459,
  ITEM_STICKYTRAP_PIECE = 458,
  ITEM_TELEPORTER = 581,
  ITEM_TELEPORTER_BACKPACK = 570,
  ITEM_TELESCOPE = 411,
  ITEM_TENTACLE = 471,
  ITEM_TENTACLE_LAST_PIECE = 473,
  ITEM_TENTACLE_PIECE = 472,
  ITEM_TIAMAT_SHOT = 384,
  ITEM_TORCH = 416,
  ITEM_TORCHFLAME = 417,
  ITEM_TOTEM_SPEAR = 360,
  ITEM_TURKEY_NECK = 443,
  ITEM_TUTORIAL_MONSTER_SIGN = 404,
  ITEM_TV = 410,
  ITEM_UDJAT_SOCKET = 441,
  ITEM_UFO_LASER_SHOT = 379,
  ITEM_UNROLLED_ROPE = 351,
  ITEM_USHABTI = 442,
  ITEM_VAULTCHEST = 396,
  ITEM_VLADS_CAPE = 565,
  ITEM_WALLTORCH = 412,
  ITEM_WALLTORCHFLAME = 413,
  ITEM_WEB = 366,
  ITEM_WEBGUN = 576,
  ITEM_WEBSHOT = 367,
  ITEM_WHIP = 345,
  ITEM_WHIP_FLAME = 346,
  ITEM_WOODEN_ARROW = 371,
  ITEM_WOODEN_SHIELD = 591,
  JETPACK = 1170,
  JIANGSHI = 1171,
  JUMPDOG = 1172,
  JUNGLESPEARCOSMETIC = 1173,
  JUNGLETRAPTRIGGER = 1174,
  KAPALAPOWERUP = 1175,
  KINGU = 1176,
  LAHAMU = 1177,
  LAMASSU = 1178,
  LAMPFLAME = 1179,
  LANDMINE = 1180,
  LASERBEAM = 1181,
  LASERTRAP = 1182,
  LAVA = 1183,
  LAVAMANDER = 1184,
  LEAF = 1185,
  LEPRECHAUN = 1186,
  LIGHTARROW = 1187,
  LIGHTARROWPLATFORM = 1188,
  LIGHTEMITTER = 1189,
  LIGHTSHOT = 1190,
  LIMBANCHOR = 1191,
  LIQUID = 1192,
  LIQUIDSURFACE = 1193,
  LIQUID_COARSE_LAVA = 915,
  LIQUID_COARSE_WATER = 910,
  LIQUID_IMPOSTOR_LAKE = 911,
  LIQUID_IMPOSTOR_LAVA = 914,
  LIQUID_LAVA = 912,
  LIQUID_STAGNANT_LAVA = 913,
  LIQUID_WATER = 909,
  LOCKEDDOOR = 1194,
  LOGICALANCHOVYFLOCK = 1195,
  LOGICALCONVEYORBELTSOUND = 1196,
  LOGICALDOOR = 1197,
  LOGICALDRAIN = 1198,
  LOGICALLIQUIDSTREAMSOUND = 1199,
  LOGICALMINIGAME = 1200,
  LOGICALREGENERATINGBLOCK = 1201,
  LOGICALSOUND = 1202,
  LOGICALSTATICSOUND = 1203,
  LOGICALTRAPTRIGGER = 1204,
  LOGICAL_ANCHOVY_FLOCK = 873,
  LOGICAL_ARROW_TRAP_TRIGGER = 849,
  LOGICAL_BIGSPEAR_TRAP_TRIGGER = 884,
  LOGICAL_BLACKMARKET_DOOR = 848,
  LOGICAL_BOULDERSPAWNER = 880,
  LOGICAL_BURNING_ROPE_EFFECT = 863,
  LOGICAL_CAMERA_ANCHOR = 859,
  LOGICAL_CAMERA_FLASH = 865,
  LOGICAL_CINEMATIC_ANCHOR = 862,
  LOGICAL_CONSTELLATION = 844,
  LOGICAL_CONVEYORBELT_SOUND_SOURCE = 872,
  LOGICAL_CRUSH_TRAP_TRIGGER = 853,
  LOGICAL_CURSED_EFFECT = 858,
  LOGICAL_DM_ALIEN_BLAST = 889,
  LOGICAL_DM_CAMERA_ANCHOR = 887,
  LOGICAL_DM_CRATE_SPAWNING = 890,
  LOGICAL_DM_DEATH_MIST = 888,
  LOGICAL_DM_IDOL_SPAWNING = 891,
  LOGICAL_DM_SPAWN_POINT = 886,
  LOGICAL_DOOR = 846,
  LOGICAL_DOOR_AMBIENT_SOUND = 847,
  LOGICAL_DUSTWALL_APEP = 864,
  LOGICAL_DUSTWALL_SOUND_SOURCE = 877,
  LOGICAL_EGGPLANT_THROWER = 894,
  LOGICAL_FROST_BREATH = 893,
  LOGICAL_ICESLIDING_SOUND_SOURCE = 878,
  LOGICAL_JUNGLESPEAR_TRAP_TRIGGER = 851,
  LOGICAL_LAVA_DRAIN = 882,
  LOGICAL_LIMB_ANCHOR = 867,
  LOGICAL_MINIGAME = 895,
  LOGICAL_MUMMYFLIES_SOUND_SOURCE = 874,
  LOGICAL_ONFIRE_EFFECT = 856,
  LOGICAL_OUROBORO_CAMERA_ANCHOR = 860,
  LOGICAL_OUROBORO_CAMERA_ANCHOR_ZOOMIN = 861,
  LOGICAL_PIPE_TRAVELER_SOUND_SOURCE = 879,
  LOGICAL_PLATFORM_SPAWNER = 885,
  LOGICAL_POISONED_EFFECT = 857,
  LOGICAL_PORTAL = 868,
  LOGICAL_QUICKSAND_AMBIENT_SOUND_SOURCE = 875,
  LOGICAL_QUICKSAND_SOUND_SOURCE = 876,
  LOGICAL_REGENERATING_BLOCK = 883,
  LOGICAL_ROOM_LIGHT = 866,
  LOGICAL_SHOOTING_STARS_SPAWNER = 845,
  LOGICAL_SPIKEBALL_TRIGGER = 852,
  LOGICAL_SPLASH_BUBBLE_GENERATOR = 892,
  LOGICAL_STATICLAVA_SOUND_SOURCE = 869,
  LOGICAL_STREAMLAVA_SOUND_SOURCE = 870,
  LOGICAL_STREAMWATER_SOUND_SOURCE = 871,
  LOGICAL_TENTACLE_TRIGGER = 854,
  LOGICAL_TOTEM_TRAP_TRIGGER = 850,
  LOGICAL_WATER_DRAIN = 881,
  LOGICAL_WET_EFFECT = 855,
  MAGMAMAN = 1205,
  MAINEXIT = 1206,
  MANTRAP = 1207,
  MATTOCK = 1208,
  MECH = 1209,
  MEGAJELLYFISH = 1210,
  MIDBG = 810,
  MIDBG_BEEHIVE = 813,
  MIDBG_PALACE_STYLEDDECORATION = 812,
  MIDBG_PLATFORM_STRUCTURE = 814,
  MIDBG_STYLEDDECORATION = 811,
  MINIGAMEASTEROID = 1211,
  MINIGAMESHIP = 1212,
  MINIGAMESHIPOFFSET = 1213,
  MOLE = 1214,
  MONKEY = 1215,
  MONSTER = 1216,
  MONS_ALIEN = 267,
  MONS_ALIENQUEEN = 271,
  MONS_AMMIT = 280,
  MONS_ANUBIS = 253,
  MONS_ANUBIS2 = 259,
  MONS_APEP_BODY = 255,
  MONS_APEP_HEAD = 254,
  MONS_APEP_TAIL = 256,
  MONS_BAT = 224,
  MONS_BEE = 278,
  MONS_BODYGUARD = 306,
  MONS_CATMUMMY = 251,
  MONS_CAVEMAN = 225,
  MONS_CAVEMAN_BOSS = 232,
  MONS_CAVEMAN_SHOPKEEPER = 226,
  MONS_COBRA = 248,
  MONS_CRABMAN = 311,
  MONS_CRITTERANCHOVY = 335,
  MONS_CRITTERBUTTERFLY = 332,
  MONS_CRITTERCRAB = 336,
  MONS_CRITTERDRONE = 340,
  MONS_CRITTERDUNGBEETLE = 331,
  MONS_CRITTERFIREFLY = 339,
  MONS_CRITTERFISH = 334,
  MONS_CRITTERLOCUST = 337,
  MONS_CRITTERPENGUIN = 338,
  MONS_CRITTERSLIME = 341,
  MONS_CRITTERSNAIL = 333,
  MONS_CROCMAN = 247,
  MONS_EGGPLANT_MINISTER = 290,
  MONS_FEMALE_JIANGSHI = 261,
  MONS_FIREBUG = 241,
  MONS_FIREBUG_UNCHAINED = 242,
  MONS_FIREFROG = 284,
  MONS_FISH = 262,
  MONS_FROG = 283,
  MONS_GHIST = 314,
  MONS_GHIST_SHOPKEEPER = 315,
  MONS_GHOST = 317,
  MONS_GHOST_MEDIUM_HAPPY = 319,
  MONS_GHOST_MEDIUM_SAD = 318,
  MONS_GHOST_SMALL_ANGRY = 320,
  MONS_GHOST_SMALL_HAPPY = 323,
  MONS_GHOST_SMALL_SAD = 321,
  MONS_GHOST_SMALL_SURPRISED = 322,
  MONS_GIANTFISH = 265,
  MONS_GIANTFLY = 288,
  MONS_GIANTFROG = 285,
  MONS_GIANTSPIDER = 223,
  MONS_GOLDMONKEY = 309,
  MONS_GRUB = 286,
  MONS_HANGSPIDER = 222,
  MONS_HERMITCRAB = 264,
  MONS_HORNEDLIZARD = 230,
  MONS_HUNDUN = 292,
  MONS_HUNDUNS_SERVANT = 307,
  MONS_HUNDUN_BIRDHEAD = 293,
  MONS_HUNDUN_SNAKEHEAD = 294,
  MONS_IMP = 243,
  MONS_JIANGSHI = 260,
  MONS_JUMPDOG = 289,
  MONS_KINGU = 281,
  MONS_LAMASSU = 274,
  MONS_LAVAMANDER = 244,
  MONS_LEPRECHAUN = 310,
  MONS_MADAMETUSK = 305,
  MONS_MAGMAMAN = 239,
  MONS_MANTRAP = 233,
  MONS_MARLA_TUNNEL = 299,
  MONS_MEGAJELLYFISH = 312,
  MONS_MEGAJELLYFISH_BACKGROUND = 313,
  MONS_MERCHANT = 297,
  MONS_MOLE = 231,
  MONS_MONKEY = 238,
  MONS_MOSQUITO = 237,
  MONS_MUMMY = 249,
  MONS_NECROMANCER = 252,
  MONS_OCTOPUS = 263,
  MONS_OLD_HUNTER = 303,
  MONS_OLMITE_BODYARMORED = 276,
  MONS_OLMITE_HELMET = 275,
  MONS_OLMITE_NAKED = 277,
  MONS_OSIRIS_HAND = 258,
  MONS_OSIRIS_HEAD = 257,
  MONS_PET_CAT = 327,
  MONS_PET_DOG = 326,
  MONS_PET_HAMSTER = 328,
  MONS_PET_TUTORIAL = 219,
  MONS_PROTOSHOPKEEPER = 272,
  MONS_QUEENBEE = 279,
  MONS_REDSKELETON = 228,
  MONS_ROBOT = 240,
  MONS_SCARAB = 295,
  MONS_SCORPION = 229,
  MONS_SHOPKEEPER = 296,
  MONS_SHOPKEEPERCLONE = 273,
  MONS_SISTER_PARMESAN = 302,
  MONS_SISTER_PARSLEY = 300,
  MONS_SISTER_PARSNIP = 301,
  MONS_SKELETON = 227,
  MONS_SNAKE = 220,
  MONS_SORCERESS = 250,
  MONS_SPIDER = 221,
  MONS_STORAGEGUY = 308,
  MONS_TADPOLE = 287,
  MONS_THIEF = 304,
  MONS_TIAMAT = 282,
  MONS_TIKIMAN = 234,
  MONS_UFO = 266,
  MONS_VAMPIRE = 245,
  MONS_VLAD = 246,
  MONS_WITCHDOCTOR = 235,
  MONS_WITCHDOCTORSKULL = 236,
  MONS_YAMA = 291,
  MONS_YANG = 298,
  MONS_YETI = 268,
  MONS_YETIKING = 269,
  MONS_YETIQUEEN = 270,
  MOSQUITO = 1217,
  MOTHERSTATUE = 1218,
  MOUNT = 1219,
  MOUNT_AXOLOTL = 901,
  MOUNT_BASECAMP_CHAIR = 905,
  MOUNT_BASECAMP_COUCH = 906,
  MOUNT_MECH = 902,
  MOUNT_QILIN = 903,
  MOUNT_ROCKDOG = 900,
  MOUNT_TURKEY = 899,
  MOVABLE = 1220,
  MOVINGICON = 1221,
  MUMMY = 1222,
  MUMMYFLIESSOUND = 1223,
  NECROMANCER = 1224,
  NPC = 1225,
  OCTOPUS = 1226,
  OLMEC = 1227,
  OLMECCANNON = 1228,
  OLMECFLOATER = 1229,
  OLMITE = 1230,
  ONFIREEFFECT = 1231,
  ORB = 1232,
  OSIRISHAND = 1233,
  OSIRISHEAD = 1234,
  OUROBOROCAMERAANCHOR = 1235,
  OUROBOROCAMERAZOOMIN = 1236,
  PALACESIGN = 1237,
  PARACHUTEPOWERUP = 1238,
  PET = 1239,
  PIPE = 1240,
  PIPETRAVELERSOUND = 1241,
  PLAYER = 1242,
  PLAYERBAG = 1243,
  PLAYERGHOST = 1244,
  POISONEDEFFECT = 1245,
  POLEDECO = 1246,
  PORTAL = 1247,
  POT = 1248,
  POWERUP = 1249,
  POWERUPCAPABLE = 1250,
  PROTOSHOPKEEPER = 1251,
  PUNISHBALL = 1252,
  PURCHASABLE = 1334,
  PUSHBLOCK = 1253,
  QILIN = 1254,
  QUICKSAND = 1255,
  QUICKSANDSOUND = 1256,
  QUILLBACK = 1257,
  REGENBLOCK = 1258,
  ROBOT = 1259,
  ROCKDOG = 1260,
  ROLLINGITEM = 1261,
  ROOMLIGHT = 1262,
  ROOMOWNER = 1263,
  RUBBLE = 1264,
  SCARAB = 1265,
  SCEPTERSHOT = 1266,
  SCORPION = 1267,
  SHIELD = 1268,
  SHOOTINGSTARSPAWNER = 1269,
  SHOPKEEPER = 1270,
  SKELETON = 1271,
  SKULLDROPTRAP = 1272,
  SLEEPBUBBLE = 1273,
  SLIDINGWALLCEILING = 1274,
  SNAPTRAP = 1275,
  SORCERESS = 1276,
  SOUNDSHOT = 1277,
  SPARK = 1278,
  SPARKTRAP = 1279,
  SPEAR = 1280,
  SPECIALSHOT = 1281,
  SPIDER = 1282,
  SPIKEBALLTRAP = 1283,
  SPLASHBUBBLEGENERATOR = 1284,
  STICKYTRAP = 1285,
  STRETCHCHAIN = 1286,
  SWITCH = 1287,
  TADPOLE = 1288,
  TELEPORTER = 1289,
  TELEPORTERBACKPACK = 1290,
  TELEPORTINGBORDER = 1291,
  TELESCOPE = 1292,
  TENTACLE = 1293,
  TENTACLEBOTTOM = 1294,
  TERRA = 1295,
  THINICE = 1296,
  TIAMAT = 1297,
  TIAMATSHOT = 1298,
  TIMEDFORCEFIELD = 1299,
  TIMEDPOWDERKEG = 1300,
  TIMEDSHOT = 1301,
  TORCH = 1302,
  TORCHFLAME = 1303,
  TOTEMTRAP = 1304,
  TRANSFERFLOOR = 1305,
  TRAPPART = 1306,
  TREASURE = 1307,
  TREASUREHOOK = 1308,
  TRUECROWNPOWERUP = 1310,
  TUN = 1311,
  TV = 1312,
  UDJATSOCKET = 1313,
  UFO = 1314,
  UNCHAINEDSPIKEBALL = 1315,
  USHABTI = 1316,
  VAMPIRE = 1317,
  VANHORSING = 1318,
  VLAD = 1319,
  VLADSCAPE = 1320,
  WADDLER = 1321,
  WALKINGMONSTER = 1322,
  WALLTORCH = 1323,
  WEBSHOT = 1324,
  WETEFFECT = 1325,
  WITCHDOCTOR = 1326,
  WITCHDOCTORSKULL = 1327,
  WOODENLOGTRAP = 1328,
  YAMA = 1329,
  YANG = 1330,
  YELLOWCAPE = 1331,
  YETIKING = 1332,
  YETIQUEEN = 1333
}
---@alias ENT_TYPE integer
FADE = {
  IN = 3,
  LOAD = 2,
  NONE = 0,
  OUT = 1
}
---@alias FADE integer
FEAT = {
  ARENA_CHAMPION = 22,
  AWAKENED = 12,
  A_SECOND_CHANCE = 30,
  CHOSEN_ONE = 31,
  DIVINE_RIGHT = 29,
  EXCAVATOR = 13,
  FEELS_GOOD = 3,
  HER_FAVORITE = 28,
  IRONMAN = 7,
  JOURNEYMAN = 6,
  LEGENDARY = 27,
  LOW_SCORER = 9,
  MAMAS_BIG_HELPER = 20,
  MAMAS_LITTLE_HELPER = 19,
  MASTER = 11,
  MILLIONAIRE = 16,
  PARENTHOOD = 32,
  PERSISTENT = 5,
  PILGRIM = 10,
  SEEN_A_LOT = 17,
  SEEN_IT_ALL = 18,
  SHADOW_SHOPPER = 26,
  SKILLS_IMPROVING = 4,
  SPEEDLUNKY = 8,
  SUPPORT_A_LOCAL_BUSINESS = 24,
  SURVIVOR = 15,
  THE_FULL_SPELUNKY = 1,
  TORCHBEARER = 14,
  TRACK_STAR = 21,
  TURKEY_WHISPERER = 23,
  VIP = 25,
  YOU_GOT_THIS = 2
}
---@alias FEAT integer
FLOOR_SIDE = {
  BOTTOM = 1,
  BOTTOM_LEFT = 6,
  BOTTOM_RIGHT = 7,
  LEFT = 2,
  RIGHT = 3,
  TOP = 0,
  TOP_LEFT = 4,
  TOP_RIGHT = 5
}
---@alias FLOOR_SIDE integer
GAMEPAD = {
  A = 4096,
  B = 8192,
  BACK = 32,
  DOWN = 2,
  LEFT = 4,
  LEFT_SHOULDER = 256,
  LEFT_THUMB = 64,
  RIGHT = 8,
  RIGHT_SHOULDER = 512,
  RIGHT_THUMB = 128,
  START = 16,
  UP = 1,
  X = 16384,
  Y = 32768
}
---@alias GAMEPAD integer
GAMEPAD_FLAG = {
  A = 13,
  B = 14,
  BACK = 6,
  DOWN = 2,
  LEFT = 3,
  LEFT_SHOULDER = 9,
  LEFT_THUMB = 7,
  RIGHT = 4,
  RIGHT_SHOULDER = 10,
  RIGHT_THUMB = 8,
  START = 5,
  UP = 1,
  X = 15,
  Y = 16
}
---@alias GAMEPAD_FLAG integer
GAME_MODE = {
  ARENA = 2,
  COOP = 1
}
---@alias GAME_MODE integer
GAME_SETTING = {
  ANGRY_SHOPKEEPER = 28,
  BRIGHTNESS = 10,
  BRIGHT_FLASHES = 41,
  BUTTON_PROMPTS = 30,
  BUTTON_TEXTURE = 31,
  CLASSIC_AGGRO_MUSIC = 29,
  CROSSPLAY = 42,
  CROSSPROGRESS_AUTOSYNC = 47,
  CROSSPROGRESS_ENABLED = 46,
  CURRENT_PROFILE = 18,
  DIALOG_TEXT = 37,
  FEAT_POPUPS = 32,
  FREQUENCY_DENOMINATOR = 5,
  FREQUENCY_NUMERATOR = 4,
  GHOST_TEXT = 39,
  HUD_SIZE = 24,
  HUD_STYLE = 23,
  INPUT_DELAY = 43,
  INSTANT_RESTART = 22,
  KALI_TEXT = 38,
  LANGUAGE = 40,
  LEVEL_FEELINGS = 36,
  LEVEL_NUMBER = 27,
  LEVEL_TIMER = 25,
  MASTER_ENABLED = 15,
  MASTER_VOLUME = 16,
  MONITOR = 8,
  MUSIC_ENABLED = 13,
  MUSIC_VOLUME = 14,
  OUTPUT_DELAY = 44,
  OVERSCAN = 17,
  PET_STYLE = 20,
  PREV_LANGUAGE = 19,
  PSEUDONYMIZATION = 45,
  RESOLUTIONX = 2,
  RESOLUTIONY = 3,
  RESOLUTION_SCALE = 1,
  SCREEN_SHAKE = 21,
  SOUND_ENABLED = 11,
  SOUND_VOLUME = 12,
  TEXTBOX_DURATION = 34,
  TEXTBOX_OPACITY = 35,
  TEXTBOX_SIZE = 33,
  TIMER_DETAIL = 26,
  VFX = 9,
  VSYNC = 7,
  WINDOW_MODE = 6,
  WINDOW_SCALE = 0
}
---@alias GAME_SETTING integer
GHOST_BEHAVIOR = {
  MEDIUM_HAPPY = 1,
  MEDIUM_SAD = 0,
  SAD = 0,
  SMALL_ANGRY = 0,
  SMALL_HAPPY = 3,
  SMALL_SAD = 2,
  SMALL_SURPRISED = 1
}
---@alias GHOST_BEHAVIOR integer
HOTKEY_TYPE = {
  GLOBAL = 1,
  INPUT = 2,
  NORMAL = 0
}
---@alias HOTKEY_TYPE integer
HUNDUNFLAGS = {
  BIRDHEADEMERGED = 2,
  BIRDHEADSHOTLAST = 16,
  SNAKEHEADEMERGED = 4,
  TOPLEVELARENAREACHED = 8,
  WILLMOVELEFT = 1
}
---@alias HUNDUNFLAGS integer
INPUTS = {
  BOMB = 4,
  DOOR = 32,
  DOWN = 2048,
  JOURNAL = 128,
  JUMP = 1,
  LEFT = 256,
  MENU = 64,
  NONE = 0,
  RIGHT = 512,
  ROPE = 8,
  RUN = 16,
  UP = 1024,
  WHIP = 2
}
---@alias INPUTS integer
INPUT_FLAG = {
  BOMB = 3,
  DOOR = 6,
  DOWN = 12,
  JOURNAL = 8,
  JUMP = 1,
  LEFT = 9,
  MENU = 7,
  RIGHT = 10,
  ROPE = 4,
  RUN = 5,
  UP = 11,
  WHIP = 2
}
---@alias INPUT_FLAG integer
JOURNALUI_PAGE_SHOWN = {
  BESTIARY = 5,
  DEATH = 11,
  FEATS = 9,
  ITEMS = 6,
  JOURNAL = 2,
  PEOPLE = 4,
  PLACES = 3,
  PLAYER_PROFILE = 1,
  RECAP = 10,
  STORY = 8,
  TRAPS = 7
}
---@alias JOURNALUI_PAGE_SHOWN integer
JOURNALUI_STATE = {
  FADING_IN = 1,
  FADING_OUT = 5,
  FLIPPING_LEFT = 3,
  FLIPPING_RIGHT = 4,
  INVISIBLE = 0,
  STABLE = 2
}
---@alias JOURNALUI_STATE integer
JOURNAL_FLAG = {
  ANKH = 15,
  COSMOS = 20,
  CRIME_LORD = 6,
  DIED = 21,
  EGGPLANT = 10,
  FOOL = 9,
  HUNDUN = 19,
  KING = 7,
  KINGU = 16,
  LIKED_PETS = 12,
  LOVED_PETS = 13,
  NO_GOLD = 11,
  OSIRIS = 17,
  PACIFIST = 1,
  PETTY_CRIMINAL = 4,
  QUEEN = 8,
  TIAMAT = 18,
  TOOK_DAMAGE = 14,
  VEGAN = 2,
  VEGETARIAN = 3,
  WANTED_CRIMINAL = 5
}
---@alias JOURNAL_FLAG integer
JOURNAL_PAGE_TYPE = {
  BESTIARY = 4,
  DEATH_CAUSE = 9,
  DEATH_MENU = 10,
  FEATS = 8,
  ITEMS = 5,
  JOURNAL_MENU = 1,
  LAST_GAME_PLAYED = 13,
  PEOPLE = 3,
  PLACES = 2,
  PLAYER_PROFILE = 12,
  PROGRESS = 0,
  RECAP = 11,
  STORY = 7,
  TRAPS = 6
}
---@alias JOURNAL_PAGE_TYPE integer
JUNGLESISTERS = {
  GREAT_PARTY_HUH = 5,
  I_WISH_BROUGHT_A_JACKET = 6,
  PARMESAN_RESCUED = 3,
  PARSLEY_RESCUED = 1,
  PARSNIP_RESCUED = 2,
  WARNING_ONE_WAY_DOOR = 4
}
---@alias JUNGLESISTERS integer
KEY = {
  ["0"] = 48,
  ["1"] = 49,
  ["2"] = 50,
  ["3"] = 51,
  ["4"] = 52,
  ["5"] = 53,
  ["6"] = 54,
  ["7"] = 55,
  ["8"] = 56,
  ["9"] = 57,
  A = 65,
  ADD = 107,
  ALT = 18,
  B = 66,
  BACKSPACE = 8,
  C = 67,
  CAPS = 20,
  CLEAR = 12,
  COMMA = 188,
  CTRL = 17,
  D = 68,
  DECIMAL = 110,
  DELETE = 46,
  DIVIDE = 111,
  DOWN = 40,
  E = 69,
  END = 35,
  ESCAPE = 27,
  EXECUTE = 43,
  F = 70,
  F1 = 112,
  F2 = 113,
  F3 = 114,
  F4 = 115,
  F5 = 116,
  F6 = 117,
  F7 = 118,
  F8 = 119,
  F9 = 120,
  F10 = 121,
  F11 = 122,
  F12 = 123,
  F13 = 124,
  F14 = 125,
  F15 = 126,
  F16 = 127,
  F17 = 128,
  F18 = 129,
  F19 = 130,
  F20 = 131,
  F21 = 132,
  F22 = 133,
  F23 = 134,
  F24 = 135,
  G = 71,
  H = 72,
  HOME = 36,
  I = 73,
  INSERT = 45,
  J = 74,
  K = 75,
  L = 76,
  LALT = 164,
  LCONTROL = 162,
  LEFT = 37,
  LSHIFT = 160,
  M = 77,
  MINUS = 189,
  MULTIPLY = 106,
  N = 78,
  NUMPAD0 = 96,
  NUMPAD1 = 97,
  NUMPAD2 = 98,
  NUMPAD3 = 99,
  NUMPAD4 = 100,
  NUMPAD5 = 101,
  NUMPAD6 = 102,
  NUMPAD7 = 103,
  NUMPAD8 = 104,
  NUMPAD9 = 105,
  O = 79,
  OEM_1 = 186,
  OEM_2 = 191,
  OEM_3 = 192,
  OEM_4 = 219,
  OEM_5 = 220,
  OEM_6 = 221,
  OEM_7 = 222,
  OEM_8 = 223,
  OEM_102 = 226,
  OL_MOD_ALT = 2048,
  OL_MOD_CTRL = 256,
  OL_MOD_SHIFT = 512,
  OL_MOUSE_1 = 1025,
  OL_MOUSE_2 = 1026,
  OL_MOUSE_3 = 1027,
  OL_MOUSE_4 = 1028,
  OL_MOUSE_5 = 1029,
  OL_MOUSE_WHEEL_DOWN = 1041,
  OL_MOUSE_WHEEL_UP = 1042,
  P = 80,
  PAUSE = 19,
  PERIOD = 190,
  PGDN = 34,
  PGUP = 33,
  PLUS = 187,
  PRINT = 42,
  Q = 81,
  R = 82,
  RALT = 165,
  RCONTROL = 163,
  RETURN = 13,
  RIGHT = 39,
  RSHIFT = 161,
  S = 83,
  SELECT = 41,
  SEPARATOR = 108,
  SHIFT = 16,
  SNAPSHOT = 44,
  SPACE = 32,
  SUBTRACT = 109,
  T = 84,
  TAB = 9,
  U = 85,
  UP = 38,
  V = 86,
  W = 87,
  X = 88,
  Y = 89,
  Z = 90
}
---@alias KEY integer
KEY_TYPE = {
  ANY = 0,
  KEYBOARD = 255,
  MOUSE = 1024
}
---@alias KEY_TYPE integer
LAYER = {
  BACK = 1,
  BOTH = -128,
  FRONT = 0,
  PLAYER = -1,
  PLAYER1 = -1,
  PLAYER2 = -2,
  PLAYER3 = -3,
  PLAYER4 = -4
}
---@alias LAYER integer
LEVEL_CONFIG = {
  ALTAR_ROOM_CHANCE = 5,
  BACKGROUND_CHANCE = 9,
  BACK_ROOM_CHANCE = 0,
  BACK_ROOM_HIDDEN_DOOR_CACHE_CHANCE = 3,
  BACK_ROOM_HIDDEN_DOOR_CHANCE = 2,
  BACK_ROOM_INTERCONNECTION_CHANCE = 1,
  FLAGGED_LIQUID_ROOMS = 16,
  FLOOR_BOTTOM_SPREAD_CHANCE = 8,
  FLOOR_SIDE_SPREAD_CHANCE = 7,
  GROUND_BACKGROUND_CHANCE = 10,
  IDOL_ROOM_CHANCE = 6,
  MACHINE_BIGROOM_CHANCE = 11,
  MACHINE_REWARDROOM_CHANCE = 14,
  MACHINE_TALLROOM_CHANCE = 13,
  MACHINE_WIDEROOM_CHANCE = 12,
  MAX_LIQUID_PARTICLES = 15,
  MOUNT_CHANCE = 4
}
---@alias LEVEL_CONFIG integer
LIGHT_TYPE = {
  FOLLOW_CAMERA = 1,
  FOLLOW_ENTITY = 2,
  NONE = 0,
  ROOM_LIGHT = 4
}
---@alias LIGHT_TYPE integer
LIQUID_POOL = {
  COARSE_LAVA = 4,
  COARSE_WATER = 2,
  LAVA = 3,
  STAGNANT_LAVA = 5,
  WATER = 1
}
---@alias LIQUID_POOL integer
LOGIC = {
  APEP = 15,
  ARENA_1 = 23,
  ARENA_2 = 24,
  ARENA_3 = 25,
  ARENA_ALIEN_BLAST = 26,
  ARENA_LOOSE_BOMBS = 27,
  BLACK_MARKET = 21,
  BUBBLER = 18,
  COG_SACRIFICE = 16,
  DICESHOP = 6,
  DISCOVERY_INFO = 20,
  DUAT_BOSSES = 17,
  GHOST = 3,
  GHOST_TOAST = 4,
  JELLYFISH = 22,
  MAGMAMAN_SPAWN = 11,
  MOON_CHALLENGE = 8,
  OLMEC_CUTSCENE = 13,
  OUROBOROS = 1,
  PLEASURE_PALACE = 19,
  PRE_CHALLENGE = 7,
  SPEEDRUN = 2,
  STAR_CHALLENGE = 9,
  SUN_CHALLENGE = 10,
  TIAMAT_CUTSCENE = 14,
  TUN_AGGRO = 5,
  TUTORIAL = 0,
  WATER_BUBBLES = 12
}
---@alias LOGIC integer
MASK = {
  ACTIVEFLOOR = 128,
  ANY = 0,
  BG = 1024,
  DECORATION = 512,
  EXPLOSION = 16,
  FLOOR = 256,
  FX = 64,
  ITEM = 8,
  LAVA = 16384,
  LIQUID = 24576,
  LOGICAL = 4096,
  MONSTER = 4,
  MOUNT = 2,
  PLAYER = 1,
  ROPE = 32,
  SHADOW = 2048,
  WATER = 8192
}
---@alias MASK integer
MENU_INPUT = {
  BACK = 2,
  DELETE = 4,
  DOWN = 256,
  JOURNAL = 16,
  LEFT = 32,
  NONE = 0,
  RANDOM = 8,
  RIGHT = 64,
  SELECT = 1,
  UP = 128
}
---@alias MENU_INPUT integer
ON = {
  ARENA_INTRO = 25,
  ARENA_ITEMS = 23,
  ARENA_MATCH = 26,
  ARENA_MENU = 21,
  ARENA_SCORE = 27,
  ARENA_SELECT = 24,
  ARENA_STAGES = 22,
  BLOCKED_GAME_LOOP = 162,
  BLOCKED_PROCESS_INPUT = 163,
  BLOCKED_UPDATE = 161,
  CAMP = 11,
  CHARACTER_SELECT = 9,
  CONSTELLATION = 19,
  CREDITS = 17,
  DEATH = 14,
  DEATH_MESSAGE = 137,
  FRAME = 101,
  GAMEFRAME = 108,
  GUIFRAME = 100,
  INTRO = 1,
  LEADERBOARD = 7,
  LEVEL = 12,
  LOAD = 107,
  LOADING = 104,
  LOGO = 0,
  MENU = 4,
  ONLINE_LOADING = 28,
  ONLINE_LOBBY = 29,
  OPTIONS = 5,
  PLAYER_PROFILE = 6,
  POST_GAME_LOOP = 156,
  POST_LAYER_CREATION = 148,
  POST_LAYER_DESTRUCTION = 152,
  POST_LEVEL_CREATION = 146,
  POST_LEVEL_DESTRUCTION = 150,
  POST_LEVEL_GENERATION = 112,
  POST_LOAD_JOURNAL_CHAPTER = 139,
  POST_LOAD_SCREEN = 136,
  POST_LOAD_STATE = 160,
  POST_PROCESS_INPUT = 154,
  POST_ROOM_GENERATION = 111,
  POST_SAVE_STATE = 158,
  POST_UPDATE = 143,
  PRE_GAME_LOOP = 155,
  PRE_GET_FEAT = 140,
  PRE_GET_RANDOM_ROOM = 113,
  PRE_HANDLE_ROOM_TILES = 114,
  PRE_LAYER_CREATION = 147,
  PRE_LAYER_DESTRUCTION = 151,
  PRE_LEVEL_CREATION = 145,
  PRE_LEVEL_DESTRUCTION = 149,
  PRE_LEVEL_GENERATION = 110,
  PRE_LOAD_JOURNAL_CHAPTER = 138,
  PRE_LOAD_LEVEL_FILES = 109,
  PRE_LOAD_SCREEN = 135,
  PRE_LOAD_STATE = 159,
  PRE_PROCESS_INPUT = 153,
  PRE_SAVE_STATE = 157,
  PRE_SET_FEAT = 141,
  PRE_UPDATE = 142,
  PROLOGUE = 2,
  RECAP = 20,
  RENDER_POST_BLURRED_BACKGROUND = 122,
  RENDER_POST_DRAW_DEPTH = 124,
  RENDER_POST_GAME = 132,
  RENDER_POST_HUD = 118,
  RENDER_POST_JOURNAL_PAGE = 126,
  RENDER_POST_LAYER = 128,
  RENDER_POST_LEVEL = 130,
  RENDER_POST_PAUSE_MENU = 120,
  RENDER_PRE_BLURRED_BACKGROUND = 121,
  RENDER_PRE_DRAW_DEPTH = 123,
  RENDER_PRE_GAME = 131,
  RENDER_PRE_HUD = 117,
  RENDER_PRE_JOURNAL_PAGE = 125,
  RENDER_PRE_LAYER = 127,
  RENDER_PRE_LEVEL = 129,
  RENDER_PRE_PAUSE_MENU = 119,
  RESET = 105,
  SAVE = 106,
  SCORES = 18,
  SCREEN = 102,
  SCRIPT_DISABLE = 116,
  SCRIPT_ENABLE = 115,
  SEED_INPUT = 8,
  SPACESHIP = 15,
  SPEECH_BUBBLE = 133,
  START = 103,
  TEAM_SELECT = 10,
  TITLE = 3,
  TOAST = 134,
  TRANSITION = 13,
  USER_DATA = 144,
  WIN = 16
}
---@alias ON integer
PARTICLEEMITTER = {
  ACIDBUBBLEBURST_BUBBLES = 101,
  ACIDBUBBLEBURST_SPARKS = 102,
  ALIENBLAST_SHOCKWAVE = 179,
  ALTAR_MONSTER_APPEAR_POOF = 162,
  ALTAR_SKULL = 95,
  ALTAR_SMOKE = 96,
  ALTAR_SPARKS = 97,
  APEP_DUSTWALL = 158,
  ARROWPOOF = 67,
  AU_GOLD_SPARKLES = 74,
  AXOLOTL_BIGBUBBLEKILL = 186,
  AXOLOTL_SMALLBUBBLEKILL = 185,
  BLAST_PLASMAWARP_TRAIL = 137,
  BLOODTRAIL = 64,
  BLUESPARKS = 106,
  BOMB_SMOKE = 24,
  BOOMERANG_TRAIL = 172,
  BROKENORB_BLAST_LARGE = 204,
  BROKENORB_BLAST_MEDIUM = 203,
  BROKENORB_BLAST_SMALL = 202,
  BROKENORB_ORBS_LARGE = 210,
  BROKENORB_ORBS_MEDIUM = 209,
  BROKENORB_ORBS_SMALL = 208,
  BROKENORB_SHOCKWAVE_LARGE = 216,
  BROKENORB_SHOCKWAVE_MEDIUM = 215,
  BROKENORB_SHOCKWAVE_SMALL = 214,
  BROKENORB_SPARKS_LARGE = 213,
  BROKENORB_SPARKS_MEDIUM = 212,
  BROKENORB_SPARKS_SMALL = 211,
  BROKENORB_WARP_LARGE = 207,
  BROKENORB_WARP_MEDIUM = 206,
  BROKENORB_WARP_SMALL = 205,
  BULLETPOOF = 66,
  CAMERA_FRAME = 105,
  CAVEMAN_SPITTLE = 190,
  CHARSELECTOR_MIST = 197,
  CHARSELECTOR_TORCHFLAME_FLAMES = 8,
  CHARSELECTOR_TORCHFLAME_SMOKE = 7,
  CLOVER_WITHER_HUD = 78,
  COFFINDOORPOOF_SPARKS = 141,
  COG_SPARKLE = 80,
  COG_TUNNEL_FOG = 81,
  COLLECTPOOF_CLOUDS = 99,
  COLLECTPOOF_SPARKS = 98,
  CONTACTEFFECT_SPARKS = 131,
  COOKFIRE_FLAMES = 34,
  COOKFIRE_SMOKE = 33,
  COOKFIRE_WARP = 35,
  CRUSHTRAPPOOF = 58,
  CURSEDEFFECT_PIECES = 116,
  CURSEDEFFECT_PIECES_HUD = 117,
  CURSEDEFFECT_SKULL = 118,
  CURSEDPOT_BEHINDSMOKE = 47,
  CURSEDPOT_SMOKE = 46,
  DMCOUNTDOWN_BLAST = 21,
  DMCOUNTDOWN_DUST = 17,
  DMCOUNTDOWN_FLAMES = 20,
  DMCOUNTDOWN_FOG = 23,
  DMCOUNTDOWN_HIGH_TENSION_THUNDERBOLT = 22,
  DMCOUNTDOWN_RUBBLES = 15,
  DMCOUNTDOWN_RUBBLES_LARGE = 16,
  DMCOUNTDOWN_SPARKS = 18,
  DMCOUNTDOWN_SPARKS_SMALL = 19,
  DMPREMATCH_ASH_2P = 151,
  DMPREMATCH_ASH_3P = 152,
  DMPREMATCH_ASH_4P = 153,
  DMPREMATCH_SEPARATOR_GLOW_TRAIL = 201,
  DMRESULTS_ASH = 150,
  DMRESULT_BLOOD = 13,
  DMRESULT_MEATPIECES = 14,
  DM_DEATH_MIST = 178,
  DUSTWALL = 157,
  EGGSHIP_SMOKE = 43,
  ELIXIREFFECT_WORNOUT = 119,
  ENDINGTREASURE_DUST = 45,
  ENDING_TREASURE_HUNDUN_SPARKLE = 73,
  ENDING_TREASURE_TIAMAT_SPARKLE = 72,
  EVAPORATION_WATER = 144,
  EXPLOSION_SHOCKWAVE = 27,
  EXPLOSION_SMOKE = 25,
  EXPLOSION_SPARKS = 26,
  EXPLOSION_WHITESMOKE = 28,
  FIREBALL_DESTROYED = 42,
  FIREBALL_TRAIL = 41,
  FLAMETHROWER_SMOKE = 40,
  FLAMETRAIL_FLAMES = 82,
  FLAMETRAIL_SMOKE = 83,
  FLOORDUST = 48,
  FLOORFALLINGDUST_RUBBLE = 50,
  FLOORFALLINGDUST_SMOKE = 49,
  FLOORPOOF = 52,
  FLOORPOOF_BIG = 54,
  FLOORPOOF_SMALL = 53,
  FLOORPOOF_TRAIL = 59,
  FLOORPOOF_TRAIL_BIG = 60,
  FLYPOOF = 56,
  FROST_BREATH = 198,
  GASTRAIL = 142,
  GASTRAIL_BIG = 143,
  GHOST_FOG = 92,
  GHOST_MIST = 90,
  GHOST_WARP = 91,
  GREENBLOODTRAIL = 65,
  GRUB_TRAIL = 174,
  HIGH_TENSION_THUNDERBOLT = 191,
  HITEFFECT_HALO = 126,
  HITEFFECT_RING = 124,
  HITEFFECT_SMACK = 125,
  HITEFFECT_SPARKS = 120,
  HITEFFECT_SPARKS_BIG = 121,
  HITEFFECT_STARS_BIG = 123,
  HITEFFECT_STARS_SMALL = 122,
  HORIZONTALLASERBEAM_SPARKLES = 164,
  HORIZONTALLASERBEAM_SPARKLES_END = 166,
  HORIZONTALLASERBEAM_SPARKS = 168,
  ICECAGE_MIST = 156,
  ICECAVES_DIAMONDDUST = 154,
  ICEFLOOR_MIST = 155,
  ICESPIRETRAIL_SPARKLES = 108,
  INKSPIT_BUBBLEBURST = 104,
  INKSPIT_TRAIL = 103,
  ITEMDUST = 62,
  ITEM_CRUSHED_SPARKS = 79,
  JETPACK_LITTLEFLAME = 85,
  JETPACK_SMOKETRAIL = 84,
  KINGUDUST = 170,
  KINGUSLIDINGDUST = 171,
  LAMASSU_AIMING_SPARKLES = 195,
  LAMASSU_SHOT_SPARKLES = 194,
  LAMASSU_SHOT_WARP = 193,
  LARGEITEMDUST = 63,
  LASERBEAM_CONTACT = 169,
  LASERBEAM_SPARKLES = 163,
  LASERBEAM_SPARKLES_END = 165,
  LASERBEAM_SPARKS = 167,
  LAVAHEAT = 146,
  LAVAPOT_DRIP = 187,
  LEVEL_MIST = 192,
  LIONTRAP_SPARKLE = 77,
  MAGMAMANHEAT = 147,
  MAINMENU_CEILINGDUST_RUBBLE = 10,
  MAINMENU_CEILINGDUST_RUBBLE_SMALL = 12,
  MAINMENU_CEILINGDUST_SMOKE = 9,
  MAINMENU_CEILINGDUST_SMOKE_SMALL = 11,
  MERCHANT_APPEAR_POOF = 161,
  MINIGAME_ASTEROID_DUST = 217,
  MINIGAME_ASTEROID_DUST_SMALL = 218,
  MINIGAME_BROKENASTEROID_SMOKE = 220,
  MINIGAME_UFO_SMOKE = 219,
  MOLEFLOORPOOF = 61,
  MOUNT_TAMED = 159,
  MUSIC_NOTES = 199,
  NECROMANCER_SUMMON = 184,
  NOHITEFFECT_RING = 129,
  NOHITEFFECT_SMACK = 130,
  NOHITEFFECT_SPARKS = 127,
  NOHITEFFECT_STARS = 128,
  OLMECFLOORPOOF = 57,
  OLMECSHIP_HOLE_DUST = 44,
  ONFIREEFFECT_FLAME = 111,
  ONFIREEFFECT_SMOKE = 110,
  OUROBORO_EMBERS = 89,
  OUROBORO_FALLING_RUBBLE = 51,
  OUROBORO_MIST = 88,
  PETTING_PET = 160,
  PINKSPARKS = 107,
  PLAYERGHOST_FREEZESPARKLES = 93,
  POISONEDEFFECT_BUBBLES_BASE = 112,
  POISONEDEFFECT_BUBBLES_BURST = 113,
  POISONEDEFFECT_BUBBLES_HUD = 114,
  POISONEDEFFECT_SKULL = 115,
  PORTAL_DUST_FAST = 176,
  PORTAL_DUST_SLOW = 175,
  PORTAL_WARP = 177,
  PRIZEAPPEARING_CLOUDS = 100,
  SANDFLOORPOOF = 55,
  SCEPTERKILL_SPARKLES = 134,
  SCEPTERKILL_SPARKS = 135,
  SCEPTER_BLAST = 136,
  SHOTGUNBLAST_SMOKE = 86,
  SHOTGUNBLAST_SPARKS = 87,
  SMALLFLAME_FLAMES = 37,
  SMALLFLAME_SMOKE = 36,
  SMALLFLAME_WARP = 38,
  SPARKTRAP_TRAIL = 200,
  SPLASH_WATER = 145,
  TELEPORTEFFECT_GREENSPARKLES = 139,
  TELEPORTEFFECT_REDSPARKLES = 140,
  TELEPORTEFFECT_SPARKS = 138,
  TIAMAT_SCREAM_WARP = 196,
  TITLE_TORCHFLAME_ASH = 6,
  TITLE_TORCHFLAME_BACKFLAMES = 2,
  TITLE_TORCHFLAME_BACKFLAMES_ANIMATED = 4,
  TITLE_TORCHFLAME_FLAMES = 3,
  TITLE_TORCHFLAME_FLAMES_ANIMATED = 5,
  TITLE_TORCHFLAME_SMOKE = 1,
  TOMB_FOG = 94,
  TORCHFLAME_FLAMES = 31,
  TORCHFLAME_IGNITION_SPARK = 29,
  TORCHFLAME_SMOKE = 30,
  TORCHFLAME_WARP = 32,
  TREASURE_SPARKLE_HIGH = 70,
  TREASURE_SPARKLE_HUD = 71,
  TREASURE_SPARKLE_LOW = 68,
  TREASURE_SPARKLE_MEDIUM = 69,
  UFOLASERSHOTHITEFFECT_BIG = 132,
  UFOLASERSHOTHITEFFECT_SMALL = 133,
  USHABTI_GOLD = 75,
  USHABTI_JADE = 76,
  VOLCANO_ASH = 149,
  VOLCANO_FOG = 148,
  WATER_DROP_DESTROYED = 188,
  WATER_DROP_DESTROYED_UPWARDS = 189,
  WETEFFECT_DROPS = 109,
  WHIPFLAME_FLAMES = 39,
  WITCHDOCTORSKULL_TRAIL = 173,
  YETIKING_YELL_DUST = 181,
  YETIKING_YELL_FOG = 180,
  YETIKING_YELL_SPARKLES = 182,
  YETIQUEEN_LANDING_SNOWDUST = 183
}
---@alias PARTICLEEMITTER integer
PAUSE = {
  ANKH = 32,
  CUTSCENE = 4,
  FADE = 2,
  FLAG4 = 8,
  FLAG5 = 16,
  MENU = 1
}
---@alias PAUSE integer
PAUSEUI_VISIBILITY = {
  INVISIBLE = 0,
  SLIDING_DOWN = 1,
  SLIDING_UP = 3,
  VISIBLE = 2
}
---@alias PAUSEUI_VISIBILITY integer
PAUSE_SCREEN = {
  ARENA_INTRO = 33554432,
  ARENA_ITEMS = 8388608,
  ARENA_LEVEL = 67108864,
  ARENA_MENU = 2097152,
  ARENA_SCORE = 134217728,
  ARENA_SELECT = 16777216,
  ARENA_STAGES = 4194304,
  CAMP = 2048,
  CHARACTER_SELECT = 512,
  CONSTELLATION = 524288,
  CREDITS = 131072,
  DEATH = 16384,
  EXIT = -2147483648,
  INTRO = 2,
  LEADERBOARD = 128,
  LEVEL = 4096,
  LOADING = 1073741824,
  LOGO = 1,
  MENU = 16,
  NONE = 0,
  ONLINE_LOADING = 268435456,
  ONLINE_LOBBY = 536870912,
  OPTIONS = 32,
  PLAYER_PROFILE = 64,
  PROLOGUE = 4,
  RECAP = 1048576,
  SCORES = 262144,
  SEED_INPUT = 256,
  SPACESHIP = 32768,
  TEAM_SELECT = 1024,
  TITLE = 8,
  TRANSITION = 8192,
  WIN = 65536
}
---@alias PAUSE_SCREEN integer
PAUSE_TRIGGER = {
  EXIT = 8,
  FADE_END = 2,
  FADE_START = 1,
  NONE = 0,
  ONCE = 16,
  SCREEN = 4
}
---@alias PAUSE_TRIGGER integer
PAUSE_TYPE = {
  ANKH = 32,
  CUTSCENE = 4,
  FADE = 2,
  FLAG4 = 8,
  FLAG5 = 16,
  FORCE_STATE = 512,
  MENU = 1,
  NONE = 0,
  PRE_GAME_LOOP = 128,
  PRE_PROCESS_INPUT = 256,
  PRE_UPDATE = 64
}
---@alias PAUSE_TYPE integer
PLATFORM = {
  DISCORD = 17,
  NONE = 16,
  PLAYSTATION = 48,
  STEAM = 18,
  SWITCH = 32,
  XBOX = 19
}
---@alias PLATFORM integer
POS_TYPE = {
  AIR = 4,
  ALCOVE = 16,
  CEILING = 2,
  DEFAULT = 4096,
  EMPTY = 1024,
  FLOOR = 1,
  HOLE = 64,
  LAVA = 256,
  PIT = 32,
  SAFE = 512,
  SOLID = 2048,
  WALL = 8,
  WALL_LEFT = 8192,
  WALL_RIGHT = 16384,
  WATER = 128
}
---@alias POS_TYPE integer
PRESENCE_FLAG = {
  BLACK_MARKET = 2,
  DRILL = 3,
  MOON_CHALLENGE = 9,
  STAR_CHALLENGE = 10,
  SUN_CHALLENGE = 11,
  UDJAT_EYE = 1,
  VLADS_CASTLE = 3
}
---@alias PRESENCE_FLAG integer
PRNG_CLASS = {
  AI = 5,
  CHAR_AI = 1,
  ENTITY_VARIATION = 3,
  EXTRA_SPAWNS = 5,
  FX = 9,
  LEVEL_DECO = 8,
  LEVEL_GEN = 0,
  LIQUID = 7,
  PARTICLES = 2,
  PROCEDURAL_SPAWNS = 0
}
---@alias PRNG_CLASS integer
PROCEDURAL_CHANCE = {
  ADD_GOLD_BAR = 444,
  ADD_GOLD_BARS = 445,
  ARROWTRAP_CHANCE = 0,
  BAT = 224,
  BEE = 278,
  BEEHIVE_CHANCE = 16,
  BIGSPEARTRAP_CHANCE = 11,
  CAT = 251,
  CAVEMAN = 225,
  CHAIN_BLOCKS_CHANCE = 6,
  COBRA = 248,
  CRABMAN = 311,
  CRITTERANCHOVY = 335,
  CRITTERBUTTERFLY = 332,
  CRITTERCRAB = 336,
  CRITTERDRONE = 340,
  CRITTERDUNGBEETLE = 331,
  CRITTERFIREFLY = 339,
  CRITTERFISH = 334,
  CRITTERLOCUST = 337,
  CRITTERPENGUIN = 338,
  CRITTERSLIME = 341,
  CRITTERSNAIL = 333,
  CROCMAN = 247,
  CRUSHER_TRAP_CHANCE = 7,
  DIAMOND = 446,
  EGGSAC_CHANCE = 14,
  ELEVATOR = 443,
  EMERALD = 447,
  FEMALE_JIANGSHI = 261,
  FIREBUG = 241,
  FIREFROG = 284,
  FISH = 262,
  FROG = 283,
  GIANTFLY = 288,
  GIANTSPIDER = 223,
  HANGSPIDER = 222,
  HERMITCRAB = 264,
  HORNEDLIZARD = 230,
  IMP = 243,
  JIANGSHI = 260,
  JUNGLE_SPEAR_TRAP_CHANCE = 4,
  LANDMINE = 439,
  LASERTRAP_CHANCE = 9,
  LAVAMANDER = 244,
  LEPRECHAUN = 310,
  LEPRECHAUN_CHANCE = 17,
  LIONTRAP_CHANCE = 8,
  LITWALLTORCH = 442,
  MANTRAP = 233,
  MINISTER_CHANCE = 15,
  MOLE = 231,
  MONKEY = 238,
  MOSQUITO = 237,
  NECROMANCER = 252,
  OCTOPUS = 263,
  OLMITE = 276,
  PUSHBLOCK_CHANCE = 2,
  RED_SKELETON = 440,
  ROBOT = 240,
  RUBY = 449,
  SAPPHIRE = 448,
  SKULLDROP_CHANCE = 13,
  SNAKE = 220,
  SNAP_TRAP_CHANCE = 3,
  SORCERESS = 250,
  SPARKTRAP_CHANCE = 10,
  SPIDER = 221,
  SPIKE_BALL_CHANCE = 5,
  SPRINGTRAP = 73,
  STICKYTRAP_CHANCE = 12,
  TADPOLE = 287,
  TIKIMAN = 234,
  TOTEMTRAP_CHANCE = 1,
  UFO = 266,
  VAMPIRE = 245,
  WALLTORCH = 441,
  WITCHDOCTOR = 235,
  YETI = 268
}
---@alias PROCEDURAL_CHANCE integer
QUEST_FLAG = {
  BLACK_MARKET_SPAWNED = 18,
  CAVEMAN_SHOPPIE_AGGROED = 9,
  DAILY = 8,
  DARK_LEVEL_SPAWNED = 2,
  DRILL_SPAWNED = 19,
  EGGPLANT_CROWN_PICKED_UP = 12,
  MOON_CHALLENGE_SPAWNED = 25,
  RESET = 1,
  SEEDED = 7,
  SHOP_BOUGHT_OUT = 11,
  SHOP_SPAWNED = 5,
  SHORTCUT_USED = 6,
  SPAWN_OUTPOST = 4,
  STAR_CHALLENGE_SPAWNED = 26,
  SUN_CHALLENGE_SPAWNED = 27,
  UDJAT_EYE_SPAWNED = 17,
  VAULT_SPAWNED = 3,
  WADDLER_AGGROED = 10
}
---@alias QUEST_FLAG integer
RAW_BUTTON = {
  A = 4,
  B = 5,
  BACK = 14,
  DOWN = 1,
  LEFT = 2,
  LEFT_SHOULDER = 8,
  LEFT_THUMB = 12,
  LEFT_TRIGGER = 10,
  RIGHT = 3,
  RIGHT_SHOULDER = 9,
  RIGHT_THUMB = 13,
  RIGHT_TRIGGER = 11,
  START = 15,
  UP = 0,
  X = 6,
  Y = 7
}
---@alias RAW_BUTTON integer
RAW_DUALSHOCK = {
  CIRCLE = 5,
  CROSS = 4,
  DOWN = 1,
  L1 = 8,
  L2 = 10,
  L3 = 12,
  LEFT = 2,
  OPTIONS = 15,
  R1 = 9,
  R2 = 11,
  R3 = 13,
  RIGHT = 3,
  SHARE = 14,
  SQUARE = 6,
  TRIANGLE = 7,
  UP = 0
}
---@alias RAW_DUALSHOCK integer
RAW_KEY = {
  ["0"] = 65,
  ["1"] = 66,
  ["2"] = 67,
  ["3"] = 68,
  ["4"] = 69,
  ["5"] = 70,
  ["6"] = 71,
  ["7"] = 72,
  ["8"] = 73,
  ["9"] = 74,
  A = 7,
  ADD = 86,
  ALT = 37,
  B = 8,
  BACKSPACE = 40,
  C = 9,
  COMMA = 93,
  CTRL = 33,
  D = 10,
  DECIMAL = 90,
  DELETE = 104,
  DIVIDE = 88,
  DOWN = 1,
  E = 11,
  END = 106,
  ESCAPE = 4,
  F = 12,
  F1 = 41,
  F2 = 42,
  F3 = 43,
  F4 = 44,
  F5 = 45,
  F6 = 46,
  F7 = 47,
  F8 = 48,
  F9 = 49,
  F10 = 50,
  F11 = 51,
  F12 = 52,
  G = 13,
  H = 14,
  HOME = 108,
  I = 15,
  INSERT = 103,
  J = 16,
  K = 17,
  L = 18,
  LALT = 37,
  LCONTROL = 33,
  LEFT = 2,
  LSHIFT = 35,
  M = 19,
  MINUS = 92,
  MULTIPLY = 87,
  N = 20,
  NUMPAD0 = 75,
  NUMPAD1 = 76,
  NUMPAD2 = 77,
  NUMPAD3 = 78,
  NUMPAD4 = 79,
  NUMPAD5 = 80,
  NUMPAD6 = 81,
  NUMPAD7 = 82,
  NUMPAD8 = 83,
  NUMPAD9 = 84,
  NUMPADENTER = 110,
  O = 21,
  P = 22,
  PERIOD = 94,
  PGDN = 105,
  PGUP = 107,
  PLUS = 98,
  Q = 23,
  R = 24,
  RALT = 38,
  RCONTROL = 34,
  RETURN = 5,
  RIGHT = 3,
  RSHIFT = 36,
  S = 25,
  SHIFT = 35,
  SPACE = 39,
  SUBTRACT = 85,
  T = 26,
  TAB = 6,
  U = 27,
  UP = 0,
  V = 28,
  W = 29,
  X = 30,
  Y = 31,
  Z = 32
}
---@alias RAW_KEY integer
READY_STATE = {
  NOT_READY = 0,
  READY = 1,
  SEARCHING = 2
}
---@alias READY_STATE integer
RECURSIVE_MODE = {
  EXCLUSIVE = 0,
  INCLUSIVE = 1,
  NONE = 2
}
---@alias RECURSIVE_MODE integer
RENDER_INFO_OVERRIDE = {
  DRAW = 1,
  DTOR = 0,
  RENDER = 3
}
---@alias RENDER_INFO_OVERRIDE integer
REPEAT_TYPE = {
  BACK_AND_FORTH = 2,
  LINEAR = 1,
  NO_REPEAT = 0
}
---@alias REPEAT_TYPE integer
ROOM_TEMPLATE = {
  ABZU_BACKDOOR = 132,
  ALTAR = 115,
  ANUBIS_ROOM = 38,
  APEP = 128,
  BEEHIVE = 31,
  BEEHIVE_ENTRANCE = 32,
  BLACKMARKET = 118,
  BLACKMARKET_COFFIN = 34,
  BLACKMARKET_ENTRANCE = 33,
  BLACKMARKET_EXIT = 35,
  BOSS_ARENA = 22,
  CACHE = 14,
  CAVEMANSHOP = 79,
  CAVEMANSHOP_LEFT = 80,
  CHALLANGE_ENTRANCE_BACKLAYER = 90,
  CHALLENGE_0_0 = 92,
  CHALLENGE_0_1 = 93,
  CHALLENGE_0_2 = 94,
  CHALLENGE_0_3 = 95,
  CHALLENGE_1_0 = 96,
  CHALLENGE_1_1 = 97,
  CHALLENGE_1_2 = 98,
  CHALLENGE_1_3 = 99,
  CHALLENGE_BOTTOM = 91,
  CHALLENGE_ENTRANCE = 81,
  CHALLENGE_ENTRANCE_LEFT = 82,
  CHALLENGE_SPECIAL = 100,
  CHUNK_AIR = 17,
  CHUNK_DOOR = 18,
  CHUNK_GROUND = 16,
  COFFIN_FROG = 111,
  COFFIN_PLAYER = 25,
  COFFIN_PLAYER_VERTICAL = 26,
  COFFIN_UNLOCKABLE = 27,
  COFFIN_UNLOCKABLE_LEFT = 28,
  COG_ALTAR_TOP = 125,
  CRASHEDSHIP_ENTRANCE = 61,
  CRASHEDSHIP_ENTRANCE_NOTOP = 62,
  CURIOSHOP = 77,
  CURIOSHOP_LEFT = 78,
  DICESHOP = 75,
  DICESHOP_LEFT = 76,
  EMPRESS_GRAVE = 136,
  EMPTY_BACKLAYER = 9,
  ENTRANCE = 5,
  ENTRANCE_DROP = 6,
  EXIT = 7,
  EXIT_NOTOP = 8,
  FEELING_FACTORY = 104,
  FEELING_PRISON = 105,
  FEELING_TOMB = 106,
  GHISTROOM = 15,
  GHISTSHOP = 135,
  GHISTSHOP_BACKLAYER = 87,
  IDOL = 116,
  IDOL_TOP = 117,
  LAKEOFFIRE_BACK_ENTRANCE = 36,
  LAKEOFFIRE_BACK_EXIT = 37,
  LAKE_EXIT = 129,
  LAKE_NORMAL = 130,
  LAKE_NOTOP = 131,
  MACHINE_BIGROOM_PATH = 102,
  MACHINE_BIGROOM_SIDE = 103,
  MACHINE_KEYROOM = 113,
  MACHINE_REWARDROOM = 114,
  MACHINE_TALLROOM_PATH = 109,
  MACHINE_TALLROOM_SIDE = 110,
  MACHINE_WIDEROOM_PATH = 107,
  MACHINE_WIDEROOM_SIDE = 108,
  MOAI = 39,
  MOTHERSHIP_COFFIN = 41,
  MOTHERSHIP_ENTRANCE = 40,
  MOTHERSHIP_EXIT = 127,
  MOTHERSHIP_ROOM = 125,
  MOTHERSTATUE_ROOM = 43,
  OLDHUNTER_CURSEDROOM = 139,
  OLDHUNTER_KEYROOM = 137,
  OLDHUNTER_REWARDROOM = 138,
  OLMECSHIP_ROOM = 42,
  PALACEOFPLEASURE_0_0 = 49,
  PALACEOFPLEASURE_0_1 = 50,
  PALACEOFPLEASURE_0_2 = 51,
  PALACEOFPLEASURE_1_0 = 52,
  PALACEOFPLEASURE_1_1 = 53,
  PALACEOFPLEASURE_1_2 = 54,
  PALACEOFPLEASURE_2_0 = 55,
  PALACEOFPLEASURE_2_1 = 56,
  PALACEOFPLEASURE_2_2 = 57,
  PALACEOFPLEASURE_3_0 = 58,
  PALACEOFPLEASURE_3_1 = 59,
  PALACEOFPLEASURE_3_2 = 60,
  PASSAGE_HORZ = 11,
  PASSAGE_TURN = 13,
  PASSAGE_VERT = 12,
  PATH_DROP = 2,
  PATH_DROP_NOTOP = 4,
  PATH_NORMAL = 1,
  PATH_NOTOP = 3,
  PEN_ROOM = 45,
  POSSE = 24,
  QUEST_THIEF1 = 140,
  QUEST_THIEF2 = 141,
  ROOM2 = 10,
  SETROOM0_0 = 142,
  SETROOM0_1 = 142,
  SETROOM0_2 = 142,
  SETROOM0_3 = 142,
  SETROOM0_4 = 142,
  SETROOM0_5 = 142,
  SETROOM0_6 = 142,
  SETROOM0_7 = 142,
  SETROOM1_0 = 142,
  SETROOM1_1 = 142,
  SETROOM1_2 = 142,
  SETROOM1_3 = 142,
  SETROOM1_4 = 142,
  SETROOM1_5 = 142,
  SETROOM1_6 = 142,
  SETROOM1_7 = 142,
  SETROOM2_0 = 142,
  SETROOM2_1 = 142,
  SETROOM2_2 = 142,
  SETROOM2_3 = 142,
  SETROOM2_4 = 142,
  SETROOM2_5 = 142,
  SETROOM2_6 = 142,
  SETROOM2_7 = 142,
  SETROOM3_0 = 142,
  SETROOM3_1 = 142,
  SETROOM3_2 = 142,
  SETROOM3_3 = 142,
  SETROOM3_4 = 142,
  SETROOM3_5 = 142,
  SETROOM3_6 = 142,
  SETROOM3_7 = 142,
  SETROOM4_0 = 142,
  SETROOM4_1 = 142,
  SETROOM4_2 = 142,
  SETROOM4_3 = 142,
  SETROOM4_4 = 142,
  SETROOM4_5 = 142,
  SETROOM4_6 = 142,
  SETROOM4_7 = 142,
  SETROOM5_0 = 142,
  SETROOM5_1 = 142,
  SETROOM5_2 = 142,
  SETROOM5_3 = 142,
  SETROOM5_4 = 142,
  SETROOM5_5 = 142,
  SETROOM5_6 = 142,
  SETROOM5_7 = 142,
  SETROOM6_0 = 142,
  SETROOM6_1 = 142,
  SETROOM6_2 = 142,
  SETROOM6_3 = 142,
  SETROOM6_4 = 142,
  SETROOM6_5 = 142,
  SETROOM6_6 = 142,
  SETROOM6_7 = 142,
  SETROOM7_0 = 142,
  SETROOM7_1 = 142,
  SETROOM7_2 = 142,
  SETROOM7_3 = 142,
  SETROOM7_4 = 142,
  SETROOM7_5 = 142,
  SETROOM7_6 = 142,
  SETROOM7_7 = 142,
  SETROOM8_0 = 142,
  SETROOM8_1 = 142,
  SETROOM8_2 = 142,
  SETROOM8_3 = 142,
  SETROOM8_4 = 142,
  SETROOM8_5 = 142,
  SETROOM8_6 = 142,
  SETROOM8_7 = 142,
  SETROOM9_0 = 142,
  SETROOM9_1 = 142,
  SETROOM9_2 = 142,
  SETROOM9_3 = 142,
  SETROOM9_4 = 142,
  SETROOM9_5 = 142,
  SETROOM9_6 = 142,
  SETROOM9_7 = 142,
  SETROOM10_0 = 142,
  SETROOM10_1 = 142,
  SETROOM10_2 = 142,
  SETROOM10_3 = 142,
  SETROOM10_4 = 142,
  SETROOM10_5 = 142,
  SETROOM10_6 = 142,
  SETROOM10_7 = 142,
  SETROOM11_0 = 142,
  SETROOM11_1 = 142,
  SETROOM11_2 = 142,
  SETROOM11_3 = 142,
  SETROOM11_4 = 142,
  SETROOM11_5 = 142,
  SETROOM11_6 = 142,
  SETROOM11_7 = 142,
  SETROOM12_0 = 142,
  SETROOM12_1 = 142,
  SETROOM12_2 = 142,
  SETROOM12_3 = 142,
  SETROOM12_4 = 142,
  SETROOM12_5 = 142,
  SETROOM12_6 = 142,
  SETROOM12_7 = 142,
  SETROOM13_0 = 142,
  SETROOM13_1 = 142,
  SETROOM13_2 = 142,
  SETROOM13_3 = 142,
  SETROOM13_4 = 142,
  SETROOM13_5 = 142,
  SETROOM13_6 = 142,
  SETROOM13_7 = 142,
  SETROOM14_0 = 142,
  SETROOM14_1 = 142,
  SETROOM14_2 = 142,
  SETROOM14_3 = 142,
  SETROOM14_4 = 142,
  SETROOM14_5 = 142,
  SETROOM14_6 = 142,
  SETROOM14_7 = 142,
  SHOP = 65,
  SHOP_ATTIC = 71,
  SHOP_ATTIC_LEFT = 72,
  SHOP_BASEMENT = 73,
  SHOP_BASEMENT_LEFT = 74,
  SHOP_ENTRANCE_DOWN = 69,
  SHOP_ENTRANCE_DOWN_LEFT = 70,
  SHOP_ENTRANCE_UP = 67,
  SHOP_ENTRANCE_UP_LEFT = 68,
  SHOP_JAIL_BACKLAYER = 44,
  SHOP_LEFT = 66,
  SIDE = 0,
  SISTERS_ROOM = 46,
  SOLID = 21,
  STORAGE_ROOM = 118,
  TUSKDICESHOP = 83,
  TUSKDICESHOP_LEFT = 84,
  TUSKFRONTDICESHOP = 47,
  TUSKFRONTDICESHOP_LEFT = 48,
  UDJATENTRANCE = 29,
  UDJATTOP = 30,
  USHABTI_ENTRANCE = 133,
  USHABTI_ROOM = 134,
  VAULT = 23,
  VLAD_BOTTOM_EXIT = 123,
  VLAD_BOTTOM_TUNNEL = 122,
  VLAD_DRILL = 120,
  VLAD_ENTRANCE = 121,
  VLAD_TUNNEL = 124,
  WADDLER = 86
}
---@alias ROOM_TEMPLATE integer
ROOM_TEMPLATE_TYPE = {
  ENTRANCE = 1,
  EXIT = 2,
  MACHINE_ROOM = 4,
  NONE = 0,
  SHOP = 3
}
---@alias ROOM_TEMPLATE_TYPE integer
SAFE_SETTING = {
  ANGRY_SHOPKEEPER = 28,
  BUTTON_PROMPTS = 30,
  DIALOG_TEXT = 37,
  FEAT_POPUPS = 32,
  GHOST_TEXT = 39,
  HUD_SIZE = 24,
  HUD_STYLE = 23,
  KALI_TEXT = 38,
  LEVEL_FEELINGS = 36,
  LEVEL_NUMBER = 27,
  LEVEL_TIMER = 25,
  PET_STYLE = 20,
  SCREEN_SHAKE = 21,
  TEXTBOX_DURATION = 34,
  TEXTBOX_OPACITY = 35,
  TEXTBOX_SIZE = 33,
  TIMER_DETAIL = 26
}
---@alias SAFE_SETTING integer
SCREEN = {
  ARENA_INTRO = 25,
  ARENA_ITEMS = 23,
  ARENA_LEVEL = 26,
  ARENA_MENU = 21,
  ARENA_SCORE = 27,
  ARENA_SELECT = 24,
  ARENA_STAGES = 22,
  CAMP = 11,
  CHARACTER_SELECT = 9,
  CONSTELLATION = 19,
  CREDITS = 17,
  DEATH = 14,
  INTRO = 1,
  LEADERBOARD = 7,
  LEVEL = 12,
  LOGO = 0,
  MENU = 4,
  ONLINE_LOADING = 28,
  ONLINE_LOBBY = 29,
  OPTIONS = 5,
  PLAYER_PROFILE = 6,
  PROLOGUE = 2,
  RECAP = 20,
  SCORES = 18,
  SEED_INPUT = 8,
  SPACESHIP = 15,
  TEAM_SELECT = 10,
  TITLE = 3,
  TRANSITION = 13,
  WIN = 16
}
---@alias SCREEN integer
SHAPE = {
  CIRCLE = 2,
  RECTANGLE = 1
}
---@alias SHAPE integer
SHOP_TYPE = {
  CAVEMAN_SHOP = 10,
  CLOTHING_SHOP = 1,
  CURIO_SHOP = 9,
  DICE_SHOP = 6,
  GENERAL_STORE = 0,
  GHIST_SHOP = 12,
  HEDJET_SHOP = 8,
  HIRED_HAND_SHOP = 4,
  PET_SHOP = 5,
  SPECIALTY_SHOP = 3,
  TURKEY_SHOP = 11,
  TUSK_DICE_SHOP = 13,
  WEAPON_SHOP = 2
}
---@alias SHOP_TYPE integer
SOUND_LOOP_MODE = {
  BIDIRECTIONAL = 2,
  LOOP = 1,
  OFF = 0
}
---@alias SOUND_LOOP_MODE integer
SOUND_TYPE = {
  MUSIC = 1,
  SFX = 0
}
---@alias SOUND_TYPE integer
SPARROW = {
  ANGRY = -2,
  DEAD = -1,
  FINAL_REWARD_THROWN = 9,
  FINISHED_LEVEL_WITH_THIEF_STATUS = 2,
  FIRST_ENCOUNTER_ROPES_THROWN = 4,
  FIRST_HIDEOUT_SPAWNED_ROPE_THROW = 3,
  MEETING_AT_TUSK_BASEMENT = 8,
  QUEST_NOT_STARTED = 0,
  SECOND_ENCOUNTER_INTERACTED = 7,
  SECOND_HIDEOUT_SPAWNED_NEOBAB = 6,
  THIEF_STATUS = 1,
  TUSK_IDOL_STOLEN = 5
}
---@alias SPARROW integer
SPAWN_TYPE = {
  ANY = 63,
  LEVEL_GEN = 15,
  LEVEL_GEN_FLOOR_SPREADING = 4,
  LEVEL_GEN_GENERAL = 8,
  LEVEL_GEN_PROCEDURAL = 2,
  LEVEL_GEN_TILE_CODE = 1,
  SCRIPT = 16,
  SYSTEMIC = 32
}
---@alias SPAWN_TYPE integer
TEXTURE = {
  DATA_TEXTURES_BASE_EGGSHIP2_0 = 108,
  DATA_TEXTURES_BASE_EGGSHIP2_1 = 109,
  DATA_TEXTURES_BASE_EGGSHIP2_2 = 110,
  DATA_TEXTURES_BASE_EGGSHIP3_0 = 111,
  DATA_TEXTURES_BASE_EGGSHIP3_1 = 112,
  DATA_TEXTURES_BASE_EGGSHIP3_2 = 113,
  DATA_TEXTURES_BASE_EGGSHIP3_3 = 114,
  DATA_TEXTURES_BASE_EGGSHIP_0 = 102,
  DATA_TEXTURES_BASE_EGGSHIP_1 = 103,
  DATA_TEXTURES_BASE_EGGSHIP_2 = 104,
  DATA_TEXTURES_BASE_EGGSHIP_3 = 105,
  DATA_TEXTURES_BASE_EGGSHIP_4 = 106,
  DATA_TEXTURES_BASE_EGGSHIP_5 = 107,
  DATA_TEXTURES_BASE_SKYNIGHT_0 = 115,
  DATA_TEXTURES_BASE_SURFACE2_0 = 118,
  DATA_TEXTURES_BASE_SURFACE_0 = 116,
  DATA_TEXTURES_BASE_SURFACE_1 = 117,
  DATA_TEXTURES_BG_BABYLON_0 = 195,
  DATA_TEXTURES_BG_BEEHIVE_0 = 226,
  DATA_TEXTURES_BG_CAVE_0 = 136,
  DATA_TEXTURES_BG_DUAT2_0 = 215,
  DATA_TEXTURES_BG_DUAT_0 = 211,
  DATA_TEXTURES_BG_DUAT_1 = 213,
  DATA_TEXTURES_BG_DUAT_2 = 214,
  DATA_TEXTURES_BG_EGGPLANT_0 = 223,
  DATA_TEXTURES_BG_GOLD_0 = 269,
  DATA_TEXTURES_BG_ICE_0 = 184,
  DATA_TEXTURES_BG_JUNGLE_0 = 143,
  DATA_TEXTURES_BG_MOTHERSHIP_0 = 272,
  DATA_TEXTURES_BG_STONE_0 = 158,
  DATA_TEXTURES_BG_SUNKEN_0 = 207,
  DATA_TEXTURES_BG_TEMPLE_0 = 165,
  DATA_TEXTURES_BG_TIDEPOOL_0 = 175,
  DATA_TEXTURES_BG_VLAD_0 = 260,
  DATA_TEXTURES_BG_VOLCANO_0 = 153,
  DATA_TEXTURES_BORDER_MAIN_0 = 283,
  DATA_TEXTURES_BORDER_MAIN_1 = 284,
  DATA_TEXTURES_CHAR_BLACK_0 = 288,
  DATA_TEXTURES_CHAR_BLUE_0 = 294,
  DATA_TEXTURES_CHAR_CERULEAN_0 = 293,
  DATA_TEXTURES_CHAR_CINNABAR_0 = 289,
  DATA_TEXTURES_CHAR_CYAN_0 = 287,
  DATA_TEXTURES_CHAR_EGGCHILD_0 = 306,
  DATA_TEXTURES_CHAR_GOLD_0 = 298,
  DATA_TEXTURES_CHAR_GRAY_0 = 302,
  DATA_TEXTURES_CHAR_GREEN_0 = 290,
  DATA_TEXTURES_CHAR_HIRED_0 = 305,
  DATA_TEXTURES_CHAR_IRIS_0 = 297,
  DATA_TEXTURES_CHAR_KHAKI_0 = 303,
  DATA_TEXTURES_CHAR_LEMON_0 = 296,
  DATA_TEXTURES_CHAR_LIME_0 = 295,
  DATA_TEXTURES_CHAR_MAGENTA_0 = 286,
  DATA_TEXTURES_CHAR_OLIVE_0 = 291,
  DATA_TEXTURES_CHAR_ORANGE_0 = 304,
  DATA_TEXTURES_CHAR_PINK_0 = 300,
  DATA_TEXTURES_CHAR_RED_0 = 299,
  DATA_TEXTURES_CHAR_VIOLET_0 = 301,
  DATA_TEXTURES_CHAR_WHITE_0 = 292,
  DATA_TEXTURES_CHAR_YELLOW_0 = 285,
  DATA_TEXTURES_COFFINS_0 = 385,
  DATA_TEXTURES_COFFINS_1 = 386,
  DATA_TEXTURES_COFFINS_2 = 387,
  DATA_TEXTURES_COFFINS_3 = 388,
  DATA_TEXTURES_COFFINS_4 = 389,
  DATA_TEXTURES_COFFINS_5 = 390,
  DATA_TEXTURES_CREDITS_0 = 391,
  DATA_TEXTURES_CREDITS_1 = 392,
  DATA_TEXTURES_DECO_BABYLON_0 = 191,
  DATA_TEXTURES_DECO_BABYLON_1 = 192,
  DATA_TEXTURES_DECO_BABYLON_2 = 193,
  DATA_TEXTURES_DECO_BASECAMP_0 = 119,
  DATA_TEXTURES_DECO_BASECAMP_1 = 120,
  DATA_TEXTURES_DECO_BASECAMP_2 = 121,
  DATA_TEXTURES_DECO_BASECAMP_3 = 122,
  DATA_TEXTURES_DECO_BASECAMP_4 = 123,
  DATA_TEXTURES_DECO_BASECAMP_5 = 124,
  DATA_TEXTURES_DECO_BASECAMP_6 = 125,
  DATA_TEXTURES_DECO_BASECAMP_7 = 126,
  DATA_TEXTURES_DECO_CAVE_0 = 134,
  DATA_TEXTURES_DECO_CAVE_1 = 135,
  DATA_TEXTURES_DECO_COSMIC_0 = 228,
  DATA_TEXTURES_DECO_COSMIC_1 = 229,
  DATA_TEXTURES_DECO_COSMIC_2 = 230,
  DATA_TEXTURES_DECO_EGGPLANT_0 = 202,
  DATA_TEXTURES_DECO_EGGPLANT_1 = 222,
  DATA_TEXTURES_DECO_EXTRA_0 = 279,
  DATA_TEXTURES_DECO_EXTRA_1 = 280,
  DATA_TEXTURES_DECO_GOLD_0 = 266,
  DATA_TEXTURES_DECO_GOLD_1 = 267,
  DATA_TEXTURES_DECO_GOLD_2 = 268,
  DATA_TEXTURES_DECO_ICE_0 = 180,
  DATA_TEXTURES_DECO_ICE_1 = 181,
  DATA_TEXTURES_DECO_ICE_2 = 182,
  DATA_TEXTURES_DECO_ICE_3 = 183,
  DATA_TEXTURES_DECO_JUNGLE_0 = 141,
  DATA_TEXTURES_DECO_JUNGLE_1 = 142,
  DATA_TEXTURES_DECO_JUNGLE_2 = 156,
  DATA_TEXTURES_DECO_SUNKEN_0 = 203,
  DATA_TEXTURES_DECO_SUNKEN_1 = 204,
  DATA_TEXTURES_DECO_SUNKEN_2 = 205,
  DATA_TEXTURES_DECO_SUNKEN_3 = 206,
  DATA_TEXTURES_DECO_TEMPLE_0 = 163,
  DATA_TEXTURES_DECO_TEMPLE_1 = 164,
  DATA_TEXTURES_DECO_TEMPLE_2 = 216,
  DATA_TEXTURES_DECO_TIDEPOOL_0 = 173,
  DATA_TEXTURES_DECO_TIDEPOOL_1 = 174,
  DATA_TEXTURES_DECO_TUTORIAL_0 = 127,
  DATA_TEXTURES_DECO_TUTORIAL_1 = 128,
  DATA_TEXTURES_DECO_VOLCANO_0 = 149,
  DATA_TEXTURES_DECO_VOLCANO_1 = 150,
  DATA_TEXTURES_DECO_VOLCANO_2 = 151,
  DATA_TEXTURES_DECO_VOLCANO_3 = 152,
  DATA_TEXTURES_FLOORMISC_0 = 281,
  DATA_TEXTURES_FLOORMISC_1 = 282,
  DATA_TEXTURES_FLOORSTYLED_BABYLON_0 = 246,
  DATA_TEXTURES_FLOORSTYLED_BABYLON_1 = 247,
  DATA_TEXTURES_FLOORSTYLED_BABYLON_2 = 248,
  DATA_TEXTURES_FLOORSTYLED_BABYLON_3 = 249,
  DATA_TEXTURES_FLOORSTYLED_BABYLON_4 = 250,
  DATA_TEXTURES_FLOORSTYLED_BEEHIVE_0 = 225,
  DATA_TEXTURES_FLOORSTYLED_BEEHIVE_1 = 227,
  DATA_TEXTURES_FLOORSTYLED_DUAT_0 = 212,
  DATA_TEXTURES_FLOORSTYLED_DUAT_1 = 273,
  DATA_TEXTURES_FLOORSTYLED_GOLD_0 = 261,
  DATA_TEXTURES_FLOORSTYLED_GOLD_1 = 262,
  DATA_TEXTURES_FLOORSTYLED_GOLD_2 = 263,
  DATA_TEXTURES_FLOORSTYLED_GOLD_3 = 264,
  DATA_TEXTURES_FLOORSTYLED_GOLD_4 = 270,
  DATA_TEXTURES_FLOORSTYLED_GOLD_NORMAL_0 = 265,
  DATA_TEXTURES_FLOORSTYLED_GUTS_0 = 278,
  DATA_TEXTURES_FLOORSTYLED_MOTHERSHIP_0 = 271,
  DATA_TEXTURES_FLOORSTYLED_PAGODA_0 = 243,
  DATA_TEXTURES_FLOORSTYLED_PAGODA_1 = 244,
  DATA_TEXTURES_FLOORSTYLED_PAGODA_2 = 245,
  DATA_TEXTURES_FLOORSTYLED_PALACE_0 = 274,
  DATA_TEXTURES_FLOORSTYLED_PALACE_1 = 275,
  DATA_TEXTURES_FLOORSTYLED_PALACE_2 = 276,
  DATA_TEXTURES_FLOORSTYLED_PALACE_3 = 277,
  DATA_TEXTURES_FLOORSTYLED_STONE_0 = 157,
  DATA_TEXTURES_FLOORSTYLED_STONE_1 = 159,
  DATA_TEXTURES_FLOORSTYLED_STONE_2 = 236,
  DATA_TEXTURES_FLOORSTYLED_STONE_3 = 237,
  DATA_TEXTURES_FLOORSTYLED_STONE_4 = 238,
  DATA_TEXTURES_FLOORSTYLED_STONE_5 = 239,
  DATA_TEXTURES_FLOORSTYLED_SUNKEN_0 = 251,
  DATA_TEXTURES_FLOORSTYLED_SUNKEN_1 = 252,
  DATA_TEXTURES_FLOORSTYLED_SUNKEN_2 = 253,
  DATA_TEXTURES_FLOORSTYLED_SUNKEN_3 = 254,
  DATA_TEXTURES_FLOORSTYLED_TEMPLE_0 = 240,
  DATA_TEXTURES_FLOORSTYLED_TEMPLE_1 = 241,
  DATA_TEXTURES_FLOORSTYLED_TEMPLE_2 = 242,
  DATA_TEXTURES_FLOORSTYLED_VLAD_0 = 255,
  DATA_TEXTURES_FLOORSTYLED_VLAD_1 = 256,
  DATA_TEXTURES_FLOORSTYLED_VLAD_2 = 257,
  DATA_TEXTURES_FLOORSTYLED_VLAD_3 = 258,
  DATA_TEXTURES_FLOORSTYLED_VLAD_4 = 259,
  DATA_TEXTURES_FLOORSTYLED_WOOD_0 = 231,
  DATA_TEXTURES_FLOORSTYLED_WOOD_1 = 232,
  DATA_TEXTURES_FLOORSTYLED_WOOD_2 = 233,
  DATA_TEXTURES_FLOORSTYLED_WOOD_3 = 234,
  DATA_TEXTURES_FLOORSTYLED_WOOD_4 = 235,
  DATA_TEXTURES_FLOOR_BABYLON_0 = 187,
  DATA_TEXTURES_FLOOR_BABYLON_1 = 188,
  DATA_TEXTURES_FLOOR_BABYLON_2 = 189,
  DATA_TEXTURES_FLOOR_BABYLON_3 = 190,
  DATA_TEXTURES_FLOOR_BABYLON_4 = 196,
  DATA_TEXTURES_FLOOR_CAVE_0 = 129,
  DATA_TEXTURES_FLOOR_CAVE_1 = 130,
  DATA_TEXTURES_FLOOR_CAVE_2 = 131,
  DATA_TEXTURES_FLOOR_CAVE_3 = 132,
  DATA_TEXTURES_FLOOR_CAVE_4 = 133,
  DATA_TEXTURES_FLOOR_CAVE_5 = 137,
  DATA_TEXTURES_FLOOR_EGGPLANT_0 = 217,
  DATA_TEXTURES_FLOOR_EGGPLANT_1 = 218,
  DATA_TEXTURES_FLOOR_EGGPLANT_2 = 219,
  DATA_TEXTURES_FLOOR_EGGPLANT_3 = 220,
  DATA_TEXTURES_FLOOR_EGGPLANT_4 = 221,
  DATA_TEXTURES_FLOOR_EGGPLANT_5 = 224,
  DATA_TEXTURES_FLOOR_ICE_0 = 177,
  DATA_TEXTURES_FLOOR_ICE_1 = 178,
  DATA_TEXTURES_FLOOR_ICE_2 = 179,
  DATA_TEXTURES_FLOOR_ICE_3 = 185,
  DATA_TEXTURES_FLOOR_ICE_4 = 186,
  DATA_TEXTURES_FLOOR_JUNGLE_0 = 138,
  DATA_TEXTURES_FLOOR_JUNGLE_1 = 139,
  DATA_TEXTURES_FLOOR_JUNGLE_2 = 140,
  DATA_TEXTURES_FLOOR_JUNGLE_3 = 144,
  DATA_TEXTURES_FLOOR_SUNKEN_0 = 197,
  DATA_TEXTURES_FLOOR_SUNKEN_1 = 198,
  DATA_TEXTURES_FLOOR_SUNKEN_2 = 199,
  DATA_TEXTURES_FLOOR_SUNKEN_3 = 200,
  DATA_TEXTURES_FLOOR_SUNKEN_4 = 201,
  DATA_TEXTURES_FLOOR_SUNKEN_5 = 208,
  DATA_TEXTURES_FLOOR_SUNKEN_6 = 209,
  DATA_TEXTURES_FLOOR_SUNKEN_7 = 210,
  DATA_TEXTURES_FLOOR_SURFACE_0 = 101,
  DATA_TEXTURES_FLOOR_TEMPLE_0 = 160,
  DATA_TEXTURES_FLOOR_TEMPLE_1 = 161,
  DATA_TEXTURES_FLOOR_TEMPLE_2 = 162,
  DATA_TEXTURES_FLOOR_TEMPLE_3 = 166,
  DATA_TEXTURES_FLOOR_TEMPLE_4 = 167,
  DATA_TEXTURES_FLOOR_TIDEPOOL_0 = 168,
  DATA_TEXTURES_FLOOR_TIDEPOOL_1 = 169,
  DATA_TEXTURES_FLOOR_TIDEPOOL_2 = 170,
  DATA_TEXTURES_FLOOR_TIDEPOOL_3 = 171,
  DATA_TEXTURES_FLOOR_TIDEPOOL_4 = 172,
  DATA_TEXTURES_FLOOR_TIDEPOOL_5 = 176,
  DATA_TEXTURES_FLOOR_VOLCANO_0 = 145,
  DATA_TEXTURES_FLOOR_VOLCANO_1 = 146,
  DATA_TEXTURES_FLOOR_VOLCANO_2 = 147,
  DATA_TEXTURES_FLOOR_VOLCANO_3 = 148,
  DATA_TEXTURES_FLOOR_VOLCANO_4 = 154,
  DATA_TEXTURES_FLOOR_VOLCANO_5 = 155,
  DATA_TEXTURES_FONTDEBUG_0 = 20,
  DATA_TEXTURES_FONTFIRASANS_0 = 7,
  DATA_TEXTURES_FONTMONO_0 = 19,
  DATA_TEXTURES_FONTNEWRODIN_0 = 8,
  DATA_TEXTURES_FONTNOTOSANSBOLDCS_0_0 = 12,
  DATA_TEXTURES_FONTNOTOSANSBOLDCT_0_0 = 14,
  DATA_TEXTURES_FONTNOTOSANSBOLDKO_0_0 = 16,
  DATA_TEXTURES_FONTNOTOSANSBOLDNICKNAMES_0_0 = 18,
  DATA_TEXTURES_FONTNOTOSANSBOLDRU_0_0 = 10,
  DATA_TEXTURES_FONTNOTOSANSITALICCS_0_0 = 13,
  DATA_TEXTURES_FONTNOTOSANSITALICCT_0_0 = 15,
  DATA_TEXTURES_FONTNOTOSANSITALICKO_0_0 = 17,
  DATA_TEXTURES_FONTNOTOSANSITALICRU_0_0 = 11,
  DATA_TEXTURES_FONTRODINCATTLEYA_0 = 9,
  DATA_TEXTURES_FONTYORKTEN_0 = 6,
  DATA_TEXTURES_FX_ANKH_0 = 382,
  DATA_TEXTURES_FX_ANKH_1 = 383,
  DATA_TEXTURES_FX_BIG_0 = 380,
  DATA_TEXTURES_FX_EXPLOSION_0 = 375,
  DATA_TEXTURES_FX_RUBBLE_0 = 381,
  DATA_TEXTURES_FX_SMALL2_0 = 377,
  DATA_TEXTURES_FX_SMALL3_0 = 378,
  DATA_TEXTURES_FX_SMALL3_1 = 379,
  DATA_TEXTURES_FX_SMALL_0 = 376,
  DATA_TEXTURES_HUD_0 = 65,
  DATA_TEXTURES_HUD_1 = 66,
  DATA_TEXTURES_HUD_2 = 67,
  DATA_TEXTURES_HUD_CONTROLLER_BUTTONS_0 = 69,
  DATA_TEXTURES_HUD_CONTROLLER_BUTTONS_1 = 70,
  DATA_TEXTURES_HUD_CONTROLLER_BUTTONS_2 = 71,
  DATA_TEXTURES_HUD_CONTROLLER_BUTTONS_3 = 72,
  DATA_TEXTURES_HUD_CONTROLLER_BUTTONS_4 = 73,
  DATA_TEXTURES_HUD_TEXT_0 = 68,
  DATA_TEXTURES_ITEMS_0 = 373,
  DATA_TEXTURES_ITEMS_USHABTI_0 = 194,
  DATA_TEXTURES_JOURNAL_BACK_0 = 76,
  DATA_TEXTURES_JOURNAL_ELEMENTS_0 = 93,
  DATA_TEXTURES_JOURNAL_ELEMENTS_1 = 94,
  DATA_TEXTURES_JOURNAL_ENTRY_BG_0 = 86,
  DATA_TEXTURES_JOURNAL_ENTRY_ITEMS_0 = 92,
  DATA_TEXTURES_JOURNAL_ENTRY_MONS_0 = 88,
  DATA_TEXTURES_JOURNAL_ENTRY_MONS_BIG_0 = 89,
  DATA_TEXTURES_JOURNAL_ENTRY_PEOPLE_0 = 90,
  DATA_TEXTURES_JOURNAL_ENTRY_PEOPLE_1 = 91,
  DATA_TEXTURES_JOURNAL_ENTRY_PLACE_0 = 87,
  DATA_TEXTURES_JOURNAL_ENTRY_TRAPS_0 = 95,
  DATA_TEXTURES_JOURNAL_ENTRY_TRAPS_1 = 96,
  DATA_TEXTURES_JOURNAL_ENTRY_TRAPS_2 = 97,
  DATA_TEXTURES_JOURNAL_ENTRY_TRAPS_3 = 100,
  DATA_TEXTURES_JOURNAL_PAGEFLIP_0 = 77,
  DATA_TEXTURES_JOURNAL_PAGETORN_0 = 75,
  DATA_TEXTURES_JOURNAL_SELECT_0 = 78,
  DATA_TEXTURES_JOURNAL_STICKERS_0 = 79,
  DATA_TEXTURES_JOURNAL_STICKERS_1 = 80,
  DATA_TEXTURES_JOURNAL_STICKERS_2 = 81,
  DATA_TEXTURES_JOURNAL_STORY_0 = 98,
  DATA_TEXTURES_JOURNAL_TOP_ENTRY_0 = 83,
  DATA_TEXTURES_JOURNAL_TOP_GAMEOVER_0 = 84,
  DATA_TEXTURES_JOURNAL_TOP_MAIN_0 = 82,
  DATA_TEXTURES_JOURNAL_TOP_PROFILE_0 = 85,
  DATA_TEXTURES_JOURNAL_TROPHIES_0 = 99,
  DATA_TEXTURES_KEYBOARD_BUTTONS_0 = 74,
  DATA_TEXTURES_LOADING_0 = 1,
  DATA_TEXTURES_LUT_BACKLAYER_0 = 396,
  DATA_TEXTURES_LUT_BLACKMARKET_0 = 397,
  DATA_TEXTURES_LUT_ICECAVES_0 = 399,
  DATA_TEXTURES_LUT_ORIGINAL_0 = 395,
  DATA_TEXTURES_LUT_VLAD_0 = 398,
  DATA_TEXTURES_MAIN_BODY_0 = 23,
  DATA_TEXTURES_MAIN_DIRT_0 = 24,
  DATA_TEXTURES_MAIN_DOORBACK_0 = 26,
  DATA_TEXTURES_MAIN_DOORFRAME_0 = 27,
  DATA_TEXTURES_MAIN_DOOR_0 = 25,
  DATA_TEXTURES_MAIN_FORE1_0 = 28,
  DATA_TEXTURES_MAIN_FORE2_0 = 29,
  DATA_TEXTURES_MAIN_HEAD_0 = 30,
  DATA_TEXTURES_MENU_BASIC_0 = 41,
  DATA_TEXTURES_MENU_BASIC_1 = 42,
  DATA_TEXTURES_MENU_BASIC_2 = 43,
  DATA_TEXTURES_MENU_BASIC_3 = 44,
  DATA_TEXTURES_MENU_BASIC_4 = 45,
  DATA_TEXTURES_MENU_BASIC_5 = 46,
  DATA_TEXTURES_MENU_BASIC_6 = 47,
  DATA_TEXTURES_MENU_BRICK1_0 = 39,
  DATA_TEXTURES_MENU_BRICK2_0 = 40,
  DATA_TEXTURES_MENU_CAVE1_0 = 37,
  DATA_TEXTURES_MENU_CAVE2_0 = 38,
  DATA_TEXTURES_MENU_CHARDOOR_0 = 35,
  DATA_TEXTURES_MENU_CHARSEL_0 = 34,
  DATA_TEXTURES_MENU_DEATHMATCH2_0 = 54,
  DATA_TEXTURES_MENU_DEATHMATCH2_1 = 55,
  DATA_TEXTURES_MENU_DEATHMATCH3_0 = 56,
  DATA_TEXTURES_MENU_DEATHMATCH4_0 = 57,
  DATA_TEXTURES_MENU_DEATHMATCH5_0 = 58,
  DATA_TEXTURES_MENU_DEATHMATCH5_1 = 59,
  DATA_TEXTURES_MENU_DEATHMATCH5_2 = 60,
  DATA_TEXTURES_MENU_DEATHMATCH5_3 = 61,
  DATA_TEXTURES_MENU_DEATHMATCH6_0 = 62,
  DATA_TEXTURES_MENU_DEATHMATCH_0 = 53,
  DATA_TEXTURES_MENU_DISP_0 = 31,
  DATA_TEXTURES_MENU_DISP_1 = 32,
  DATA_TEXTURES_MENU_DISP_2 = 33,
  DATA_TEXTURES_MENU_GENERIC_0 = 36,
  DATA_TEXTURES_MENU_HEADER_0 = 48,
  DATA_TEXTURES_MENU_LEADER_0 = 49,
  DATA_TEXTURES_MENU_LEADER_1 = 50,
  DATA_TEXTURES_MENU_LEADER_2 = 51,
  DATA_TEXTURES_MENU_LEADER_3 = 52,
  DATA_TEXTURES_MENU_ONLINE_0 = 63,
  DATA_TEXTURES_MENU_TITLEGAL_0 = 22,
  DATA_TEXTURES_MENU_TITLE_0 = 21,
  DATA_TEXTURES_MENU_TUNNEL_0 = 64,
  DATA_TEXTURES_MONSTERS01_0 = 311,
  DATA_TEXTURES_MONSTERS02_0 = 312,
  DATA_TEXTURES_MONSTERS02_1 = 371,
  DATA_TEXTURES_MONSTERS02_2 = 372,
  DATA_TEXTURES_MONSTERS03_0 = 313,
  DATA_TEXTURES_MONSTERS03_1 = 314,
  DATA_TEXTURES_MONSTERSBASIC01_0 = 308,
  DATA_TEXTURES_MONSTERSBASIC02_0 = 309,
  DATA_TEXTURES_MONSTERSBASIC03_0 = 310,
  DATA_TEXTURES_MONSTERSBIG01_0 = 315,
  DATA_TEXTURES_MONSTERSBIG02_0 = 316,
  DATA_TEXTURES_MONSTERSBIG02_1 = 317,
  DATA_TEXTURES_MONSTERSBIG02_2 = 318,
  DATA_TEXTURES_MONSTERSBIG03_0 = 319,
  DATA_TEXTURES_MONSTERSBIG04_0 = 320,
  DATA_TEXTURES_MONSTERSBIG04_1 = 321,
  DATA_TEXTURES_MONSTERSBIG04_2 = 322,
  DATA_TEXTURES_MONSTERSBIG04_3 = 370,
  DATA_TEXTURES_MONSTERSBIG05_0 = 323,
  DATA_TEXTURES_MONSTERSBIG05_1 = 324,
  DATA_TEXTURES_MONSTERSBIG05_2 = 326,
  DATA_TEXTURES_MONSTERSBIG05_3 = 327,
  DATA_TEXTURES_MONSTERSBIG05_4 = 328,
  DATA_TEXTURES_MONSTERSBIG06_0 = 329,
  DATA_TEXTURES_MONSTERSBIG06_1 = 330,
  DATA_TEXTURES_MONSTERSBIG06_2 = 331,
  DATA_TEXTURES_MONSTERSBIG06_3 = 332,
  DATA_TEXTURES_MONSTERSBIG06_4 = 333,
  DATA_TEXTURES_MONSTERSBIG06_5 = 334,
  DATA_TEXTURES_MONSTERSBIG06_6 = 335,
  DATA_TEXTURES_MONSTERSBIG06_7 = 336,
  DATA_TEXTURES_MONSTERSBIG06_8 = 337,
  DATA_TEXTURES_MONSTERSBIG06_9 = 338,
  DATA_TEXTURES_MONSTERSBIG06_10 = 339,
  DATA_TEXTURES_MONSTERS_GHOST_0 = 340,
  DATA_TEXTURES_MONSTERS_GHOST_1 = 341,
  DATA_TEXTURES_MONSTERS_HUNDUN_0 = 364,
  DATA_TEXTURES_MONSTERS_HUNDUN_1 = 365,
  DATA_TEXTURES_MONSTERS_HUNDUN_2 = 366,
  DATA_TEXTURES_MONSTERS_HUNDUN_3 = 367,
  DATA_TEXTURES_MONSTERS_HUNDUN_4 = 368,
  DATA_TEXTURES_MONSTERS_HUNDUN_5 = 369,
  DATA_TEXTURES_MONSTERS_OLMEC_0 = 342,
  DATA_TEXTURES_MONSTERS_OLMEC_1 = 343,
  DATA_TEXTURES_MONSTERS_OLMEC_2 = 344,
  DATA_TEXTURES_MONSTERS_OLMEC_3 = 345,
  DATA_TEXTURES_MONSTERS_OLMEC_4 = 346,
  DATA_TEXTURES_MONSTERS_OSIRIS_0 = 347,
  DATA_TEXTURES_MONSTERS_OSIRIS_1 = 348,
  DATA_TEXTURES_MONSTERS_OSIRIS_2 = 349,
  DATA_TEXTURES_MONSTERS_OSIRIS_3 = 350,
  DATA_TEXTURES_MONSTERS_OSIRIS_4 = 351,
  DATA_TEXTURES_MONSTERS_PETS_0 = 307,
  DATA_TEXTURES_MONSTERS_TIAMAT_0 = 352,
  DATA_TEXTURES_MONSTERS_TIAMAT_1 = 353,
  DATA_TEXTURES_MONSTERS_TIAMAT_2 = 354,
  DATA_TEXTURES_MONSTERS_TIAMAT_3 = 355,
  DATA_TEXTURES_MONSTERS_TIAMAT_4 = 356,
  DATA_TEXTURES_MONSTERS_TIAMAT_5 = 357,
  DATA_TEXTURES_MONSTERS_TIAMAT_6 = 358,
  DATA_TEXTURES_MONSTERS_TIAMAT_7 = 359,
  DATA_TEXTURES_MONSTERS_TIAMAT_8 = 360,
  DATA_TEXTURES_MONSTERS_YAMA_0 = 361,
  DATA_TEXTURES_MONSTERS_YAMA_1 = 362,
  DATA_TEXTURES_MONSTERS_YAMA_2 = 363,
  DATA_TEXTURES_MOUNTS_0 = 374,
  DATA_TEXTURES_NOISE0_0 = 393,
  DATA_TEXTURES_NOISE1_0 = 394,
  DATA_TEXTURES_PLACEHOLDER_0 = 0,
  DATA_TEXTURES_SAVING_0 = 2,
  DATA_TEXTURES_SHADOWS_0 = 384,
  DATA_TEXTURES_SHINE_0 = 400,
  DATA_TEXTURES_SPLASH0_0 = 3,
  DATA_TEXTURES_SPLASH1_0 = 4,
  DATA_TEXTURES_SPLASH2_0 = 5
}
---@alias TEXTURE integer
THEME = {
  ABZU = 13,
  ARENA = 18,
  BASE_CAMP = 17,
  CITY_OF_GOLD = 11,
  COSMIC_OCEAN = 10,
  DUAT = 12,
  DWELLING = 1,
  EGGPLANT_WORLD = 15,
  HUNDUN = 16,
  ICE_CAVES = 7,
  JUNGLE = 2,
  NEO_BABYLON = 8,
  OLMEC = 4,
  SUNKEN_CITY = 9,
  TEMPLE = 6,
  TIAMAT = 14,
  TIDE_POOL = 5,
  VOLCANA = 3
}
---@alias THEME integer
THEME_OVERRIDE = {
  BACKLAYER_LIGHT_LEVEL = 40,
  BASE_ID = 27,
  COFFIN = 11,
  DIRK_COFFIN = 8,
  DO_PROCEDURAL_SPAWN = 51,
  DTOR = 0,
  ENT_BACKWALL = 33,
  ENT_BORDER = 34,
  ENT_CRITTER = 35,
  ENT_FLOOR_SPREADING = 28,
  ENT_FLOOR_SPREADING2 = 29,
  ENT_TRANSITION_STYLED_FLOOR = 32,
  EXIT_ROOM_Y_LEVEL = 46,
  FEELING = 12,
  GENERATE_PATH = 5,
  GRAVITY = 36,
  IDOL = 9,
  INIT_FLAGS = 2,
  INIT_LEVEL = 3,
  INIT_ROOMS = 4,
  LOOP = 41,
  PLAYER_COFFIN = 7,
  PLAYER_DAMAGE = 37,
  POST_PROCESS_ENTITIES = 17,
  POST_PROCESS_LEVEL = 15,
  POST_TRANSITION = 22,
  PRE_TRANSITION = 45,
  RESET_THEME_FLAGS = 1,
  SHOP_CHANCE = 47,
  SOOT = 38,
  SPAWN_BACKGROUND = 19,
  SPAWN_BORDER = 14,
  SPAWN_DECORATION = 48,
  SPAWN_DECORATION2 = 49,
  SPAWN_EFFECTS = 24,
  SPAWN_EXTRA = 50,
  SPAWN_LEVEL = 13,
  SPAWN_LIGHTS = 20,
  SPAWN_PLAYERS = 23,
  SPAWN_PROCEDURAL = 18,
  SPAWN_TRANSITION = 21,
  SPAWN_TRAPS = 16,
  SPECIAL_ROOMS = 6,
  TEXTURE_BACKLAYER_LUT = 39,
  TEXTURE_DYNAMIC = 44,
  THEME_FLAG = 43,
  THEME_ID = 26,
  TRANSITION_MODIFIER = 31,
  TRANSITION_STYLED_FLOOR = 30,
  VAULT = 10,
  VAULT_LEVEL = 42
}
---@alias THEME_OVERRIDE integer
TILE_CODE = {
  ADJACENT_FLOOR = 29,
  ALIEN = 148,
  ALIENQUEEN = 154,
  ALIEN_GENERATOR = 156,
  ALTAR = 185,
  ALTAR_DUAT = 341,
  AMMIT = 174,
  ANKH = 197,
  ANUBIS = 130,
  ANUBIS2 = 253,
  APEP = 379,
  APEP_LEFT = 380,
  APEP_RIGHT = 381,
  ARROW_METAL = 393,
  ARROW_METAL_POISON = 395,
  ARROW_TRAP = 55,
  ARROW_WOODEN = 392,
  ARROW_WOODEN_POISON = 394,
  ASSASSIN = 254,
  AUTOWALLTORCH = 100,
  BABYLON_FLOOR = 18,
  BACK_HOVERPACK = 311,
  BACK_JETPACK = 309,
  BACK_POWERPACK = 312,
  BACK_TELEPACK = 310,
  BAT = 241,
  BAT_FLYING = 242,
  BEE = 257,
  BEEHIVE_FLOOR = 21,
  BEE_QUEEN = 258,
  BIGSPEAR_TRAP = 176,
  BODYGUARD = 230,
  BOMB = 285,
  BOMB_BAG = 283,
  BOMB_BOX = 284,
  BONE_BLOCK = 6,
  BONE_KEY = 305,
  BOOMBOX = 397,
  BOOMERANG = 318,
  BOULDER = 378,
  BUBBLE_PLATFORM = 369,
  BUNKBED = 75,
  BUSH_BLOCK = 7,
  CAMERA = 316,
  CAPE = 307,
  CATMUMMY = 135,
  CAVEMAN = 114,
  CAVEMANBOSS = 117,
  CAVEMANSHOPKEEPER = 219,
  CAVEMAN_ASLEEP = 115,
  CHAINANDBLOCKS_CEILING = 59,
  CHAIN_CEILING = 60,
  CHAIR_LOOKING_LEFT = 78,
  CHAIR_LOOKING_RIGHT = 79,
  CHALLENGE_WAITROOM = 218,
  CHUNK_AIR = 2,
  CHUNK_DOOR = 3,
  CHUNK_GROUND = 1,
  CLIMBING_GLOVES = 290,
  CLIMBING_POLE = 36,
  CLONEGUN = 324,
  CLOVER = 107,
  COARSE_LAVA = 235,
  COARSE_WATER = 233,
  COBRA = 132,
  COBWEB = 343,
  COFFIN = 196,
  COG_DOOR = 239,
  COG_FLOOR = 23,
  COMPASS = 295,
  COMPASS_ALIEN = 296,
  CONSTRUCTION_SIGN = 72,
  CONVEYORBELT_LEFT = 61,
  CONVEYORBELT_RIGHT = 62,
  COOKED_TURKEY = 105,
  COOKFIRE = 192,
  COSMIC_JELLY = 263,
  COSMIC_ORB = 339,
  COUCH = 80,
  CRABMAN = 374,
  CRATE = 92,
  CRATE_BOMBS = 93,
  CRATE_PARACHUTE = 95,
  CRATE_ROPES = 94,
  CRITTER_BUTTERFLY = 361,
  CRITTER_CRAB = 364,
  CRITTER_DRONE = 368,
  CRITTER_DUNGBEETLE = 360,
  CRITTER_FIREFLY = 367,
  CRITTER_FISH = 363,
  CRITTER_LOCUST = 365,
  CRITTER_PENGUIN = 366,
  CRITTER_SLIME = 389,
  CRITTER_SNAIL = 362,
  CROCMAN = 131,
  CROSSBOW = 202,
  CROWN = 301,
  CROWN_STATUE = 127,
  CRUSHING_ELEVATOR = 184,
  CRUSHTRAP = 64,
  CRUSHTRAPLARGE = 65,
  CURSED_POT = 109,
  DIAMOND = 277,
  DIE = 215,
  DININGTABLE = 76,
  DM_SPAWN_POINT = 50,
  DOG_SIGN = 82,
  DOOR = 43,
  DOOR2 = 45,
  DOOR2_SECRET = 46,
  DOOR_DROP_HELD = 47,
  DRESSER = 74,
  DRILL = 193,
  DUAT_FLOOR = 25,
  DUSTWALL = 240,
  EGGPLANT = 275,
  EGGPLANT_ALTAR = 157,
  EGGPLANT_CHILD = 159,
  EGGPLANT_CROWN = 302,
  EGGPLANT_DOOR = 179,
  EGGPLUP = 412,
  EGGSAC = 344,
  EGGSAC_BOTTOM = 348,
  EGGSAC_LEFT = 345,
  EGGSAC_RIGHT = 347,
  EGGSAC_TOP = 346,
  ELEVATOR = 68,
  ELIXIR = 287,
  EMERALD = 278,
  EMPRESS_GRAVE = 160,
  EMPTY = 0,
  EMPTY_MECH = 111,
  ENTRANCE = 40,
  ENTRANCE_SHORTCUT = 41,
  EXCALIBUR = 320,
  EXCALIBUR_BROKEN = 321,
  EXCALIBUR_STONE = 198,
  EXCALIBUR_STONE_EMPTY = 322,
  EXIT = 42,
  FACTORY_GENERATOR = 63,
  FALLING_PLATFORM = 58,
  FIREBUG = 248,
  FLOOR = 4,
  FLOOR_HARD = 28,
  FLYING_FISH = 373,
  FORCEFIELD = 217,
  FORCEFIELD_HORIZONTAL = 333,
  FORCEFIELD_HORIZONTAL_TOP = 334,
  FORCEFIELD_TIMED = 416,
  FORCEFIELD_TOP = 70,
  FOUNTAIN_DRAIN = 143,
  FOUNTAIN_HEAD = 142,
  FROG = 259,
  FROG_ORANGE = 260,
  FURNITURE_CHAIR_LOOKING_LEFT = 405,
  FURNITURE_CHAIR_LOOKING_RIGHT = 406,
  FURNITURE_CONSTRUCTION_SIGN = 402,
  FURNITURE_DININGTABLE = 407,
  FURNITURE_DRESSER = 398,
  FURNITURE_SIDETABLE = 409,
  FURNITURE_SINGLEBED = 403,
  GHIST_DOOR2 = 48,
  GHIST_PRESENT = 358,
  GHIST_SHOPKEEPER = 220,
  GHOST = 264,
  GHOST_MED_HAPPY = 266,
  GHOST_MED_SAD = 265,
  GHOST_SMALL_ANGRY = 267,
  GHOST_SMALL_HAPPY = 270,
  GHOST_SMALL_SAD = 268,
  GHOST_SMALL_SURPRISED = 269,
  GIANTCLAM = 141,
  GIANTFOOD = 286,
  GIANT_FLY = 372,
  GIANT_FROG = 180,
  GIANT_SPIDER = 122,
  GOLDBARS = 91,
  GOLD_BAR = 276,
  GROWABLE_CLIMBING_POLE = 37,
  GROWABLE_VINE = 35,
  GRUB = 349,
  GUN_FREEZERAY = 315,
  GUN_SHOTGUN = 314,
  GUN_WEBGUN = 313,
  GUTS_FLOOR = 27,
  HAUNTED_CORPSE = 110,
  HEDJET = 300,
  HERMITCRAB = 140,
  HONEY_DOWNWARDS = 200,
  HONEY_UPWARDS = 199,
  HOUYIBOW = 205,
  HUMPHEAD = 331,
  HUNDUN = 261,
  HUNDUN_SPIKES = 408,
  ICEFLOOR = 152,
  IDOL = 186,
  IDOL_FLOOR = 187,
  IDOL_HOLD = 188,
  IMP = 124,
  JIANGSHI = 138,
  JUMPDOG = 181,
  JUNGLE_FLOOR = 10,
  JUNGLE_SPEAR_TRAP = 57,
  KAPALA = 299,
  KEY = 203,
  KINGU = 144,
  LADDER = 32,
  LADDER_PLAT = 33,
  LAMASSU = 161,
  LAMP_HANG = 209,
  LANDMINE = 149,
  LASER_TRAP = 162,
  LAVA = 234,
  LAVAMANDER = 125,
  LAVA_POT = 354,
  LEAF = 271,
  LEPRECHAUN = 137,
  LIGHTARROW = 206,
  LION_TRAP = 238,
  LITTORCH = 97,
  LITWALLTORCH = 99,
  LIZARD = 245,
  LOCKEDCHEST = 101,
  LOCKED_DOOR = 49,
  MACHETE = 319,
  MADAMETUSK = 229,
  MANTRAP = 120,
  MATTOCK = 201,
  MERCHANT = 216,
  MINEWOOD_FLOOR = 12,
  MINEWOOD_FLOOR_HANGING_HIDE = 14,
  MINEWOOD_FLOOR_NOREPLACE = 13,
  MINISTER = 182,
  MOAI_STATUE = 158,
  MOLE = 246,
  MONKEY = 247,
  MONKEY_GOLD = 340,
  MOSQUITO = 121,
  MOTHERSHIP_FLOOR = 24,
  MOTHER_STATUE = 178,
  MOUNT_AXOLOTL = 329,
  MOUNT_QILIN = 330,
  MOUNT_ROCKDOG = 328,
  MOVABLE_SPIKES = 396,
  MUMMY = 133,
  MUSHROOM_BASE = 103,
  NECROMANCER = 136,
  NONREPLACEABLE_BABYLON_FLOOR = 19,
  NONREPLACEABLE_FLOOR = 5,
  OCTOPUS = 139,
  OLDHUNTER = 227,
  OLMEC = 128,
  OLMECSHIP = 173,
  OLMITE = 164,
  OLMITE_ARMORED = 384,
  OLMITE_HELMET = 383,
  OLMITE_NAKED = 382,
  OSIRIS = 252,
  PAGODA_FLOOR = 17,
  PAGODA_PLATFORM = 39,
  PALACE_BOOKCASE = 171,
  PALACE_CANDLE = 170,
  PALACE_CHANDELIER = 169,
  PALACE_ENTRANCE = 166,
  PALACE_FLOOR = 26,
  PALACE_SIGN = 359,
  PALACE_TABLE = 167,
  PALACE_TABLE_TRAY = 168,
  PANGXIE = 415,
  PARACHUTE = 297,
  PASTE = 294,
  PEN_FLOOR = 224,
  PEN_LOCKED_DOOR = 225,
  PET_MONTY = 335,
  PET_PERCY = 336,
  PET_POOCHI = 337,
  PILLAR = 129,
  PIPE = 175,
  PITCHERS_MITT = 291,
  PLASMA_CANNON = 204,
  PLATFORM = 38,
  PLAYERBAG = 306,
  POT = 108,
  POTOFGOLD = 90,
  POWDER_KEG = 54,
  POWDER_KEG_TIMED = 417,
  PRESENT = 332,
  PROTO_GENERATOR = 411,
  PROTO_SHOPKEEPER = 355,
  PUNISHBALL = 370,
  PUNISHBALL_ATTACH = 371,
  PUNISHBALL_ATTACH_BOTTOM = 388,
  PUNISHBALL_ATTACH_LEFT = 385,
  PUNISHBALL_ATTACH_RIGHT = 386,
  PUNISHBALL_ATTACH_TOP = 387,
  PUSH_BLOCK = 53,
  QUICKSAND = 66,
  QUILLBACK = 401,
  REDSKELETON = 244,
  REGENERATING_BLOCK = 31,
  ROBOT = 123,
  ROCK = 96,
  ROPE = 281,
  ROPE_PILE = 282,
  ROPE_UNROLLED = 338,
  ROYAL_JELLY = 106,
  RUBY = 280,
  SAPPHIRE = 279,
  SCARAB = 262,
  SCEPTER = 323,
  SCORPION = 116,
  SEEDED_RUN_UNLOCKER = 288,
  SHIELD_METAL = 326,
  SHIELD_WOODEN = 325,
  SHOES_SPIKE = 293,
  SHOES_SPRING = 292,
  SHOPKEEPER = 214,
  SHOPKEEPER_CLONE = 356,
  SHOPKEEPER_VAT = 155,
  SHOP_DOOR = 207,
  SHOP_ITEM = 221,
  SHOP_PAGODAWALL = 212,
  SHOP_SIGN = 208,
  SHOP_WALL = 210,
  SHOP_WOODWALL = 211,
  SHORTCUT_STATION_BANNER = 83,
  SIDETABLE = 77,
  SINGLEBED = 73,
  SISTER = 226,
  SKELETON = 243,
  SKELETON_KEY = 400,
  SKULL = 390,
  SKULL_DROP_TRAP = 353,
  SLEEPING_HIREDHAND = 222,
  SLIDINGWALL = 375,
  SLIDINGWALL_CEILING = 146,
  SLIDINGWALL_SWITCH = 145,
  SNAKE = 113,
  SNAP_TRAP = 112,
  SORCERESS = 134,
  SPARK_TRAP = 163,
  SPARROW = 410,
  SPECS = 289,
  SPIDER = 350,
  SPIDER_FALLING = 351,
  SPIDER_HANGING = 352,
  SPIKEBALL = 342,
  SPIKEBALL_NO_BOUNCE = 377,
  SPIKEBALL_TRAP = 376,
  SPIKES = 51,
  SPIKES_UPSIDEDOWN = 418,
  SPRING_TRAP = 151,
  STARTING_EXIT = 44,
  STICKY_TRAP = 177,
  STONE_FLOOR = 15,
  STORAGE_FLOOR = 189,
  STORAGE_GUY = 231,
  STYLED_FLOOR = 11,
  SUNKEN_FLOOR = 20,
  SURFACE_FLOOR = 8,
  SURFACE_HIDDEN_FLOOR = 9,
  TABLET = 304,
  TADPOLE = 357,
  TELEPORTER = 317,
  TELESCOPE = 84,
  TEMPLE_FLOOR = 16,
  THIEF = 228,
  THINICE = 153,
  THIN_ICE = 399,
  THORN_VINE = 56,
  TIAMAT = 172,
  TIKIMAN = 118,
  TIMED_FORCEFIELD = 69,
  TIMED_POWDER_KEG = 67,
  TOMB_FLOOR = 30,
  TOTEM_TRAP = 237,
  TREASURE = 87,
  TREASURE_CHEST = 88,
  TREASURE_VAULTCHEST = 89,
  TREE_BASE = 102,
  TRUE_CROWN = 303,
  TUN = 413,
  TURKEY = 104,
  TUTORIAL_MENU_SIGN = 274,
  TUTORIAL_SPEEDRUN_SIGN = 273,
  TV = 81,
  UDJAT_CHEST = 404,
  UDJAT_EYE = 298,
  UDJAT_KEY = 272,
  UDJAT_SOCKET = 194,
  UDJAT_TARGET = 327,
  UFO = 147,
  UPSIDEDOWN_SPIKES = 52,
  USHABTI = 71,
  VAMPIRE = 249,
  VAMPIRE_FLYING = 250,
  VAN_HORSING = 414,
  VAULT_WALL = 195,
  VENOM = 391,
  VINE = 34,
  VLAD = 126,
  VLADS_CAPE = 308,
  VLAD_FLOOR = 22,
  VLAD_FLYING = 251,
  WALLTORCH = 98,
  WANTED_POSTER = 213,
  WATER = 232,
  WITCHDOCTOR = 119,
  WOODENLOG_TRAP = 190,
  WOODENLOG_TRAP_CEILING = 191,
  YAMA = 183,
  YANG = 223,
  YETI = 150,
  YETI_KING = 255,
  YETI_QUEEN = 256,
  ZOO_EXHIBIT = 165
}
---@alias TILE_CODE integer
TUSK = {
  ANGRY = -2,
  DEAD = -1,
  DICE_HOUSE_SPAWNED = 1,
  HIGH_ROLLER_STATUS = 2,
  PALACE_WELCOME_MESSAGE = 3,
  QUEST_NOT_STARTED = 0
}
---@alias TUSK integer
VANHORSING = {
  FIRST_ENCOUNTER_DIAMOND_THROWN = 2,
  JAILCELL_SPAWNED = 1,
  QUEST_NOT_STARTED = 0,
  SECOND_ENCOUNTER_COMPASS_THROWN = 6,
  SHOT_VLAD = 4,
  SPAWNED_IN_VLADS_CASTLE = 3,
  TEMPLE_HIDEOUT_SPAWNED = 5,
  TUSK_CELLAR = 7
}
---@alias VANHORSING integer
VANILLA_FONT_STYLE = {
  BOLD = 2,
  ITALIC = 1,
  NORMAL = 0
}
---@alias VANILLA_FONT_STYLE integer
VANILLA_SOUND = {
  BGM_BGM_BASECAMP = "BGM/BGM_basecamp",
  BGM_BGM_CREDITS = "BGM/BGM_credits",
  BGM_BGM_DM = "BGM/BGM_dm",
  BGM_BGM_ENDING = "BGM/BGM_ending",
  BGM_BGM_MASTER = "BGM/BGM_master",
  BGM_BGM_MENU = "BGM/BGM_menu",
  BGM_BGM_TITLE = "BGM/BGM_title",
  CRITTERS_DRONE_CRASH = "Critters/Drone_crash",
  CRITTERS_DRONE_LOOP = "Critters/Drone_loop",
  CRITTERS_FIREFLY_FLASH = "Critters/Firefly_flash",
  CRITTERS_LOCUST_LOOP = "Critters/Locust_loop",
  CRITTERS_PENGUIN_JUMP1 = "Critters/Penguin_jump1",
  CRITTERS_PENGUIN_JUMP2 = "Critters/Penguin_jump2",
  CRITTERS_SNAIL_ATTACH = "Critters/Snail_attach",
  CUTSCENE_ANKH_CRACK = "Cutscene/Ankh_crack",
  CUTSCENE_ANKH_LOOP = "Cutscene/Ankh_loop",
  CUTSCENE_ANKH_PULSE = "Cutscene/Ankh_pulse",
  CUTSCENE_ANKH_SHATTER = "Cutscene/Ankh_shatter",
  CUTSCENE_BIG_TREASURE_LAND = "Cutscene/Big_treasure_land",
  CUTSCENE_BIG_TREASURE_OPEN = "Cutscene/Big_treasure_open",
  CUTSCENE_CAVE_RUMBLE = "Cutscene/Cave_rumble",
  CUTSCENE_CONSTELLATION_LOOP = "Cutscene/Constellation_loop",
  CUTSCENE_CREDITS_ASTEROID = "Cutscene/Credits_asteroid",
  CUTSCENE_CREDITS_THRUSTER_LOOP = "Cutscene/Credits_thruster_loop",
  CUTSCENE_CREDITS_UFO_BONK = "Cutscene/Credits_ufo_bonk",
  CUTSCENE_EGGSHIP_AMB_LOOP = "Cutscene/Eggship_amb_loop",
  CUTSCENE_EGGSHIP_DOOR = "Cutscene/Eggship_door",
  CUTSCENE_EGGSHIP_EMERGE = "Cutscene/Eggship_emerge",
  CUTSCENE_EGGSHIP_LAND = "Cutscene/Eggship_land",
  CUTSCENE_EGGSHIP_LOOP = "Cutscene/Eggship_loop",
  CUTSCENE_EGGSHIP_THRUSTER_LOOP = "Cutscene/Eggship_thruster_loop",
  CUTSCENE_JOURNAL_PAGE = "Cutscene/Journal_page",
  CUTSCENE_KEY_DROP = "Cutscene/Key_drop",
  CUTSCENE_MENU_INTRO_LOOP = "Cutscene/Menu_intro_loop",
  CUTSCENE_OUROBOROS_DOOR_LOOP = "Cutscene/Ouroboros_door_loop",
  CUTSCENE_OUROBOROS_LOOP = "Cutscene/Ouroboros_loop",
  CUTSCENE_OUROBOROS_SHAKE = "Cutscene/Ouroboros_shake",
  CUTSCENE_RUMBLE_LOOP = "Cutscene/Rumble_loop",
  DEATHMATCH_DM_BANNER = "Deathmatch/dm_banner",
  DEATHMATCH_DM_COUNTDOWN = "Deathmatch/dm_countdown",
  DEATHMATCH_DM_ITEM_SPAWN = "Deathmatch/dm_item_spawn",
  DEATHMATCH_DM_ITEM_WARN = "Deathmatch/dm_item_warn",
  DEATHMATCH_DM_PILLAR_CRUSH = "Deathmatch/dm_pillar_crush",
  DEATHMATCH_DM_PILLAR_LOOP = "Deathmatch/dm_pillar_loop",
  DEATHMATCH_DM_SCORE = "Deathmatch/dm_score",
  DEATHMATCH_DM_SPLASH = "Deathmatch/dm_splash",
  DEATHMATCH_DM_TIMER = "Deathmatch/dm_timer",
  DEFAULT_SOUND = "default_sound",
  ENEMIES_ALIEN_JUMP = "Enemies/Alien_jump",
  ENEMIES_ALIEN_QUEEN_LOOP = "Enemies/Alien_queen_loop",
  ENEMIES_ALIEN_QUEEN_SHOT = "Enemies/Alien_queen_shot",
  ENEMIES_ALIEN_QUEEN_SIGHT_LOOP = "Enemies/Alien_queen_sight_loop",
  ENEMIES_ALIEN_SPEECH = "Enemies/Alien_speech",
  ENEMIES_AMMIT_WALK = "Enemies/Ammit_walk",
  ENEMIES_ANUBIS_ACTIVATE = "Enemies/Anubis_activate",
  ENEMIES_ANUBIS_ATK = "Enemies/Anubis_atk",
  ENEMIES_ANUBIS_SPECIAL_SHOT = "Enemies/Anubis_special_shot",
  ENEMIES_ANUBIS_WARN = "Enemies/Anubis_warn",
  ENEMIES_APEP_BODY_LOOP = "Enemies/Apep_body_loop",
  ENEMIES_APEP_HEAD_LOOP = "Enemies/Apep_head_loop",
  ENEMIES_BAT_FLAP = "Enemies/Bat_flap",
  ENEMIES_BEE_LOOP = "Enemies/Bee_loop",
  ENEMIES_BEE_QUEEN_LOOP = "Enemies/Bee_queen_loop",
  ENEMIES_BOSS_CAVEMAN_BONK = "Enemies/Boss_caveman_bonk",
  ENEMIES_BOSS_CAVEMAN_CRUSH = "Enemies/Boss_caveman_crush",
  ENEMIES_BOSS_CAVEMAN_JUMP = "Enemies/Boss_caveman_jump",
  ENEMIES_BOSS_CAVEMAN_LAND = "Enemies/Boss_caveman_land",
  ENEMIES_BOSS_CAVEMAN_ROLL_LOOP = "Enemies/Boss_caveman_roll_loop",
  ENEMIES_BOSS_CAVEMAN_STEP = "Enemies/Boss_caveman_step",
  ENEMIES_BOSS_CAVEMAN_STOMP = "Enemies/Boss_caveman_stomp",
  ENEMIES_CATMUMMY_ATK = "Enemies/Catmummy_atk",
  ENEMIES_CATMUMMY_JUMP = "Enemies/Catmummy_jump",
  ENEMIES_CATMUMMY_RUN = "Enemies/Catmummy_run",
  ENEMIES_CAVEMAN_PRAY_LOOP = "Enemies/Caveman_pray_loop",
  ENEMIES_CAVEMAN_STEP = "Enemies/Caveman_step",
  ENEMIES_CAVEMAN_TALK = "Enemies/Caveman_talk",
  ENEMIES_CAVEMAN_TRIGGER = "Enemies/Caveman_trigger",
  ENEMIES_COBRA_ATK = "Enemies/Cobra_atk",
  ENEMIES_CROCMAN_ATK = "Enemies/Crocman_atk",
  ENEMIES_CROCMAN_TRIGGER = "Enemies/Crocman_trigger",
  ENEMIES_EGGPLANT_DOG_BOUNCE = "Enemies/Eggplant_dog_bounce",
  ENEMIES_EGGPLANT_MINISTER_LOOP = "Enemies/Eggplant_minister_loop",
  ENEMIES_EGGPLANT_MINISTER_MORPH = "Enemies/Eggplant_minister_morph",
  ENEMIES_EGGSAC_BURST = "Enemies/Eggsac_burst",
  ENEMIES_EGGSAC_WARN = "Enemies/Eggsac_warn",
  ENEMIES_ENEMY_HIT_INVINCIBLE = "Enemies/Enemy_hit_invincible",
  ENEMIES_FIREBUG_ARM = "Enemies/Firebug_arm",
  ENEMIES_FIREBUG_ATK_LOOP = "Enemies/Firebug_atk_loop",
  ENEMIES_FIREBUG_FLY_LOOP = "Enemies/Firebug_fly_loop",
  ENEMIES_FLYINGFISH_BONK = "Enemies/Flyingfish_bonk",
  ENEMIES_FLYINGFISH_FLAP = "Enemies/Flyingfish_flap",
  ENEMIES_FLYINGFISH_WIGGLE = "Enemies/Flyingfish_wiggle",
  ENEMIES_FROG_CHARGE_LOOP = "Enemies/Frog_charge_loop",
  ENEMIES_FROG_EAT = "Enemies/Frog_eat",
  ENEMIES_FROG_GIANT_OPEN = "Enemies/Frog_giant_open",
  ENEMIES_FROG_JUMP = "Enemies/Frog_jump",
  ENEMIES_GHIST_LOOP = "Enemies/Ghist_loop",
  ENEMIES_GHOST_LOOP = "Enemies/Ghost_loop",
  ENEMIES_GHOST_SPLIT = "Enemies/Ghost_split",
  ENEMIES_GIANT_FLY_EAT = "Enemies/Giant_fly_eat",
  ENEMIES_GIANT_FLY_LOOP = "Enemies/Giant_fly_loop",
  ENEMIES_GIANT_SPIDER_DROP = "Enemies/Giant_spider_drop",
  ENEMIES_GIANT_SPIDER_JUMP = "Enemies/Giant_spider_jump",
  ENEMIES_GIANT_SPIDER_WALK = "Enemies/Giant_spider_walk",
  ENEMIES_GOLD_MONKEY_JUMP = "Enemies/Gold_monkey_jump",
  ENEMIES_GOLD_MONKEY_POOP = "Enemies/Gold_monkey_poop",
  ENEMIES_GRUB_EVOLVE = "Enemies/Grub_evolve",
  ENEMIES_GRUB_JUMP = "Enemies/Grub_jump",
  ENEMIES_GRUB_LOOP = "Enemies/Grub_loop",
  ENEMIES_HANGSPIDER_ATK = "Enemies/Hangspider_atk",
  ENEMIES_HERMITCRAB_ATK = "Enemies/Hermitcrab_atk",
  ENEMIES_HERMITCRAB_MORPH = "Enemies/Hermitcrab_morph",
  ENEMIES_HUMPHEAD_LOOP = "Enemies/Humphead_loop",
  ENEMIES_HUNDUN_ATK = "Enemies/Hundun_atk",
  ENEMIES_HUNDUN_DEATH_LAND = "Enemies/Hundun_death_land",
  ENEMIES_HUNDUN_HEAD_DESTROY = "Enemies/Hundun_head_destroy",
  ENEMIES_HUNDUN_HEAD_EMERGE = "Enemies/Hundun_head_emerge",
  ENEMIES_HUNDUN_HURT = "Enemies/Hundun_hurt",
  ENEMIES_HUNDUN_STEP = "Enemies/Hundun_step",
  ENEMIES_HUNDUN_WARN = "Enemies/Hundun_warn",
  ENEMIES_HUNDUN_WINGS_EMERGE = "Enemies/Hundun_wings_emerge",
  ENEMIES_HUNDUN_WING_FLAP = "Enemies/Hundun_wing_flap",
  ENEMIES_IMP_DROP = "Enemies/Imp_drop",
  ENEMIES_IMP_FLAP = "Enemies/Imp_flap",
  ENEMIES_JELLYFISH_LOOP = "Enemies/Jellyfish_loop",
  ENEMIES_JIANGSHI_CHARGE = "Enemies/Jiangshi_charge",
  ENEMIES_JIANGSHI_FEMALE_JUMP = "Enemies/Jiangshi_female_jump",
  ENEMIES_JIANGSHI_FLIP = "Enemies/Jiangshi_flip",
  ENEMIES_JIANGSHI_JUMP = "Enemies/Jiangshi_jump",
  ENEMIES_KILLED_ENEMY = "Enemies/Killed_enemy",
  ENEMIES_KILLED_ENEMY_BONES = "Enemies/Killed_enemy_bones",
  ENEMIES_KILLED_ENEMY_CORPSE = "Enemies/Killed_enemy_corpse",
  ENEMIES_KINGU_GRIP = "Enemies/Kingu_grip",
  ENEMIES_KINGU_HURT = "Enemies/Kingu_hurt",
  ENEMIES_KINGU_SLIDE_LOOP = "Enemies/Kingu_slide_loop",
  ENEMIES_LAMASSU_AIM_LOCK = "Enemies/Lamassu_aim_lock",
  ENEMIES_LAMASSU_AIM_LOOP = "Enemies/Lamassu_aim_loop",
  ENEMIES_LAMASSU_ATK_HIT = "Enemies/Lamassu_atk_hit",
  ENEMIES_LAMASSU_ATK_LOOP = "Enemies/Lamassu_atk_loop",
  ENEMIES_LAMASSU_FLY = "Enemies/Lamassu_fly",
  ENEMIES_LAMASSU_WALK = "Enemies/Lamassu_walk",
  ENEMIES_LAVAMANDER_ATK = "Enemies/Lavamander_atk",
  ENEMIES_LAVAMANDER_CHARGE = "Enemies/Lavamander_charge",
  ENEMIES_LAVAMANDER_JUMP = "Enemies/Lavamander_jump",
  ENEMIES_LAVAMANDER_TRANSFORM = "Enemies/Lavamander_transform",
  ENEMIES_LEPRECHAUN_JUMP = "Enemies/Leprechaun_jump",
  ENEMIES_LEPRECHAUN_LOOP = "Enemies/Leprechaun_loop",
  ENEMIES_LEPRECHAUN_STEAL_LOOP = "Enemies/Leprechaun_steal_loop",
  ENEMIES_LIZARD_BONK = "Enemies/Lizard_bonk",
  ENEMIES_LIZARD_CURL_LOOP = "Enemies/Lizard_curl_loop",
  ENEMIES_LIZARD_JUMP = "Enemies/Lizard_jump",
  ENEMIES_LIZARD_UNROLL = "Enemies/Lizard_unroll",
  ENEMIES_MAGMAMAN_JUMP = "Enemies/Magmaman_jump",
  ENEMIES_MAGMAMAN_TRANSFORM = "Enemies/Magmaman_transform",
  ENEMIES_MANTRAP_BITE = "Enemies/Mantrap_bite",
  ENEMIES_MOLERAT_DIG_LOOP = "Enemies/Molerat_dig_loop",
  ENEMIES_MOLERAT_RUN_LOOP = "Enemies/Molerat_run_loop",
  ENEMIES_MONKEY_JUMP = "Enemies/Monkey_jump",
  ENEMIES_MONKEY_STEAL_END = "Enemies/Monkey_steal_end",
  ENEMIES_MONKEY_STEAL_LOOP = "Enemies/Monkey_steal_loop",
  ENEMIES_MOSQUITO_LOOP = "Enemies/Mosquito_loop",
  ENEMIES_MOSQUITO_PIERCE = "Enemies/Mosquito_pierce",
  ENEMIES_MUMMY_FLIES_LOOP = "Enemies/Mummy_flies_loop",
  ENEMIES_MUMMY_STEP = "Enemies/Mummy_step",
  ENEMIES_MUMMY_VOMIT = "Enemies/Mummy_vomit",
  ENEMIES_NECROMANCER_CHARGE_LOOP = "Enemies/Necromancer_charge_loop",
  ENEMIES_NECROMANCER_SPAWN = "Enemies/Necromancer_spawn",
  ENEMIES_OCTOPUS_ATK = "Enemies/Octopus_atk",
  ENEMIES_OCTOPUS_BONK = "Enemies/Octopus_bonk",
  ENEMIES_OCTOPUS_JUMP = "Enemies/Octopus_jump",
  ENEMIES_OCTOPUS_WALK = "Enemies/Octopus_walk",
  ENEMIES_OLMEC_BOMB_SPAWN = "Enemies/Olmec_bomb_spawn",
  ENEMIES_OLMEC_CRUSH = "Enemies/Olmec_crush",
  ENEMIES_OLMEC_HOVER_LOOP = "Enemies/Olmec_hover_loop",
  ENEMIES_OLMEC_PAD_BREAK = "Enemies/Olmec_pad_break",
  ENEMIES_OLMEC_PAD_SHOW = "Enemies/Olmec_pad_show",
  ENEMIES_OLMEC_SPLASH = "Enemies/Olmec_splash",
  ENEMIES_OLMEC_STOMP = "Enemies/Olmec_stomp",
  ENEMIES_OLMEC_TRANSFORM_CLOSE = "Enemies/Olmec_transform_close",
  ENEMIES_OLMEC_TRANSFORM_OPEN = "Enemies/Olmec_transform_open",
  ENEMIES_OLMEC_UFO_SPAWN = "Enemies/Olmec_ufo_spawn",
  ENEMIES_OLMEC_UNCOVER = "Enemies/Olmec_uncover",
  ENEMIES_OLMITE_ARMOR_BREAK = "Enemies/Olmite_armor_break",
  ENEMIES_OLMITE_JUMP = "Enemies/Olmite_jump",
  ENEMIES_OLMITE_STOMP = "Enemies/Olmite_stomp",
  ENEMIES_OSIRIS_APPEAR = "Enemies/Osiris_appear",
  ENEMIES_OSIRIS_PUNCH = "Enemies/Osiris_punch",
  ENEMIES_PANGXIE_BUBBLE_ATK = "Enemies/Pangxie_bubble_atk",
  ENEMIES_PANGXIE_PUNCH1 = "Enemies/Pangxie_punch1",
  ENEMIES_PANGXIE_PUNCH2 = "Enemies/Pangxie_punch2",
  ENEMIES_PROTO_BURST_LOOP = "Enemies/Proto_burst_loop",
  ENEMIES_PROTO_CRAWL = "Enemies/Proto_crawl",
  ENEMIES_ROBOT_LOOP = "Enemies/Robot_loop",
  ENEMIES_ROBOT_TRIGGER = "Enemies/Robot_trigger",
  ENEMIES_SCORPION_ATK = "Enemies/Scorpion_atk",
  ENEMIES_SKELETON_COLLAPSE = "Enemies/Skeleton_collapse",
  ENEMIES_SKELETON_MATERIALIZE = "Enemies/Skeleton_materialize",
  ENEMIES_SNAKE_ATK = "Enemies/Snake_atk",
  ENEMIES_SORCERESS_ATK = "Enemies/Sorceress_atk",
  ENEMIES_SORCERESS_ATK_SPAWN = "Enemies/Sorceress_atk_spawn",
  ENEMIES_SORCERESS_CHARGE_LOOP = "Enemies/Sorceress_charge_loop",
  ENEMIES_SORCERESS_JUMP = "Enemies/Sorceress_jump",
  ENEMIES_SPIDER_JUMP = "Enemies/Spider_jump",
  ENEMIES_SPIDER_TRIGGER = "Enemies/Spider_trigger",
  ENEMIES_STONE_TRANSFORM_LOOP = "Enemies/Stone_transform_loop",
  ENEMIES_STORAGE_KEEPER_DIE = "Enemies/Storage_keeper_die",
  ENEMIES_STORAGE_KEEPER_JUMP = "Enemies/Storage_keeper_jump",
  ENEMIES_TADPOLE_SWIM = "Enemies/Tadpole_swim",
  ENEMIES_TIAMAT_HURT = "Enemies/Tiamat_hurt",
  ENEMIES_TIAMAT_ORB_LOOP = "Enemies/Tiamat_orb_loop",
  ENEMIES_TIAMAT_SCEPTER = "Enemies/Tiamat_scepter",
  ENEMIES_TIAMAT_SCREAM1 = "Enemies/Tiamat_scream1",
  ENEMIES_TIAMAT_SCREAM2 = "Enemies/Tiamat_scream2",
  ENEMIES_TIKIMAN_TALK = "Enemies/Tikiman_talk",
  ENEMIES_UFO_ATK_END = "Enemies/UFO_atk_end",
  ENEMIES_UFO_ATK_LOOP = "Enemies/UFO_atk_loop",
  ENEMIES_UFO_CHARGE = "Enemies/UFO_charge",
  ENEMIES_UFO_DAMAGE = "Enemies/UFO_damage",
  ENEMIES_UFO_EJECT = "Enemies/UFO_eject",
  ENEMIES_UFO_LOOP = "Enemies/UFO_loop",
  ENEMIES_VAMPIRE_JUMP = "Enemies/Vampire_jump",
  ENEMIES_VLAD_TRIGGER = "Enemies/Vlad_trigger",
  ENEMIES_WITCHDOCTOR_CHANT_LOOP = "Enemies/Witchdoctor_chant_loop",
  ENEMIES_WITCHDOCTOR_STAB = "Enemies/Witchdoctor_stab",
  ENEMIES_WITCHDOCTOR_TALK = "Enemies/Witchdoctor_talk",
  ENEMIES_WITCHDOCTOR_TRIGGER = "Enemies/Witchdoctor_trigger",
  ENEMIES_YETI_BIG_CHARGE = "Enemies/Yeti_big_charge",
  ENEMIES_YETI_BIG_PUNCH = "Enemies/Yeti_big_punch",
  ENEMIES_YETI_BIG_STEP = "Enemies/Yeti_big_step",
  ENEMIES_YETI_KING_ROAR = "Enemies/Yeti_king_roar",
  ENEMIES_YETI_QUEEN_JUMP = "Enemies/Yeti_queen_jump",
  ENEMIES_YETI_QUEEN_SLAM = "Enemies/Yeti_queen_slam",
  FX_FX_ANUBIS_WARN = "FX/FX_anubis_warn",
  FX_FX_COSMIC_ORB = "FX/FX_cosmic_orb",
  FX_FX_CURSE = "FX/FX_curse",
  FX_FX_DM_BANNER = "FX/FX_dm_banner",
  FX_FX_JOURNAL_ENTRY = "FX/FX_journal_entry",
  FX_FX_JOURNAL_PAGE = "FX/FX_journal_page",
  ITEMS_ARROW_STICK = "Items/Arrow_stick",
  ITEMS_BACKPACK_WARN = "Items/Backpack_warn",
  ITEMS_BOMB_BIG_TIMER = "Items/Bomb_big_timer",
  ITEMS_BOMB_STICK = "Items/Bomb_stick",
  ITEMS_BOMB_TIMER = "Items/Bomb_timer",
  ITEMS_BOOMBOX_OFF = "Items/Boombox_off",
  ITEMS_BOOMERANG_CATCH = "Items/Boomerang_catch",
  ITEMS_BOOMERANG_LOOP = "Items/Boomerang_loop",
  ITEMS_BOW = "Items/Bow",
  ITEMS_BOW_RELOAD = "Items/Bow_reload",
  ITEMS_CAMERA = "Items/Camera",
  ITEMS_CAPE_LOOP = "Items/Cape_loop",
  ITEMS_CAPE_VLAD_FLAP = "Items/Cape_vlad_flap",
  ITEMS_CLONE_GUN = "Items/Clone_gun",
  ITEMS_COIN_BOUNCE = "Items/Coin_bounce",
  ITEMS_CROSSBOW = "Items/Crossbow",
  ITEMS_CROSSBOW_RELOAD = "Items/Crossbow_reload",
  ITEMS_DAMSEL_CALL = "Items/Damsel_call",
  ITEMS_DAMSEL_PET = "Items/Damsel_pet",
  ITEMS_EXCALIBUR = "Items/Excalibur",
  ITEMS_FREEZE_RAY = "Items/Freeze_ray",
  ITEMS_FREEZE_RAY_HIT = "Items/Freeze_ray_hit",
  ITEMS_HOVERPACK_LOOP = "Items/Hoverpack_loop",
  ITEMS_JETPACK_END = "Items/Jetpack_end",
  ITEMS_JETPACK_IGNITE = "Items/Jetpack_ignite",
  ITEMS_JETPACK_LOOP = "Items/Jetpack_loop",
  ITEMS_MACHETE = "Items/Machete",
  ITEMS_MATTOCK_BREAK = "Items/Mattock_break",
  ITEMS_MATTOCK_HIT = "Items/Mattock_hit",
  ITEMS_MATTOCK_SWING = "Items/Mattock_swing",
  ITEMS_PARACHUTE = "Items/Parachute",
  ITEMS_PLASMA_CANNON = "Items/Plasma_cannon",
  ITEMS_PLASMA_CANNON_CHARGE = "Items/Plasma_cannon_charge",
  ITEMS_ROPE_ATTACH = "Items/Rope_attach",
  ITEMS_ROPE_BURN_LOOP = "Items/Rope_burn_loop",
  ITEMS_SCEPTER = "Items/Scepter",
  ITEMS_SHOTGUN_FIRE = "Items/Shotgun_fire",
  ITEMS_SPRING_SHOES = "Items/Spring_shoes",
  ITEMS_TV_LOOP = "Items/TV_loop",
  ITEMS_UDJAT_BLINK = "Items/Udjat_blink",
  ITEMS_USHABTI_RATTLE = "Items/Ushabti_rattle",
  ITEMS_WEBGUN = "Items/Webgun",
  ITEMS_WEBGUN_HIT = "Items/Webgun_hit",
  ITEMS_WITCHDOCTORSKULL_LOOP = "Items/Witchdoctorskull_loop",
  ITEMS_WOODEN_SHIELD_BREAK = "Items/Wooden_shield_break",
  ITEMS_WOODEN_SHIELD_DAMAGE = "Items/Wooden_shield_damage",
  LIQUIDS_LAVA_STATIC_LOOP = "Liquids/Lava_static_loop",
  LIQUIDS_LAVA_STREAM_LOOP = "Liquids/Lava_stream_loop",
  LIQUIDS_WATER_REV_STREAM_LOOP = "Liquids/Water_rev_stream_loop",
  LIQUIDS_WATER_SPLASH = "Liquids/Water_splash",
  LIQUIDS_WATER_STREAM_LOOP = "Liquids/Water_stream_loop",
  MENU_CANCEL = "Menu/Cancel",
  MENU_CHARSEL_DESELECTION = "Menu/Charsel_deselection",
  MENU_CHARSEL_DOOR = "Menu/Charsel_door",
  MENU_CHARSEL_NAVI = "Menu/Charsel_navi",
  MENU_CHARSEL_QUICK_NAVI = "Menu/Charsel_quick_navi",
  MENU_CHARSEL_QUICK_NOPE = "Menu/Charsel_quick_nope",
  MENU_CHARSEL_QUICK_OPEN = "Menu/Charsel_quick_open",
  MENU_CHARSEL_SCROLL = "Menu/Charsel_scroll",
  MENU_CHARSEL_SELECTION = "Menu/Charsel_selection",
  MENU_CHARSEL_SELECTION2 = "Menu/Charsel_selection2",
  MENU_DIRT_FALL = "Menu/Dirt_fall",
  MENU_JOURNAL_STICKER = "Menu/Journal_sticker",
  MENU_MM_BAR = "Menu/MM_bar",
  MENU_MM_NAVI = "Menu/MM_navi",
  MENU_MM_OPTIONS_SUB = "Menu/MM_options_sub",
  MENU_MM_RESET = "Menu/MM_reset",
  MENU_MM_SELECTION = "Menu/MM_selection",
  MENU_MM_SET = "Menu/MM_set",
  MENU_MM_TOGGLE = "Menu/MM_toggle",
  MENU_NAVI = "Menu/Navi",
  MENU_PAGE_RETURN = "Menu/Page_return",
  MENU_PAGE_TURN = "Menu/Page_turn",
  MENU_SELECTION = "Menu/Selection",
  MENU_TITLE_SELECTION = "Menu/Title_selection",
  MENU_TITLE_TORCH_LOOP = "Menu/Title_torch_loop",
  MENU_ZOOM_IN = "Menu/Zoom_in",
  MENU_ZOOM_OUT = "Menu/Zoom_out",
  MOUNTS_AXOLOTL_ATK = "Mounts/Axolotl_atk",
  MOUNTS_AXOLOTL_ATK_HIT = "Mounts/Axolotl_atk_hit",
  MOUNTS_AXOLOTL_JUMP1 = "Mounts/Axolotl_jump1",
  MOUNTS_AXOLOTL_JUMP2 = "Mounts/Axolotl_jump2",
  MOUNTS_AXOLOTL_UNTAMED_LOOP = "Mounts/Axolotl_untamed_loop",
  MOUNTS_AXOLOTL_WALK = "Mounts/Axolotl_walk",
  MOUNTS_MECH_DRIVE_LOOP = "Mounts/Mech_drive_loop",
  MOUNTS_MECH_JUMP = "Mounts/Mech_jump",
  MOUNTS_MECH_PUNCH1 = "Mounts/Mech_punch1",
  MOUNTS_MECH_SMASH = "Mounts/Mech_smash",
  MOUNTS_MECH_SPARK = "Mounts/Mech_spark",
  MOUNTS_MECH_TRANSFORM = "Mounts/Mech_transform",
  MOUNTS_MECH_TURN = "Mounts/Mech_turn",
  MOUNTS_MECH_WALK1 = "Mounts/Mech_walk1",
  MOUNTS_MECH_WALK2 = "Mounts/Mech_walk2",
  MOUNTS_MECH_WARN = "Mounts/Mech_warn",
  MOUNTS_MOUNT = "Mounts/Mount",
  MOUNTS_MOUNT_LAND = "Mounts/Mount_land",
  MOUNTS_MOUNT_TAME = "Mounts/Mount_tame",
  MOUNTS_QILIN_FLY_LOOP = "Mounts/Qilin_fly_loop",
  MOUNTS_QILIN_HATCH = "Mounts/Qilin_hatch",
  MOUNTS_QILIN_JUMP1 = "Mounts/Qilin_jump1",
  MOUNTS_QILIN_JUMP2 = "Mounts/Qilin_jump2",
  MOUNTS_QILIN_WALK = "Mounts/Qilin_walk",
  MOUNTS_TURKEY_ATK = "Mounts/Turkey_atk",
  MOUNTS_TURKEY_FLAP = "Mounts/Turkey_flap",
  MOUNTS_TURKEY_JUMP = "Mounts/Turkey_jump",
  MOUNTS_TURKEY_UNTAMED_LOOP = "Mounts/Turkey_untamed_loop",
  MOUNTS_TURKEY_WALK = "Mounts/Turkey_walk",
  MOUNTS_WILDDOG_FIREBALL_LOOP = "Mounts/Wilddog_fireball_loop",
  MOUNTS_WILDDOG_JUMP1 = "Mounts/Wilddog_jump1",
  MOUNTS_WILDDOG_JUMP2 = "Mounts/Wilddog_jump2",
  MOUNTS_WILDDOG_UNTAMED_LOOP = "Mounts/Wilddog_untamed_loop",
  MOUNTS_WILDDOG_WALK = "Mounts/Wilddog_walk",
  PLAYER_DEATH_GHOST = "Player/Death_ghost",
  PLAYER_ENTER_DOOR = "Player/Enter_door",
  PLAYER_EQUIP = "Player/Equip",
  PLAYER_GRAB_LEDGE = "Player/Grab_ledge",
  PLAYER_INKED = "Player/Inked",
  PLAYER_JUMP = "Player/Jump",
  PLAYER_LAND_CHAIN = "Player/Land_chain",
  PLAYER_LISE_DRIVE_LOOP = "Player/LISE_drive_loop",
  PLAYER_LISE_LOADING_LOOP = "Player/LISE_loading_loop",
  PLAYER_LISE_PUSH_LOOP = "Player/LISE_push_loop",
  PLAYER_LISE_RADAR_LOOP = "Player/LISE_radar_loop",
  PLAYER_LISE_WARNING = "Player/LISE_warning",
  PLAYER_NO_ITEM = "Player/No_item",
  PLAYER_PGHOST_ATK = "Player/Pghost_atk",
  PLAYER_PGHOST_CHARGE_LOOP = "Player/Pghost_charge_loop",
  PLAYER_PGHOST_DASH = "Player/Pghost_dash",
  PLAYER_PGHOST_SHAKE = "Player/Pghost_shake",
  PLAYER_PGHOST_SPAWN = "Player/Pghost_spawn",
  PLAYER_PGHOST_SPIN = "Player/Pghost_spin",
  PLAYER_PUSH_BLOCK_LOOP = "Player/Push_block_loop",
  PLAYER_TOSS_ROPE = "Player/Toss_rope",
  PLAYER_WHIP1 = "Player/Whip1",
  PLAYER_WHIP2 = "Player/Whip2",
  PLAYER_WHIP_JUMP = "Player/Whip_jump",
  SHARED_ANGER = "Shared/Anger",
  SHARED_BLOCK_LAND = "Shared/Block_land",
  SHARED_BLOOD_SPLURT = "Shared/Blood_splurt",
  SHARED_BUBBLE_BONK = "Shared/Bubble_bonk",
  SHARED_BUBBLE_BURST = "Shared/Bubble_burst",
  SHARED_BUBBLE_BURST_BIG = "Shared/Bubble_burst_big",
  SHARED_CEILING_CRUMBLE = "Shared/Ceiling_crumble",
  SHARED_CLIMB = "Shared/Climb",
  SHARED_COFFIN_BREAK = "Shared/Coffin_break",
  SHARED_COFFIN_RATTLE = "Shared/Coffin_rattle",
  SHARED_COLLISION_SURFACE = "Shared/Collision_surface",
  SHARED_COSMIC_ORB_DESTROY = "Shared/Cosmic_orb_destroy",
  SHARED_COSMIC_ORB_LOOP = "Shared/Cosmic_orb_loop",
  SHARED_CURSED_LOOP = "Shared/Cursed_loop",
  SHARED_CURSE_GET = "Shared/Curse_get",
  SHARED_DAMAGED = "Shared/Damaged",
  SHARED_DAMAGED_FIRE = "Shared/Damaged_fire",
  SHARED_DAMAGED_POISON = "Shared/Damaged_poison",
  SHARED_DARK_LEVEL_START = "Shared/Dark_level_start",
  SHARED_DESTRUCTIBLE_BREAK = "Shared/Destructible_break",
  SHARED_DOOR_AMB_LOOP = "Shared/Door_amb_loop",
  SHARED_DOOR_UNLOCK = "Shared/Door_unlock",
  SHARED_DROP = "Shared/Drop",
  SHARED_EXPLOSION = "Shared/Explosion",
  SHARED_EXPLOSION_MODERN = "Shared/Explosion_modern",
  SHARED_FIRE_IGNITE = "Shared/Fire_ignite",
  SHARED_FIRE_LOOP = "Shared/Fire_loop",
  SHARED_GRAB_CLIMBABLE = "Shared/Grab_climbable",
  SHARED_HH_ANGER = "Shared/HH_anger",
  SHARED_HH_OBEY = "Shared/HH_obey",
  SHARED_HUMANOID_JUMP = "Shared/Humanoid_jump",
  SHARED_ICE_BREAK = "Shared/Ice_break",
  SHARED_ICE_SLIDE_LOOP = "Shared/Ice_slide_loop",
  SHARED_IMPALED = "Shared/Impaled",
  SHARED_LAND = "Shared/Land",
  SHARED_LANTERN_BREAK = "Shared/Lantern_break",
  SHARED_NEON_SIGN_LOOP = "Shared/Neon_sign_loop",
  SHARED_OPEN_CHEST = "Shared/Open_chest",
  SHARED_OPEN_CRATE = "Shared/Open_crate",
  SHARED_PICK_UP = "Shared/Pick_up",
  SHARED_POISON_WARN = "Shared/Poison_warn",
  SHARED_PORTAL_LOOP = "Shared/Portal_loop",
  SHARED_RICOCHET = "Shared/Ricochet",
  SHARED_RUBBLE_BREAK = "Shared/Rubble_break",
  SHARED_SACRIFICE = "Shared/Sacrifice",
  SHARED_SACRIFICE_EGGPLANT = "Shared/Sacrifice_eggplant",
  SHARED_SCARAB_LOOP = "Shared/Scarab_loop",
  SHARED_SLEEP_BUBBLE = "Shared/Sleep_bubble",
  SHARED_SMOKE_TELEPORT = "Shared/Smoke_teleport",
  SHARED_STORAGE_PAD_ACTIVATE = "Shared/Storage_pad_activate",
  SHARED_STUNNED_WAKE = "Shared/Stunned_wake",
  SHARED_TELEPORT = "Shared/Teleport",
  SHARED_TILE_BREAK = "Shared/Tile_break",
  SHARED_TOSS = "Shared/Toss",
  SHARED_TOSS_FIRE = "Shared/Toss_fire",
  SHARED_TRIP = "Shared/Trip",
  SHARED_WAKE_BLINK = "Shared/Wake_blink",
  SHARED_WEBBED = "Shared/Webbed",
  SHOP_SHOP_BUY = "Shop/Shop_buy",
  SHOP_SHOP_ENTER = "Shop/Shop_enter",
  SHOP_SHOP_FOCUS = "Shop/Shop_focus",
  SHOP_SHOP_NOPE = "Shop/Shop_nope",
  SHOP_SHOP_PICK_UP = "Shop/Shop_pick_up",
  TRANSITIONS_TRANS_ANGER = "Transitions/Trans_anger",
  TRANSITIONS_TRANS_ANKH = "Transitions/Trans_ankh",
  TRANSITIONS_TRANS_DARK = "Transitions/Trans_dark",
  TRANSITIONS_TRANS_DARK_FIRST = "Transitions/Trans_dark_first",
  TRANSITIONS_TRANS_DEATH = "Transitions/Trans_death",
  TRANSITIONS_TRANS_DM_RESULTS = "Transitions/Trans_dm_results",
  TRANSITIONS_TRANS_LAYER = "Transitions/Trans_layer",
  TRANSITIONS_TRANS_LAYER_SPECIAL = "Transitions/Trans_layer_special",
  TRANSITIONS_TRANS_OUROBOROS = "Transitions/Trans_ouroboros",
  TRANSITIONS_TRANS_PAUSE = "Transitions/Trans_pause",
  TRANSITIONS_TRANS_PIPE = "Transitions/Trans_pipe",
  TRANSITIONS_TRANS_SHOP = "Transitions/Trans_shop",
  TRANSITIONS_TRANS_THEME = "Transitions/Trans_theme",
  TRANSITIONS_TRANS_TUNNEL = "Transitions/Trans_tunnel",
  TRAPS_ARROWTRAP_TRIGGER = "Traps/Arrowtrap_trigger",
  TRAPS_BOULDER_CRUSH = "Traps/Boulder_crush",
  TRAPS_BOULDER_EMERGE = "Traps/Boulder_emerge",
  TRAPS_BOULDER_LOOP = "Traps/Boulder_loop",
  TRAPS_BOULDER_WARN_LOOP = "Traps/Boulder_warn_loop",
  TRAPS_CONVEYOR_BELT_LOOP = "Traps/Conveyor_belt_loop",
  TRAPS_CRUSHTRAP_BIG_STOP = "Traps/Crushtrap_big_stop",
  TRAPS_CRUSHTRAP_STOP = "Traps/Crushtrap_stop",
  TRAPS_DRILL_LOOP = "Traps/Drill_loop",
  TRAPS_DUAT_WALL_LOOP = "Traps/Duat_wall_loop",
  TRAPS_ELEVATOR_DOWN = "Traps/Elevator_down",
  TRAPS_ELEVATOR_UP = "Traps/Elevator_up",
  TRAPS_GENERATOR_GENERATE = "Traps/Generator_generate",
  TRAPS_GIANTCLAM_CLOSE = "Traps/Giantclam_close",
  TRAPS_GIANTCLAM_OPEN = "Traps/Giantclam_open",
  TRAPS_KALI_ANGERED = "Traps/Kali_angered",
  TRAPS_LASERBEAM_CHARGE = "Traps/Laserbeam_charge",
  TRAPS_LASERBEAM_COLLISION = "Traps/Laserbeam_collision",
  TRAPS_LASERBEAM_END = "Traps/Laserbeam_end",
  TRAPS_LASERBEAM_LOOP = "Traps/Laserbeam_loop",
  TRAPS_LASERTRAP_CHARGE = "Traps/Lasertrap_charge",
  TRAPS_LASERTRAP_TRIGGER = "Traps/Lasertrap_trigger",
  TRAPS_LIONTRAP_ATK = "Traps/Liontrap_atk",
  TRAPS_LIONTRAP_TRIGGER = "Traps/Liontrap_trigger",
  TRAPS_MINE_ACTIVATE = "Traps/Mine_activate",
  TRAPS_MINE_BLINK = "Traps/Mine_blink",
  TRAPS_MINE_DEACTIVATE = "Traps/Mine_deactivate",
  TRAPS_PIPE_LOOP = "Traps/Pipe_loop",
  TRAPS_PLATFORM_BREAK = "Traps/Platform_break",
  TRAPS_PLATFORM_TRIGGER = "Traps/Platform_trigger",
  TRAPS_QUICKSAND_AMB_LOOP = "Traps/Quicksand_amb_loop",
  TRAPS_QUICKSAND_LOOP = "Traps/Quicksand_loop",
  TRAPS_REGENBLOCK_GROW = "Traps/Regenblock_grow",
  TRAPS_SKULLBLOCK_ATK = "Traps/Skullblock_atk",
  TRAPS_SKULLBLOCK_TRIGGER = "Traps/Skullblock_trigger",
  TRAPS_SKULLDROP_DROP = "Traps/Skulldrop_drop",
  TRAPS_SKULLDROP_LOOP = "Traps/Skulldrop_loop",
  TRAPS_SLIDEWALL_STOMP = "Traps/Slidewall_stomp",
  TRAPS_SNAPTRAP_CLOSE = "Traps/Snaptrap_close",
  TRAPS_SNAPTRAP_OPEN = "Traps/Snaptrap_open",
  TRAPS_SPARK_HIT = "Traps/Spark_hit",
  TRAPS_SPARK_LOOP = "Traps/Spark_loop",
  TRAPS_SPEARTRAP_ATK = "Traps/Speartrap_atk",
  TRAPS_SPEARTRAP_TRIGGER = "Traps/Speartrap_trigger",
  TRAPS_SPIKE_BALL_DROP_LOOP = "Traps/Spike_ball_drop_loop",
  TRAPS_SPIKE_BALL_END = "Traps/Spike_ball_end",
  TRAPS_SPIKE_BALL_HIT = "Traps/Spike_ball_hit",
  TRAPS_SPIKE_BALL_RISE_LOOP = "Traps/Spike_ball_rise_loop",
  TRAPS_SPRING_TRIGGER = "Traps/Spring_trigger",
  TRAPS_STICKYTRAP_DROP_LOOP = "Traps/Stickytrap_drop_loop",
  TRAPS_STICKYTRAP_END = "Traps/Stickytrap_end",
  TRAPS_STICKYTRAP_HIT = "Traps/Stickytrap_hit",
  TRAPS_STICKYTRAP_RISE_LOOP = "Traps/Stickytrap_rise_loop",
  TRAPS_STICKYTRAP_WAKE = "Traps/Stickytrap_wake",
  TRAPS_SWITCH_FLICK = "Traps/Switch_flick",
  TRAPS_THINICE_CRACK = "Traps/Thinice_crack",
  TRAPS_TIKI_ATK = "Traps/Tiki_atk",
  TRAPS_TIKI_TRIGGER = "Traps/Tiki_trigger",
  TRAPS_WOODENLOG_CRUSH = "Traps/Woodenlog_crush",
  TRAPS_WOODENLOG_TRIGGER = "Traps/Woodenlog_trigger",
  UI_DAMSEL_KISS = "UI/Damsel_kiss",
  UI_DEPOSIT = "UI/Deposit",
  UI_GET_GEM = "UI/Get_gem",
  UI_GET_GOLD = "UI/Get_gold",
  UI_GET_ITEM1 = "UI/Get_item1",
  UI_GET_ITEM2 = "UI/Get_item2",
  UI_GET_SCARAB = "UI/Get_scarab",
  UI_JOURNAL_ENTRY = "UI/Journal_entry",
  UI_JOURNAL_OFF = "UI/Journal_off",
  UI_JOURNAL_ON = "UI/Journal_on",
  UI_KAPPALA_HEAL = "UI/Kappala_heal",
  UI_NPC_VOCAL = "UI/NPC_vocal",
  UI_PAUSE_MENU_OFF = "UI/Pause_menu_off",
  UI_PAUSE_MENU_ON = "UI/Pause_menu_on",
  UI_SECRET = "UI/Secret",
  UI_SECRET2 = "UI/Secret2",
  UI_TEXT_DESCRIPTION = "UI/Text_description",
  UI_TUNNEL_COUNT = "UI/Tunnel_count",
  UI_TUNNEL_SCROLL = "UI/Tunnel_scroll",
  UI_TUNNEL_TABLET_DOWN = "UI/Tunnel_tablet_down",
  UI_TUNNEL_TABLET_UP = "UI/Tunnel_tablet_up",
  UI_ZOOM_IN = "UI/Zoom_in",
  UI_ZOOM_OUT = "UI/Zoom_out"
}
---@alias VANILLA_SOUND integer
VANILLA_SOUND_CALLBACK_TYPE = {
  CREATED = 1,
  DESTROYED = 2,
  RESTARTED = 16,
  STARTED = 8,
  START_FAILED = 64,
  STOPPED = 32
}
---@alias VANILLA_SOUND_CALLBACK_TYPE integer
VANILLA_SOUND_PARAM = {
  ANGER_PROXIMITY = 11,
  ANGER_STATE = 12,
  CAM_DEPTH = 24,
  COLLISION_MATERIAL = 14,
  CURRENT_LAYER2 = 37,
  CURRENT_LEVEL = 35,
  CURRENT_SHOP_TYPE = 36,
  CURRENT_THEME = 34,
  CURSED = 28,
  DIST_CENTER_X = 1,
  DIST_CENTER_Y = 2,
  DIST_PLAYER = 4,
  DIST_Z = 3,
  DM_STATE = 32,
  FAST_FORWARD = 33,
  FIRST_RUN = 23,
  GHOST = 9,
  LIGHTNESS = 16,
  LIQUID_INTENSITY = 7,
  LIQUID_STREAM = 6,
  MONSTER_ID = 19,
  PAGE = 31,
  PLAYER_ACTIVITY = 20,
  PLAYER_CHARACTER = 30,
  PLAYER_CONTROLLED = 29,
  PLAYER_DEPTH = 22,
  PLAYER_LIFE = 21,
  POISONED = 27,
  POS_SCREEN_X = 0,
  RESTLESS_DEAD = 25,
  SIZE = 17,
  SPECIAL_MACHINE = 26,
  SUBMERGED = 5,
  TORCH_PROXIMITY = 13,
  TRIGGER = 10,
  TYPE_ID = 18,
  VALUE = 8,
  VELOCITY = 15
}
---@alias VANILLA_SOUND_PARAM integer
VANILLA_TEXT_ALIGNMENT = {
  CENTER = 1,
  LEFT = 0,
  RIGHT = 2
}
---@alias VANILLA_TEXT_ALIGNMENT integer
WIN_STATE = {
  COSMIC_OCEAN_WIN = 3,
  HUNDUN_WIN = 2,
  NO_WIN = 0,
  TIAMAT_WIN = 1
}
---@alias WIN_STATE integer
WORLD_SHADER = {
  COLOR = 0,
  DEFERRED_COLOR_TRANSPARENT = 6,
  DEFERRED_TEXTURE_COLOR = 7,
  DEFERRED_TEXTURE_COLOR_CURSED = 9,
  DEFERRED_TEXTURE_COLOR_EMISSIVE = 16,
  DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW = 22,
  DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW = 23,
  DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION = 24,
  DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW = 18,
  DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS = 20,
  DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY = 19,
  DEFERRED_TEXTURE_COLOR_POISONED = 8,
  DEFERRED_TEXTURE_COLOR_POISONED_CURSED = 10,
  DEFERRED_TEXTURE_COLOR_TRANSPARENT = 11,
  DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED = 12,
  TEXTURE = 1,
  TEXTURE_ALPHA_COLOR = 3,
  TEXTURE_COLOR = 2,
  TEXTURE_COLORS_WARP = 5
}
---@alias WORLD_SHADER integer
YANG = {
  ANGRY = -1,
  BOTH_TURKEYS_DELIVERED = 3,
  ONE_TURKEY_BOUGHT = 5,
  QUEST_NOT_STARTED = 0,
  THREE_TURKEYS_BOUGHT = 7,
  TURKEY_PEN_SPAWNED = 2,
  TURKEY_SHOP_SPAWNED = 4,
  TWO_TURKEYS_BOUGHT = 6
}
---@alias YANG integer
local MAX_PLAYERS = 4

---@class Logic

---@alias OnlinePlayerShort any
---@alias UdpServer any
---@alias SpearDanglerAnimFrames any
---@alias OnlineLobbyScreenPlayer any
---@alias SoundCallbackFunction function

--## Aliases

---@alias IMAGE number
---@alias CallbackId integer;
---@alias Flags integer;
---@alias uColor integer;
---@alias SHORT_TILE_CODE integer;
---@alias STRINGID integer;
---@alias SOUNDID integer;
---@alias FEAT integer;
