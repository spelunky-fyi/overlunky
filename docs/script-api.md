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
### `debug`
### `json`
To save data in your mod it makes a lot of sense to use `json` to encode a table into a string and decode strings to table. For example this code that saves table and loads it back:
```Lua
local some_mod_data_that_should_be_saved = {{
    kills = 0,
    unlocked = false
}}
set_callback(function(save_ctx)
    local save_data_str = json.encode(some_mod_data_that_should_be_saved)
    save_ctx:save(save_data_str)
end, ON.SAVE)

set_callback(function(load_ctx)
    local load_data_str = load_ctx:load()
    if load_data_str ~= "" then
        some_mod_data_that_should_be_saved = json.decode(load_data_str)
    end
end, ON.LOAD)
```
### `inspect`
This module is a great substitute for `tostring` because it can convert any type to a string and thus helps a lot with debugging. Use for example like this:
```Lua
local look_ma_no_tostring = {
    number = 15,
    nested_table = {
        array = {
            1,
            2,
            4
        }
    }
}
message(inspect(look_ma_no_tostring))
--[[prints:
{
    number = 15,
    nested_table = {
        array = { 1, 2, 4 }
    }
}
]]
```
### `format`
This allows you to make strings without having to do a lot of `tostring` and `..` by placing your variables directly inside of the string. Use `F` in front of your string and wrap variables you want to print in `{}`, for example like this:
```Lua
for _, player in players do
    local royal_title = nil
    if player:is_female() then
        royal_title = 'Queen'
    else
        royal_title = 'King'
    end
    local name = F'{player:get_name()} aka {royal_title} {player:get_short_name()}'
    message(name)
end
```
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
### [`print`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=print)
`nil print(string message)`<br/>
Print a log message on screen.
### [`message`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=message)
`nil message(string message)`<br/>
Same as `print`
### [`prinspect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=prinspect)
`nil prinspect(variadic_args objects)`<br/>
Prints any type of object by first funneling it through `inspect`, no need for a manual `tostring` or `inspect`.
For example use it like this
```lua
prinspect(state.level, state.level_next)
local some_stuff_in_a_table = {
    some = state.time_total,
    stuff = state.world
}
prinspect(some_stuff_in_a_table)
```
### [`messpect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=messpect)
`nil messpect(variadic_args objects)`<br/>
Same as `prinspect`
### [`set_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_interval)
`CallbackId set_interval(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called every `frames` engine frames. Timer is paused on pause and cleared on level transition.
### [`set_timeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_timeout)
`CallbackId set_timeout(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
### [`set_global_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_interval)
`CallbackId set_global_interval(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.
### [`set_global_timeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_timeout)
`CallbackId set_global_timeout(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.
### [`set_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback)
`CallbackId set_callback(function cb, int screen)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called on an [event](#on).
### [`clear_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_callback)
`nil clear_callback(CallbackId id)`<br/>
Clear previously added callback `id`
### [`load_script`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load_script)
`nil load_script(string id)`<br/>
Load another script by id "author/name"
### [`seed_prng`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed_prng)
`nil seed_prng(int seed)`<br/>
Seed the game prng.
### [`read_prng`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_prng)
`array<int> read_prng()`<br/>
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
`nil toast(string message)`<br/>
Show a message that looks like a level feeling.
### [`say`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=say)
`nil say(int entity_uid, string message, int unk_type, bool top)`<br/>
Show a message coming from an entity
### [`register_option_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_int)
`nil register_option_int(string name, string desc, string long_desc, int value, int min, int max)`<br/>
Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
limits, you can override them in the UI with double click.
### [`register_option_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_float)
`nil register_option_float(string name, string desc, string long_desc, float value, float min, float max)`<br/>
Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
limits, you can override them in the UI with double click.
### [`register_option_bool`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_bool)
`nil register_option_bool(string name, string desc, string long_desc, bool value)`<br/>
Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
### [`register_option_string`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_string)
`nil register_option_string(string name, string desc, string long_desc, string value)`<br/>
Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
### [`register_option_combo`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_combo)
`nil register_option_combo(string name, string desc, string long_desc, string opts)`<br/>
Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
with a double `\0\0` at the end.
### [`register_option_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_button)
`nil register_option_button()`<br/>
### [`spawn_liquid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_liquid)
`nil spawn_liquid(int entity_type, float x, float y)`<br/>
Spawn a "block" of liquids, always spawns in the front layer and will have fun effects if `entity_type` is not a liquid.
Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
### [`spawn_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity)
`int spawn_entity(int entity_type, float x, float y, int layer, float vx, float vy)`<br/>
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
`int spawn(int entity_type, float x, float y, int layer, float vx, float vy)`<br/>
Short for [spawn_entity](#spawn_entity).
### [`spawn_entity_nonreplaceable`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_nonreplaceable)
`int spawn_entity_nonreplaceable(int entity_type, float x, float y, int layer, float vx, float vy)`<br/>
Same as `spawn_entity` but does not trigger any pre-entity-spawn callbacks, so it will not be replaced by another script
### [`spawn_critical`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_critical)
`int spawn_critical(int entity_type, float x, float y, int layer, float vx, float vy)`<br/>
Short for [spawn_entity_nonreplaceable](#spawn_entity_nonreplaceable).
### [`spawn_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_door)
`int spawn_door(float x, float y, int layer, int w, int l, int t)`<br/>
Spawn a door to another world, level and theme and return the uid of spawned entity.
Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
### [`door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door)
`int door(float x, float y, int layer, int w, int l, int t)`<br/>
Short for [spawn_door](#spawn_door).
### [`spawn_layer_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_layer_door)
`nil spawn_layer_door(float x, float y)`<br/>
Spawn a door to backlayer.
### [`layer_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=layer_door)
`nil layer_door(float x, float y)`<br/>
Short for [spawn_layer_door](#spawn_layer_door).
### [`set_pre_entity_spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_entity_spawn)
`CallbackId set_pre_entity_spawn(function cb, SPAWN_TYPE flags, int mask, variadic_args entity_types)`<br/>
Add a callback for a spawn of specific entity types or mask. Set `mask` to `0` to ignore that.
This is run before the entity is spawned, spawn your own entity and return its uid to replace the intended spawn.
In many cases replacing the intended entity won't have the indended effect or will even break the game, so use only if you really know what you're doing.
The callback signature is `optional<int> pre_entity_spawn(entity_type, x, y, layer, overlay_entity)`
### [`set_post_entity_spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_entity_spawn)
`CallbackId set_post_entity_spawn(function cb, SPAWN_TYPE flags, int mask, variadic_args entity_types)`<br/>
Add a callback for a spawn of specific entity types or mask. Set `mask` to `0` to ignore that.
This is run right after the entity is spawned but before and particular properties are changed, e.g. owner or velocity.
The callback signature is `nil post_entity_spawn(entity)`
### [`warp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=warp)
`nil warp(int w, int l, int t)`<br/>
Warp to a level immediately.
### [`set_seed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_seed)
`nil set_seed(int seed)`<br/>
Set seed and reset run.
### [`god`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=god)
`nil god(bool g)`<br/>
Enable/disable godmode.
### [`force_dark_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_dark_level)
`nil force_dark_level(bool g)`<br/>
Try to force next levels to be dark.
### [`zoom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=zoom)
`nil zoom(float level)`<br/>
Set the zoom level used in levels and shops. 13.5 is the default.
### [`pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause)
`nil pause(bool p)`<br/>
Enable/disable game engine pause.
### [`move_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_entity)
`nil move_entity(int uid, float x, float y, float vx, float vy)`<br/>
Teleport entity to coordinates with optional velocity
### [`set_door_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_door_target)
`nil set_door_target(int id, int w, int l, int t)`<br/>
Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
### [`set_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_door)
`nil set_door(int id, int w, int l, int t)`<br/>
Short for [set_door_target](#set_door_target).
### [`get_door_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_door_target)
`tuple<int, int, int> get_door_target(int uid)`<br/>
Get door target `world`, `level`, `theme`
### [`set_contents`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_contents)
`nil set_contents(int uid, int item_entity_type)`<br/>
Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `uid` to ENT_TYPE... `item_entity_type`
### [`get_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity)
`Entity get_entity(int uid)`<br/>
Get the [Entity](#entity) behind an uid
### [`get_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_type)
`EntityDB get_type(int id)`<br/>
Get the [EntityDB](#entitydb) behind an ENT_TYPE...
### [`get_entities`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities)
`array<int> get_entities()`<br/>
Get uids of all entities currently loaded
### [`get_entities_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by)
`array<int> get_entities_by(int entity_type, int mask, int layer)`<br/>
Get uids of entities by some conditions. Set `entity_type` or `mask` to `0` to ignore that.
### [`get_entities_by_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_type)
`array<int> get_entities_by_type(int, int...)`<br/>
Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
You can even pass a table! Example:
```lua
types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
function on_level()
    uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    -- is not the same thing as this, but also works
    uids2 = get_entities_by_type(entity_types)
    message(tostring(#uids).." == "..tostring(#uids2))
end
```
### [`get_entities_by_mask`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_mask)
`array<int> get_entities_by_mask(int mask)`<br/>
Get uids of entities by some search_flags
### [`get_entities_by_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_layer)
`array<int> get_entities_by_layer(int layer)`<br/>
Get uids of entities by layer. `0` for main level, `1` for backlayer, `-1` for layer of the player.
### [`get_entities_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_at)
`array<int> get_entities_at(int entity_type, int mask, float x, float y, int layer, float radius)`<br/>
Get uids of matching entities inside some radius. Set `entity_type` or `mask` to `0` to ignore that.
### [`get_entities_overlapping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping)
`array<int> get_entities_overlapping(int entity_type, int mask, float sx, float sy, float sx2, float sy2, int layer)`<br/>
Get uids of matching entities overlapping with the given rect. Set `entity_type` or `mask` to `0` to ignore that.
### [`get_entity_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags)
`int get_entity_flags(int uid)`<br/>
Get the `flags` field from entity by uid
### [`set_entity_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags)
`nil set_entity_flags(int uid, int flags)`<br/>
Set the `flags` field from entity by uid
### [`get_entity_flags2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags2)
`int get_entity_flags2(int id)`<br/>
Get the `more_flags` field from entity by uid
### [`set_entity_flags2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags2)
`nil set_entity_flags2(int uid, int flags)`<br/>
Set the `more_flags` field from entity by uid
### [`get_entity_ai_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_ai_state)
`int get_entity_ai_state(int uid)`<br/>
Get the `move_state` field from entity by uid
### [`get_level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_level_flags)
`int get_level_flags()`<br/>
Get `state.level_flags`
### [`set_level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_level_flags)
`nil set_level_flags(int flags)`<br/>
Set `state.level_flags`
### [`get_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_type)
`int get_entity_type(int uid)`<br/>
Get the ENT_TYPE... for entity by uid
### [`get_zoom_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_zoom_level)
`float get_zoom_level()`<br/>
Get the current set zoom level
### [`game_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_position)
`tuple<float, float> game_position(float x, float y)`<br/>
Get the game coordinates at the screen position (`x`, `y`)
### [`screen_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_position)
`tuple<float, float> screen_position(float x, float y)`<br/>
Translate an entity position to screen position to be used in drawing functions
### [`screen_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_distance)
`float screen_distance(float x)`<br/>
Translate a distance of `x` tiles to screen distance to be be used in drawing functions
### [`get_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_position)
`tuple<float, float, int> get_position(int id)`<br/>
Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
you're standing on, not real level coordinates.
### [`get_render_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_render_position)
`tuple<float, float, int> get_render_position(int id)`<br/>
Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...
### [`entity_remove_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_remove_item)
`nil entity_remove_item(int id, int item_uid)`<br/>
Remove item by uid from entity
### [`spawn_entity_over`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_over)
`int spawn_entity_over(int item_uid, int over_uid, float x, float y)`<br/>
Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`
### [`entity_has_item_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_uid)
`bool entity_has_item_uid(int uid, int item_uid)`<br/>
Check if the entity `uid` has some specific `item_uid` by uid in their inventory
### [`entity_has_item_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_type)
`bool entity_has_item_type(int uid, int entity_type)`<br/>
Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory
### [`entity_get_items_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_get_items_by)
`array<int> entity_get_items_by(int uid, int entity_type, int mask)`<br/>
Gets all items of `entity_type` and `mask` from an entity's inventory. Set `entity_type` and `mask` to 0 to return all inventory items.
### [`kill_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kill_entity)
`nil kill_entity(int uid)`<br/>
Kills an entity by uid.
### [`pick_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pick_up)
`nil pick_up(int who_uid, int what_uid)`<br/>
Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen. Example:
```lua
-- spawn and equip a jetpack
pick_up(players[1].uid, spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER, 0, 0))
```
### [`apply_entity_db`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=apply_entity_db)
`nil apply_entity_db(int uid)`<br/>
Apply changes made in [get_type](#get_type)() to entity instance by uid.
### [`lock_door_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lock_door_at)
`nil lock_door_at(float x, float y)`<br/>
Try to lock the exit at coordinates
### [`unlock_door_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unlock_door_at)
`nil unlock_door_at(float x, float y)`<br/>
Try to unlock the exit at coordinates
### [`get_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_frame)
`int get_frame()`<br/>
Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
### [`get_ms`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ms)
`nil get_ms()`<br/>
Get the current timestamp in milliseconds since the Unix Epoch.
### [`carry`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carry)
`nil carry(int mount_uid, int rider_uid)`<br/>
Make `mount_uid` carry `rider_uid` on their back. Only use this with actual mounts and living things.
### [`set_arrowtrap_projectile`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_arrowtrap_projectile)
`nil set_arrowtrap_projectile(int regular_entity_type, int poison_entity_type)`<br/>
Sets the arrow type (wooden, metal, light) that is shot from a regular arrow trap and a poison arrow trap.
### [`set_kapala_blood_threshold`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_kapala_blood_threshold)
`nil set_kapala_blood_threshold(int threshold)`<br/>
Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).
### [`set_kapala_hud_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_kapala_hud_icon)
`nil set_kapala_hud_icon(int icon_index)`<br/>
Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour).
If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!
### [`modify_sparktraps`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=modify_sparktraps)
`nil modify_sparktraps(float angle_increment = 0.015, float distance = 3.0)`<br/>
Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!
### [`set_blood_multiplication`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_blood_multiplication)
`nil set_blood_multiplication(int default_multiplier, int vladscape_multiplier)`<br/>
Sets the multiplication factor for blood droplets (default/no Vlad's cape = 1, with Vlad's cape = 2)
### [`flip_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flip_entity)
`nil flip_entity(int uid)`<br/>
Flip entity around by uid. All new entities face right by default.
### [`set_olmec_phase_y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_olmec_phase_y_level)
`nil set_olmec_phase_y_level(int phase, float y)`<br/>
Sets the Y-level at which Olmec changes phases
### [`set_ghost_spawn_times`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_ghost_spawn_times)
`nil set_ghost_spawn_times(int normal = 10800, int cursed = 9000)`<br/>
Determines when the ghost appears, either when the player is cursed or not
### [`force_co_subtheme`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_co_subtheme)
`nil force_co_subtheme(int subtheme)`<br/>
Forces the theme of the next cosmic ocean level(s) (use e.g. force_co_subtheme(COSUBTHEME.JUNGLE)  Use COSUBTHEME.RESET to reset to default random behaviour)
### [`set_journal_enabled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_journal_enabled)
`nil set_journal_enabled(bool b)`<br/>
Enables or disables the journal
### [`waddler_count_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_count_entity)
`int waddler_count_entity(int entity_type)`<br/>
Returns how many of a specific entity type Waddler has stored
### [`waddler_store_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_store_entity)
`int waddler_store_entity(int entity_type)`<br/>
Store an entity type in Waddler's storage. Returns the slot number the item was stored in or -1 when storage is full and the item couldn't be stored.
### [`waddler_remove_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_remove_entity)
`nil waddler_remove_entity(int entity_type, int amount_to_remove = 99)`<br/>
Removes an entity type from Waddler's storage. Second param determines how many of the item to remove (default = remove all)
### [`waddler_get_entity_meta`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_get_entity_meta)
`int waddler_get_entity_meta(int slot)`<br/>
Gets the 16-bit meta-value associated with the entity type in the associated slot
### [`waddler_set_entity_meta`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_set_entity_meta)
`nil waddler_set_entity_meta(int slot, int meta)`<br/>
Sets the 16-bit meta-value associated with the entity type in the associated slot
### [`waddler_entity_type_in_slot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_entity_type_in_slot)
`int waddler_entity_type_in_slot(int slot)`<br/>
Gets the entity type of the item in the provided slot
### [`distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance)
`float distance(int uid_a, int uid_b)`<br/>
Calculate the tile distance of two entities by uid
### [`get_bounds`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_bounds)
`tuple<float, float, float, float> get_bounds()`<br/>
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
`tuple<float, float> get_camera_position()`<br/>
Gets the current camera position in the level
### [`set_camera_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_camera_position)
`nil set_camera_position(float cx, float cy)`<br/>
Sets the current camera position in the level.
Note: The camera will still try to follow the player and this doesn't actually work at all.
### [`set_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_flag)
`Flags set_flag(Flags flags, int bit)`<br/>
Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### [`setflag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=setflag)
`nil setflag()`<br/>
### [`clr_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clr_flag)
`Flags clr_flag(Flags flags, int bit)`<br/>
Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### [`clrflag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clrflag)
`nil clrflag()`<br/>
### [`test_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=test_flag)
`bool test_flag(Flags flags, int bit)`<br/>
Returns true if a bit is set in the flags
### [`testflag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=testflag)
`nil testflag()`<br/>
### [`get_window_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_window_size)
`tuple<int, int> get_window_size()`<br/>
Gets the resolution (width and height) of the screen
### [`steal_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=steal_input)
`nil steal_input(int uid)`<br/>
Steal input from a Player or HH.
### [`return_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=return_input)
`nil return_input(int uid)`<br/>
Return input
### [`send_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=send_input)
`nil send_input(int uid, BUTTONS buttons)`<br/>
Send input
### [`read_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_input)
`BUTTONS read_input(int uid)`<br/>
Read input
### [`read_stolen_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_stolen_input)
`nil read_stolen_input(int uid)`<br/>
Read input that has been previously stolen with steal_input
### [`toast_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast_visible)
`bool toast_visible()`<br/>
### [`speechbubble_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_visible)
`bool speechbubble_visible()`<br/>
### [`clear_entity_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_entity_callback)
`nil clear_entity_callback(int uid, CallbackId cb_id)`<br/>
Clears a callback that is specific to an entity.
### [`set_pre_statemachine`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_statemachine)
`optional<CallbackId> set_pre_statemachine(int uid, function fun)`<br/>
Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
`uid` has to be the uid of a `Movable` or else stuff will break.
Sets a callback that is called right before the statemachine, return `true` to skip the statemachine update.
Use this only when no other approach works, this call can be expensive if overused.
### [`set_post_statemachine`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_statemachine)
`optional<CallbackId> set_post_statemachine(int uid, function fun)`<br/>
Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
`uid` has to be the uid of a `Movable` or else stuff will break.
Sets a callback that is called right after the statemachine, so you can override any values the satemachine might have set (e.g. `animation_frame`).
Use this only when no other approach works, this call can be expensive if overused.
### [`get_particle_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_particle_type)
`nil get_particle_type()`<br/>
Get the [ParticleDB](#particledb) details of the specified ID
### [`generate_particles`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=generate_particles)
`nil generate_particles(int particle_emitter_id, int uid)`<br/>
Generate particles of the specified type around the specified entity uid (use e.g. generate_particles(PARTICLEEMITTER.PETTING_PET, player.uid))
### [`set_pre_tile_code_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_tile_code_callback)
`CallbackId set_pre_tile_code_callback(function cb, string tile_code)`<br/>
Add a callback for a specific tile code that is called before the game handles the tile code.
Return true in order to stop the game or scripts loaded after this script from handling this tile code.
For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
### [`set_post_tile_code_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_tile_code_callback)
`CallbackId set_post_tile_code_callback(function cb, string tile_code)`<br/>
Add a callback for a specific tile code that is called after the game handles the tile code.
Use this to affect what the game or other scripts spawned in this position.
This is received even if a previous pre-tile-code-callback has returned true
### [`define_tile_code`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_tile_code)
`nil define_tile_code(string tile_code)`<br/>
Define a new tile code, to make this tile code do anything you have to use either `set_pre_tile_code_callback` or `set_post_tile_code_callback`.
If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.
### [`create_sound`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_sound)
`optional<CustomSound> create_sound(string path)`<br/>
Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
### [`get_sound`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_sound)
`optional<CustomSound> get_sound(string path_or_vanilla_sound)`<br/>
Gets an existing sound, either if a file at the same path was already loaded or if it is already loaded by the game
### [`set_vanilla_sound_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_vanilla_sound_callback)
`CallbackId set_vanilla_sound_callback(VANILLA_SOUND name, VANILLA_SOUND_CALLBACK_TYPE types, function cb)`<br/>
Returns unique id for the callback to be used in [clear_vanilla_sound_callback](#clear_vanilla_sound_callback).
Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any
properties on the sound. Otherwise you may cause a deadlock.
### [`clear_vanilla_sound_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_vanilla_sound_callback)
`nil clear_vanilla_sound_callback(CallbackId id)`<br/>
Clears a previously set callback
### [`rgba`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rgba)
`uColor rgba(int r, int g, int b, int a)`<br/>
Converts a color to int to be used in drawing functions. Use values from `0..255`.
### [`draw_line`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_line)
`nil draw_line(float x1, float y1, float x2, float y2, float thickness, int color)`<br/>
Draws a line on screen
### [`draw_rect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect)
`nil draw_rect(float x1, float y1, float x2, float y2, float thickness, float rounding, int color)`<br/>
Draws a rectangle on screen from top-left to bottom-right.
### [`draw_rect_filled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled)
`nil draw_rect_filled(float x1, float y1, float x2, float y2, float rounding, int color)`<br/>
Draws a filled rectangle on screen from top-left to bottom-right.
### [`draw_circle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle)
`nil draw_circle(float x, float y, float radius, float thickness, int color)`<br/>
Draws a circle on screen
### [`draw_circle_filled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle_filled)
`nil draw_circle_filled(float x, float y, float radius, int color)`<br/>
Draws a filled circle on screen
### [`draw_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text)
`nil draw_text(float x, float y, float size, string text, int color)`<br/>
Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
### [`draw_text_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text_size)
`tuple<float, float> draw_text_size(float size, string text)`<br/>
Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.
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
`tuple<int, int, int> create_image(string path)`<br/>
Create image from file. Returns a tuple containing id, width and height.
### [`draw_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image)
`nil draw_image(int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, int color)`<br/>
Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
### [`draw_image_rotated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image_rotated)
`nil draw_image_rotated(int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, int color, float angle, float px, float py)`<br/>
Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
### [`window`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=window)
`nil window(string title, float x, float y, float w, float h, bool movable, function callback)`<br/>
Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the
[ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to
autosize in center. Use just a `##Label` as title to hide titlebar.
Important: Keep all your labels unique! If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to
give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text... The window api is probably evolving
still, this is just the first draft. Felt cute, might delete later!
Returns false if the window was closed from the X.
### [`win_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_text)
`nil win_text(string text)`<br/>
Add some text to window, automatically wrapped
### [`win_separator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_separator)
`nil win_separator()`<br/>
Add a separator line to window
### [`win_inline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_inline)
`nil win_inline()`<br/>
Add next thing on the same line. This is same as `win_sameline(0, -1)`
### [`win_sameline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_sameline)
`nil win_sameline(float offset, float spacing)`<br/>
Add next thing on the same line, with an offset
### [`win_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_button)
`bool win_button(string text)`<br/>
Add a button
### [`win_input_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_text)
`string win_input_text(string label, string value)`<br/>
Add a text field
### [`win_input_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_int)
`int win_input_int(string label, int value)`<br/>
Add an integer field
### [`win_input_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_float)
`float win_input_float(string label, float value)`<br/>
Add a float field
### [`win_slider_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_int)
`int win_slider_int(string label, int value, int min, int max)`<br/>
Add an integer slider
### [`win_drag_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_int)
`int win_drag_int(string label, int value, int min, int max)`<br/>
Add an integer dragfield
### [`win_slider_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_float)
`float win_slider_float(string label, float value, float min, float max)`<br/>
Add an float slider
### [`win_drag_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_float)
`float win_drag_float(string label, float value, float min, float max)`<br/>
Add an float dragfield
### [`win_check`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_check)
`bool win_check(string label, bool value)`<br/>
Add a checkbox
### [`win_combo`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_combo)
`int win_combo(string label, int selected, string opts)`<br/>
Add a combo box
### [`win_pushid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_pushid)
`nil win_pushid(int id)`<br/>
Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
### [`win_popid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_popid)
`nil win_popid()`<br/>
Pop unique identifier from the stack. Put after the input.
### [`win_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_image)
`nil win_image(int image, int width, int height)`<br/>
Draw image to window.
### [`set_drop_chance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_drop_chance)
`nil set_drop_chance(int dropchance_id, int new_drop_chance)`<br/>
Alters the drop chance for the provided monster-item combination (use e.g. set_drop_chance(DROPCHANCE.MOLE_MATTOCK, 10) for a 1 in 10 chance)
### [`replace_drop`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=replace_drop)
`nil replace_drop(int drop_id, int new_drop_entity_type)`<br/>
Changes a particular drop, e.g. what Van Horsing throws at you (use e.g. replace_drop(DROP.VAN_HORSING_DIAMOND, ENT_TYPE.ITEM_PLASMACANNON))
### [`get_texture_definition`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture_definition)
`TextureDefinition get_texture_definition(int texture_id)`<br/>
Gets a `TextureDefinition` for equivalent to the one used to define the texture with `id`
### [`define_texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_texture)
`int define_texture(TextureDefinition texture_data)`<br/>
Defines a new texture that can be used in Entity::set_texture
## Types
Using the api through these directly is kinda dangerous, but such is life. I got pretty bored writing this doc generator at this point, so you can find the variable types in the [source files](https://github.com/spelunky-fyi/overlunky/tree/main/src/game_api). They're mostly just ints and floats. Example:
```lua
-- This doesn't make any sense, as you could just access the variables directly from players[]
-- It's just a weird example OK!
ids = get_entities_by_mask(MASK.PLAYER) -- This just covers CHARs
for i,id in ipairs(ids) do
    e = get_entity(id):as_player() -- cast Entity to Player to access inventory
    e.health = 99
    e.inventory.bombs = 99
    e.inventory.ropes = 99
    e.type.jump = 0.36
end
```
### `SaveContext`
- [`sol::no_constructor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sol::no_constructor) 
- [`bool save(string data)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=save) &SaveContext::Save
### `LoadContext`
- [`sol::no_constructor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sol::no_constructor) 
- [`string load()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load) &LoadContext::Load
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
- [`presence_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=presence_flags) &StateMemory::presence_flags
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
- [`player_inputs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_inputs) sol::readonly(&StateMemory::player_inputs)
- [`quests`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quests) &StateMemory::quests
- [`camera`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=camera) &StateMemory::camera
- [`special_visibility_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_visibility_flags) &StateMemory::special_visibility_flags
- [`cause_of_death`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cause_of_death) &StateMemory::cause_of_death
- [`cause_of_death_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cause_of_death_entity_type) &StateMemory::cause_of_death_entity_type
- [`toast_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast_timer) &StateMemory::toast_timer
- [`speechbubble_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_timer) &StateMemory::speechbubble_timer
- [`speechbubble_owner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_owner) &StateMemory::speechbubble_owner
### `LightParams`
- [`red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red) &LightParams::red
- [`green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=green) &LightParams::green
- [`blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blue) &LightParams::blue
- [`size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=size) &LightParams::size
### `Illumination`
- [`light1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light1) &Illumination::light1
- [`light2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light2) &Illumination::light2
- [`light3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light3) &Illumination::light3
- [`light4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light4) &Illumination::light4
- [`brightness`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brightness) &Illumination::brightness
- [`brightness_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brightness_multiplier) &Illumination::brightness_multiplier
- [`light_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light_pos_x) &Illumination::light_pos_x
- [`light_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light_pos_y) &Illumination::light_pos_y
- [`offset_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offset_x) &Illumination::offset_x
- [`offset_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offset_y) &Illumination::offset_y
- [`distortion`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distortion) &Illumination::distortion
- [`entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_uid) &Illumination::entity_uid
- [`flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flags) &Illumination::flags
### `Camera`
- [`bounds_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_left) &Camera::bounds_left
- [`bounds_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_right) &Camera::bounds_right
- [`bounds_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_bottom) &Camera::bounds_bottom
- [`bounds_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_top) &Camera::bounds_top
- [`adjusted_focus_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=adjusted_focus_x) &Camera::adjusted_focus_x
- [`adjusted_focus_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=adjusted_focus_y) &Camera::adjusted_focus_y
- [`focus_offset_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_offset_x) &Camera::focus_offset_x
- [`focus_offset_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_offset_y) &Camera::focus_offset_y
- [`focus_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_x) &Camera::focus_x
- [`focus_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_y) &Camera::focus_y
- [`vertical_pan`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_pan) &Camera::vertical_pan
- [`shake_countdown_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_countdown_start) &Camera::shake_countdown_start
- [`shake_countdown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_countdown) &Camera::shake_countdown
- [`shake_amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_amplitude) &Camera::shake_amplitude
- [`shake_multiplier_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_multiplier_x) &Camera::shake_multiplier_x
- [`shake_multiplier_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_multiplier_y) &Camera::shake_multiplier_y
- [`uniform_shake`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=uniform_shake) &Camera::uniform_shake
- [`focused_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focused_entity_uid) &Camera::focused_entity_uid
- [`inertia`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inertia) &Camera::inertia
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
- [`collected_money_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=collected_money_total) &Inventory::collected_money_total
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
- [`leaves_corpse_behind`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=leaves_corpse_behind) &EntityDB::leaves_corpse_behind
- [`sound_killed_by_player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sound_killed_by_player) &EntityDB::sound_killed_by_player
- [`sound_killed_by_other`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sound_killed_by_other) &EntityDB::sound_killed_by_other
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
- [`bool overlaps_with(Entity other)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlaps_with) overlaps_with
- [`get_texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture) &Entity::get_texture
- [`bool set_texture(int texture_id)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_texture) &Entity::set_texture
- [`as_movable`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_movable) &Entity::as&lt;Movable&gt;
- [`as_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_door) &Entity::as&lt;Door&gt;
- [`as_player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_player) &Entity::as&lt;Player&gt;
- [`as_crushtrap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_crushtrap) &Entity::as&lt;Crushtrap&gt;
- [`as_arrowtrap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_arrowtrap) &Entity::as&lt;Arrowtrap&gt;
- [`as_olmec`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_olmec) &Entity::as&lt;Olmec&gt;
- [`as_olmec_floater`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=as_olmec_floater) &Entity::as&lt;OlmecFloater&gt;
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
- [`bool is_poisoned()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_poisoned) &Movable::is_poisoned
- [`nil poison(int frames)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison) &Movable::poison
- [`dark_shadow_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dark_shadow_timer) &Movable::dark_shadow_timer
- [`exit_invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=exit_invincibility_timer) &Movable::exit_invincibility_timer
- [`invincibility_frames_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_frames_timer) &Movable::invincibility_frames_timer
- [`frozen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frozen_timer) &Movable::frozen_timer
- [`bool is_button_pressed(int button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_pressed) &Movable::is_button_pressed
- [`bool is_button_held(int button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_held) &Movable::is_button_held
- [`bool is_button_released(int button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_released) &Movable::is_button_released
- [`price`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=price) &Movable::price
### `Player`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`inventory`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inventory) &Player::inventory_ptr
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Player::emitted_light
- [`set_jetpack_fuel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_jetpack_fuel) &Player::set_jetpack_fuel
- [`kapala_blood_amount`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kapala_blood_amount) &Player::kapala_blood_amount
- [`get_name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_name) &Player::get_name
- [`get_short_name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_name) &Player::get_short_name
- [`get_heart_color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_heart_color) &Player::get_heart_color
- [`is_female`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_female) &Player::is_female
- [`set_heart_color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_heart_color) &Player::set_heart_color
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
- [`int broken_floaters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=broken_floaters) &Olmec::broken_floaters
### `OlmecFloater`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`both_floaters_intact`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=both_floaters_intact) &OlmecFloater::both_floaters_intact
### `Mount`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`carry`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carry) &Mount::carry
- [`tame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tame) &Mount::tame
- [`rider_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rider_uid) &Mount::rider_uid
- [`can_doublejump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_doublejump) &Mount::can_doublejump
- [`tamed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tamed) &Mount::tamed
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Mount::walk_pause_timer
- [`taming_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=taming_timer) &Mount::taming_timer
### `Rockdog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`attack_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown) &Rockdog::attack_cooldown
### `Axolotl`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`attack_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown) &Axolotl::attack_cooldown
- [`can_teleport`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_teleport) &Axolotl::can_teleport
### `Mech`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`gun_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=gun_cooldown) &Mech::gun_cooldown
- [`walking`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking) &Mech::walking
- [`breaking_wall`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=breaking_wall) &Mech::breaking_wall
### `Qilin`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`attack_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown) &Qilin::attack_cooldown
### `Monster`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`chased_target_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chased_target_uid) &Monster::chased_target_uid
- [`target_selection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_selection_timer) &Monster::target_selection_timer
### `Ghost`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`split_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=split_timer) &Ghost::split_timer
- [`velocity_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_multiplier) &Ghost::velocity_multiplier
- [`ghost_behaviour`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ghost_behaviour) &Ghost::ghost_behaviour
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Ghost::emitted_light
- [`linked_ghost`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=linked_ghost) &Ghost::linked_ghost
### `Jiangshi`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`wait_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wait_timer) &Jiangshi::wait_timer
### `Monkey`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Monkey::jump_timer
- [`on_vine`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_vine) &Monkey::on_vine
### `GoldMonkey`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &GoldMonkey::jump_timer
- [`poop_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poop_timer) &GoldMonkey::poop_timer
- [`poop_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poop_count) &GoldMonkey::poop_count
### `Mole`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`burrowing_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrowing_particle) &Mole::burrowing_particle
- [`burrow_dir_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrow_dir_x) &Mole::burrow_dir_x
- [`burrow_dir_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrow_dir_y) &Mole::burrow_dir_y
- [`burrowing_in_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrowing_in_uid) &Mole::burrowing_in_uid
- [`counter_burrowing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=counter_burrowing) &Mole::counter_burrowing
- [`counter_nonburrowing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=counter_nonburrowing) &Mole::counter_nonburrowing
- [`countdown_for_appearing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=countdown_for_appearing) &Mole::countdown_for_appearing
### `Spider`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`ceiling_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_x) &Spider::ceiling_pos_x
- [`ceiling_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_y) &Spider::ceiling_pos_y
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Spider::jump_timer
### `HangSpider`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`ceiling_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_x) &HangSpider::ceiling_pos_x
- [`ceiling_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_y) &HangSpider::ceiling_pos_y
- [`dangle_jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dangle_jump_timer) &HangSpider::dangle_jump_timer
### `Shopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`related_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=related_entity_uid) &Shopkeeper::related_entity_uid
- [`climb_y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_y_direction) &Shopkeeper::climb_y_direction
- [`patrol_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=patrol_timer) &Shopkeeper::patrol_timer
- [`lose_interest_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lose_interest_timer) &Shopkeeper::lose_interest_timer
- [`is_patrolling`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_patrolling) &Shopkeeper::is_patrolling
### `GhistShopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Shopkeeper`](#shopkeeper)
- [`transparency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=transparency) &GhistShopkeeper::transparency
- [`fadeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadeout) &GhistShopkeeper::fadeout
### `Yang`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Shopkeeper`](#shopkeeper)
- [`turkeys_in_pen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=turkeys_in_pen) &Yang::turkeys_in_pen
- [`quest_incomplete`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quest_incomplete) &Yang::quest_incomplete
### `Tun`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Shopkeeper`](#shopkeeper)
- [`arrows_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrows_left) &Tun::arrows_left
- [`reload_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=reload_timer) &Tun::reload_timer
- [`challenge_fee_paid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=challenge_fee_paid) &Tun::challenge_fee_paid
- [`murdered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=murdered) &Tun::murdered
- [`shop_entered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shop_entered) &Tun::shop_entered
### `Pet`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`yell_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=yell_counter) &Pet::yell_counter
- [`active_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=active_state) &Pet::active_state
### `Caveman`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`chatting_to_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chatting_to_uid) &Caveman::chatting_to_uid
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Caveman::walk_pause_timer
- [`chatting_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chatting_timer) &Caveman::chatting_timer
- [`wake_up_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wake_up_timer) &Caveman::wake_up_timer
- [`can_pick_up_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_pick_up_timer) &Caveman::can_pick_up_timer
- [`aggro_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro_timer) &Caveman::aggro_timer
### `CavemanShopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`tripping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tripping) &CavemanShopkeeper::tripping
- [`shop_entered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shop_entered) &CavemanShopkeeper::shop_entered
### `HornedLizard`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`dungbeetle_being_eaten`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dungbeetle_being_eaten) &HornedLizard::dungbeetle_being_eaten
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &HornedLizard::walk_pause_timer
- [`attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &HornedLizard::attack_cooldown_timer
- [`blood_squirt_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blood_squirt_timer) &HornedLizard::blood_squirt_timer
- [`particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &HornedLizard::particle
### `Mosquito`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`direction_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=direction_x) &Mosquito::direction_x
- [`direction_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=direction_y) &Mosquito::direction_y
- [`stuck_rel_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stuck_rel_pos_x) &Mosquito::stuck_rel_pos_x
- [`stuck_rel_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stuck_rel_pos_y) &Mosquito::stuck_rel_pos_y
- [`timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Mosquito::timer
### `Mantrap`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Mantrap::walk_pause_timer
- [`eaten_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=eaten_uid) &Mantrap::eaten_uid
### `Skeleton`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`explosion_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=explosion_timer) &Skeleton::explosion_timer
### `Scarab`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Scarab::emitted_light
### `Imp`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`carrying_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carrying_uid) &Imp::carrying_uid
- [`patrol_y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=patrol_y_level) &Imp::patrol_y_level
### `Lavamander`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Lavamander::emitted_light
- [`shoot_lava_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shoot_lava_timer) &Lavamander::shoot_lava_timer
- [`jump_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_pause_timer) &Lavamander::jump_pause_timer
- [`lava_detection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lava_detection_timer) &Lavamander::lava_detection_timer
- [`is_hot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_hot) &Lavamander::is_hot
### `Firebug`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`fire_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fire_timer) &Firebug::fire_timer
- [`going_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=going_up) &Firebug::going_up
- [`detached_from_chain`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=detached_from_chain) &Firebug::detached_from_chain
### `FirebugUnchained`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`max_flight_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=max_flight_height) &FirebugUnchained::max_flight_height
- [`walking_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking_timer) &FirebugUnchained::walking_timer
### `Robot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Robot::walk_pause_timer
- [`targeting_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeting_timer) &Robot::targeting_timer
- [`emitted_light_explosion`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light_explosion) &Robot::emitted_light_explosion
### `Quillback`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Quillback::walk_pause_timer
- [`particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Quillback::particle
### `Leprechaun`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Leprechaun::walk_pause_timer
- [`hump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hump_timer) &Leprechaun::hump_timer
- [`target_in_sight_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_in_sight_timer) &Leprechaun::target_in_sight_timer
- [`timer_after_humping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer_after_humping) &Leprechaun::timer_after_humping
### `Crocman`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Crocman::walk_pause_timer
### `Mummy`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Mummy::walk_pause_timer
### `VanHorsing`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`climb_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_direction) &VanHorsing::climb_direction
- [`target_spotted_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_spotted_timer) &VanHorsing::target_spotted_timer
- [`attack_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_state) &VanHorsing::attack_state
- [`aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &VanHorsing::aggro
### `WitchDoctor`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &WitchDoctor::walk_pause_timer
- [`attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &WitchDoctor::attack_timer
- [`skull_regen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=skull_regen_timer) &WitchDoctor::skull_regen_timer
### `WitchDoctorSkull`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`witch_doctor_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=witch_doctor_uid) &WitchDoctorSkull::witch_doctor_uid
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &WitchDoctorSkull::emitted_light
- [`rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &WitchDoctorSkull::rotation_angle
### `ForestSister`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`target_in_sight_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_in_sight_timer) &ForestSister::target_in_sight_timer
- [`aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &ForestSister::aggro
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &ForestSister::walk_pause_timer
### `Vampire`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`jump_trigger_distance_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_trigger_distance_x) &Vampire::jump_trigger_distance_x
- [`jump_trigger_distance_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_trigger_distance_y) &Vampire::jump_trigger_distance_y
- [`sleep_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sleep_pos_x) &Vampire::sleep_pos_x
- [`sleep_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sleep_pos_y) &Vampire::sleep_pos_y
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Vampire::walk_pause_timer
### `Vlad`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Vampire`](#vampire)
- [`teleport_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teleport_timer) &Vlad::teleport_timer
- [`aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &Vlad::aggro
### `Waddler`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`lose_interest_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lose_interest_timer) &Waddler::lose_interest_timer
- [`player_detected`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_detected) &Waddler::player_detected
- [`on_the_ground`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_the_ground) &Waddler::on_the_ground
- [`air_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=air_timer) &Waddler::air_timer
### `Octopus`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Octopus::walk_pause_timer
### `Bodyguard`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`climb_y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_y_direction) &Bodyguard::climb_y_direction
- [`target_detection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_detection_timer) &Bodyguard::target_detection_timer
- [`aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &Bodyguard::aggro
### `MadameTusk`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`target_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_timer) &MadameTusk::target_timer
### `Fish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &Fish::change_direction_timer
### `GiantFish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &GiantFish::change_direction_timer
- [`lose_interest_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lose_interest_timer) &GiantFish::lose_interest_timer
### `Crabman`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Crabman::walk_pause_timer
- [`invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_timer) &Crabman::invincibility_timer
- [`poison_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison_attack_timer) &Crabman::poison_attack_timer
- [`attacking_claw_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attacking_claw_uid) &Crabman::attacking_claw_uid
- [`at_maximum_attack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=at_maximum_attack) &Crabman::at_maximum_attack
### `Kingu`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`climb_direction_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_direction_x) &Kingu::climb_direction_x
- [`climb_direction_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_direction_y) &Kingu::climb_direction_y
- [`climb_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_pause_timer) &Kingu::climb_pause_timer
- [`shell_invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shell_invincibility_timer) &Kingu::shell_invincibility_timer
- [`monster_spawn_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=monster_spawn_timer) &Kingu::monster_spawn_timer
- [`initial_shell_health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=initial_shell_health) &Kingu::initial_shell_health
### `AnubisTwo`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &AnubisTwo::spawn_x
- [`spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &AnubisTwo::spawn_y
- [`attack_proximity_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_proximity_y) &AnubisTwo::attack_proximity_y
- [`attack_proximity_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_proximity_x) &AnubisTwo::attack_proximity_x
- [`next_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_attack_timer) &AnubisTwo::next_attack_timer
### `Anubis`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &Anubis::spawn_x
- [`spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &Anubis::spawn_y
- [`attack_proximity_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_proximity_y) &Anubis::attack_proximity_y
- [`attack_proximity_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_proximity_x) &Anubis::attack_proximity_x
- [`next_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_attack_timer) &Anubis::next_attack_timer
- [`psychic_orbs_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=psychic_orbs_counter) &Anubis::psychic_orbs_counter
- [`awake`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=awake) &Anubis::awake
### `Cobra`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`spit_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spit_timer) &Cobra::spit_timer
### `CatMummy`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &CatMummy::attack_timer
### `Sorceress`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Sorceress::walk_pause_timer
- [`inbetween_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inbetween_attack_timer) &Sorceress::inbetween_attack_timer
- [`airtime`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=airtime) &Sorceress::airtime
- [`halo_emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=halo_emitted_light) &Sorceress::halo_emitted_light
- [`fx_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_entity) &Sorceress::fx_entity
- [`hover_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hover_timer) &Sorceress::hover_timer
### `MagmaMan`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &MagmaMan::emitted_light
- [`particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &MagmaMan::particle
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &MagmaMan::jump_timer
- [`alive_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=alive_timer) &MagmaMan::alive_timer
### `Bee`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`fly_hang_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fly_hang_timer) &Bee::fly_hang_timer
- [`targeting_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeting_timer) &Bee::targeting_timer
- [`wobble_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wobble_x) &Bee::wobble_x
- [`wobble_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wobble_y) &Bee::wobble_y
### `Ammit`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Ammit::walk_pause_timer
### `ApepPart`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y) &ApepPart::y
- [`sine_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_angle) &ApepPart::sine_angle
### `ApepHead`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`ApepPart`](#apeppart)
- [`distance_traveled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance_traveled) &ApepHead::distance_traveled
- [`tail_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_uid) &ApepHead::tail_uid
- [`fx_mouthpiece1_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_mouthpiece1_uid) &ApepHead::fx_mouthpiece1_uid
- [`fx_mouthpiece2_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_mouthpiece2_uid) &ApepHead::fx_mouthpiece2_uid
### `OsirisHead`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`right_hand_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_hand_uid) &OsirisHead::right_hand_uid
- [`left_hand_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_hand_uid) &OsirisHead::left_hand_uid
- [`moving_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=moving_left) &OsirisHead::moving_left
- [`targeting_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeting_timer) &OsirisHead::targeting_timer
- [`invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_timer) &OsirisHead::invincibility_timer
### `OsirisHand`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &OsirisHand::attack_cooldown_timer
### `Yeti`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Yeti::walk_pause_timer
### `Alien`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Alien::jump_timer
### `UFO`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`patrol_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=patrol_distance) &UFO::patrol_distance
- [`attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &UFO::attack_cooldown_timer
### `Lahamu`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &Lahamu::attack_cooldown_timer
### `YetiQueen`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &YetiQueen::walk_pause_timer
### `YetiKing`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &YetiKing::walk_pause_timer
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &YetiKing::emitted_light
### `Lamassu`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`attack_effect_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_effect_entity) &Lamassu::attack_effect_entity
- [`particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Lamassu::particle
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Lamassu::emitted_light
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Lamassu::walk_pause_timer
- [`flight_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flight_timer) &Lamassu::flight_timer
- [`attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &Lamassu::attack_timer
- [`attack_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_angle) &Lamassu::attack_angle
### `Olmite`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Olmite::walk_pause_timer
- [`attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &Olmite::attack_cooldown_timer
### `Tiamat`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`fx_tiamat_head`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_tiamat_head) &Tiamat::fx_tiamat_head
- [`fx_tiamat_arm_right1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_tiamat_arm_right1) &Tiamat::fx_tiamat_arm_right1
- [`fx_tiamat_arm_right2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_tiamat_arm_right2) &Tiamat::fx_tiamat_arm_right2
- [`frown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frown_timer) &Tiamat::frown_timer
- [`damage_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=damage_timer) &Tiamat::damage_timer
- [`attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &Tiamat::attack_timer
### `GiantFrog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`door_front_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door_front_layer) &GiantFrog::door_front_layer
- [`door_back_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door_back_layer) &GiantFrog::door_back_layer
- [`platform`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=platform) &GiantFrog::platform
- [`attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &GiantFrog::attack_timer
- [`frogs_ejected_in_cycle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frogs_ejected_in_cycle) &GiantFrog::frogs_ejected_in_cycle
### `Frog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Frog::jump_timer
### `Grub`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`rotation_delta`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_delta) &Grub::rotation_delta
- [`drop`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=drop) &Grub::drop
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Grub::walk_pause_timer
### `Tadpole`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`acceleration_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=acceleration_timer) &Tadpole::acceleration_timer
- [`player_spotted`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_spotted) &Tadpole::player_spotted
### `GiantFly`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`head_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=head_entity) &GiantFly::head_entity
- [`particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &GiantFly::particle
- [`sine_amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_amplitude) &GiantFly::sine_amplitude
- [`sine_frequency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_frequency) &GiantFly::sine_frequency
- [`delta_y_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=delta_y_angle) &GiantFly::delta_y_angle
- [`sine_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_counter) &GiantFly::sine_counter
### `Ghist`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`linked_corpse_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=linked_corpse_entity_uid) &Ghist::linked_corpse_entity_uid
- [`transparency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=transparency) &Ghist::transparency
- [`disappear_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=disappear_timer) &Ghist::disappear_timer
### `JumpDog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &JumpDog::walk_pause_timer
- [`squish_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=squish_timer) &JumpDog::squish_timer
### `EggplantMinister`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &EggplantMinister::walk_pause_timer
- [`squish_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=squish_timer) &EggplantMinister::squish_timer
### `Yama`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`message_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=message_shown) &Yama::message_shown
### `Hundun`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`applied_hor_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_hor_velocity) &Hundun::applied_hor_velocity
- [`applied_ver_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_ver_velocity) &Hundun::applied_ver_velocity
- [`birdhead_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=birdhead_entity_uid) &Hundun::birdhead_entity_uid
- [`snakehead_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=snakehead_entity_uid) &Hundun::snakehead_entity_uid
- [`y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_level) &Hundun::y_level
- [`bounce_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounce_timer) &Hundun::bounce_timer
- [`fireball_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fireball_timer) &Hundun::fireball_timer
- [`birdhead_defeated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=birdhead_defeated) &Hundun::birdhead_defeated
- [`snakehead_defeated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=snakehead_defeated) &Hundun::snakehead_defeated
- [`hundun_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hundun_flags) &Hundun::hundun_flags
### `HundunHead`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`egg_crack_effect_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=egg_crack_effect_uid) &HundunHead::egg_crack_effect_uid
- [`targeted_player_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeted_player_uid) &HundunHead::targeted_player_uid
### `MegaJellyfish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`flipper1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipper1) &MegaJellyfish::flipper1
- [`flipper2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipper2) &MegaJellyfish::flipper2
- [`orb_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=orb_uid) &MegaJellyfish::orb_uid
- [`tail_bg_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_bg_uid) &MegaJellyfish::tail_bg_uid
- [`applied_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_velocity) &MegaJellyfish::applied_velocity
- [`wagging_tail_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wagging_tail_counter) &MegaJellyfish::wagging_tail_counter
- [`flipper_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipper_distance) &MegaJellyfish::flipper_distance
- [`velocity_application_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_application_timer) &MegaJellyfish::velocity_application_timer
### `Scorpion`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Scorpion::walk_pause_timer
- [`jump_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_cooldown_timer) &Scorpion::jump_cooldown_timer
### `Hermitcrab`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`carried_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carried_entity_uid) &Hermitcrab::carried_entity_uid
- [`walk_spit_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_spit_timer) &Hermitcrab::walk_spit_timer
- [`is_active`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_active) &Hermitcrab::is_active
- [`spawn_new_carried_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_new_carried_item) &Hermitcrab::spawn_new_carried_item
### `Necromancer`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Necromancer::walk_pause_timer
- [`red_skeleton_spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red_skeleton_spawn_x) &Necromancer::red_skeleton_spawn_x
- [`red_skeleton_spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red_skeleton_spawn_y) &Necromancer::red_skeleton_spawn_y
- [`resurrection_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=resurrection_uid) &Necromancer::resurrection_uid
- [`resurrection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=resurrection_timer) &Necromancer::resurrection_timer
### `ProtoShopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`movement_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=movement_state) &ProtoShopkeeper::movement_state
- [`walk_pause_explode_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_explode_timer) &ProtoShopkeeper::walk_pause_explode_timer
- [`walking_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking_speed) &ProtoShopkeeper::walking_speed
### `ShopkeeperClone`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`climb_y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_y_direction) &ShopkeeperClone::climb_y_direction
- [`target_spotted_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_spotted_timer) &ShopkeeperClone::target_spotted_timer
- [`spawn_gun`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_gun) &ShopkeeperClone::spawn_gun
### `Sparrow`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`target_spotted_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_spotted_timer) &Sparrow::target_spotted_timer
- [`aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &Sparrow::aggro
### `Beg`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Beg::walk_pause_timer
- [`disappear_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=disappear_timer) &Beg::disappear_timer
### `CritterCrab`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &CritterCrab::walk_pause_timer
- [`walking_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking_left) &CritterCrab::walking_left
### `CritterButterfly`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &CritterButterfly::change_direction_timer
- [`vertical_flight_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_flight_direction) &CritterButterfly::vertical_flight_direction
### `CritterLocust`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &CritterLocust::jump_timer
### `CritterSnail`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`x_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x_direction) &CritterSnail::x_direction
- [`y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_direction) &CritterSnail::y_direction
- [`pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_x) &CritterSnail::pos_x
- [`pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_y) &CritterSnail::pos_y
- [`rotation_center_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_x) &CritterSnail::rotation_center_x
- [`rotation_center_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_y) &CritterSnail::rotation_center_y
- [`rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &CritterSnail::rotation_angle
- [`rotation_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_speed) &CritterSnail::rotation_speed
### `CritterFish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`swim_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=swim_pause_timer) &CritterFish::swim_pause_timer
- [`player_in_proximity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_in_proximity) &CritterFish::player_in_proximity
### `CritterPenguin`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &CritterPenguin::walk_pause_timer
- [`jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &CritterPenguin::jump_timer
### `CritterFirefly`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`sine_amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_amplitude) &CritterFirefly::sine_amplitude
- [`sine_frequency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_frequency) &CritterFirefly::sine_frequency
- [`sine_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_angle) &CritterFirefly::sine_angle
- [`change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &CritterFirefly::change_direction_timer
- [`sit_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sit_timer) &CritterFirefly::sit_timer
- [`sit_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sit_cooldown_timer) &CritterFirefly::sit_cooldown_timer
### `CritterDrone`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &CritterDrone::emitted_light
- [`applied_hor_momentum`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_hor_momentum) &CritterDrone::applied_hor_momentum
- [`applied_ver_momentum`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_ver_momentum) &CritterDrone::applied_ver_momentum
- [`move_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_timer) &CritterDrone::move_timer
### `CritterSlime`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster) [`Critter`](#critter)
- [`x_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x_direction) &CritterSlime::x_direction
- [`y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_direction) &CritterSlime::y_direction
- [`pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_x) &CritterSlime::pos_x
- [`pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_y) &CritterSlime::pos_y
- [`rotation_center_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_x) &CritterSlime::rotation_center_x
- [`rotation_center_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_y) &CritterSlime::rotation_center_y
- [`rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &CritterSlime::rotation_angle
- [`rotation_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_speed) &CritterSlime::rotation_speed
- [`walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &CritterSlime::walk_pause_timer
### `Container`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`inside`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inside) &Container::inside
- [`timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Container::timer
### `Bomb`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`scale_hor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_hor) &Bomb::scale_hor
- [`scale_ver`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_ver) &Bomb::scale_ver
### `Backpack`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`explosion_trigger`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=explosion_trigger) &Backpack::explosion_trigger
- [`explosion_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=explosion_timer) &Backpack::explosion_timer
### `Jetpack`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack)
- [`flame_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_on) &Jetpack::flame_on
- [`fuel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fuel) &Jetpack::fuel
### `Hoverpack`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack)
- [`is_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_on) &Hoverpack::is_on
### `Cape`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`floating_down`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=floating_down) &VladsCape::floating_down
### `VladsCape`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Cape`](#cape)
- [`can_double_jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_double_jump) &VladsCape::can_double_jump
### `KapalaPowerup`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`amount_of_blood`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=amount_of_blood) &KapalaPowerup::amount_of_blood
### `Mattock`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`remaining`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=remaining) &Mattock::remaining
### `Gun`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cooldown) &Gun::cooldown
- [`shots`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shots) &Gun::shots
- [`shots2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shots2) &Gun::shots2
- [`in_chamber`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_chamber) &Gun::in_chamber
### `ParticleDB`
- [`id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=id) &ParticleDB::id
- [`spawn_count_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_count_min) &ParticleDB::spawn_count_min
- [`spawn_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_count) &ParticleDB::spawn_count
- [`lifespan_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lifespan_min) &ParticleDB::lifespan_min
- [`lifespan`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lifespan) &ParticleDB::lifespan
- [`sheet_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sheet_id) &ParticleDB::sheet_id
- [`animation_sequence_length`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_sequence_length) &ParticleDB::animation_sequence_length
- [`spawn_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_interval) &ParticleDB::spawn_interval
- [`shrink_growth_factor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shrink_growth_factor) &ParticleDB::shrink_growth_factor
- [`rotation_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_speed) &ParticleDB::rotation_speed
- [`opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=opacity) &ParticleDB::opacity
- [`hor_scattering`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_scattering) &ParticleDB::hor_scattering
- [`ver_scattering`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_scattering) &ParticleDB::ver_scattering
- [`scale_x_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_x_min) &ParticleDB::scale_x_min
- [`scale_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_x) &ParticleDB::scale_x
- [`scale_y_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_y_min) &ParticleDB::scale_y_min
- [`scale_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_y) &ParticleDB::scale_y
- [`hor_deflection_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_deflection_1) &ParticleDB::hor_deflection_1
- [`ver_deflection_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_deflection_1) &ParticleDB::ver_deflection_1
- [`hor_deflection_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_deflection_2) &ParticleDB::hor_deflection_2
- [`ver_deflection_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_deflection_2) &ParticleDB::ver_deflection_2
- [`hor_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_velocity) &ParticleDB::hor_velocity
- [`ver_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_velocity) &ParticleDB::ver_velocity
- [`red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red) &ParticleDB::red
- [`green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=green) &ParticleDB::green
- [`blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blue) &ParticleDB::blue
- [`permanent`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=permanent) &ParticleDB::permanent
- [`invisible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invisible) &ParticleDB::invisible
- [`get_texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture) &ParticleDB::get_texture
- [`set_texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_texture) &ParticleDB::set_texture
### `QuestsInfo`
- [`yang_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=yang_state) &QuestsInfo::yang_state
- [`jungle_sisters_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_sisters_flags) &QuestsInfo::jungle_sisters_flags
- [`van_horsing_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=van_horsing_state) &QuestsInfo::van_horsing_state
- [`sparrow_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sparrow_state) &QuestsInfo::sparrow_state
- [`madame_tusk_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=madame_tusk_state) &QuestsInfo::madame_tusk_state
- [`beg_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=beg_state) &QuestsInfo::beg_state
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
### `CustomSound`
Handle to a loaded sound, can be used to play the sound and receive a `PlayingSound` for more control
It is up to you to not release this as long as any sounds returned by `CustomSound:play()` are still playing
- [`PlayingSound play(bool start_paused, SOUND_TYPE sound_type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play) play
- [`map<VANILLA_SOUND_PARAM,string> get_parameters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameters) &CustomSound::get_parameters
### `PlayingSound`
Handle to a playing sound, start the sound paused to make sure you can apply changes before playing it
You can just discard this handle if you do not need extended control anymore
- [`bool is_playing()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_playing) &PlayingSound::is_playing
- [`bool stop()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stop) &PlayingSound::stop
- [`bool set_pause(bool pause)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pause) &PlayingSound::set_pause
- [`bool set_mute(bool mute)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_mute) &PlayingSound::set_mute
- [`bool set_pitch(float pitch)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pitch) &PlayingSound::set_pitch
- [`bool set_pan(float pan)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pan) &PlayingSound::set_pan
- [`bool set_volume(float volume)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_volume) &PlayingSound::set_volume
- [`bool set_looping(SOUND_LOOP_MODE looping)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_looping) &PlayingSound::set_looping
- [`bool set_callback(function callback)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback) std::move(sound_set_callback)
- [`map<VANILLA_SOUND_PARAM,string> get_parameters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameters) &PlayingSound::get_parameters
- [`optional<float> get_parameter(VANILLA_SOUND_PARAM param)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameter) &PlayingSound::get_parameter
- [`bool set_parameter(VANILLA_SOUND_PARAM param, float value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_parameter) &PlayingSound::set_parameter
### `PlayerSlotSettings`
- [`controller_vibration`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=controller_vibration) sol::readonly(&PlayerSlotSettings::controller_vibration)
- [`auto_run_enabled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=auto_run_enabled) sol::readonly(&PlayerSlotSettings::auto_run_enabled)
- [`controller_right_stick`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=controller_right_stick) sol::readonly(&PlayerSlotSettings::controller_right_stick)
### `PlayerSlot`
- [`buttons_gameplay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons_gameplay) sol::readonly(&PlayerSlot::buttons_gameplay)
- [`buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) sol::readonly(&PlayerSlot::buttons)
- [`input_mapping_keyboard`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=input_mapping_keyboard) sol::readonly(&PlayerSlot::input_mapping_keyboard)
- [`input_mapping_controller`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=input_mapping_controller) sol::readonly(&PlayerSlot::input_mapping_controller)
- [`player_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_id) sol::readonly(&PlayerSlot::player_id)
- [`is_participating`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_participating) sol::readonly(&PlayerSlot::is_participating)
### `InputMapping`
- [`jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump) sol::readonly(&InputMapping::jump)
- [`attack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack) sol::readonly(&InputMapping::attack)
- [`bomb`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bomb) sol::readonly(&InputMapping::bomb)
- [`rope`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rope) sol::readonly(&InputMapping::rope)
- [`walk_run`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_run) sol::readonly(&InputMapping::walk_run)
- [`use_door_buy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=use_door_buy) sol::readonly(&InputMapping::use_door_buy)
- [`pause_menu`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause_menu) sol::readonly(&InputMapping::pause_menu)
- [`journal`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=journal) sol::readonly(&InputMapping::journal)
- [`left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left) sol::readonly(&InputMapping::left)
- [`right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right) sol::readonly(&InputMapping::right)
- [`up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=up) sol::readonly(&InputMapping::up)
- [`down`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=down) sol::readonly(&InputMapping::down)
### `PlayerInputs`
- [`player_slot_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_1) sol::readonly(&PlayerInputs::player_slot_1)
- [`player_slot_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_2) sol::readonly(&PlayerInputs::player_slot_2)
- [`player_slot_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_3) sol::readonly(&PlayerInputs::player_slot_3)
- [`player_slot_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_4) sol::readonly(&PlayerInputs::player_slot_4)
- [`player_slot_1_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_1_settings) sol::readonly(&PlayerInputs::player_slot_1_settings)
- [`player_slot_2_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_2_settings) sol::readonly(&PlayerInputs::player_slot_2_settings)
- [`player_slot_3_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_3_settings) sol::readonly(&PlayerInputs::player_slot_3_settings)
- [`player_slot_4_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_4_settings) sol::readonly(&PlayerInputs::player_slot_4_settings)
### `TextureDefinition`
Use `TextureDefinition.new()` to get a new instance to this and pass it to define_entity_texture.
`width` and `height` always have to be the size of the image file. They should be divisible by `tile_width` and `tile_height` respectively.
`tile_width` and `tile_height` define the size of a single tile, the image will automatically be divided into these tiles.
Tiles are labeled in sequence starting at the top left, going right and down at the end of the image (you know, like sentences work in the English language). Use those numbers in `Entity::animation_frame`.
`sub_image_offset_x`, `sub_image_offset_y`, `sub_image_width` and `sub_image_height` can be used if only a part of the image should be used. Leave them at zero to ignore this.
- [`texture_path`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=texture_path) &TextureDefinition::texture_path
- [`width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &TextureDefinition::width
- [`height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &TextureDefinition::height
- [`tile_width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_width) &TextureDefinition::tile_width
- [`tile_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_height) &TextureDefinition::tile_height
- [`sub_image_offset_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_offset_x) &TextureDefinition::sub_image_offset_x
- [`sub_image_offset_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_offset_y) &TextureDefinition::sub_image_offset_y
- [`sub_image_width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_width) &TextureDefinition::sub_image_width
- [`sub_image_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_height) &TextureDefinition::sub_image_height
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
### PARTICLEEMITTER
- [`TITLE_TORCHFLAME_SMOKE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.TITLE_TORCHFLAME_SMOKE) 1
- ...check particle_emitters.txt output by Overlunky...
- [`MINIGAME_BROKENASTEROID_SMOKE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.MINIGAME_BROKENASTEROID_SMOKE) 219
### VANILLA_SOUND
- [`BGM_BGM_TITLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.BGM_BGM_TITLE) BGM/BGM_title
- ...check vanilla_sounds.txt output by Overlunky...
- [`FX_FX_DM_BANNER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.FX_FX_DM_BANNER) FX/FX_dm_banner
### VANILLA_SOUND_PARAM
- [`POS_SCREEN_X`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.POS_SCREEN_X) 0
- ...check vanilla_sound_params.txt output by Overlunky...
- [`CURRENT_LAYER2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.CURRENT_LAYER2) 37
### DROPCHANCE
- [`BONEBLOCK_SKELETONKEY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROPCHANCE.BONEBLOCK_SKELETONKEY) 0
- ...see drops.hpp for a list of possible dropchances...
- [`YETI_PITCHERSMITT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROPCHANCE.YETI_PITCHERSMITT) 10
### DROP
- [`ALTAR_DICE_CLIMBINGGLOVES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROP.ALTAR_DICE_CLIMBINGGLOVES) 0
- ...see drops.hpp for a list of possible drops...
- [`YETI_PITCHERSMITT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROP.YETI_PITCHERSMITT) 85
### TEXTURE
- [`DATA_TEXTURES_PLACEHOLDER_0`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_PLACEHOLDER_0) 0
- ...see textures.txt for a list of textures...
- [`DATA_TEXTURES_SHINE_0`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_SHINE_0) 388
- [`DATA_TEXTURES_OLDTEXTURES_AI_0`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_OLDTEXTURES_AI_0) 389
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
\
Runs every frame the game is rendered, thus runs at selected framerate. Drawing functions are only available during this callback
- [`FRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.FRAME) 101
\
Runs while playing the game while the player is controllable, not in the base camp or the arena mode
- [`GAMEFRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GAMEFRAME) 108
\
Runs whenever the game engine is actively running. This includes base camp, arena, level transition and death screen
- [`SCREEN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCREEN) 102
\
Runs whenever state.screen changes
- [`START`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.START) 103
\
Runs on the first ON.SCREEN of a run
- [`LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOADING) 104
- [`RESET`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RESET) 105
\
Runs when resetting a run
- [`SAVE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SAVE) 106
\
Params: `SaveContext save_ctx`\
Runs at the same times as ON.SCREEN, but receives the save_ctx
- [`LOAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOAD) 107
\
Params: `LoadContext load_ctx`\
Runs as soon as your script is loaded, including reloads, then never again
- [`SCRIPT_ENABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_ENABLE) 109
- [`SCRIPT_DISABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_DISABLE) 110
### SPAWN_TYPE
- [`LEVEL_GEN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN) SPAWN_TYPE_LEVEL_GEN
\
For any spawn happening during level generation, even if the call happened from the Lua API during a tile code callback.
- [`SCRIPT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.SCRIPT) SPAWN_TYPE_SCRIPT
\
Runs for any spawn happening through a call from the Lua API, also during level generation.
- [`SYSTEMIC`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.SYSTEMIC) SPAWN_TYPE_SYSTEMIC
\
Covers all other spawns, such as items from crates or the player throwing bombs.
- [`ANY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.ANY) SPAWN_TYPE_ANY
\
Covers all of the above.
### CONST
- [`ENGINE_FPS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ENGINE_FPS) 60
### WIN_STATE
After setting the WIN_STATE, the exit door on the current level will lead to the chosen ending
- [`NO_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.NO_WIN) 0
- [`TIAMAT_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.TIAMAT_WIN) 1
- [`HUNDUN_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.HUNDUN_WIN) 2
- [`COSMIC_OCEAN_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.COSMIC_OCEAN_WIN) 3
### CAUSE_OF_DEATH
- [`DEATH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.DEATH) 0
- [`ENTITY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.ENTITY) 1
- [`LONG_FALL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.LONG_FALL) 2
- [`STILL_FALLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.STILL_FALLING) 3
- [`MISSED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.MISSED) 4
- [`POISONED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.POISONED) 5
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
### HUNDUNFLAGS
- [`WILLMOVELEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.WILLMOVELEFT) 1
- [`BIRDHEADEMERGED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.BIRDHEADEMERGED) 2
- [`SNAKEHEADEMERGED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.SNAKEHEADEMERGED) 4
- [`TOPLEVELARENAREACHED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.TOPLEVELARENAREACHED) 8
- [`BIRDHEADSHOTLAST`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.BIRDHEADSHOTLAST) 16
### LAYER
- [`FRONT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.FRONT) 0
- [`BACK`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.BACK) 1
- [`PLAYER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER) -1
- [`PLAYER1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER1) -1
- [`PLAYER2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER2) -2
- [`PLAYER3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER3) -3
- [`PLAYER4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER4) -4
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
### COSUBTHEME
Parameter to force_co_subtheme
- [`RESET`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.RESET) -1
- [`DWELLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.DWELLING) 0
- [`JUNGLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.JUNGLE) 1
- [`VOLCANA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.VOLCANA) 2
- [`TIDEPOOL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.TIDEPOOL) 3
- [`TEMPLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.TEMPLE) 4
- [`ICECAVES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.ICECAVES) 5
- [`NEOBABYLON`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.NEOBABYLON) 6
- [`SUNKENCITY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.SUNKENCITY) 7
### YANG
Yang quest states
- [`ANGRY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.ANGRY) -1
- [`QUEST_NOT_STARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.QUEST_NOT_STARTED) 0
- [`TURKEY_PEN_SPAWNED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.TURKEY_PEN_SPAWNED) 2
- [`BOTH_TURKEYS_DELIVERED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.BOTH_TURKEYS_DELIVERED) 3
- [`TURKEY_SHOP_SPAWNED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.TURKEY_SHOP_SPAWNED) 4
- [`ONE_TURKEY_BOUGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.ONE_TURKEY_BOUGHT) 5
- [`TWO_TURKEYS_BOUGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.TWO_TURKEYS_BOUGHT) 6
- [`THREE_TURKEYS_BOUGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.THREE_TURKEYS_BOUGHT) 7
### JUNGLESISTERS
Jungle sister quest flags (angry = -1)
- [`PARSLEY_RESCUED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.PARSLEY_RESCUED) 1
- [`PARSNIP_RESCUED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.PARSNIP_RESCUED) 2
- [`PARMESAN_RESCUED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.PARMESAN_RESCUED) 3
- [`WARNING_ONE_WAY_DOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.WARNING_ONE_WAY_DOOR) 4
- [`GREAT_PARTY_HUH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.GREAT_PARTY_HUH) 5
- [`I_WISH_BROUGHT_A_JACKET`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.I_WISH_BROUGHT_A_JACKET) 6
### VANHORSING
Van Horsing quest states
- [`QUEST_NOT_STARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.QUEST_NOT_STARTED) 0
- [`JAILCELL_SPAWNED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.JAILCELL_SPAWNED) 1
- [`FIRST_ENCOUNTER_DIAMOND_THROWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.FIRST_ENCOUNTER_DIAMOND_THROWN) 2
- [`SPAWNED_IN_VLADS_CASTLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.SPAWNED_IN_VLADS_CASTLE) 3
- [`SHOT_VLAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.SHOT_VLAD) 4
- [`TEMPLE_HIDEOUT_SPAWNED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.TEMPLE_HIDEOUT_SPAWNED) 5
- [`SECOND_ENCOUNTER_COMPASS_THROWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.SECOND_ENCOUNTER_COMPASS_THROWN) 6
- [`TUSK_CELLAR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.TUSK_CELLAR) 7
### SPARROW
Sparrow quest states
- [`QUEST_NOT_STARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.QUEST_NOT_STARTED) 0
- [`THIEF_STATUS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.THIEF_STATUS) 1
- [`FINISHED_LEVEL_WITH_THIEF_STATUS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FINISHED_LEVEL_WITH_THIEF_STATUS) 2
- [`FIRST_HIDEOUT_SPAWNED_ROPE_THROW`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FIRST_HIDEOUT_SPAWNED_ROPE_THROW) 3
- [`FIRST_ENCOUNTER_ROPES_THROWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FIRST_ENCOUNTER_ROPES_THROWN) 4
- [`TUSK_IDOL_STOLEN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.TUSK_IDOL_STOLEN) 5
- [`SECOND_HIDEOUT_SPAWNED_NEOBAB`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.SECOND_HIDEOUT_SPAWNED_NEOBAB) 6
- [`SECOND_ENCOUNTER_INTERACTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.SECOND_ENCOUNTER_INTERACTED) 7
- [`MEETING_AT_TUSK_BASEMENT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.MEETING_AT_TUSK_BASEMENT) 8
### TUSK
Madame Tusk quest states
- [`ANGRY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.ANGRY) -2
- [`DEAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.DEAD) -1
- [`QUEST_NOT_STARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.QUEST_NOT_STARTED) 0
- [`DICE_HOUSE_SPAWNED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.DICE_HOUSE_SPAWNED) 1
- [`HIGH_ROLLER_STATUS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.HIGH_ROLLER_STATUS) 2
- [`PALACE_WELCOME_MESSAGE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.PALACE_WELCOME_MESSAGE) 3
### BEG
Beg quest states
- [`QUEST_NOT_STARTED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.QUEST_NOT_STARTED) 0
- [`ALTAR_DESTROYED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.ALTAR_DESTROYED) 1
- [`SPAWNED_WITH_BOMBBAG`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.SPAWNED_WITH_BOMBBAG) 2
- [`BOMBBAG_THROWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.BOMBBAG_THROWN) 3
- [`SPAWNED_WITH_TRUECROWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.SPAWNED_WITH_TRUECROWN) 4
- [`TRUECROWN_THROWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.TRUECROWN_THROWN) 5
### SOUND_TYPE
Third parameter to `CustomSound:play()`, specifies which group the sound will be played in and thus how the player controls its volume
- [`SFX`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE.SFX) 0
- [`MUSIC`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE.MUSIC) 1
### SOUND_LOOP_MODE
Paramater to `PlayingSound:set_looping()`, specifies what type of looping this sound should do
- [`OFF`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.OFF) 0
- [`LOOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.LOOP) 1
- [`BIDIRECTIONAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.BIDIRECTIONAL) 2
### VANILLA_SOUND_CALLBACK_TYPE
Bitmask parameter to `set_vanilla_sound_callback()`
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
### ENT_FLAG
- [`INVISIBLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INVISIBLE) 1
- [`INDESTRUCTIBLE_OR_SPECIAL_FLOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INDESTRUCTIBLE_OR_SPECIAL_FLOOR) 2
- [`SOLID`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.SOLID) 3
- [`PASSES_THROUGH_OBJECTS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PASSES_THROUGH_OBJECTS) 4
- [`PASSES_THROUGH_EVERYTHING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PASSES_THROUGH_EVERYTHING) 5
- [`TAKE_NO_DAMAGE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.TAKE_NO_DAMAGE) 6
- [`THROWABLE_OR_KNOCKBACKABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.THROWABLE_OR_KNOCKBACKABLE) 7
- [`IS_PLATFORM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.IS_PLATFORM) 8
- [`CLIMBABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.CLIMBABLE) 9
- [`NO_GRAVITY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.NO_GRAVITY) 10
- [`INTERACT_WITH_WATER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INTERACT_WITH_WATER) 11
- [`STUNNABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.STUNNABLE) 12
- [`COLLIDES_WALLS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.COLLIDES_WALLS) 13
- [`INTERACT_WITH_SEMISOLIDS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INTERACT_WITH_SEMISOLIDS) 14
- [`CAN_BE_STOMPED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.CAN_BE_STOMPED) 15
- [`POWER_STOMPS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.POWER_STOMPS) 16
- [`FACING_LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.FACING_LEFT) 17
- [`PICKUPABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PICKUPABLE) 18
- [`USABLE_ITEM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.USABLE_ITEM) 19
- [`ENABLE_BUTTON_PROMPT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.ENABLE_BUTTON_PROMPT) 20
- [`INTERACT_WITH_WEBS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INTERACT_WITH_WEBS) 21
- [`LOCKED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.LOCKED) 22
- [`SHOP_ITEM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.SHOP_ITEM) 23
- [`SHOP_FLOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.SHOP_FLOOR) 24
- [`PASSES_THROUGH_PLAYER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PASSES_THROUGH_PLAYER) 25
- [`PAUSE_AI_AND_PHYSICS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PAUSE_AI_AND_PHYSICS) 28
- [`DEAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.DEAD) 29
- [`HAS_BACKITEM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.HAS_BACKITEM) 32
### CHAR_STATE
- [`FLAILING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.FLAILING) 0
- [`STANDING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.STANDING) 1
- [`SITTING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.SITTING) 2
- [`HANGING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.HANGING) 4
- [`DUCKING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.DUCKING) 5
- [`CLIMBING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.CLIMBING) 6
- [`PUSHING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.PUSHING) 7
- [`JUMPING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.JUMPING) 8
- [`FALLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.FALLING) 9
- [`DROPPING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.DROPPING) 10
- [`ATTACKING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.ATTACKING) 12
- [`THROWING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.THROWING) 17
- [`STUNNED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.STUNNED) 18
- [`ENTERING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.ENTERING) 19
- [`LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.LOADING) 20
- [`EXITING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.EXITING) 21
- [`DYING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.DYING) 22
## Aliases
We use those to clarify what kind of values can be passed and returned from a function, even if the underlying type is really just an integer or a string. This should help to avoid bugs where one would for example just pass a random integer to a function expecting a callback id.
### CallbackId == int;
### Flags == int;
### uColor == int;
## External Function Library
If you use a text editor/IDE that has a Lua linter available you can download [spel2.lua](https://raw.githubusercontent.com/spelunky-fyi/overlunky/main/docs/game_data/spel2.lua), place it in a folder of your choice and specify that folder as a "external function library". For example [VSCode](https://code.visualstudio.com/) with the [Lua Extension](https://marketplace.visualstudio.com/items?itemName=sumneko.lua) offers this feature. This will allow you to get auto-completion of API functions along with linting
