# Overlunky Lua API
- Everything here is still changing, don't be sad if your scripts break next week!
- This doc doesn't have a lot of examples, that's why we have [examples/](https://github.com/spelunky-fyi/overlunky/tree/main/examples).
- This doc and the examples are written for a person who already knows [how to program in Lua](http://lua-users.org/wiki/TutorialDirectory).
- This doc is up to date for the [WHIP build](https://github.com/spelunky-fyi/overlunky/releases/tag/whip). If you're using an official release from the past, you might find some things here don't work.
- You can find changes to and earlier versions of this doc [here](https://github.com/spelunky-fyi/overlunky/commits/main/docs/script-api.md).
- Click on the names of things to search for examples on how to use that function or variable.
## Lua libraries
The following Lua libraries and their functions are available. You can read more about them in the [Lua documentation](https://www.lua.org/manual/5.4/manual.html#6).
### `math`
### `base`
### `string`
### `table`
### `coroutine`
### `package`
## Unsafe mode
Setting `meta.unsafe = true` enables the rest of the standard Lua libraries like `io` and `os`, loading dlls with require and `package.loadlib`. Using unsafe scripts requires users to enable the option in the overlunky.ini file which is found in the Spelunky 2 installation directory.
## Modules
You can load modules with `require "mymod"` or `require "mydir.mymod"`, just put `mymod.lua` in the same directory the script is, or in `mydir/` to keep things organized.

Check the [Lua tutorial](http://lua-users.org/wiki/ModulesTutorial) or examples how to actually make modules.
## Global variables
These variables are always there to use.
### [`meta`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=meta)
Table of strings where you should set some script metadata shown in the UI.
- `meta.name` Script name
- `meta.version` Version
- `meta.description` Short description of the script
- `meta.author` Your name
### [`state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state)
A bunch of [game state](#statememory) variables
Example:
```lua
if state.time_level > 300 and state.theme == THEME.DWELLING then
    toast("Congratulations for lasting 5 seconds in Dwelling")
end
```
### [`players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players)
An array of [Player](#player) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
### [`savegame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=savegame)
Provides a read-only access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.)
### [`options`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=options)
Table of options set in the UI, added with the [register_option_functions](#register_option_int).
## Event functions
Define these in your script to be called on an event. For example:
```lua
function on_level()
    toast("Welcome to the level")
end
```
### [`on_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_frame)
Runs on every game engine frame.
### [`on_camp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_camp)
Runs on entering the camp.
### [`on_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_level)
Runs on the start of every level.
### [`on_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_start)
Runs on the start of first level.
### [`on_transition`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_transition)
Runs on the start of level transition.
### [`on_death`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_death)
Runs on the death screen.
### [`on_win`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_win)
Runs on any ending cutscene.
### [`on_screen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_screen)
Runs on any [screen change](#on).
### [`on_guiframe`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_guiframe)
Runs on every screen frame. You need this to use draw functions.
## Functions
Note: The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen.
### [`message`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=message)
#### Params: `string message`
Print a log message on screen.
### [`set_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_interval)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called every `frames` engine frames. Timer is paused on pause and cleared on level transition.
### [`set_timeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_timeout)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
### [`set_global_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_interval)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.
### [`set_global_timeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_timeout)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.
### [`set_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback)
#### Params: `function cb, int screen`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called on an [event](#on).
### [`clear_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_callback)
#### Params: `int id`
Clear previously added callback `id`
### [`load_script`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load_script)
#### Params: `string id`
Load another script by id "author/name"
### [`seed_prng`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed_prng)
#### Params: `int64_t seed`
Seed the game prng.
### [`read_prng`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_prng)
#### Returns: `int[20]`
Read the game prng state. Maybe you can use these and math.randomseed() to make deterministic things, like online scripts :shrug:. Example:
```lua
-- this should always print the same table D877...E555
set_callback(function()
  seed_prng(42069)
  local prng = read_prng()
  for i,v in ipairs(prng) do
    message(string.format("%08X", v))
  end
end, ON.LEVEL)
```
### [`toast`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast)
#### Params: `string message`
Show a message that looks like a level feeling.
### [`say`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=say)
#### Params: `int entity_id, string message, int unk_type, bool top`
Show a message coming from an entity
### [`register_option_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_int)
#### Params: `string name, string desc, string long_desc, int value, int min, int max`
Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
limits, you can override them in the UI with double click.
### [`register_option_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_float)
#### Params: `string name, string desc, string long_desc, float value, float min, float max`
Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
limits, you can override them in the UI with double click.
### [`register_option_bool`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_bool)
#### Params: `string name, string desc, string long_desc, bool value`
Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
### [`register_option_string`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_string)
#### Params: `string name, string desc, string long_desc, string value`
Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
### [`register_option_combo`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_combo)
#### Params: `string name, string desc, string long_desc, string opts`
Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
with a double `\0\0` at the end.
### [`register_option_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_button)
### [`spawn_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity)
#### Params: `int id, float x, float y, int layer, float vx, float vy`
#### Returns: `int`
Spawn an entity in position with some velocity and return the uid of spawned entity.
Uses level coordinates with [LAYER.FRONT](#layer) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn.
Example:
```lua
-- spawn megajelly using absolute coordinates
set_callback(function()
    x, y, layer = get_position(players[1].uid)
    spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
end, ON.LEVEL)
-- spawn clover using player-relative coordinates
set_callback(function()
    spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 0, 1, LAYER.PLAYER1, 0, 0)
end, ON.LEVEL)
```
### [`spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn)
#### Params: `int id, float x, float y, int layer, float vx, float vy`
#### Returns: `int`
Short for [spawn_entity](#spawn_entity).
### [`spawn_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_door)
#### Params: `float x, float y, int layer, int w, int l, int t`
#### Returns: `int`
Spawn a door to another world, level and theme and return the uid of spawned entity.
Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
### [`door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door)
#### Params: `float x, float y, int layer, int w, int l, int t`
#### Returns: `int`
Short for [spawn_door](#spawn_door).
### [`spawn_layer_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_layer_door)
#### Params: `float x, float y`
Spawn a door to backlayer.
### [`layer_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=layer_door)
#### Params: `float x, float y`
Short for [spawn_layer_door](#spawn_layer_door).
### [`warp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=warp)
#### Params: `int w, int l, int t`
Warp to a level immediately.
### [`set_seed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_seed)
#### Params: `int seed`
Set seed and reset run.
### [`god`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=god)
#### Params: `bool g`
Enable/disable godmode.
### [`force_dark_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_dark_level)
#### Params: `bool g`
Try to force next levels to be dark.
### [`zoom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=zoom)
#### Params: `float level`
Set the zoom level used in levels and shops. 13.5 is the default.
### [`pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause)
#### Params: `bool p`
Enable/disable game engine pause.
### [`move_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_entity)
#### Params: `int id, float x, float y, float vx, float vy`
Teleport entity to coordinates with optional velocity
### [`set_door_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_door_target)
#### Params: `int id, int w, int l, int t`
Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
### [`set_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_door)
#### Params: `int id, int w, int l, int t`
Short for [set_door_target](#set_door_target).
### [`get_door_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_door_target)
#### Params: `int id`
#### Returns: `tuple<int, int, int>`
Get door target `world`, `level`, `theme`
### [`set_contents`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_contents)
#### Params: `int id, int item`
Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `id` to ENT_TYPE... `item`
### [`get_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity)
#### Params: `int id`
#### Returns: `Entity`
Get the [Entity](#entity) behind an uid
### [`get_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_type)
#### Params: `int id`
#### Returns: `EntityDB`
Get the [EntityDB](#entitydb) behind an ENT_TYPE...
### [`get_entities`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities)
#### Returns: `array<int>`
Get uids of all entities currently loaded
### [`get_entities_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by)
#### Params: `int type, int mask, int layer`
#### Returns: `array<int>`
Get uids of entities by some conditions. Set `type` or `mask` to `0` to ignore that.
### [`get_entities_by_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_type)
#### Params: `int, int...`
#### Returns: `array<int>`
Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
You can even pass a table! Example:
```lua
types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
function on_level()
    uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    -- is not the same thing as this, but also works
    uids2 = get_entities_by_type(types)
    message(tostring(#uids).." == "..tostring(#uids2))
end
```
### [`get_entities_by_mask`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_mask)
#### Params: `int mask`
#### Returns: `array<int>`
Get uids of entities by some search_flags
### [`get_entities_by_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_layer)
#### Params: `int layer`
#### Returns: `array<int>`
Get uids of entities by layer. `0` for main level, `1` for backlayer, `-1` for layer of the player.
### [`get_entities_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_at)
#### Params: `int type, int mask, float x, float y, int layer, float r`
#### Returns: `array<int>`
Get uids of matching entities inside some radius. Set `type` or `mask` to `0` to ignore that.
### [`get_entities_overlapping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping)
#### Params: `int type, int mask, float sx, float sy, float sx2, float sy2, int layer`
#### Returns: `array<int>`
Get uids of matching entities overlapping with the given rect. Set `type` or `mask` to `0` to ignore that.
list[uint32_t] get_entities_overlapping(uint32_t type, uint32_t mask, float sx, float sy, float sx2, float sy2, int layer)
### [`get_entity_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags)
#### Params: `int id`
#### Returns: `int`
Get the `flags` field from entity by uid
### [`set_entity_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags)
#### Params: `int id, int flags`
Set the `flags` field from entity by uid
### [`get_entity_flags2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags2)
#### Params: `int id`
#### Returns: `int`
Get the `more_flags` field from entity by uid
### [`set_entity_flags2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags2)
#### Params: `int id, int flags`
Set the `more_flags` field from entity by uid
### [`get_entity_ai_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_ai_state)
#### Params: `int id`
#### Returns: `int`
Get the `move_state` field from entity by uid
### [`get_level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_level_flags)
#### Returns: `int`
Get `state.level_flags`
### [`set_level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_level_flags)
#### Params: `int flags`
Set `state.level_flags`
### [`get_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_type)
#### Params: `int id`
#### Returns: `int`
Get the ENT_TYPE... for entity by uid
### [`get_zoom_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_zoom_level)
#### Returns: `float`
Get the current set zoom level
### [`game_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_position)
#### Params: `float x, float y`
#### Returns: `pair<float, float>`
Get the game coordinates at the screen position (`x`, `y`)
### [`screen_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_position)
#### Params: `float x, float y`
#### Returns: `pair<float, float>`
Translate an entity position to screen position to be used in drawing functions
### [`screen_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_distance)
#### Params: `float x`
#### Returns: `float`
Translate a distance of `x` tiles to screen distance to be be used in drawing functions
### [`get_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_position)
Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
you're standing on, not real level coordinates.
### [`get_render_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_render_position)
Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...
### [`entity_remove_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_remove_item)
#### Params: `int id, int item`
Remove item by uid from entity
### [`spawn_entity_over`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_over)
#### Params: `int id, int over, float x, float y`
#### Returns: `int`
Spawn an entity by `id` attached to some other entity `over`, in offset `x`, `y`
### [`entity_has_item_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_uid)
#### Params: `int id, int item`
#### Returns: `bool`
Check if the entity `id` has some specific `item` by uid in their inventory
### [`entity_has_item_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_type)
#### Params: `int id, int type`
#### Returns: `bool`
Check if the entity `id` has some ENT_TYPE `type` in their inventory
### [`kill_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kill_entity)
#### Params: `int id`
Kills an entity by uid.
### [`pick_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pick_up)
#### Params: `int who, int what`
Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen. Example:
```lua
-- spawn and equip a jetpack
pick_up(players[1].uid, spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER, 0, 0))
```
### [`apply_entity_db`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=apply_entity_db)
#### Params: `int id`
Apply changes made in [get_type](#get_type)() to entity instance by uid.
### [`lock_door_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lock_door_at)
#### Params: `float x, float y`
Try to lock the exit at coordinates
### [`unlock_door_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unlock_door_at)
#### Params: `float x, float y`
Try to unlock the exit at coordinates
### [`get_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_frame)
#### Returns: `int`
Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
### [`get_ms`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ms)
Get the current timestamp in milliseconds since the Unix Epoch.
### [`carry`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carry)
#### Params: `int mount, int rider`
Make `mount` carry `rider` on their back. Only use this with actual mounts and living things.
### [`set_arrowtrap_projectile`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_arrowtrap_projectile)
#### Params: `int regular_item_id, int poison_item_id`
Sets the arrow type (wooden, metal, light) that is shot from a regular arrow trap and a poison arrow trap.
### [`set_kapala_blood_threshold`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_kapala_blood_threshold)
#### Params: `int threshold`
Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).
### [`set_kapala_hud_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_kapala_hud_icon)
#### Params: `int8_t icon_index`
Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour).
If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!
### [`modify_sparktraps`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=modify_sparktraps)
#### Params: `float angle_increment = 0.015, float distance = 3.0`
Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!
### [`set_blood_multiplication`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_blood_multiplication)
#### Params: `int default_multiplier, int vladscape_multiplier`
Sets the multiplication factor for blood droplets (default/no Vlad's cape = 1, with Vlad's cape = 2)
### [`flip_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flip_entity)
#### Params: `int id`
Flip entity around by uid. All new entities face right by default.
### [`set_olmec_phase_y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_olmec_phase_y_level)
#### Params: `int phase, float y`
Sets the Y-level at which Olmec changes phases
### [`set_ghost_spawn_times`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_ghost_spawn_times)
#### Params: `int normal = 10800, int cursed = 9000`
Determines when the ghost appears, either when the player is cursed or not
### [`get_particle_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_particle_type)
Get the [ParticleDB](#particledb) details of the specified ID
### [`distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance)
#### Params: `int a, int b`
Calculate the tile distance of two entities by uid
### [`get_bounds`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_bounds)
#### Returns: `float`, `float`, `float`, `float`
Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
inside these boundaries. The order is: top left x, top left y, bottom right x, bottom right y Example:
```lua
-- Draw the level boundaries
set_callback(function()
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(xmin, ymin) -- top left
    sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)
```
### [`get_camera_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_camera_position)
#### Returns: `pair<float, float>`
Gets the current camera position in the level
### [`set_camera_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_camera_position)
#### Params: `float cx, float cy`
Sets the current camera position in the level.
Note: The camera will still try to follow the player and this doesn't actually work at all.
### [`set_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_flag)
#### Params: `int flags, int bit`
Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### [`setflag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=setflag)
### [`clr_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clr_flag)
#### Params: `int flags, int bit`
Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### [`clrflag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clrflag)
### [`test_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=test_flag)
#### Params: `int flags, int bit`
Returns true if a bit is set in the flags
### [`testflag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=testflag)
### [`rgba`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rgba)
#### Params: `int r, int g, int b, int a`
Converts a color to int to be used in drawing functions. Use values from `0..255`.
### [`draw_line`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_line)
#### Params: `float x1, float y1, float x2, float y2, float thickness, int color`
Draws a line on screen
### [`draw_rect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect)
#### Params: `float x1, float y1, float x2, float y2, float thickness, float rounding, int color`
Draws a rectangle on screen from top-left to bottom-right.
### [`draw_rect_filled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled)
#### Params: `float x1, float y1, float x2, float y2, float rounding, int color`
Draws a filled rectangle on screen from top-left to bottom-right.
### [`draw_circle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle)
#### Params: `float x, float y, float radius, float thickness, int color`
Draws a circle on screen
### [`draw_circle_filled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle_filled)
#### Params: `float x, float y, float radius, int color`
Draws a filled circle on screen
### [`draw_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text)
#### Params: `float x, float y, float size, string text, int color`
Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
### [`draw_text_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text_size)
#### Params: `float size, string text`
#### Returns: `w`, `h` in screen distance.
Calculate the bounding box of text, so you can center it etc.
Example:
```lua
function on_guiframe()
    -- get a random color
    color = math.random(0, 0xffffffff)
    -- zoom the font size based on frame
    size = (get_frame() % 199)+1
    text = 'Awesome!'
    -- calculate size of text
    w, h = draw_text_size(size, text)
    -- draw to the center of screen
    draw_text(0-w/2, 0-h/2, size, text, color)
end
```
### [`create_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_image)
#### Params: `string path`
#### Returns: `id, width, height`
Create image from file.
### [`draw_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image)
#### Params: `int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, int color`
Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
### [`get_window_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_window_size)
Gets the resolution (width and height) of the screen
### [`create_sound`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_sound)
#### Params: `string path`
Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
### [`get_sound`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_sound)
#### Params: `string path_or_vanilla_sound`
Gets an existing sound, either if a file at the same path was already loaded or if it is already loaded by the game
### [`set_vanilla_sound_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_vanilla_sound_callback)
#### Params: `VANILLA_SOUND name, VANILLA_SOUND_CALLBACK_TYPE types, function cb`
Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any
properties on the sound. Otherwise you may cause a deadlock.
### [`clear_vanilla_sound_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_vanilla_sound_callback)
#### Params: `int id`
Clears a previously set callback
### [`steal_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=steal_input)
#### Params: `int uid`
Steal input from a Player or HH.
### [`return_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=return_input)
#### Params: `int uid`
Return input
### [`send_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=send_input)
#### Params: `int uid, int buttons`
Send input
### [`read_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_input)
#### Params: `int uid`
Read input
### [`window`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=window)
#### Params: `string title, float x, float y, float w, float h, bool movable, function callback`
#### Returns: `bool` (false if the window was closed from the X)
Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the
[ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to
autosize in center. Use just a `##Label` as title to hide titlebar.
Important: Keep all your labels unique! If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to
give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text... The window api is probably evolving
still, this is just the first draft. Felt cute, might delete later!
### [`win_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_text)
#### Params: `string text`
Add some text to window, automatically wrapped
### [`win_separator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_separator)
Add a separator line to window
### [`win_inline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_inline)
Add next thing on the same line. This is same as `win_sameline(0, -1)`
### [`win_sameline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_sameline)
#### Params: `float offset, float spacing`
Add next thing on the same line, with an offset
### [`win_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_button)
#### Params: `string text`
#### Returns: `boolean`
Add a button
### [`win_input_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_text)
#### Params: `string label, string value`
#### Returns: `string`
Add a text field
### [`win_input_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_int)
#### Params: `string label, int value`
#### Returns: `int`
Add an integer field
### [`win_input_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_float)
#### Params: `string label, float value`
#### Returns: `float`
Add a float field
### [`win_slider_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_int)
#### Params: `string label, int value, int min, int max`
#### Returns: `int`
Add an integer slider
### [`win_drag_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_int)
#### Params: `string label, int value, int min, int max`
#### Returns: `int`
Add an integer dragfield
### [`win_slider_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_float)
#### Params: `string label, float value, float min, float max`
#### Returns: `float`
Add an float slider
### [`win_drag_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_float)
#### Params: `string label, float value, float min, float max`
#### Returns: `float`
Add an float dragfield
### [`win_check`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_check)
#### Params: `string label, bool value`
#### Returns: `boolean`
### [`win_combo`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_combo)
#### Params: `string label, int selected, string opts`
#### Returns: `int`
### [`win_pushid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_pushid)
#### Params: `int id`
Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
### [`win_popid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_popid)
Pop unique identifier from the stack. Put after the input.
### [`win_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_image)
#### Params: `int image, int width, int height`
Draw image to window.
## Types
Using the api through these directly is kinda dangerous, but such is life. I got pretty bored writing this doc generator at this point, so you can find the variable types in the [source files](https://github.com/spelunky-fyi/overlunky/tree/main/src/game_api). They're mostly just ints and floats. Example:
```lua
-- This doesn't make any sense, as you could just access the variables directly from players[]
-- It's just a weird example OK!
ids = get_entities_by_mask(1) -- I think this just covers CHARs
for i,id in ipairs(ids) do
    e = get_entity(id):as_player() -- cast Entity to Player to access inventory
    e.health = 99
    e.inventory.bombs = 99
    e.inventory.ropes = 99
    e.type.jump = 0.36
end
```
### `Color`
- [`r`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=r) &Color::r
- [`g`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=g) &Color::g
- [`b`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=b) &Color::b
- [`a`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=a) &Color::a
### `Inventory`
- [`money`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=money) &Inventory::money
- [`bombs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombs) &Inventory::bombs
- [`ropes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ropes) &Inventory::ropes
- [`kills_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kills_level) &Inventory::kills_level
- [`kills_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kills_total) &Inventory::kills_total
### `Animation`
- [`first_tile`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=first_tile) &Animation::texture
- [`num_tiles`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=num_tiles) &Animation::count
- [`interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=interval) &Animation::interval
- [`repeat_mode`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=repeat_mode) &Animation::repeat
### `EntityDB`
- [`id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=id) &EntityDB::id
- [`search_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=search_flags) &EntityDB::search_flags
- [`width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &EntityDB::width
- [`height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &EntityDB::height
- [`draw_depth`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_depth) &EntityDB::draw_depth
- [`friction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=friction) &EntityDB::friction
- [`elasticity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=elasticity) &EntityDB::elasticity
- [`weight`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=weight) &EntityDB::weight
- [`acceleration`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=acceleration) &EntityDB::acceleration
- [`max_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=max_speed) &EntityDB::max_speed
- [`sprint_factor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sprint_factor) &EntityDB::sprint_factor
- [`jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump) &EntityDB::jump
- [`glow_red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_red) &EntityDB::glow_red
- [`glow_green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_green) &EntityDB::glow_green
- [`glow_blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_blue) &EntityDB::glow_blue
- [`glow_alpha`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_alpha) &EntityDB::glow_alpha
- [`damage`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=damage) &EntityDB::damage
- [`life`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=life) &EntityDB::life
- [`blood_content`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blood_content) &EntityDB::blood_content
- [`texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=texture) &EntityDB::texture
- [`animations`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animations) &EntityDB::animations
- [`properties_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=properties_flags) &EntityDB::properties_flags
- [`default_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=default_flags) &EntityDB::default_flags
- [`default_more_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=default_more_flags) &EntityDB::default_more_flags
### `Entity`
- [`type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=type) &Entity::type
- [`overlay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlay) &Entity::overlay
- [`flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flags) &Entity::flags
- [`more_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=more_flags) &Entity::more_flags
- [`uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=uid) &Entity::uid
- [`animation_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_frame) &Entity::animation_frame
- [`x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x) &Entity::x
- [`y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y) &Entity::y
- [`width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &Entity::w
- [`height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &Entity::h
- [`angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=angle) &Movable::angle
- [`topmost`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topmost) &Entity::topmost
- [`topmost_mount`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topmost_mount) &Entity::topmost_mount
- [`bool overlaps_with(Entity other)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlaps_with)
- [`as_movable`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_movable) &Entity::as&lt;Movable&gt;
- [`as_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_door) &Entity::as&lt;Door&gt;
- [`as_container`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_container) &Entity::as&lt;Container&gt;
- [`as_mattock`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_mattock) &Entity::as&lt;Mattock&gt;
- [`as_mount`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_mount) &Entity::as&lt;Mount&gt;
- [`as_player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_player) &Entity::as&lt;Player&gt;
- [`as_monster`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_monster) &Entity::as&lt;Monster&gt;
- [`as_gun`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_gun) &Entity::as&lt;Gun&gt;
- [`as_bomb`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_bomb) &Entity::as&lt;Bomb&gt;
- [`as_crushtrap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_crushtrap) &Entity::as&lt;Crushtrap&gt;
- [`as_arrowtrap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_arrowtrap) &Entity::as&lt;Arrowtrap&gt;
- [`as_olmec`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_olmec) &Entity::as&lt;Olmec&gt;
- [`as_olmec_floater`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_olmec_floater) &Entity::as&lt;OlmecFloater&gt;
- [`as_cape`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_cape) &Entity::as&lt;Cape&gt;
- [`as_vlads_cape`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_vlads_cape) &Entity::as&lt;VladsCape&gt;
- [`as_chasingmonster`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_chasingmonster) &Entity::as&lt;ChasingMonster&gt;
- [`as_ghost`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_ghost) &Entity::as&lt;Ghost&gt;
- [`as_jiangshi`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_jiangshi) &Entity::as&lt;Jiangshi&gt;
### `Movable`
Derived from [`Entity`](#entity)
- [`movex`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=movex) &Movable::movex
- [`movey`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=movey) &Movable::movey
- [`buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &Movable::buttons
- [`stand_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stand_counter) &Movable::stand_counter
- [`jump_height_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_height_multiplier) &Movable::jump_height_multiplier
- [`owner_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=owner_uid) &Movable::owner_uid
- [`last_owner_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_owner_uid) &Movable::last_owner_uid
- [`idle_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=idle_counter) &Movable::idle_counter
- [`standing_on_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=standing_on_uid) &Movable::standing_on_uid
- [`velocityx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocityx) &Movable::velocityx
- [`velocityy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocityy) &Movable::velocityy
- [`holding_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=holding_uid) &Movable::holding_uid
- [`state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &Movable::state
- [`last_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_state) &Movable::last_state
- [`move_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_state) &Movable::move_state
- [`health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=health) &Movable::health
- [`stun_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stun_timer) &Movable::stun_timer
- [`stun_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stun_state) &Movable::stun_state
- [`some_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=some_state) &Movable::some_state
- [`color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=color) &Movable::color
- [`hitboxx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hitboxx) &Movable::hitboxx
- [`hitboxy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hitboxy) &Movable::hitboxy
- [`offsetx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offsetx) &Movable::offsetx
- [`offsety`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offsety) &Movable::offsety
- [`airtime`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=airtime) &Movable::airtime
- [`bool is_poisoned()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_poisoned)
- [`void poison(int16_t frames)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison)
- [`dark_shadow_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dark_shadow_timer) &Movable::dark_shadow_timer
- [`exit_invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=exit_invincibility_timer) &Movable::exit_invincibility_timer
- [`invincibility_frames_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_frames_timer) &Movable::invincibility_frames_timer
- [`frozen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frozen_timer) &Movable::frozen_timer
- [`bool is_button_pressed(uint32_t button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_pressed)
- [`bool is_button_held(uint32_t button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_held)
- [`bool is_button_released(uint32_t button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_released)
- [`price`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=price) &Movable::price
### `Monster`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`sol::base_classes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sol::base_classes) sol::bases&lt;Entity
### `Player`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`inventory`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inventory) &Player::inventory_ptr
- [`set_jetpack_fuel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_jetpack_fuel) &Player::set_jetpack_fuel
- [`kapala_blood_amount`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kapala_blood_amount) &Player::kapala_blood_amount
### `Mount`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`carry`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carry) &Mount::carry
- [`tame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tame) &Mount::tame
### `Bomb`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`scale_hor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_hor) &Bomb::scale_hor
- [`scale_ver`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_ver) &Bomb::scale_ver
### `Container`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`inside`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inside) &Container::inside
- [`timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Container::timer
### `Gun`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cooldown) &Gun::cooldown
- [`shots`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shots) &Gun::shots
- [`shots2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shots2) &Gun::shots2
- [`in_chamber`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_chamber) &Gun::in_chamber
### `Crushtrap`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`dirx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dirx) &Crushtrap::dirx
- [`diry`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=diry) &Crushtrap::diry
### `Arrowtrap`
Derived from [`Entity`](#entity)
- [`arrow_shot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_shot) &Arrowtrap::arrow_shot
- [`rearm`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rearm) &Arrowtrap::rearm
### `Olmec`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`target_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_uid) &Olmec::target_uid
- [`attack_phase`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_phase) &Olmec::attack_phase
- [`attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &Olmec::attack_timer
- [`ai_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_timer) &Olmec::ai_timer
- [`move_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_direction) &Olmec::move_direction
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Olmec::jump_timer
- [`phase1_amount_of_bomb_salvos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=phase1_amount_of_bomb_salvos) &Olmec::phase1_amount_of_bomb_salvos
- [`unknown_attack_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown_attack_state) &Olmec::unknown_attack_state
- [`int broken_floaters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=broken_floaters)
### `OlmecFloater`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`both_floaters_intact`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=both_floaters_intact) &OlmecFloater::both_floaters_intact
### `Cape`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`floating_down`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=floating_down) &VladsCape::floating_down
### `VladsCape`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Cape`](#cape)
- [`can_double_jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_double_jump) &VladsCape::can_double_jump
### `ChasingMonster`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`chased_target_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chased_target_uid) &Ghost::chased_target_uid
- [`target_selection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_selection_timer) &Ghost::target_selection_timer
### `Ghost`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`ChasingMonster`](#chasingmonster)
- [`split_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=split_timer) &Ghost::split_timer
- [`velocity_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_multiplier) &Ghost::velocity_multiplier
### `Jiangshi`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`ChasingMonster`](#chasingmonster)
- [`wait_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wait_timer) &Jiangshi::wait_timer
### `StateMemory`
- [`screen_last`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_last) &StateMemory::screen_last
- [`screen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen) &StateMemory::screen
- [`screen_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_next) &StateMemory::screen_next
- [`ingame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ingame) &StateMemory::ingame
- [`playing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=playing) &StateMemory::playing
- [`pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause) &StateMemory::pause
- [`width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &StateMemory::w
- [`height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &StateMemory::h
- [`kali_favor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kali_favor) &StateMemory::kali_favor
- [`kali_status`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kali_status) &StateMemory::kali_status
- [`kali_altars_destroyed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kali_altars_destroyed) &StateMemory::kali_altars_destroyed
- [`seed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed) &StateMemory::seed
- [`time_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_total) &StateMemory::time_total
- [`world`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world) &StateMemory::world
- [`world_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world_next) &StateMemory::world_next
- [`world_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world_start) &StateMemory::world_start
- [`level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level) &StateMemory::level
- [`level_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_next) &StateMemory::level_next
- [`level_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_start) &StateMemory::level_start
- [`theme`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme) &StateMemory::theme
- [`theme_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme_next) &StateMemory::theme_next
- [`theme_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme_start) &StateMemory::theme_start
- [`shoppie_aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shoppie_aggro) &StateMemory::shoppie_aggro
- [`shoppie_aggro_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shoppie_aggro_next) &StateMemory::shoppie_aggro_levels
- [`merchant_aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=merchant_aggro) &StateMemory::merchant_aggro
- [`kills_npc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kills_npc) &StateMemory::kills_npc
- [`level_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_count) &StateMemory::level_count
- [`journal_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=journal_flags) &StateMemory::journal_flags
- [`time_last_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_last_level) &StateMemory::time_last_level
- [`time_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_level) &StateMemory::time_level
- [`level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_flags) &StateMemory::hud_flags
- [`loading`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=loading) &StateMemory::loading
- [`quest_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quest_flags) &StateMemory::quest_flags
- [`fadevalue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadevalue) &StateMemory::fadevalue
- [`fadeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadeout) &StateMemory::fadeout
- [`fadein`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadein) &StateMemory::fadein
- [`loading_black_screen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=loading_black_screen_timer) &StateMemory::loading_black_screen_timer
- [`saved_dogs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saved_dogs) &StateMemory::saved_dogs
- [`saved_cats`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saved_cats) &StateMemory::saved_cats
- [`saved_hamsters`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saved_hamsters) &StateMemory::saved_hamsters
- [`win_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_state) &StateMemory::win_state
- [`illumination`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=illumination) &StateMemory::illumination
- [`money_last_levels`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=money_last_levels) &StateMemory::money_last_levels
- [`money_shop_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=money_shop_total) &StateMemory::money_shop_total
### `SaturationVignette`
- [`red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red) &SaturationVignette::red
- [`green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=green) &SaturationVignette::green
- [`blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blue) &SaturationVignette::blue
- [`vignette_aperture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vignette_aperture) &SaturationVignette::vignette_aperture
### `Illumination`
- [`saturation_vignette`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saturation_vignette) &Illumination::saturation_vignette
- [`brightness1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brightness1) &Illumination::brightness1
- [`brightness2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brightness2) &Illumination::brightness2
- [`frontlayer_global_illumination`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frontlayer_global_illumination) &Illumination::frontlayer_global_illumination
- [`backlayer_global_illumination`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=backlayer_global_illumination) &Illumination::backlayer_global_illumination
### `ParticleDB`
- [`id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=id) &ParticleDB::id
- [`sheet_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sheet_id) &ParticleDB::sheet_id
- [`shrink_growth_factor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shrink_growth_factor) &ParticleDB::shrink_growth_factor
- [`opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=opacity) &ParticleDB::opacity
- [`hor_scattering`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_scattering) &ParticleDB::hor_scattering
- [`ver_scattering`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_scattering) &ParticleDB::ver_scattering
- [`scale_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_x) &ParticleDB::scale_x
- [`scale_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_y) &ParticleDB::scale_y
- [`hor_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_velocity) &ParticleDB::hor_velocity
- [`ver_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_velocity) &ParticleDB::ver_velocity
### `CustomSound`
- [`PlayingSound play(bool start_paused, SOUND_TYPE sound_type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play)
- [`string>> get_parameters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameters)
### `PlayingSound`
- [`bool is_playing()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_playing)
- [`bool stop()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stop)
- [`bool set_pause(bool pause)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pause)
- [`bool set_mute(bool mute)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_mute)
- [`bool set_pitch(float pitch)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pitch)
- [`bool set_pan(float pan)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pan)
- [`bool set_volume(float volume)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_volume)
- [`bool set_looping(SOUND_LOOP_MODE looping)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_looping)
- [`bool set_callback(function callback)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback)
- [`string>> get_parameters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameters)
- [`optional<float> get_parameter(VANILLA_SOUND_PARAM param)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameter)
- [`bool set_parameter(VANILLA_SOUND_PARAM param, float value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_parameter)
### `SaveData`
- [`places`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=places) bool[]
- [`bestiary`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bestiary) bool[]
- [`people`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=people) bool[]
- [`items`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=items) bool[]
- [`traps`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=traps) bool[]
- [`last_daily`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_daily) sol::readonly(&SaveData::last_daily)
- [`characters`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=characters) sol::readonly(&SaveData::characters)
- [`shortcuts`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shortcuts) sol::readonly(&SaveData::shortcuts)
- [`bestiary_killed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bestiary_killed) int[]
- [`bestiary_killed_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bestiary_killed_by) int[]
- [`people_killed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=people_killed) int[]
- [`people_killed_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=people_killed_by) int[]
- [`plays`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=plays) sol::readonly(&SaveData::plays)
- [`deaths`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deaths) sol::readonly(&SaveData::deaths)
- [`wins_normal`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins_normal) sol::readonly(&SaveData::wins_normal)
- [`wins_hard`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins_hard) sol::readonly(&SaveData::wins_hard)
- [`wins_special`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins_special) sol::readonly(&SaveData::wins_special)
- [`score_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=score_total) sol::readonly(&SaveData::score_total)
- [`score_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=score_top) sol::readonly(&SaveData::score_top)
- [`deepest_area`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deepest_area) sol::readonly(&SaveData::deepest_area)
- [`deepest_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deepest_level) sol::readonly(&SaveData::deepest_level)
### `SaveContext`
- [`bool save(string data)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=save)
### `LoadContext`
- [`string load()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load)
## Enums
Enums are like numbers but in text that's easier to remember. Example:
```lua
set_callback(function()
    if state.theme == THEME.COSMIC_OCEAN then
        x, y, l = get_position(players[1].uid)
        spawn(ENT_TYPE.ITEM_JETPACK, x, y, l, 0, 0)
    end
end, ON.LEVEL)
```
### ENT_TYPE
- [`FLOOR_BORDERTILE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.FLOOR_BORDERTILE) 1
- ...blah blah read your entities.txt...
- [`LIQUID_STAGNANT_LAVA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.LIQUID_STAGNANT_LAVA) 898
### VANILLA_SOUND
- [`BGM_BGM_TITLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.BGM_BGM_TITLE) BGM/BGM_title
- ...check vanilla_sounds.txt output by Overlunky...
- [`FX_FX_DM_BANNER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.FX_FX_DM_BANNER) FX/FX_dm_banner
### VANILLA_SOUND_PARAM
- [`POS_SCREEN_X`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.POS_SCREEN_X) 0
- ...check vanilla_sound_params.txt output by Overlunky...
- [`CURRENT_LAYER2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.CURRENT_LAYER2) 37
### PARTICLEEMITTER
- [`TITLE_TORCHFLAME_SMOKE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.TITLE_TORCHFLAME_SMOKE) 1
- ...check particle_emitters.txt output by Overlunky...
- [`MINIGAME_BROKENASTEROID_SMOKE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.MINIGAME_BROKENASTEROID_SMOKE) 219
### THEME
- [`DWELLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.DWELLING) 1
- [`JUNGLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.JUNGLE) 2
- [`VOLCANA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.VOLCANA) 3
- [`OLMEC`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.OLMEC) 4
- [`TIDE_POOL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TIDE_POOL) 5
- [`TEMPLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TEMPLE) 6
- [`ICE_CAVES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ICE_CAVES) 7
- [`NEO_BABYLON`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.NEO_BABYLON) 8
- [`SUNKEN_CITY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.SUNKEN_CITY) 9
- [`COSMIC_OCEAN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.COSMIC_OCEAN) 10
- [`CITY_OF_GOLD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.CITY_OF_GOLD) 11
- [`DUAT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.DUAT) 12
- [`ABZU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ABZU) 13
- [`TIAMAT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TIAMAT) 14
- [`EGGPLANT_WORLD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.EGGPLANT_WORLD) 15
- [`HUNDUN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.HUNDUN) 16
- [`BASE_CAMP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.BASE_CAMP) 17
- [`ARENA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ARENA) 18
### ON
- [`LOGO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOGO) 0
- [`INTRO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.INTRO) 1
- [`PROLOGUE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PROLOGUE) 2
- [`TITLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TITLE) 3
- [`MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.MENU) 4
- [`OPTIONS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.OPTIONS) 5
- [`LEADERBOARD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEADERBOARD) 7
- [`SEED_INPUT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SEED_INPUT) 8
- [`CHARACTER_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CHARACTER_SELECT) 9
- [`TEAM_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TEAM_SELECT) 10
- [`CAMP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CAMP) 11
- [`LEVEL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEVEL) 12
- [`TRANSITION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TRANSITION) 13
- [`DEATH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH) 14
- [`SPACESHIP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPACESHIP) 15
- [`WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.WIN) 16
- [`CREDITS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CREDITS) 17
- [`SCORES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCORES) 18
- [`CONSTELLATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CONSTELLATION) 19
- [`RECAP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RECAP) 20
- [`ARENA_MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MENU) 21
- [`ARENA_INTRO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_INTRO) 25
- [`ARENA_MATCH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MATCH) 26
- [`ARENA_SCORE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_SCORE) 27
- [`ONLINE_LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOADING) 28
- [`ONLINE_LOBBY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOBBY) 29
- [`GUIFRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GUIFRAME) 100
- [`FRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.FRAME) 101
- [`SCREEN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCREEN) 102
- [`START`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.START) 103
- [`LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOADING) 104
- [`RESET`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RESET) 105
- [`SAVE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SAVE) 106
\
Params: `SaveContext save_ctx`\
Runs at the same times as ON.SCREEN, but receives the save_ctx
- [`LOAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOAD) 107
\
Params: `LoadContext load_ctx`\
Runs as soon as your script is loaded, including reloads, then never again
- [`GAMEFRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GAMEFRAME) 108
### LAYER
- [`FRONT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.FRONT) 0
- [`BACK`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.BACK) 1
- [`PLAYER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER) -1
- [`PLAYER1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER1) -1
- [`PLAYER2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER2) -2
- [`PLAYER3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER3) -3
- [`PLAYER4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER4) -4
### BUTTON
- [`JUMP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.JUMP) 1
- [`WHIP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.WHIP) 2
- [`BOMB`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.BOMB) 4
- [`ROPE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.ROPE) 8
- [`RUN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.RUN) 16
- [`DOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.DOOR) 32
### MASK
- [`PLAYER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.PLAYER) 0x1
- [`MOUNT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.MOUNT) 0x2
- [`MONSTER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.MONSTER) 0x4
- [`ITEM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ITEM) 0x8
- [`EXPLOSION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.EXPLOSION) 0x10
- [`ROPE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ROPE) 0x20
- [`FX`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.FX) 0x40
- [`ACTIVEFLOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ACTIVEFLOOR) 0x80
- [`FLOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.FLOOR) 0x100
- [`DECORATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.DECORATION) 0x200
- [`BG`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.BG) 0x400
- [`SHADOW`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.SHADOW) 0x800
- [`LOGICAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.LOGICAL) 0x1000
- [`WATER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.WATER) 0x2000
- [`LAVA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.LAVA) 0x4000
### SOUND_TYPE
- [`SFX`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE.SFX) 0
- [`MUSIC`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE.MUSIC) 1
### SOUND_LOOP_MODE
- [`OFF`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.OFF) 0
- [`LOOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.LOOP) 1
- [`BIDIRECTIONAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.BIDIRECTIONAL) 2
### VANILLA_SOUND_CALLBACK_TYPE
- [`CREATED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.CREATED) FMODStudio::EventCallbackType::Created
\
Params: `PlayingSound vanilla_sound`
- [`DESTROYED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.DESTROYED) FMODStudio::EventCallbackType::Destroyed
\
Params: `PlayingSound vanilla_sound`
- [`STARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.STARTED) FMODStudio::EventCallbackType::Started
\
Params: `PlayingSound vanilla_sound`
- [`RESTARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.RESTARTED) FMODStudio::EventCallbackType::Restarted
\
Params: `PlayingSound vanilla_sound`
- [`STOPPED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.STOPPED) FMODStudio::EventCallbackType::Stopped
\
Params: `PlayingSound vanilla_sound`
- [`START_FAILED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.START_FAILED) FMODStudio::EventCallbackType::StartFailed
\
Params: `PlayingSound vanilla_sound`
### CONST
- [`ENGINE_FPS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ENGINE_FPS) 60
### WIN_STATE
- [`NO_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.NO_WIN) 0
- [`TIAMAT_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.TIAMAT_WIN) 1
- [`HUNDUN_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.HUNDUN_WIN) 2
- [`COSMIC_OCEAN_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.COSMIC_OCEAN_WIN) 3
