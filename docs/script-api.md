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
### [`game_manager`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_manager)
The GameManager gives access to a couple of Screens as well as the pause and journal UI elements
### [`online`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=online)
The Online object has information about the online lobby and its players
### [`players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players)
An array of [Player](#player) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
### [`savegame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=savegame)
Provides a read-only access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.)
### [`options`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=options)
Table of options set in the UI, added with the [register_option_functions](#register_option_int).
### [`prng`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=prng)
PRNG (short for Pseudo-Random-Number-Generator) holds 10 128bit wide buffers of memory that are mutated on every generation of a random number.
The game uses specific buffers for specific scenarios, for example the third buffer is used every time particles are spawned to determine a random velocity.
The used buffer is determined by [`PRNG_CLASS`](#PRNG_CLASS). If you want to make a mod that does not affect level generation but still uses the prng then you want to stay away from specific buffers.
If you don't care what part of the game you affect just use `prng.random`.
The global prng state, calling any function on it will advance the prng state, thus desynchronizing clients if it does not happen on both clients.
## Functions
Note: The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen.
### [`lua_print`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lua_print)
`nil lua_print()`<br/>
Standard lua print function, prints directly to the console but not to the game
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
### [`register_console_command`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_console_command)
`nil register_console_command(string name, function cmd)`<br/>
Adds a command that can be used in the console.
### [`set_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_interval)
`CallbackId set_interval(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback). You can also return `false` from your function to clear the callback.
Add per level callback function to be called every `frames` engine frames. Timer is paused on pause and cleared on level transition.
### [`set_timeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_timeout)
`CallbackId set_timeout(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.
### [`set_global_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_interval)
`CallbackId set_global_interval(function cb, int frames)`<br/>
Returns unique id for the callback to be used in [clear_callback](#clear_callback). You can also return `false` from your function to clear the callback.
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
`nil register_option_button(string name, string desc, string long_desc, function on_click)`<br/>
Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.
### [`spawn_liquid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_liquid)
`nil spawn_liquid(ENT_TYPE entity_type, float x, float y)`<br/>
Spawn a "block" of liquids, always spawns in the front layer and will have fun effects if `entity_type` is not a liquid.
Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
### [`spawn_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity)
`int spawn_entity(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)`<br/>
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
`int spawn(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)`<br/>
Short for [spawn_entity](#spawn_entity).
### [`spawn_entity_snapped_to_floor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_snapped_to_floor)
`int spawn_entity_snapped_to_floor(ENT_TYPE entity_type, float x, float y, LAYER layer)`<br/>
Spawns an entity directly on the floor below the tile at the given position.
Use this to avoid the little fall that some entities do when spawned during level gen callbacks.
### [`spawn_on_floor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_on_floor)
`int spawn_on_floor(ENT_TYPE entity_type, float x, float y, LAYER layer)`<br/>
Short for [spawn_entity_snapped_to_floor](#spawn_entity_snapped_to_floor).
### [`spawn_grid_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_grid_entity)
`int spawn_grid_entity(ENT_TYPE entity_type, float x, float y, LAYER layer)`<br/>
Spawn a grid entity, such as floor or traps, that snaps to the grid.
### [`spawn_entity_nonreplaceable`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_nonreplaceable)
`int spawn_entity_nonreplaceable(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)`<br/>
Same as `spawn_entity` but does not trigger any pre-entity-spawn callbacks, so it will not be replaced by another script
### [`spawn_critical`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_critical)
`int spawn_critical(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)`<br/>
Short for [spawn_entity_nonreplaceable](#spawn_entity_nonreplaceable).
### [`spawn_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_door)
`int spawn_door(float x, float y, LAYER layer, int w, int l, int t)`<br/>
Spawn a door to another world, level and theme and return the uid of spawned entity.
Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
### [`door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door)
`int door(float x, float y, LAYER layer, int w, int l, int t)`<br/>
Short for [spawn_door](#spawn_door).
### [`spawn_layer_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_layer_door)
`nil spawn_layer_door(float x, float y)`<br/>
Spawn a door to backlayer.
### [`layer_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=layer_door)
`nil layer_door(float x, float y)`<br/>
Short for [spawn_layer_door](#spawn_layer_door).
### [`spawn_apep`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_apep)
`int spawn_apep(float x, float y, LAYER layer, bool right)`<br/>
Spawns apep with the choice if it going left or right, if you want the game to choose use regular spawn functions with `ENT_TYPE.MONS_APEP_HEAD`
### [`spawn_tree`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_tree)
`nil spawn_tree(float x, float y, LAYER layer)`<br/>
Spawns and grows a tree
### [`set_pre_entity_spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_entity_spawn)
`CallbackId set_pre_entity_spawn(function cb, SPAWN_TYPE flags, int mask, variadic_args entity_types)`<br/>
Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
This is run before the entity is spawned, spawn your own entity and return its uid to replace the intended spawn.
In many cases replacing the intended entity won't have the indended effect or will even break the game, so use only if you really know what you're doing.
The callback signature is `optional<int> pre_entity_spawn(entity_type, x, y, layer, overlay_entity, spawn_flags)`
### [`set_post_entity_spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_entity_spawn)
`CallbackId set_post_entity_spawn(function cb, SPAWN_TYPE flags, int mask, variadic_args entity_types)`<br/>
Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
This is run right after the entity is spawned but before and particular properties are changed, e.g. owner or velocity.
The callback signature is `nil post_entity_spawn(entity, spawn_flags)`
### [`warp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=warp)
`nil warp(int w, int l, int t)`<br/>
Warp to a level immediately.
### [`set_seed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_seed)
`nil set_seed(int seed)`<br/>
Set seed and reset run.
### [`god`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=god)
`nil god(bool g)`<br/>
Enable/disable godmode for players.
### [`god_companions`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=god_companions)
`nil god_companions(bool g)`<br/>
Enable/disable godmode for companions.
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
`nil set_contents(int uid, ENT_TYPE item_entity_type)`<br/>
Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `uid` to ENT_TYPE... `item_entity_type`
### [`get_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity)
`Entity get_entity(int uid)`<br/>
Get the [Entity](#entity) behind an uid, converted to the correct type. To see what type you will get, consult the [entity hierarchy list](entities-hierarchy.md)
### [`get_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_type)
`EntityDB get_type(int id)`<br/>
Get the [EntityDB](#entitydb) behind an ENT_TYPE...
### [`get_grid_entity_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_grid_entity_at)
`int get_grid_entity_at(float x, float y, LAYER layer)`<br/>
Gets a grid entity, such as floor or spikes, at the given position and layer.
### [`filter_entities`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=filter_entities)
`array<int> filter_entities(array<int> entities, function predicate)`<br/>
Returns a list of all uids in `entities` for which `predicate(get_entity(uid))` returns true
### [`get_entities_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by)
`array<int> get_entities_by(array<ENT_TYPE> entity_types, int mask, LAYER layer)`<br/>
Get uids of entities by some conditions. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
### [`get_entities_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by)
`array<int> get_entities_by(ENT_TYPE entity_type, int mask, LAYER layer)`<br/>
Get uids of entities by some conditions. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
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
### [`get_entities_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_at)
`array<int> get_entities_at(array<ENT_TYPE> entity_types, int mask, float x, float y, LAYER layer, float radius)`<br/>
Get uids of matching entities inside some radius. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
### [`get_entities_at`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_at)
`array<int> get_entities_at(ENT_TYPE entity_type, int mask, float x, float y, LAYER layer, float radius)`<br/>
Get uids of matching entities inside some radius. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
### [`get_entities_overlapping_hitbox`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping_hitbox)
`array<int> get_entities_overlapping_hitbox(array<ENT_TYPE> entity_types, int mask, AABB hitbox, LAYER layer)`<br/>
Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
### [`get_entities_overlapping_hitbox`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping_hitbox)
`array<int> get_entities_overlapping_hitbox(ENT_TYPE entity_type, int mask, AABB hitbox, LAYER layer)`<br/>
Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types
### [`attach_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attach_entity)
`nil attach_entity(int overlay_uid, int attachee_uid)`<br/>
Attaches `attachee` to `overlay`, similar to setting `get_entity(attachee).overlay = get_entity(overlay)`.
However this function offsets `attachee` (so you don't have to) and inserts it into `overlay`'s inventory.
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
### [`get_level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_level_flags)
`int get_level_flags()`<br/>
Get `state.level_flags`
### [`set_level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_level_flags)
`nil set_level_flags(int flags)`<br/>
Set `state.level_flags`
### [`get_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_type)
`ENT_TYPE get_entity_type(int uid)`<br/>
Get the ENT_TYPE... of the entity by uid
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
`tuple<float, float, int> get_position(int uid)`<br/>
Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
you're standing on, not real level coordinates.
### [`get_render_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_render_position)
`tuple<float, float, int> get_render_position(int uid)`<br/>
Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...
### [`get_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_velocity)
`tuple<float, float> get_velocity(int uid)`<br/>
Get velocity `vx, vy` of an entity by uid. Use this, don't use `Entity.velocityx/velocityy` because those are relative to `Entity.overlay`.
### [`entity_remove_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_remove_item)
`nil entity_remove_item(int id, int item_uid)`<br/>
Remove item by uid from entity
### [`attach_ball_and_chain`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attach_ball_and_chain)
`int attach_ball_and_chain(int uid, float off_x, float off_y)`<br/>
Spawns and attaches ball and chain to `uid`, the initial position of the ball is at the entity position plus `off_x`, `off_y`
### [`spawn_entity_over`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_over)
`int spawn_entity_over(ENT_TYPE entity_type, int over_uid, float x, float y)`<br/>
Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`
### [`spawn_over`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_over)
`int spawn_over(ENT_TYPE entity_type, int over_uid, float x, float y)`<br/>
Short for [spawn_entity_over](#spawn_entity_over)
### [`entity_has_item_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_uid)
`bool entity_has_item_uid(int uid, int item_uid)`<br/>
Check if the entity `uid` has some specific `item_uid` by uid in their inventory
### [`entity_has_item_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_type)
`bool entity_has_item_type(int uid, array<ENT_TYPE> entity_types)`<br/>
Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory, can also use table of entity_types
### [`entity_has_item_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_type)
`bool entity_has_item_type(int uid, ENT_TYPE entity_type)`<br/>
Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory, can also use table of entity_types
### [`entity_get_items_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_get_items_by)
`array<int> entity_get_items_by(int uid, array<ENT_TYPE> entity_types, int mask)`<br/>
Gets uids of entities attached to given entity uid. Use `entity_type` and `mask` to filter, set them to 0 to return all attached entities.
### [`entity_get_items_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_get_items_by)
`array<int> entity_get_items_by(int uid, ENT_TYPE entity_type, int mask)`<br/>
Gets uids of entities attached to given entity uid. Use `entity_type` and `mask` to filter, set them to 0 to return all attached entities.
### [`kill_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kill_entity)
`nil kill_entity(int uid, optional<bool> destroy_corpse = nullopt)`<br/>
Kills an entity by uid. `destroy_corpse` defaults to `true`, if you are killing for example a caveman and want the corpse to stay make sure to pass `false`.
### [`pick_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pick_up)
`nil pick_up(int who_uid, int what_uid)`<br/>
Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen. Example:
```lua
-- spawn and equip a jetpack
pick_up(players[1].uid, spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER, 0, 0))
```
### [`drop`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=drop)
`nil drop(int who_uid, int what_uid)`<br/>
Drop an entity by uid
### [`unequip_backitem`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unequip_backitem)
`nil unequip_backitem(int who_uid)`<br/>
Unequips the currently worn backitem
### [`worn_backitem`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=worn_backitem)
`int worn_backitem(int who_uid)`<br/>
Returns the uid of the currently worn backitem, or -1 if wearing nothing
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
`nil set_arrowtrap_projectile(ENT_TYPE regular_entity_type, ENT_TYPE poison_entity_type)`<br/>
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
Sets the multiplication factor for blood droplets upon death (default/no Vlad's cape = 1, with Vlad's cape = 2)
Due to changes in 1.23.x only the Vlad's cape value you provide will be used. The default is automatically Vlad's cape value - 1
### [`flip_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flip_entity)
`nil flip_entity(int uid)`<br/>
Flip entity around by uid. All new entities face right by default.
### [`set_olmec_phase_y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_olmec_phase_y_level)
`nil set_olmec_phase_y_level(int phase, float y)`<br/>
Sets the Y-level at which Olmec changes phases
### [`set_ghost_spawn_times`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_ghost_spawn_times)
`nil set_ghost_spawn_times(int normal = 10800, int cursed = 9000)`<br/>
Determines when the ghost appears, either when the player is cursed or not
### [`set_journal_enabled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_journal_enabled)
`nil set_journal_enabled(bool b)`<br/>
Enables or disables the journal
### [`set_camp_camera_bounds_enabled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_camp_camera_bounds_enabled)
`nil set_camp_camera_bounds_enabled(bool b)`<br/>
Enables or disables the default position based camp camera bounds, to set them manually yourself
### [`waddler_count_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_count_entity)
`int waddler_count_entity(ENT_TYPE entity_type)`<br/>
Returns how many of a specific entity type Waddler has stored
### [`waddler_store_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_store_entity)
`int waddler_store_entity(ENT_TYPE entity_type)`<br/>
Store an entity type in Waddler's storage. Returns the slot number the item was stored in or -1 when storage is full and the item couldn't be stored.
### [`waddler_remove_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_remove_entity)
`nil waddler_remove_entity(ENT_TYPE entity_type, int amount_to_remove = 99)`<br/>
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
### [`spawn_companion`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_companion)
`int spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer)`<br/>
Spawn a companion (hired hand, player character, eggplant child)
### [`distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance)
`float distance(int uid_a, int uid_b)`<br/>
Calculate the tile distance of two entities by uid
### [`get_bounds`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_bounds)
`tuple<float, float, float, float> get_bounds()`<br/>
Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
inside these boundaries. The order is: top left x, top left y, bottom right x, bottom right y Example:
```lua
-- Draw the level boundaries
set_callback(function(draw_ctx)
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(xmin, ymin) -- top left
    sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    draw_ctx:draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)
```
### [`get_camera_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_camera_position)
`tuple<float, float> get_camera_position()`<br/>
Gets the current camera position in the level
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
`nil send_input(int uid, INPUTS buttons)`<br/>
Send input
### [`read_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_input)
`INPUTS read_input(int uid)`<br/>
Read input
### [`read_stolen_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_stolen_input)
`INPUTS read_stolen_input(int uid)`<br/>
Read input that has been previously stolen with steal_input
### [`clear_screen_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_screen_callback)
`nil clear_screen_callback(int screen_id, CallbackId cb_id)`<br/>
Clears a callback that is specific to a screen.
### [`set_pre_render_screen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_render_screen)
`optional<CallbackId> set_pre_render_screen(int screen_id, function fun)`<br/>
Returns unique id for the callback to be used in [clear_screen_callback](#clear_screen_callback) or `nil` if screen_id is not valid.
Sets a callback that is called right before the screen is drawn, return `true` to skip the default rendering.
### [`set_post_render_screen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_render_screen)
`optional<CallbackId> set_post_render_screen(int screen_id, function fun)`<br/>
Returns unique id for the callback to be used in [clear_screen_callback](#clear_screen_callback) or `nil` if screen_id is not valid.
Sets a callback that is called right after the screen is drawn.
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
### [`set_on_destroy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_destroy)
`optional<CallbackId> set_on_destroy(int uid, function fun)`<br/>
Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right when an entity is destroyed, e.g. as if by `Entity.destroy()` before the game applies any side effects.
The callback signature is `nil on_destroy(Entity self)`
Use this only when no other approach works, this call can be expensive if overused.
### [`set_on_kill`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_kill)
`optional<CallbackId> set_on_kill(int uid, function fun)`<br/>
Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right when an entity is eradicated (killing monsters that leave a body behind will not trigger this), before the game applies any side effects.
The callback signature is `nil on_kill(Entity self, Entity killer)`
Use this only when no other approach works, this call can be expensive if overused.
### [`set_on_open`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_open)
`optional<CallbackId> set_on_open(int uid, function fun)`<br/>
Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
`uid` has to be the uid of a `Container` or else stuff will break.
Sets a callback that is called right when a container is opened via up+door.
The callback signature is `nil on_open(Entity self, Entity opener)`
Use this only when no other approach works, this call can be expensive if overused.
### [`raise`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=raise)
`nil raise()`<br/>
Raise a signal and probably crash the game
### [`hash_to_stringid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hash_to_stringid)
`STRINGID hash_to_stringid(int hash)`<br/>
Convert the hash to stringid
Check [strings00_hashed.str](game_data/strings00_hashed.str) for the hash values, or extract assets with modlunky and check those.
### [`get_string`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_string)
`const string get_string(STRINGID string_id)`<br/>
Get string behind STRINGID (don't use stringid diretcly for vanilla string, use `hash_to_stringid` first)
Will return the string of currently choosen language
### [`change_string`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_string)
`nil change_string(STRINGID string_id, string str)`<br/>
Change string at the given id (don't use stringid diretcly for vanilla string, use `hash_to_stringid` first)
This edits custom string and in game strings but changing the language in settings will reset game strings
### [`add_string`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_string)
`STRINGID add_string(string str)`<br/>
Add custom string, currently can only be used for names of shop items (Entitydb->description)
Returns STRINGID of the new string
### [`toast_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast_visible)
`bool toast_visible()`<br/>
### [`speechbubble_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_visible)
`bool speechbubble_visible()`<br/>
### [`cancel_toast`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cancel_toast)
`nil cancel_toast()`<br/>
### [`cancel_speechbubble`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cancel_speechbubble)
`nil cancel_speechbubble()`<br/>
### [`seed_prng`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed_prng)
`nil seed_prng(int seed)`<br/>
Seed the game prng.
### [`get_character_name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_character_name)
`string get_character_name(ENT_TYPE type_id)`<br/>
Same as `Player.get_name`
### [`get_character_short_name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_character_short_name)
`string get_character_short_name(ENT_TYPE type_id)`<br/>
Same as `Player.get_short_name`
### [`get_character_heart_color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_character_heart_color)
`Color get_character_heart_color(ENT_TYPE type_id)`<br/>
Same as `Player.get_heart_color`
### [`is_character_female`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_character_female)
`bool is_character_female(ENT_TYPE type_id)`<br/>
Same as `Player.is_female`
### [`set_character_heart_color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_character_heart_color)
`nil set_character_heart_color(ENT_TYPE type_id, Color color)`<br/>
Same as `Player.set_heart_color`
### [`get_particle_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_particle_type)
`ParticleDB get_particle_type(int id)`<br/>
Get the [ParticleDB](#particledb) details of the specified ID
### [`generate_particles`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=generate_particles)
`nil generate_particles(int particle_emitter_id, int uid)`<br/>
Generate particles of the specified type around the specified entity uid (use e.g. generate_particles(PARTICLEEMITTER.PETTING_PET, player.uid))
### [`default_spawn_is_valid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=default_spawn_is_valid)
`bool default_spawn_is_valid(float x, float y, int layer)`<br/>
Default function in spawn definitions to check whether a spawn is valid or not
### [`set_pre_tile_code_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_tile_code_callback)
`CallbackId set_pre_tile_code_callback(function cb, string tile_code)`<br/>
Add a callback for a specific tile code that is called before the game handles the tile code.
The callback signature is `bool pre_tile_code(x, y, layer, room_template)`
Return true in order to stop the game or scripts loaded after this script from handling this tile code.
For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
### [`set_post_tile_code_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_tile_code_callback)
`CallbackId set_post_tile_code_callback(function cb, string tile_code)`<br/>
Add a callback for a specific tile code that is called after the game handles the tile code.
The callback signature is `nil post_tile_code(x, y, layer, room_template)`
Use this to affect what the game or other scripts spawned in this position.
This is received even if a previous pre-tile-code-callback has returned true
### [`define_tile_code`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_tile_code)
`TILE_CODE define_tile_code(string tile_code)`<br/>
Define a new tile code, to make this tile code do anything you have to use either `set_pre_tile_code_callback` or `set_post_tile_code_callback`.
If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.
### [`get_short_tile_code`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_tile_code)
`optional<int> get_short_tile_code(ShortTileCodeDef short_tile_code_def)`<br/>
Gets a short tile code based on definition, returns `nil` if it can't be found
### [`get_short_tile_code_definition`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_tile_code_definition)
`optional<ShortTileCodeDef> get_short_tile_code_definition(SHORT_TILE_CODE short_tile_code)`<br/>
Gets the definition of a short tile code (if available), will vary depending on which file is loaded
### [`define_procedural_spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_procedural_spawn)
`PROCEDURAL_CHANCE define_procedural_spawn(string procedural_spawn, function do_spawn, function is_valid)`<br/>
Define a new procedural spawn, the function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
Use for example when you can spawn only on the ceiling, under water or inside a shop.
Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
If a user disables your script but still uses your level mod nothing will be spawned in place of your procedural spawn.
### [`define_extra_spawn`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_extra_spawn)
`int define_extra_spawn(function do_spawn, function is_valid, int num_spawns_frontlayer, int num_spawns_backlayer)`<br/>
Define a new extra spawn, these are semi-guaranteed level gen spawns with a fixed upper bound.
The function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
Use for example when you can spawn only on the ceiling, under water or inside a shop.
Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
To change the number of spawns use `PostRoomGenerationContext::set_num_extra_spawns` during `ON.POST_ROOM_GENERATION`
No name is attached to the extra spawn since it is not modified from level files, instead every call to this function will return a new uniqe id.
### [`get_missing_extra_spawns`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_missing_extra_spawns)
`tuple<int, int> get_missing_extra_spawns(int extra_spawn_chance_id)`<br/>
Use to query whether any of the requested spawns could not be made, usually because there were not enough valid spaces in the level.
Returns missing spawns in the front layer and missing spawns in the back layer in that order.
The value only makes sense after level generation is complete, aka after `ON.POST_LEVEL_GENERATION` has run.
### [`get_room_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_index)
`tuple<int, int> get_room_index(float x, float y)`<br/>
Transform a position to a room index to be used in `get_room_template` and `PostRoomGenerationContext.set_room_template`
### [`get_room_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_pos)
`tuple<float, float> get_room_pos(int x, int y)`<br/>
Transform a room index into the top left corner position in the room
### [`get_room_template`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_template)
`optional<int> get_room_template(int x, int y, LAYER layer)`<br/>
Get the room template given a certain index, returns `nil` if coordinates are out of bounds
### [`is_room_flipped`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_room_flipped)
`bool is_room_flipped(int x, int y)`<br/>
Get whether a room is flipped at the given index, returns `false` if coordinates are out of bounds
### [`get_room_template_name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_template_name)
`string_view get_room_template_name(int room_template)`<br/>
For debugging only, get the name of a room template, returns `'invalid'` if room template is not defined
### [`define_room_template`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_room_template)
`int define_room_template(string room_template, ROOM_TEMPLATE_TYPE type)`<br/>
Define a new room remplate to use with `set_room_template`
### [`set_room_template_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_room_template_size)
`bool set_room_template_size(int room_template, int width, int height)`<br/>
Set the size of room template in tiles, the template must be of type `ROOM_TEMPLATE_TYPE.MACHINE_ROOM`.
### [`get_procedural_spawn_chance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_procedural_spawn_chance)
`int get_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id)`<br/>
Get the inverse chance of a procedural spawn for the current level.
A return value of 0 does not mean the chance is infinite, it means the chance is zero.
### [`get_co_subtheme`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_co_subtheme)
`int get_co_subtheme()`<br/>
Gets the sub theme of the current cosmic ocean level, returns `COSUBTHEME.NONE` if the current level is not a CO level.
### [`force_co_subtheme`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_co_subtheme)
`nil force_co_subtheme(int subtheme)`<br/>
Forces the theme of the next cosmic ocean level(s) (use e.g. `force_co_subtheme(COSUBTHEME.JUNGLE)`. Use `COSUBTHEME.RESET` to reset to default random behaviour)
### [`get_level_config`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_level_config)
`int get_level_config(LEVEL_CONFIG config)`<br/>
Gets the value for the specified config
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
properties on the sound. Otherwise you may cause a deadlock. The callback signature is `nil on_vanilla_sound(PlayingSound sound)`
### [`clear_vanilla_sound_callback`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_vanilla_sound_callback)
`nil clear_vanilla_sound_callback(CallbackId id)`<br/>
Clears a previously set callback
### [`rgba`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rgba)
`uColor rgba(int r, int g, int b, int a)`<br/>
Converts a color to int to be used in drawing functions. Use values from `0..255`.
### [`draw_text_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text_size)
`tuple<float, float> draw_text_size(float size, string text)`<br/>
Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.
Example:
```lua
function on_guiframe(draw_ctx)
    -- get a random color
    color = math.random(0, 0xffffffff)
    -- zoom the font size based on frame
    size = (get_frame() % 199)+1
    text = 'Awesome!'
    -- calculate size of text
    w, h = draw_text_size(size, text)
    -- draw to the center of screen
    draw_ctx:draw_text(0-w/2, 0-h/2, size, text, color)
end
```
### [`create_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_image)
`tuple<IMAGE, int, int> create_image(string path)`<br/>
Create image from file. Returns a tuple containing id, width and height.
### [`mouse_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mouse_position)
`tuple<float, float> mouse_position()`<br/>
Current mouse cursor position in screen coordinates.
### [`set_drop_chance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_drop_chance)
`nil set_drop_chance(int dropchance_id, int new_drop_chance)`<br/>
Alters the drop chance for the provided monster-item combination (use e.g. set_drop_chance(DROPCHANCE.MOLE_MATTOCK, 10) for a 1 in 10 chance)
### [`replace_drop`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=replace_drop)
`nil replace_drop(int drop_id, ENT_TYPE new_drop_entity_type)`<br/>
Changes a particular drop, e.g. what Van Horsing throws at you (use e.g. replace_drop(DROP.VAN_HORSING_DIAMOND, ENT_TYPE.ITEM_PLASMACANNON))
### [`get_texture_definition`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture_definition)
`TextureDefinition get_texture_definition(TEXTURE texture_id)`<br/>
Gets a `TextureDefinition` for equivalent to the one used to define the texture with `id`
### [`define_texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_texture)
`TEXTURE define_texture(TextureDefinition texture_data)`<br/>
Defines a new texture that can be used in Entity::set_texture
### [`get_hitbox`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_hitbox)
`AABB get_hitbox(int uid, optional<float> extrude, optional<float> offsetx, optional<float> offsety)`<br/>
Gets the hitbox of an entity, use `extrude` to make the hitbox bigger/smaller in all directions and `offset` to offset the hitbox in a given direction
### [`get_render_hitbox`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_render_hitbox)
`AABB get_render_hitbox(int uid, optional<float> extrude, optional<float> offsetx, optional<float> offsety)`<br/>
Same as `get_hitbox` but based on `get_render_position`
### [`screen_aabb`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_aabb)
`AABB screen_aabb(AABB box)`<br/>
Convert an `AABB` to a screen `AABB` that can be directly passed to draw functions
## Deprecated Functions
#### These functions still exist but their usage is discouraged, they all have alternatives mentioned here so please use those!
### [`on_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_frame)
Use `set_callback(function, ON.FRAME)` instead
### [`on_camp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_camp)
Use `set_callback(function, ON.CAMP)` instead
### [`on_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_level)
Use `set_callback(function, ON.LEVEL)` instead
### [`on_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_start)
Use `set_callback(function, ON.START)` instead
### [`on_transition`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_transition)
Use `set_callback(function, ON.TRANSITION)` instead
### [`on_death`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_death)
Use `set_callback(function, ON.DEATH)` instead
### [`on_win`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_win)
Use `set_callback(function, ON.WIN)` instead
### [`on_screen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_screen)
Use `set_callback(function, ON.SCREEN)` instead
### [`on_guiframe`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_guiframe)
Use `set_callback(function, ON.GUIFRAME)` instead
### [`force_dark_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_dark_level)
`nil force_dark_level(bool g)`<br/>
Set level flag 18 on post room generation instead, to properly force every level to dark
```lua
set_callback(function()
    state.level_flags = set_flag(state.level_flags, 18)
end, ON.POST_ROOM_GENERATION)
```
### [`get_entities`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities)
`array<int> get_entities()`<br/>
Use `get_entities_by(0, MASK.ANY, LAYER.BOTH)` instead
### [`get_entities_by_mask`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_mask)
`array<int> get_entities_by_mask(int mask)`<br/>
Use `get_entities_by(0, mask, LAYER.BOTH)` instead
### [`get_entities_by_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_layer)
`array<int> get_entities_by_layer(LAYER layer)`<br/>
Use `get_entities_by(0, MASK.ANY, layer)` instead
### [`get_entities_overlapping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping)
`array<int> get_entities_overlapping(array<ENT_TYPE> entity_types, int mask, float sx, float sy, float sx2, float sy2, LAYER layer)`<br/>
Use `get_entities_overlapping_hitbox` instead
### [`get_entities_overlapping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping)
`array<int> get_entities_overlapping(ENT_TYPE entity_type, int mask, float sx, float sy, float sx2, float sy2, LAYER layer)`<br/>
Use `get_entities_overlapping_hitbox` instead
### [`get_entity_ai_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_ai_state)
`int get_entity_ai_state(int uid)`<br/>
As the name is misleading. use entity `move_state` field instead
### [`set_camera_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_camera_position)
`nil set_camera_position(float cx, float cy)`<br/>
this doesn't actually work at all. See State -> Camera the for proper camera handling
### [`draw_line`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_line)
`nil draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color)`<br/>
Use `GuiDrawContext.draw_line` instead
### [`draw_rect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect)
`nil draw_rect(float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color)`<br/>
Use `GuiDrawContext.draw_rect` instead
### [`draw_rect_filled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled)
`nil draw_rect_filled(float x1, float y1, float x2, float y2, float rounding, uColor color)`<br/>
Use `GuiDrawContext.draw_rect_filled` instead
### [`draw_circle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle)
`nil draw_circle(float x, float y, float radius, float thickness, uColor color)`<br/>
Use `GuiDrawContext.draw_circle` instead
### [`draw_circle_filled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle_filled)
`nil draw_circle_filled(float x, float y, float radius, uColor color)`<br/>
Use `GuiDrawContext.draw_circle_filled` instead
### [`draw_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text)
`nil draw_text(float x, float y, float size, string text, uColor color)`<br/>
Use `GuiDrawContext.draw_text` instead
### [`draw_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image)
`nil draw_image(IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)`<br/>
Use `GuiDrawContext.draw_image` instead
### [`draw_image_rotated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image_rotated)
`nil draw_image_rotated(IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)`<br/>
Use `GuiDrawContext.draw_image_rotated` instead
### [`window`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=window)
`nil window(string title, float x, float y, float w, float h, bool movable, function callback)`<br/>
Use `GuiDrawContext.window` instead
### [`win_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_text)
`nil win_text(string text)`<br/>
Use `GuiDrawContext.win_text` instead
### [`win_separator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_separator)
`nil win_separator()`<br/>
Use `GuiDrawContext.win_separator` instead
### [`win_inline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_inline)
`nil win_inline()`<br/>
Use `GuiDrawContext.win_inline` instead
### [`win_sameline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_sameline)
`nil win_sameline(float offset, float spacing)`<br/>
Use `GuiDrawContext.win_sameline` instead
### [`win_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_button)
`bool win_button(string text)`<br/>
Use `GuiDrawContext.win_button` instead
### [`win_input_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_text)
`string win_input_text(string label, string value)`<br/>
Use `GuiDrawContext.win_input_text` instead
### [`win_input_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_int)
`int win_input_int(string label, int value)`<br/>
Use `GuiDrawContext.win_input_int` instead
### [`win_input_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_float)
`float win_input_float(string label, float value)`<br/>
Use `GuiDrawContext.win_input_float` instead
### [`win_slider_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_int)
`int win_slider_int(string label, int value, int min, int max)`<br/>
Use `GuiDrawContext.win_slider_int` instead
### [`win_drag_int`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_int)
`int win_drag_int(string label, int value, int min, int max)`<br/>
Use `GuiDrawContext.win_drag_int` instead
### [`win_slider_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_float)
`float win_slider_float(string label, float value, float min, float max)`<br/>
Use `GuiDrawContext.win_slider_float` instead
### [`win_drag_float`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_float)
`float win_drag_float(string label, float value, float min, float max)`<br/>
Use `GuiDrawContext.win_drag_float` instead
### [`win_check`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_check)
`bool win_check(string label, bool value)`<br/>
Use `GuiDrawContext.win_check` instead
### [`win_combo`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_combo)
`int win_combo(string label, int selected, string opts)`<br/>
Use `GuiDrawContext.win_combo` instead
### [`win_pushid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_pushid)
`nil win_pushid(int id)`<br/>
Use `GuiDrawContext.win_pushid` instead
### [`win_popid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_popid)
`nil win_popid()`<br/>
Use `GuiDrawContext.win_popid` instead
### [`win_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_image)
`nil win_image(IMAGE image, int width, int height)`<br/>
Use `GuiDrawContext.win_image` instead
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
- [`bool save(string data)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=save) &SaveContext::Save
### `LoadContext`
- [`string load()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load) &LoadContext::Load
### `SelectPlayerSlot`
- [`bool activated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=activated) &SelectPlayerSlot::activated
- [`ENT_TYPE character`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character) &SelectPlayerSlot::character
- [`int texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=texture) &SelectPlayerSlot::texture_id
### `Items`
- [`player_select`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_select) sol::property([](Items&s){returnstd::ref(s.player_select_slots
### `ArenaConfigArenas`
- [`bool dwelling_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dwelling_1) &ArenaConfigArenas::dwelling_1
- [`bool dwelling_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dwelling_2) &ArenaConfigArenas::dwelling_2
- [`bool dwelling_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dwelling_3) &ArenaConfigArenas::dwelling_3
- [`bool dwelling_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dwelling_4) &ArenaConfigArenas::dwelling_4
- [`bool dwelling_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dwelling_5) &ArenaConfigArenas::dwelling_5
- [`bool jungle_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_1) &ArenaConfigArenas::jungle_1
- [`bool jungle_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_2) &ArenaConfigArenas::jungle_2
- [`bool jungle_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_3) &ArenaConfigArenas::jungle_3
- [`bool jungle_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_4) &ArenaConfigArenas::jungle_4
- [`bool jungle_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_5) &ArenaConfigArenas::jungle_5
- [`bool volcana_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=volcana_1) &ArenaConfigArenas::volcana_1
- [`bool volcana_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=volcana_2) &ArenaConfigArenas::volcana_2
- [`bool volcana_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=volcana_3) &ArenaConfigArenas::volcana_3
- [`bool volcana_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=volcana_4) &ArenaConfigArenas::volcana_4
- [`bool volcana_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=volcana_5) &ArenaConfigArenas::volcana_5
- [`bool tidepool_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tidepool_1) &ArenaConfigArenas::tidepool_1
- [`bool tidepool_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tidepool_2) &ArenaConfigArenas::tidepool_2
- [`bool tidepool_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tidepool_3) &ArenaConfigArenas::tidepool_3
- [`bool tidepool_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tidepool_4) &ArenaConfigArenas::tidepool_4
- [`bool tidepool_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tidepool_5) &ArenaConfigArenas::tidepool_5
- [`bool temple_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=temple_1) &ArenaConfigArenas::temple_1
- [`bool temple_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=temple_2) &ArenaConfigArenas::temple_2
- [`bool temple_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=temple_3) &ArenaConfigArenas::temple_3
- [`bool temple_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=temple_4) &ArenaConfigArenas::temple_4
- [`bool temple_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=temple_5) &ArenaConfigArenas::temple_5
- [`bool icecaves_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=icecaves_1) &ArenaConfigArenas::icecaves_1
- [`bool icecaves_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=icecaves_2) &ArenaConfigArenas::icecaves_2
- [`bool icecaves_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=icecaves_3) &ArenaConfigArenas::icecaves_3
- [`bool icecaves_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=icecaves_4) &ArenaConfigArenas::icecaves_4
- [`bool icecaves_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=icecaves_5) &ArenaConfigArenas::icecaves_5
- [`bool neobabylon_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=neobabylon_1) &ArenaConfigArenas::neobabylon_1
- [`bool neobabylon_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=neobabylon_2) &ArenaConfigArenas::neobabylon_2
- [`bool neobabylon_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=neobabylon_3) &ArenaConfigArenas::neobabylon_3
- [`bool neobabylon_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=neobabylon_4) &ArenaConfigArenas::neobabylon_4
- [`bool neobabylon_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=neobabylon_5) &ArenaConfigArenas::neobabylon_5
- [`bool sunkencity_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sunkencity_1) &ArenaConfigArenas::sunkencity_1
- [`bool sunkencity_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sunkencity_2) &ArenaConfigArenas::sunkencity_2
- [`bool sunkencity_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sunkencity_3) &ArenaConfigArenas::sunkencity_3
- [`bool sunkencity_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sunkencity_4) &ArenaConfigArenas::sunkencity_4
- [`bool sunkencity_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sunkencity_5) &ArenaConfigArenas::sunkencity_5
### `ArenaConfigItems`
- [`bool rock`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rock) &ArenaConfigItems::rock
- [`bool pot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pot) &ArenaConfigItems::pot
- [`bool bombbag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombbag) &ArenaConfigItems::bombbag
- [`bool bombbox`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombbox) &ArenaConfigItems::bombbox
- [`bool ropepile`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ropepile) &ArenaConfigItems::ropepile
- [`bool pickup_12bag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pickup_12bag) &ArenaConfigItems::pickup_12bag
- [`bool pickup_24bag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pickup_24bag) &ArenaConfigItems::pickup_24bag
- [`bool cooked_turkey`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cooked_turkey) &ArenaConfigItems::cooked_turkey
- [`bool royal_jelly`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=royal_jelly) &ArenaConfigItems::royal_jelly
- [`bool torch`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=torch) &ArenaConfigItems::torch
- [`bool boomerang`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=boomerang) &ArenaConfigItems::boomerang
- [`bool machete`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=machete) &ArenaConfigItems::machete
- [`bool mattock`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mattock) &ArenaConfigItems::mattock
- [`bool crossbow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=crossbow) &ArenaConfigItems::crossbow
- [`bool webgun`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=webgun) &ArenaConfigItems::webgun
- [`bool freezeray`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=freezeray) &ArenaConfigItems::freezeray
- [`bool shotgun`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shotgun) &ArenaConfigItems::shotgun
- [`bool camera`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=camera) &ArenaConfigItems::camera
- [`bool plasma_cannon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=plasma_cannon) &ArenaConfigItems::plasma_cannon
- [`bool wooden_shield`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wooden_shield) &ArenaConfigItems::wooden_shield
- [`bool metal_shield`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=metal_shield) &ArenaConfigItems::metal_shield
- [`bool teleporter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teleporter) &ArenaConfigItems::teleporter
- [`bool mine`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mine) &ArenaConfigItems::mine
- [`bool snaptrap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=snaptrap) &ArenaConfigItems::snaptrap
- [`bool paste`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=paste) &ArenaConfigItems::paste
- [`bool climbing_gloves`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climbing_gloves) &ArenaConfigItems::climbing_gloves
- [`bool pitchers_mitt`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pitchers_mitt) &ArenaConfigItems::pitchers_mitt
- [`bool spike_shoes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spike_shoes) &ArenaConfigItems::spike_shoes
- [`bool spring_shoes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spring_shoes) &ArenaConfigItems::spring_shoes
- [`bool parachute`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=parachute) &ArenaConfigItems::parachute
- [`bool cape`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cape) &ArenaConfigItems::cape
- [`bool vlads_cape`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vlads_cape) &ArenaConfigItems::vlads_cape
- [`bool jetpack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jetpack) &ArenaConfigItems::jetpack
- [`bool hoverpack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hoverpack) &ArenaConfigItems::hoverpack
- [`bool telepack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=telepack) &ArenaConfigItems::telepack
- [`bool powerpack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=powerpack) &ArenaConfigItems::powerpack
- [`bool excalibur`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=excalibur) &ArenaConfigItems::excalibur
- [`bool scepter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scepter) &ArenaConfigItems::scepter
- [`bool kapala`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kapala) &ArenaConfigItems::kapala
- [`bool true_crown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=true_crown) &ArenaConfigItems::true_crown
### `ArenaConfigEquippedItems`
- [`bool paste`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=paste) &ArenaConfigEquippedItems::paste
- [`bool climbing_gloves`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climbing_gloves) &ArenaConfigEquippedItems::climbing_gloves
- [`bool pitchers_mitt`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pitchers_mitt) &ArenaConfigEquippedItems::pitchers_mitt
- [`bool spike_shoes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spike_shoes) &ArenaConfigEquippedItems::spike_shoes
- [`bool spring_shoes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spring_shoes) &ArenaConfigEquippedItems::spring_shoes
- [`bool parachute`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=parachute) &ArenaConfigEquippedItems::parachute
- [`bool kapala`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kapala) &ArenaConfigEquippedItems::kapala
- [`bool scepter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scepter) &ArenaConfigEquippedItems::scepter
### `ArenaState`
- [`int current_arena`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=current_arena) &ArenaState::current_arena
- [`array<int, 4> player_teams`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_teams) &ArenaState::player_teams
- [`int format`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=format) &ArenaState::format
- [`int ruleset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ruleset) &ArenaState::ruleset
- [`array<int, 4> player_lives`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_lives) &ArenaState::player_lives
- [`array<int, 4> player_totalwins`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_totalwins) &ArenaState::player_totalwins
- [`array<bool, 4> player_won`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_won) &ArenaState::player_won
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &ArenaState::timer
\
The menu selection for timer, default values 0..20 where 0 == 30 seconds, 19 == 10 minutes and 20 == infinite. Can go higher, although this will glitch the menu text. Actual time (seconds) = (state.arena.timer + 1) x 30
- [`int timer_ending`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer_ending) &ArenaState::timer_ending
- [`int wins`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins) &ArenaState::wins
- [`int lives`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lives) &ArenaState::lives
- [`array<int, 4> player_idolheld_countdown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_idolheld_countdown) &ArenaState::player_idolheld_countdown
- [`int health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=health) &ArenaState::health
- [`int bombs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombs) &ArenaState::bombs
- [`int ropes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ropes) &ArenaState::ropes
- [`int stun_time`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stun_time) &ArenaState::stun_time
- [`int mount`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mount) &ArenaState::mount
- [`int arena_select`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arena_select) &ArenaState::arena_select
- [`ArenaConfigArenas arenas`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arenas) &ArenaState::arenas
- [`int dark_level_chance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dark_level_chance) &ArenaState::dark_level_chance
- [`int crate_frequency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=crate_frequency) &ArenaState::crate_frequency
- [`ArenaConfigItems items_enabled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=items_enabled) &ArenaState::items_enabled
- [`ArenaConfigItems items_in_crate`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=items_in_crate) &ArenaState::items_in_crate
- [`int held_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=held_item) &ArenaState::held_item
- [`int equipped_backitem`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=equipped_backitem) &ArenaState::equipped_backitem
- [`ArenaConfigEquippedItems equipped_items`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=equipped_items) &ArenaState::equipped_items
- [`int whip_damage`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=whip_damage) &ArenaState::whip_damage
- [`int final_ghost`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=final_ghost) &ArenaState::final_ghost
- [`int breath_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=breath_cooldown) &ArenaState::breath_cooldown
- [`bool punish_ball`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=punish_ball) &ArenaState::punish_ball
### `StateMemory`
- [`int screen_last`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_last) &StateMemory::screen_last
- [`int screen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen) &StateMemory::screen
- [`int screen_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_next) &StateMemory::screen_next
- [`int ingame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ingame) &StateMemory::ingame
- [`int playing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=playing) &StateMemory::playing
- [`int pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause) &StateMemory::pause
- [`int width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &StateMemory::w
- [`int height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &StateMemory::h
- [`int kali_favor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kali_favor) &StateMemory::kali_favor
- [`int kali_status`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kali_status) &StateMemory::kali_status
- [`int kali_altars_destroyed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kali_altars_destroyed) &StateMemory::kali_altars_destroyed
- [`int seed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed) &StateMemory::seed
- [`int time_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_total) &StateMemory::time_total
- [`int world`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world) &StateMemory::world
- [`int world_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world_next) &StateMemory::world_next
- [`int world_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world_start) &StateMemory::world_start
- [`int level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level) &StateMemory::level
- [`int level_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_next) &StateMemory::level_next
- [`int level_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_start) &StateMemory::level_start
- [`int theme`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme) &StateMemory::theme
- [`int theme_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme_next) &StateMemory::theme_next
- [`int theme_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme_start) &StateMemory::theme_start
- [`int shoppie_aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shoppie_aggro) &StateMemory::shoppie_aggro
- [`int shoppie_aggro_next`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shoppie_aggro_next) &StateMemory::shoppie_aggro_levels
- [`int merchant_aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=merchant_aggro) &StateMemory::merchant_aggro
- [`int kills_npc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kills_npc) &StateMemory::kills_npc
- [`int level_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_count) &StateMemory::level_count
- [`int journal_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=journal_flags) &StateMemory::journal_flags
- [`int time_last_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_last_level) &StateMemory::time_last_level
- [`int time_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_level) &StateMemory::time_level
- [`int level_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_flags) &StateMemory::level_flags
- [`int loading`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=loading) &StateMemory::loading
- [`int quest_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quest_flags) &StateMemory::quest_flags
- [`int presence_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=presence_flags) &StateMemory::presence_flags
- [`float fadevalue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadevalue) &StateMemory::fadevalue
- [`int fadeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadeout) &StateMemory::fadeout
- [`int fadein`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadein) &StateMemory::fadein
- [`int loading_black_screen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=loading_black_screen_timer) &StateMemory::loading_black_screen_timer
- [`int saved_dogs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saved_dogs) &StateMemory::saved_dogs
- [`int saved_cats`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saved_cats) &StateMemory::saved_cats
- [`int saved_hamsters`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=saved_hamsters) &StateMemory::saved_hamsters
- [`int win_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_state) &StateMemory::win_state
- [`Illumination illumination`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=illumination) &StateMemory::illumination
- [`int money_last_levels`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=money_last_levels) &StateMemory::money_last_levels
- [`int money_shop_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=money_shop_total) &StateMemory::money_shop_total
- [`PlayerInputs player_inputs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_inputs) &StateMemory::player_inputs
- [`QuestsInfo quests`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quests) &StateMemory::quests
- [`Camera camera`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=camera) &StateMemory::camera
- [`int special_visibility_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_visibility_flags) &StateMemory::special_visibility_flags
- [`int cause_of_death`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cause_of_death) &StateMemory::cause_of_death
- [`ENT_TYPE cause_of_death_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cause_of_death_entity_type) &StateMemory::cause_of_death_entity_type
- [`int toast_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast_timer) &StateMemory::toast_timer
- [`int speechbubble_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_timer) &StateMemory::speechbubble_timer
- [`int speechbubble_owner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_owner) &StateMemory::speechbubble_owner
- [`LevelGenSystem level_gen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_gen) &StateMemory::level_gen
- [`int correct_ushabti`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=correct_ushabti) &StateMemory::correct_ushabti
- [`Items items`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=items) &StateMemory::items
- [`int camera_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=camera_layer) &StateMemory::camera_layer
- [`ScreenTeamSelect screen_team_select`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_team_select) &StateMemory::screen_team_select
- [`ScreenTransition screen_transition`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_transition) &StateMemory::screen_transition
- [`ScreenDeath screen_death`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_death) &StateMemory::screen_death
- [`ScreenWin screen_win`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_win) &StateMemory::screen_win
- [`ScreenCredits screen_credits`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_credits) &StateMemory::screen_credits
- [`ScreenScores screen_scores`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_scores) &StateMemory::screen_scores
- [`ScreenConstellation screen_constellation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_constellation) &StateMemory::screen_constellation
- [`ScreenRecap screen_recap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_recap) &StateMemory::screen_recap
- [`ScreenArenaStagesSelect screen_arena_stages_select`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_arena_stages_select) &StateMemory::screen_arena_stages_select1
- [`ScreenArenaIntro screen_arena_intro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_arena_intro) &StateMemory::screen_arena_intro
- [`ScreenArenaLevel screen_arena_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_arena_level) &StateMemory::screen_arena_level
- [`ScreenArenaScore screen_arena_score`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_arena_score) &StateMemory::screen_arena_score
- [`ScreenArenaMenu screen_arena_menu`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_arena_menu) &StateMemory::screen_arena_menu
- [`ScreenArenaItems screen_arena_items`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_arena_items) &StateMemory::screen_arena_items
- [`int get_correct_ushabti()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_correct_ushabti) &StateMemory::get_correct_ushabti
\
Returns animation_frame of the correct ushabti
- [`nil set_correct_ushabti(int animation_frame)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_correct_ushabti) &StateMemory::set_correct_ushabti
- [`ArenaState arena`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arena) &StateMemory::arena
### `GameManager`
- [`GameProps game_props`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_props) &GameManager::game_props
- [`ScreenLogo screen_logo`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_logo) &GameManager::screen_logo
- [`ScreenIntro screen_intro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_intro) &GameManager::screen_intro
- [`ScreenPrologue screen_prologue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_prologue) &GameManager::screen_prologue
- [`ScreenTitle screen_title`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_title) &GameManager::screen_title
- [`ScreenMenu screen_menu`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_menu) &GameManager::screen_menu
- [`ScreenOptions screen_options`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_options) &GameManager::screen_options
- [`ScreenPlayerProfile screen_player_profile`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_player_profile) &GameManager::screen_player_profile
- [`ScreenLeaderboards screen_leaderboards`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_leaderboards) &GameManager::screen_leaderboards
- [`ScreenSeedInput screen_seed_input`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_seed_input) &GameManager::screen_seed_input
- [`ScreenCharacterSelect screen_character_select`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_character_select) &GameManager::screen_character_select
- [`ScreenCamp screen_camp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_camp) &GameManager::screen_camp
- [`ScreenLevel screen_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_level) &GameManager::screen_level
- [`ScreenOnlineLoading screen_online_loading`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_online_loading) &GameManager::screen_online_loading
- [`ScreenOnlineLobby screen_online_lobby`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_online_lobby) &GameManager::screen_online_lobby
- [`PauseUI pause_ui`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause_ui) &GameManager::pause_ui
- [`JournalUI journal_ui`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=journal_ui) &GameManager::journal_ui
### `GameProps`
- [`int buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &GameProps::buttons
- [`bool game_has_focus`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_has_focus) &GameProps::game_has_focus
### `LightParams`
- [`float red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red) &LightParams::red
- [`float green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=green) &LightParams::green
- [`float blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blue) &LightParams::blue
- [`float size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=size) &LightParams::size
### `Illumination`
- [`array<LightParams, 4> lights`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lights) &Illumination::lights
\
Table of light1, light2, ... etc.
- [`LightParams light1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light1) &Illumination::light1
- [`LightParams light2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light2) &Illumination::light2
- [`LightParams light3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light3) &Illumination::light3
- [`LightParams light4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light4) &Illumination::light4
\
It's rendered on objects around, not as an actual bright spot
- [`float brightness`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brightness) &Illumination::brightness
- [`float brightness_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brightness_multiplier) &Illumination::brightness_multiplier
- [`float light_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light_pos_x) &Illumination::light_pos_x
- [`float light_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light_pos_y) &Illumination::light_pos_y
- [`float offset_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offset_x) &Illumination::offset_x
- [`float offset_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offset_y) &Illumination::offset_y
- [`float distortion`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distortion) &Illumination::distortion
- [`int entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_uid) &Illumination::entity_uid
- [`int flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flags) &Illumination::flags
\
see [flags.hpp](../src/game_api/flags.hpp) illumination_flags
### `Camera`
- [`float bounds_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_left) &Camera::bounds_left
- [`float bounds_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_right) &Camera::bounds_right
- [`float bounds_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_bottom) &Camera::bounds_bottom
- [`float bounds_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounds_top) &Camera::bounds_top
- [`float adjusted_focus_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=adjusted_focus_x) &Camera::adjusted_focus_x
- [`float adjusted_focus_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=adjusted_focus_y) &Camera::adjusted_focus_y
- [`float focus_offset_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_offset_x) &Camera::focus_offset_x
- [`float focus_offset_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_offset_y) &Camera::focus_offset_y
- [`float focus_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_x) &Camera::focus_x
- [`float focus_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focus_y) &Camera::focus_y
- [`float vertical_pan`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_pan) &Camera::vertical_pan
- [`int shake_countdown_start`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_countdown_start) &Camera::shake_countdown_start
- [`int shake_countdown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_countdown) &Camera::shake_countdown
- [`float shake_amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_amplitude) &Camera::shake_amplitude
- [`float shake_multiplier_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_multiplier_x) &Camera::shake_multiplier_x
- [`float shake_multiplier_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_multiplier_y) &Camera::shake_multiplier_y
- [`bool uniform_shake`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=uniform_shake) &Camera::uniform_shake
- [`int focused_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=focused_entity_uid) &Camera::focused_entity_uid
- [`float inertia`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inertia) &Camera::inertia
### `Online`
- [`array<OnlinePlayer, 4> online_players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=online_players) &Online::online_players
- [`OnlinePlayerShort local_player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=local_player) &Online::local_player
- [`OnlineLobby lobby`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lobby) &Online::lobby
### `OnlinePlayer`
- [`int ready_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ready_state) &OnlinePlayer::ready_state
- [`int character`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character) &OnlinePlayer::character
- [`player_name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_name) &OnlinePlayer::player_name
### `OnlineLobby`
- [`int code`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=code) &OnlineLobby::code
- [`string get_code()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_code) &OnlineLobby::get_code
\
Gets the string equivalent of the code
### `PRNG`
PRNG (short for Pseudo-Random-Number-Generator) holds 10 128bit wide buffers of memory that are mutated on every generation of a random number.
The game uses specific buffers for specific scenarios, for example the third buffer is used every time particles are spawned to determine a random velocity.
The used buffer is determined by [`PRNG_CLASS`](#PRNG_CLASS). If you want to make a mod that does not affect level generation but still uses the prng then you want to stay away from specific buffers.
If you don't care what part of the game you affect just use `prng.random`.
- [`nil seed(int seed)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed) &PRNG::seed
\
Same as `seed_prng`
- [`float random_float(PRNG_CLASS type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random_float) &PRNG::random_float
\
Generate a random floating point number in the range `[0, 1)`
- [`bool random_chance(int inverse_chance, PRNG_CLASS type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random_chance) &PRNG::random_chance
\
Returns true with a chance of `1/inverse_chance`
- [`optional<int> random_index(int i, PRNG_CLASS type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random_index) &PRNG::random_index
\
Generate a integer number in the range `[1, i]` or `nil` if `i < 1`
- [`optional<int> random_int(int min, int max, PRNG_CLASS type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random_int) &PRNG::random_int
\
Generate a integer number in the range `[min, max]` or `nil` if `max < min`
- [`float random()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random) random
\
Drop-in replacement for `math.random()`
- [`optional<int> random(int i)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random) random
\
Drop-in replacement for `math.random(i)`
- [`optional<int> random(int min, int max)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=random) random
\
Drop-in replacement for `math.random(min, max)`
### `Color`
- [`Color()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=Color) 
\
Create a new color - defaults to black
- [`Color(const Color&)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=Color) 
- [`Color(float r_, float g_, float b_, float a_)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=Color) 
\
Create a new color by specifying its values
- [`float r`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=r) &Color::r
- [`float g`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=g) &Color::g
- [`float b`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=b) &Color::b
- [`float a`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=a) &Color::a
- [`Color white()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=white) &Color::white
- [`Color silver()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=silver) &Color::silver
- [`Color gray()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=gray) &Color::gray
- [`Color black()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=black) &Color::black
- [`Color red()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red) &Color::red
- [`Color maroon()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=maroon) &Color::maroon
- [`Color yellow()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=yellow) &Color::yellow
- [`Color olive()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=olive) &Color::olive
- [`Color lime()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lime) &Color::lime
- [`Color green()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=green) &Color::green
- [`Color aqua()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aqua) &Color::aqua
- [`Color teal()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teal) &Color::teal
- [`Color blue()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blue) &Color::blue
- [`Color navy()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=navy) &Color::navy
- [`Color fuchsia()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fuchsia) &Color::fuchsia
- [`Color purple()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=purple) &Color::purple
- [`tuple<int, int, int, int> get_rgba()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_rgba) &Color::get_rgba
\
Returns RGBA colors in 0..255 range
- [`nil set_rgba(int red, int green, int blue, int alpha)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_rgba) &Color::set_rgba
\
Changes color based on given RGBA colors in 0..255 range
- [`uColor get_ucolor()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ucolor) &Color::get_ucolor
\
Returns the `uColor` used in `GuiDrawContext` drawing functions
- [`nil set_ucolor(uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_ucolor) &Color::set_ucolor
\
Changes color based on given uColor
### `Animation`
- [`int first_tile`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=first_tile) &Animation::texture
- [`int num_tiles`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=num_tiles) &Animation::count
- [`int interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=interval) &Animation::interval
- [`REPEAT_TYPE repeat_mode`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=repeat_mode) &Animation::repeat
### `EntityDB`
- [`ENT_TYPE id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=id) &EntityDB::id
- [`int search_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=search_flags) &EntityDB::search_flags
- [`float width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &EntityDB::width
- [`float height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &EntityDB::height
- [`int draw_depth`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_depth) &EntityDB::draw_depth
- [`float friction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=friction) &EntityDB::friction
- [`float elasticity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=elasticity) &EntityDB::elasticity
- [`float weight`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=weight) &EntityDB::weight
- [`float acceleration`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=acceleration) &EntityDB::acceleration
- [`float max_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=max_speed) &EntityDB::max_speed
- [`float sprint_factor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sprint_factor) &EntityDB::sprint_factor
- [`float jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump) &EntityDB::jump
- [`float glow_red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_red) &EntityDB::glow_red
- [`float glow_green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_green) &EntityDB::glow_green
- [`float glow_blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_blue) &EntityDB::glow_blue
- [`float glow_alpha`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_alpha) &EntityDB::glow_alpha
- [`int damage`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=damage) &EntityDB::damage
- [`int life`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=life) &EntityDB::life
- [`int blood_content`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blood_content) &EntityDB::blood_content
- [`int texture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=texture) &EntityDB::texture
- [`AnimationMap animations`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animations) &EntityDB::animations
- [`int properties_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=properties_flags) &EntityDB::properties_flags
- [`int default_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=default_flags) &EntityDB::default_flags
- [`int default_more_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=default_more_flags) &EntityDB::default_more_flags
- [`bool leaves_corpse_behind`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=leaves_corpse_behind) &EntityDB::leaves_corpse_behind
- [`int sound_killed_by_player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sound_killed_by_player) &EntityDB::sound_killed_by_player
- [`int sound_killed_by_other`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sound_killed_by_other) &EntityDB::sound_killed_by_other
- [`STRINGID description`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=description) &EntityDB::description
### `Entity`
- [`EntityDB type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=type) &Entity::type
- [`Entity overlay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlay) overlay
- [`int flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flags) &Entity::flags
- [`int more_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=more_flags) &Entity::more_flags
- [`int uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=uid) &Entity::uid
- [`int animation_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_frame) &Entity::animation_frame
- [`int draw_depth`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_depth) &Entity::draw_depth
\
Don't edit this dirrectly, use `set_draw_depth`
- [`float x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x) &Entity::x
- [`float y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y) &Entity::y
- [`int layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=layer) &Entity::layer
- [`float width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &Entity::w
- [`float height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &Entity::h
- [`float special_offsetx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_offsetx) &Entity::special_offsetx
- [`float special_offsety`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_offsety) &Entity::special_offsety
- [`float tile_width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_width) &Entity::tilew
- [`float tile_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_height) &Entity::tileh
- [`float angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=angle) &Entity::angle
- [`Color color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=color) &Entity::color
- [`float hitboxx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hitboxx) &Entity::hitboxx
- [`float hitboxy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hitboxy) &Entity::hitboxy
- [`float offsetx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offsetx) &Entity::offsetx
- [`float offsety`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offsety) &Entity::offsety
- [`Entity topmost()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topmost) topmost
- [`Entity topmost_mount()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topmost_mount) topmost_mount
- [`bool overlaps_with(AABB hitbox)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlaps_with) overlaps_with
- [`bool overlaps_with(float rect_left, float rect_bottom, float rect_right, float rect_top)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlaps_with) overlaps_with
\
Deprecated
Use `overlaps_with(AABB hitbox)` instead
- [`bool overlaps_with(Entity other)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlaps_with) overlaps_with
- [`TEXTURE get_texture()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture) &Entity::get_texture
- [`bool set_texture(TEXTURE texture_id)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_texture) &Entity::set_texture
- [`nil set_draw_depth(int draw_depth)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_draw_depth) &Entity::set_draw_depth
- [`nil liberate_from_shop()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=liberate_from_shop) &Entity::liberate_from_shop
- [`Entity get_held_entity()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_held_entity) &Entity::get_held_entity
- [`nil set_layer(LAYER layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_layer) &Entity::set_layer
\
Moves the entity to specified layer, nothing else happens, so this does not emulate a door transition
- [`nil remove()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=remove) &Entity::remove
\
Moves the entity to the limbo-layer where it can later be retrieved from again via `respawn`
- [`nil respawn(LAYER layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=respawn) &Entity::respawn
\
Moves the entity from the limbo-layer (where it was previously put by `remove`) to `layer`
- [`nil destroy()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destroy) &Entity::destroy
\
Completely removes the entity from existence
### `Movable`
Derived from [`Entity`](#entity)
- [`float movex`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=movex) &Movable::movex
- [`float movey`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=movey) &Movable::movey
- [`BUTTON buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &Movable::buttons
- [`BUTTON buttons_previous`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons_previous) &Movable::buttons_previous
- [`int stand_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stand_counter) &Movable::stand_counter
- [`float jump_height_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_height_multiplier) &Movable::jump_height_multiplier
- [`int owner_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=owner_uid) &Movable::owner_uid
- [`int last_owner_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_owner_uid) &Movable::last_owner_uid
- [`int idle_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=idle_counter) &Movable::idle_counter
- [`int standing_on_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=standing_on_uid) &Movable::standing_on_uid
- [`float velocityx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocityx) &Movable::velocityx
- [`float velocityy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocityy) &Movable::velocityy
- [`int holding_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=holding_uid) &Movable::holding_uid
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &Movable::state
- [`int last_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_state) &Movable::last_state
- [`int move_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_state) &Movable::move_state
- [`int health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=health) &Movable::health
- [`int stun_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stun_timer) &Movable::stun_timer
- [`int stun_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stun_state) &Movable::stun_state
- [`int lock_input_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lock_input_timer) &Movable::lock_input_timer
\
Related to taking damage, also drops you from ladder/rope, can't be set while on the ground unless you'r on a mount
- [`int some_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=some_state) &Movable::some_state
\
Deprecated, it's the same as lock_input_timer, but this name makes no sense
- [`int airtime`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=airtime) &Movable::falling_timer
\
airtime = falling_timer
- [`int falling_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=falling_timer) &Movable::falling_timer
\
airtime = falling_timer
- [`bool is_poisoned()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_poisoned) &Movable::is_poisoned
- [`nil poison(int frames)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison) &Movable::poison
- [`int dark_shadow_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dark_shadow_timer) &Movable::dark_shadow_timer
- [`int exit_invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=exit_invincibility_timer) &Movable::exit_invincibility_timer
- [`int invincibility_frames_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_frames_timer) &Movable::invincibility_frames_timer
- [`int frozen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frozen_timer) &Movable::frozen_timer
- [`bool is_button_pressed(BUTTON button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_pressed) &Movable::is_button_pressed
- [`bool is_button_held(BUTTON button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_held) &Movable::is_button_held
- [`bool is_button_released(BUTTON button)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_button_released) &Movable::is_button_released
- [`int price`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=price) &Movable::price
- [`nil stun(int framecount)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stun) &Movable::stun
- [`nil freeze(int framecount)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=freeze) &Movable::freeze
- [`nil light_on_fire()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=light_on_fire) &Movable::light_on_fire
- [`nil set_cursed(bool b)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_cursed) &Movable::set_cursed
- [`nil drop(Entity entity_to_drop)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=drop) &Movable::drop
- [`nil pick_up(Entity entity_to_pick_up)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pick_up) &Movable::pick_up
- [`bool can_jump()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_jump) &Movable::can_jump
- [`Entity standing_on()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=standing_on) &Movable::standing_on
- [`nil add_money(int money)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_money) &Movable::add_money
\
Adds or subtracts the specified amount of money to the movable's (player's) inventory. Shows the calculation animation in the HUD.
- [`nil damage(int damage_dealer_uid, int damage_amount, int stun_time, float velocity_x, float velocity_y)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=damage) &Movable::damage
\
Damage the movable by the specified amount, stuns it for the specified amount of frames and applies the velocities
### `PowerupCapable`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`nil remove_powerup(ENT_TYPE powerup_type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=remove_powerup) &PowerupCapable::remove_powerup
\
Removes a currently applied powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Removing the Eggplant crown does not seem to undo the throwing of eggplants, the other powerups seem to work.
- [`nil give_powerup(ENT_TYPE powerup_type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=give_powerup) &PowerupCapable::give_powerup
\
Gives the player/monster the specified powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Giving true crown to a monster crashes the game.
- [`bool has_powerup(ENT_TYPE powerup_type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=has_powerup) &PowerupCapable::has_powerup
\
Checks whether the player/monster has a certain powerup
- [`array<ENT_TYPE> get_powerups()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_powerups) &PowerupCapable::get_powerups
\
Return all powerups that the entity has
- [`nil unequip_backitem()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unequip_backitem) &PowerupCapable::unequip_backitem
\
Unequips the currently worn backitem
- [`int worn_backitem()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=worn_backitem) &PowerupCapable::worn_backitem
\
Returns the uid of the currently worn backitem, or -1 if wearing nothing
### `Inventory`
- [`int money`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=money) &Inventory::money
- [`int bombs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombs) &Inventory::bombs
- [`int ropes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ropes) &Inventory::ropes
- [`int poison_tick_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison_tick_timer) &Inventory::poison_tick_timer
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`bool cursed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cursed) &Inventory::cursed
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`int health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=health) &Inventory::health
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`int kapala_blood_amount`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kapala_blood_amount) &Inventory::kapala_blood_amount
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`ENT_TYPE held_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=held_item) &Inventory::held_item
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`int held_item_metadata`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=held_item_metadata) &Inventory::held_item_metadata
\
Metadata of the held item (health, is cursed etc.) Used in level transition to transfer to new entity, is wrong during the level
- [`player_slot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot) sol::property([](Inventory&i)-&gt;int8_t{if(i.player_slot&gt;=0)returni.player_slot+1;elsereturni.player_slot;}
- [`ENT_TYPE mount_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mount_type) &Inventory::mount_type
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`int mount_metadata`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mount_metadata) &Inventory::mount_metadata
\
Metadata of the mount (health, is cursed etc.) Used in level transition to transfer to new player entity, is wrong during the level
- [`int kills_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kills_level) &Inventory::kills_level
- [`int kills_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kills_total) &Inventory::kills_total
- [`int collected_money_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=collected_money_total) &Inventory::collected_money_total
- [`array<ENT_TYPE, 512> collected_money`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=collected_money) &Inventory::collected_money
- [`array<int, 512> collected_money_values`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=collected_money_values) &Inventory::collected_money_values
- [`array<ENT_TYPE, 256> killed_enemies`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=killed_enemies) &Inventory::killed_enemies
- [`int companion_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=companion_count) &Inventory::companion_count
\
Number of companions, this is always up to date, can be edited
- [`array<int, 8> companion_poison_tick_timers`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=companion_poison_tick_timers) &Inventory::companion_poison_tick_timers
\
Companions poison tick timers, used in level transition to transfer to new player entity, is wrong during the level
- [`array<ENT_TYPE, 8> companions`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=companions) &Inventory::companions
\
Companion ENT_TYPEs, used in level transition to transfer to new player entity, is wrong during the level
- [`array<ENT_TYPE, 8> companion_held_items`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=companion_held_items) &Inventory::companion_held_items
\
Items ENT_TYPE held by companions, used in level transition to transfer to new player entity, is wrong during the level
- [`array<int, 8> companion_trust`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=companion_trust) &Inventory::companion_trust
\
0..3, used in level transition to transfer to new player entity, is wrong during the level
- [`array<int, 8> companion_health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=companion_health) &Inventory::companion_health
\
Used in level transition to transfer to new player entity, is wrong during the level
- [`array<bool, 8> is_companion_cursed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_companion_cursed) &Inventory::is_companion_cursed
\
Used in level transition to transfer to new player entity, is wrong during the level
### `Ai`
- [`Entity target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target) &Ai::target
- [`int target_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_uid) &Ai::target_uid
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Ai::timer
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &Ai::state
\
AI state (patrol, sleep, attack, aggro...)
- [`int trust`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=trust) &Ai::trust
\
Levels completed with, 0..3
- [`int whipped`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=whipped) &Ai::whipped
\
How many times master has violated us
### `Player`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable)
- [`Inventory inventory`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inventory) &Player::inventory_ptr
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Player::emitted_light
- [`int linked_companion_parent`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=linked_companion_parent) &Player::linked_companion_parent
- [`int linked_companion_child`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=linked_companion_child) &Player::linked_companion_child
- [`Ai ai`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai) &Player::ai
- [`nil set_jetpack_fuel(int fuel)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_jetpack_fuel) &Player::set_jetpack_fuel
- [`int kapala_blood_amount()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kapala_blood_amount) &Player::kapala_blood_amount
- [`string get_name()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_name) &Player::get_name
\
Get the full name of the character, this will be the modded name not only the vanilla name.
- [`string get_short_name()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_name) &Player::get_short_name
\
Get the short name of the character, this will be the modded name not only the vanilla name.
- [`Color get_heart_color()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_heart_color) &Player::get_heart_color
\
Get the heart color of the character, this will be the modded heart color not only the vanilla heart color.
- [`bool is_female()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_female) &Player::is_female
\
Check whether the character is female, will be `true` if the character was modded to be female as well.
- [`nil set_heart_color(Color hcolor)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_heart_color) &Player::set_heart_color
\
Set the heart color the character.
### `Floor`
Derived from [`Entity`](#entity)
- [`int deco_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deco_top) &Floor::deco_top
- [`int deco_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deco_bottom) &Floor::deco_bottom
- [`int deco_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deco_left) &Floor::deco_left
- [`int deco_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deco_right) &Floor::deco_right
- [`nil fix_border_tile_animation()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fix_border_tile_animation) &Floor::fix_border_tile_animation
\
Sets `animation_frame` of the floor for types `FLOOR_BORDERTILE`, `FLOOR_BORDERTILE_METAL` and `FLOOR_BORDERTILE_OCTOPUS`.
- [`nil fix_decorations(bool fix_also_neighbors, bool fix_styled_floor)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fix_decorations) &Floor::fix_decorations
\
Used to add decoration to a floor entity after it was spawned outside of level gen, is not necessary when spawning during level gen.
Set `fix_also_neighbours` to `true` to fix the neighbouring floor tile decorations on the border of the two tiles.
Set `fix_styled_floor` to `true` to fix decorations on `FLOORSTYLED_` entities, those usually only have decorations when broken.
- [`nil add_decoration(FLOOR_SIDE side)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_decoration) &Floor::add_decoration
\
Explicitly add a decoration on the given side. Corner decorations only exist for `FLOOR_BORDERTILE` and `FLOOR_BORDERTILE_OCTOPUS`.
- [`nil remove_decoration(FLOOR_SIDE side)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=remove_decoration) &Floor::remove_decoration
\
Explicitly remove a decoration on the given side. Corner decorations only exist for `FLOOR_BORDERTILE` and `FLOOR_BORDERTILE_OCTOPUS`.
- [`nil decorate_internal()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=decorate_internal) &Floor::decorate_internal
### `Door`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=counter) &Door::counter
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &Door::fx_button
### `ExitDoor`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door)
- [`bool entered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entered) &ExitDoor::entered
\
if true entering it does not load the transition
- [`bool special_door`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_door) &ExitDoor::special_door
\
use provided world/level/theme
- [`int level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level) &ExitDoor::level
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &ExitDoor::timer
- [`int world`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world) &ExitDoor::world
- [`int theme`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme) &ExitDoor::theme
### `DecoratedDoor`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door) [`ExitDoor`](#exitdoor)
- [`Entity special_bg`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_bg) &DecoratedDoor::special_bg
### `LockedDoor`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door)
- [`bool unlocked`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unlocked) &LockedDoor::unlocked
### `CityOfGoldDoor`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door) [`ExitDoor`](#exitdoor) [`DecoratedDoor`](#decorateddoor)
- [`bool unlocked`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unlocked) &CityOfGoldDoor::unlocked
### `MainExit`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door) [`ExitDoor`](#exitdoor)
### `EggShipDoor`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &EggShipDoor::timer
### `EggShipDoorS`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`Door`](#door) [`EggShipDoor`](#eggshipdoor)
- [`bool entered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entered) &EggShipDoorS::entered
### `Arrowtrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`bool arrow_shot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_shot) &Arrowtrap::arrow_shot
- [`nil rearm()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rearm) &Arrowtrap::rearm
### `TotemTrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`ENT_TYPE spawn_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_type) &TotemTrap::spawn_entity_type
- [`int first_sound_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=first_sound_id) &TotemTrap::first_sound_id
### `LaserTrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &LaserTrap::emitted_light
- [`int reset_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=reset_timer) &LaserTrap::reset_timer
\
after triggering counts from 0 to 255, changes the 'phase_2' then counts from 0 to 104
- [`bool phase_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=phase_2) &LaserTrap::phase_2
### `SparkTrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &SparkTrap::emitted_light
- [`int spark_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spark_uid) &SparkTrap::spark_uid
### `Altar`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Altar::timer
\
for normal altar: counts from 0 to 20 then 0, then 1 then 0 and sacrifice happens
### `SpikeballTrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`Entity chain`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chain) &SpikeballTrap::chain
- [`Entity end_piece`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=end_piece) &SpikeballTrap::end_piece
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &SpikeballTrap::state
\
0 - none, 1 - start, 2 - going_down, 3 - going_up, 4 - pause | going_up is only right when timer is 0, otherwise it just sits at the bottom
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &SpikeballTrap::timer
\
for the start and retract
### `TransferFloor`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`map<int, int> transferred_entities`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=transferred_entities) &TransferFloor::transferred_entities
\
Index is the uid, value is frame the entity entered the floor (time_level), use `pairs` to loop thru
### `ConveyorBelt`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`TransferFloor`](#transferfloor)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &ConveyorBelt::timer
### `Pipe`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int direction_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=direction_type) &Pipe::direction_type
\
3 - straight_horizontal, 4 - blocked, 5 - down_left_turn, 6 - down_right_turn, 8 - blocked, 9 - up_left_turn, 10 - up_right_turn, 12 - straight_vertical
- [`bool end_pipe`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=end_pipe) &Pipe::end_pipe
### `Generator`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int spawned_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawned_uid) &Generator::spawned_uid
- [`int set_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_timer) &Generator::set_timer
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Generator::timer
- [`int start_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_counter) &Generator::start_counter
\
works only for star challenge
- [`bool on_off`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_off) &Generator::on_off
\
works only for star challenge
### `SlidingWallCeiling`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`Entity attached_piece`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attached_piece) &SlidingWallCeiling::attached_piece
- [`int active_floor_part_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=active_floor_part_uid) &SlidingWallCeiling::active_floor_part_uid
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &SlidingWallCeiling::state
\
1 - going up / is at the top, 2 - pause
### `QuickSand`
Derived from [`Entity`](#entity) [`Floor`](#floor)
### `BigSpearTrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int spear_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spear_uid) &BigSpearTrap::spear_uid
- [`bool left_part`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_part) &BigSpearTrap::left_part
\
setting the left part to 0 or right part to 1 destroys the trap
### `StickyTrap`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int attached_piece_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attached_piece_uid) &StickyTrap::attached_piece_uid
- [`int ball_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ball_uid) &StickyTrap::ball_uid
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &StickyTrap::state
\
0 - none, 1 - start, 2 - going down, 3 - is at the bottom, 4 - going up, 5 - pause
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &StickyTrap::timer
### `MotherStatue`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`array<bool, 4> players_standing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players_standing) &MotherStatue::players_standing
\
Table of player1_standing, player2_standing, ... etc.
- [`bool player1_standing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player1_standing) &MotherStatue::player1_standing
- [`bool player2_standing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player2_standing) &MotherStatue::player2_standing
- [`bool player3_standing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player3_standing) &MotherStatue::player3_standing
- [`bool player4_standing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player4_standing) &MotherStatue::player4_standing
- [`array<bool, 4> players_health_received`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players_health_received) &MotherStatue::players_health_received
\
Table of player1_health_received, player2_health_received, ... etc.
- [`bool player1_health_received`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player1_health_received) &MotherStatue::player1_health_received
- [`bool player2_health_received`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player2_health_received) &MotherStatue::player2_health_received
- [`bool player3_health_received`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player3_health_received) &MotherStatue::player3_health_received
- [`bool player4_health_received`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player4_health_received) &MotherStatue::player4_health_received
- [`array<int, 4> players_health_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players_health_timer) &MotherStatue::players_health_timer
\
Table of player1_health_timer, player2_health_timer, ... etc.
- [`int player1_health_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player1_health_timer) &MotherStatue::player1_health_timer
- [`int player2_health_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player2_health_timer) &MotherStatue::player2_health_timer
- [`int player3_health_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player3_health_timer) &MotherStatue::player3_health_timer
- [`int player4_health_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player4_health_timer) &MotherStatue::player4_health_timer
- [`int eggplantchild_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=eggplantchild_timer) &MotherStatue::eggplantchild_timer
- [`bool eggplantchild_detected`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=eggplantchild_detected) &MotherStatue::eggplantchild_detected
### `TeleportingBorder`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=direction) &TeleportingBorder::direction
\
0 - right, 1 - left, 2 - bottom, 3 - top, 4 - disable
### `ForceField`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`Entity first_item_beam`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=first_item_beam) &ForceField::first_item_beam
- [`Entity fx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx) &ForceField::fx
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &ForceField::emitted_light
- [`bool is_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_on) &ForceField::is_on
- [`nil activate_laserbeam(bool turn_on)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=activate_laserbeam) &ForceField::activate_laserbeam
### `TimedForceField`
Derived from [`Entity`](#entity) [`Floor`](#floor) [`ForceField`](#forcefield)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &TimedForceField::timer
- [`bool pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause) &TimedForceField::pause
### `HorizontalForceField`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`Entity first_item_beam`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=first_item_beam) &HorizontalForceField::first_item_beam
- [`Entity fx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx) &HorizontalForceField::fx
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &HorizontalForceField::timer
- [`bool is_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_on) &HorizontalForceField::is_on
### `TentacleBottom`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int attached_piece_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attached_piece_uid) &TentacleBottom::attached_piece_uid
- [`int tentacle_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tentacle_uid) &TentacleBottom::tentacle_uid
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &TentacleBottom::state
\
0 - none, 1 - start, 2 - moving up, 3 - at the top, 4 - moving down 5 - pause
### `PoleDeco`
Derived from [`Entity`](#entity) [`Floor`](#floor)
- [`int deco_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deco_up) &PoleDeco::deco_up
- [`int deco_down`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deco_down) &PoleDeco::deco_down
### `Crushtrap`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float dirx`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dirx) &Crushtrap::dirx
- [`float diry`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=diry) &Crushtrap::diry
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Crushtrap::timer
\
counts from 30 to 0 before moving, after it stops, counts from 60 to 0 before it can be triggered again
- [`int bounce_back_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounce_back_timer) &Crushtrap::bounce_back_timer
\
counts from 7 to 0 after it hits the wall and moves away until the timer hits 0, then moves back and counts from 255 until it hits the wall again, if needed it will start the counter again for another bounce
### `Olmec`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int target_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_uid) &Olmec::target_uid
- [`int attack_phase`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_phase) &Olmec::attack_phase
\
0 = stomp, 1 = bombs, 2 = stomp+ufos, 3 = in lava
- [`int attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &Olmec::attack_timer
\
in phase 0/2: time spent looking for player, in phase 1: time between bomb salvo
- [`int ai_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_timer) &Olmec::ai_timer
\
general timer that counts down whenever olmec is active
- [`int move_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_direction) &Olmec::move_direction
\
-1 = left, 0 = down, 1 = right | phase 0/2: depends on target, phase 1: travel direction
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Olmec::jump_timer
- [`int phase1_amount_of_bomb_salvos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=phase1_amount_of_bomb_salvos) &Olmec::phase1_amount_of_bomb_salvos
- [`int unknown_attack_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown_attack_state) &Olmec::unknown_attack_state
- [`int broken_floaters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=broken_floaters) &Olmec::broken_floaters
### `WoodenlogTrap`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int ceiling_1_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_1_uid) &WoodenlogTrap::ceiling_1_uid
- [`int ceiling_2_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_2_uid) &WoodenlogTrap::ceiling_2_uid
- [`float falling_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=falling_speed) &WoodenlogTrap::falling_speed
### `Boulder`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int is_rolling`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_rolling) &Boulder::is_rolling
\
is set to 1 when the boulder first hits the ground
### `PushBlock`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ParticleEmitterInfo dust_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dust_particle) &PushBlock::dust_particle
- [`float dest_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dest_pos_x) &PushBlock::dest_pos_x
### `BoneBlock`
Derived from [`Entity`](#entity) [`Movable`](#movable)
### `ChainedPushBlock`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PushBlock`](#pushblock)
- [`bool is_chained`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_chained) &ChainedPushBlock::is_chained
### `LightArrowPlatform`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &LightArrowPlatform::emitted_light
### `FallingPlatform`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &FallingPlatform::timer
\
The name `emitted_light` is false here, don't use it, it should be called `timer`
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &FallingPlatform::timer
\
The name `emitted_light` is false here, don't use it, it should be called `timer`
- [`float shaking_factor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shaking_factor) &FallingPlatform::shaking_factor
- [`float y_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_pos) &FallingPlatform::y_pos
### `UnchainedSpikeBall`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool bounce`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounce) &UnchainedSpikeBall::bounce
### `Drill`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity top_chain_piece`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_chain_piece) &Drill::top_chain_piece
### `ThinIce`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int strength`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=strength) &ThinIce::strength
\
counts down when standing on, maximum is 134 as based of this value it changes animation_frame, and above that value it changes to wrong sprite
### `Elevator`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Elevator::emitted_light
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Elevator::timer
\
puase timer, counts down 60 to 0
- [`bool moving_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=moving_up) &Elevator::moving_up
### `ClamBase`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ENT_TYPE treasure_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=treasure_type) &ClamBase::treasure_type
- [`int treasure_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=treasure_uid) &ClamBase::treasure_uid
- [`float treasure_x_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=treasure_x_pos) &ClamBase::treasure_x_pos
- [`float treasure_y_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=treasure_y_pos) &ClamBase::treasure_y_pos
- [`int top_part_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_part_uid) &ClamBase::top_part_uid
### `RegenBlock`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool on_breaking`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_breaking) &RegenBlock::on_breaking
### `TimedPowderkeg`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PushBlock`](#pushblock)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &TimedPowderkeg::timer
\
timer till explosion, -1 = pause, counts down
### `Mount`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`nil carry(Movable rider)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carry) &Mount::carry
- [`nil tame(bool value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tame) &Mount::tame
- [`int rider_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rider_uid) &Mount::rider_uid
- [`bool can_doublejump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_doublejump) &Mount::can_doublejump
- [`bool tamed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tamed) &Mount::tamed
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Mount::walk_pause_timer
- [`int taming_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=taming_timer) &Mount::taming_timer
### `Rockdog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`int attack_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown) &Rockdog::attack_cooldown
### `Axolotl`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`int attack_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown) &Axolotl::attack_cooldown
- [`bool can_teleport`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_teleport) &Axolotl::can_teleport
### `Mech`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`int gun_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=gun_cooldown) &Mech::gun_cooldown
- [`bool walking`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking) &Mech::walking
- [`bool breaking_wall`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=breaking_wall) &Mech::breaking_wall
### `Qilin`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Mount`](#mount)
- [`int attack_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown) &Qilin::attack_cooldown
### `Monster`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable)
- [`int chased_target_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chased_target_uid) &Monster::chased_target_uid
- [`int target_selection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_selection_timer) &Monster::target_selection_timer
### `RoomOwner`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int room_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=room_index) &RoomOwner::room_index
- [`float climb_y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_y_direction) &RoomOwner::climb_y_direction
- [`int ai_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_state) &RoomOwner::ai_state
- [`int patrol_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=patrol_timer) &RoomOwner::patrol_timer
- [`int lose_interest_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lose_interest_timer) &RoomOwner::lose_interest_timer
- [`int countdown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=countdown_timer) &RoomOwner::countdown_timer
\
can't shot when the timer is running
- [`bool is_patrolling`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_patrolling) &RoomOwner::is_patrolling
- [`bool aggro_trigger`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro_trigger) &RoomOwner::aggro_trigger
\
setting this makes him angry, if it's shopkeeper you get 2 agrro points
- [`bool was_hurt`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=was_hurt) &RoomOwner::was_hurt
\
also is set true if you set aggro to true, get's trigger even when whiping
### `WalkingMonster`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int chatting_to_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chatting_to_uid) &WalkingMonster::chatting_to_uid
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &WalkingMonster::walk_pause_timer
\
alternates between walking and pausing every time it reaches zero
- [`int cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cooldown_timer) &WalkingMonster::cooldown_timer
\
used for chatting with other monsters, attack cooldowns etc.
### `NPC`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float climb_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_direction) &NPC::climb_direction
- [`int target_in_sight_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_in_sight_timer) &NPC::target_in_sight_timer
- [`int ai_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_state) &NPC::ai_state
- [`bool aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &NPC::aggro
\
for bodyguard and shopkeeperclone it spawns a weapon as well
### `Ghost`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int split_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=split_timer) &Ghost::split_timer
\
for SMALL_HAPPY this is also the sequence timer of its various states
- [`float velocity_multiplier`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_multiplier) &Ghost::velocity_multiplier
- [`GHOST_BEHAVIOR ghost_behaviour`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ghost_behaviour) &Ghost::ghost_behaviour
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Ghost::emitted_light
- [`Entity linked_ghost`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=linked_ghost) &Ghost::linked_ghost
### `Bat`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &Bat::spawn_x
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &Bat::spawn_y
### `Jiangshi`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int wait_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wait_timer) &Jiangshi::wait_timer
\
wait time between jumps
- [`int jump_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_counter) &Jiangshi::jump_counter
\
only female aka assassin: when 0 will jump up into ceiling
- [`bool on_ceiling`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_ceiling) &Jiangshi::on_ceiling
\
only female aka assassin
### `Monkey`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Monkey::jump_timer
- [`bool on_vine`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_vine) &Monkey::on_vine
### `GoldMonkey`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &GoldMonkey::jump_timer
- [`int poop_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poop_timer) &GoldMonkey::poop_timer
- [`int poop_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poop_count) &GoldMonkey::poop_count
### `Mole`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`ParticleEmitterInfo burrowing_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrowing_particle) &Mole::burrowing_particle
- [`float burrow_dir_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrow_dir_x) &Mole::burrow_dir_x
- [`float burrow_dir_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrow_dir_y) &Mole::burrow_dir_y
- [`int burrowing_in_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burrowing_in_uid) &Mole::burrowing_in_uid
\
stores the last uid as well
- [`int counter_burrowing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=counter_burrowing) &Mole::counter_burrowing
- [`int counter_nonburrowing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=counter_nonburrowing) &Mole::counter_nonburrowing
- [`int countdown_for_appearing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=countdown_for_appearing) &Mole::countdown_for_appearing
- [`int digging_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=digging_state) &Mole::digging_state
\
0 - non_burrowed, 1 - unknown, 2 - burrowed, 3 - state_change
### `Spider`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float ceiling_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_x) &Spider::ceiling_pos_x
- [`float ceiling_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_y) &Spider::ceiling_pos_y
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Spider::jump_timer
- [`float trigger_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=trigger_distance) &Spider::trigger_distance
\
only in the x coord
### `HangSpider`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int dangle_jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dangle_jump_timer) &HangSpider::dangle_jump_timer
- [`float ceiling_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_x) &HangSpider::ceiling_pos_x
- [`float ceiling_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling_pos_y) &HangSpider::ceiling_pos_y
### `Shopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`RoomOwner`](#roomowner)
- [`int name`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=name) &Shopkeeper::name
\
0 - Ali, 1 - Bob, 2 - Comso ... and so one, anything above 28 is just random string, can crash the game
- [`int shotgun_attack_delay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shotgun_attack_delay) &Shopkeeper::shotgun_attack_delay
\
can't shot when the timer is running
- [`bool shop_owner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shop_owner) &Shopkeeper::shop_owner
### `Yang`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`RoomOwner`](#roomowner)
- [`set<int> turkeys_in_den`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=turkeys_in_den) &Yang::turkeys_in_den
\
Table of uid's of the turkeys, goes only up to 3, is nil when yang is angry
- [`bool first_message_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=first_message_shown) &Yang::first_message_shown
\
I'm looking for turkeys, wanna help?
- [`bool quest_incomplete`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quest_incomplete) &Yang::quest_incomplete
\
Is set to false when the quest is over (Yang dead or second turkey delivered)
- [`bool special_message_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=special_message_shown) &Yang::special_message_shown
\
Tusk palace/black market/one way door - message shown
### `Tun`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`RoomOwner`](#roomowner)
- [`int arrows_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrows_left) &Tun::arrows_left
- [`int reload_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=reload_timer) &Tun::reload_timer
\
when 0, a new arrow is loaded into the bow; resets when she finds an arrow on the ground
- [`bool challenge_fee_paid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=challenge_fee_paid) &Tun::challenge_fee_paid
\
affect only the speech bubble
- [`bool congrats_challenge`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=congrats_challenge) &Tun::congrats_challenge
\
congrats message shown after exiting a challenge
- [`bool murdered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=murdered) &Tun::murdered
- [`bool shop_entered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shop_entered) &Tun::shop_entered
- [`bool tiamat_encounter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tiamat_encounter) &Tun::tiamat_encounter
\
if set to false, greets you with 'you've done well to reach this place'
### `Pet`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &Pet::fx_button
- [`int petting_by_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=petting_by_uid) &Pet::petting_by_uid
\
person whos petting it, only in the camp
- [`int yell_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=yell_counter) &Pet::yell_counter
\
counts up to 400 (6.6 sec), when 0 the pet yells out
- [`int func_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=func_timer) &Pet::func_timer
\
used when free running in the camp
- [`int active_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=active_state) &Pet::active_state
\
-1 = sitting and yelling, 0 = either running, dead or picked up
### `Caveman`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`int wake_up_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wake_up_timer) &Caveman::wake_up_timer
- [`int can_pick_up_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_pick_up_timer) &Caveman::can_pick_up_timer
\
0 = can pick something up, when holding forced to 179, after tripping and regaining consciousness counts down to 0
- [`int aggro_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro_timer) &Caveman::aggro_timer
\
keeps resetting when angry and a player is nearby
### `CavemanShopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`bool tripping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tripping) &CavemanShopkeeper::tripping
- [`bool shop_entered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shop_entered) &CavemanShopkeeper::shop_entered
### `HornedLizard`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int eaten_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=eaten_uid) &HornedLizard::eaten_uid
\
dungbeetle being eaten
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &HornedLizard::walk_pause_timer
\
alternates between walking and pausing when timer reaches zero
- [`int attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &HornedLizard::attack_cooldown_timer
\
won't attack until timer reaches zero
- [`int blood_squirt_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blood_squirt_timer) &HornedLizard::blood_squirt_timer
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &HornedLizard::particle
### `Mosquito`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float direction_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=direction_x) &Mosquito::direction_x
- [`float direction_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=direction_y) &Mosquito::direction_y
- [`float stuck_rel_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stuck_rel_pos_x) &Mosquito::stuck_rel_pos_x
- [`float stuck_rel_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stuck_rel_pos_y) &Mosquito::stuck_rel_pos_y
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Mosquito::timer
### `Mantrap`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Mantrap::walk_pause_timer
\
alternates between walking and pausing every time it reaches zero
- [`int eaten_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=eaten_uid) &Mantrap::eaten_uid
\
the uid of the entity the mantrap has eaten, in case it can break out, like a shopkeeper
### `Skeleton`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int explosion_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=explosion_timer) &Skeleton::explosion_timer
### `Scarab`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Scarab::emitted_light
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Scarab::timer
\
how long to stay in current position
### `Imp`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int carrying_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carrying_uid) &Imp::carrying_uid
- [`float patrol_y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=patrol_y_level) &Imp::patrol_y_level
### `Lavamander`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Lavamander::emitted_light
- [`int shoot_lava_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shoot_lava_timer) &Lavamander::shoot_lava_timer
\
when this timer reaches zero, it appears on the surface/shoots lava, triggers on player proximity
- [`int jump_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_pause_timer) &Lavamander::jump_pause_timer
- [`int lava_detection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lava_detection_timer) &Lavamander::lava_detection_timer
- [`bool is_hot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_hot) &Lavamander::is_hot
- [`int player_detect_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_detect_state) &Lavamander::player_detect_state
\
0 - didnt_saw_player, 1 - saw_player, 2 - spited_lava | probably used so he won't spit imminently after seeing the player
### `Firebug`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int fire_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fire_timer) &Firebug::fire_timer
- [`bool going_up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=going_up) &Firebug::going_up
- [`bool detached_from_chain`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=detached_from_chain) &Firebug::detached_from_chain
### `FirebugUnchained`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float max_flight_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=max_flight_height) &FirebugUnchained::max_flight_height
- [`int ai_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_timer) &FirebugUnchained::ai_timer
- [`int walking_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking_timer) &FirebugUnchained::walking_timer
### `Robot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`Illumination emitted_light_explosion`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light_explosion) &Robot::emitted_light_explosion
### `Quillback`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Quillback::particle
### `Leprechaun`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`int hump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hump_timer) &Leprechaun::hump_timer
- [`int target_in_sight_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_in_sight_timer) &Leprechaun::target_in_sight_timer
- [`int gold`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=gold) &Leprechaun::gold
\
amount of gold he picked up, will be drooped on death
- [`int timer_after_humping`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer_after_humping) &Leprechaun::timer_after_humping
### `Crocman`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`int teleport_cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teleport_cooldown) &Crocman::teleport_cooldown
### `Mummy`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Mummy::walk_pause_timer
### `VanHorsing`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`NPC`](#npc)
- [`bool show_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=show_text) &VanHorsing::show_text
\
if set to true, he will say 'i've been hunting this fiend a long time!' when on screen
### `WitchDoctor`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`int skull_regen_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=skull_regen_timer) &WitchDoctor::skull_regen_timer
### `WitchDoctorSkull`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int witch_doctor_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=witch_doctor_uid) &WitchDoctorSkull::witch_doctor_uid
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &WitchDoctorSkull::emitted_light
- [`float rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &WitchDoctorSkull::rotation_angle
### `ForestSister`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`NPC`](#npc)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &ForestSister::walk_pause_timer
### `Vampire`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float jump_trigger_distance_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_trigger_distance_x) &Vampire::jump_trigger_distance_x
- [`float jump_trigger_distance_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_trigger_distance_y) &Vampire::jump_trigger_distance_y
- [`float sleep_pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sleep_pos_x) &Vampire::sleep_pos_x
- [`float sleep_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sleep_pos_y) &Vampire::sleep_pos_y
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Vampire::walk_pause_timer
### `Vlad`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Vampire`](#vampire)
- [`int teleport_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teleport_timer) &Vlad::teleport_timer
\
triggers when Vlad teleports, when timer running he can't teleport and will stun when hit
- [`bool aggro`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=aggro) &Vlad::aggro
\
or is awake
### `Waddler`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`RoomOwner`](#roomowner)
- [`bool player_detected`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_detected) &Waddler::player_detected
- [`bool on_the_ground`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_the_ground) &Waddler::on_the_ground
- [`int air_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=air_timer) &Waddler::air_timer
### `Octopus`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
### `Bodyguard`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`NPC`](#npc)
- [`int position_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=position_state) &Bodyguard::position_state
\
0 - none, 1 - Tusk dice shop, 2 - Entrence to pleasure palace, 3 - Basement entrance to pleasure palace
- [`bool message_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=message_shown) &Bodyguard::message_shown
### `Fish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &Fish::change_direction_timer
### `GiantFish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &GiantFish::change_direction_timer
\
when bouncing into a wall
- [`int lose_interest_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lose_interest_timer) &GiantFish::lose_interest_timer
\
delay in-between attacks
### `Crabman`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Crabman::walk_pause_timer
- [`int invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_timer) &Crabman::invincibility_timer
- [`int poison_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison_attack_timer) &Crabman::poison_attack_timer
- [`int attacking_claw_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attacking_claw_uid) &Crabman::attacking_claw_uid
- [`bool at_maximum_attack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=at_maximum_attack) &Crabman::at_maximum_attack
### `Kingu`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float climb_direction_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_direction_x) &Kingu::climb_direction_x
- [`float climb_direction_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_direction_y) &Kingu::climb_direction_y
- [`int climb_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=climb_pause_timer) &Kingu::climb_pause_timer
- [`int shell_invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shell_invincibility_timer) &Kingu::shell_invincibility_timer
- [`int monster_spawn_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=monster_spawn_timer) &Kingu::monster_spawn_timer
- [`int initial_shell_health`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=initial_shell_health) &Kingu::initial_shell_health
\
excalibur wipes out immediately, bombs take off 11 points, when 0 vulnerable to whip
- [`bool player_seen_by_kingu`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_seen_by_kingu) &Kingu::player_seen_by_kingu
### `Anubis`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &Anubis::spawn_x
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &Anubis::spawn_y
- [`float attack_proximity_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_proximity_y) &Anubis::attack_proximity_y
- [`float attack_proximity_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_proximity_x) &Anubis::attack_proximity_x
- [`int ai_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_timer) &Anubis::ai_timer
- [`int next_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_attack_timer) &Anubis::next_attack_timer
- [`int psychic_orbs_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=psychic_orbs_counter) &Anubis::psychic_orbs_counter
- [`bool awake`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=awake) &Anubis::awake
### `Cobra`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int spit_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spit_timer) &Cobra::spit_timer
### `CatMummy`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int ai_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ai_state) &CatMummy::ai_state
- [`int attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &CatMummy::attack_timer
### `Sorceress`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`int inbetween_attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inbetween_attack_timer) &Sorceress::inbetween_attack_timer
- [`float in_air_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_air_timer) &Sorceress::in_air_timer
- [`Illumination halo_emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=halo_emitted_light) &Sorceress::halo_emitted_light
- [`Entity fx_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_entity) &Sorceress::fx_entity
- [`int hover_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hover_timer) &Sorceress::hover_timer
### `MagmaMan`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &MagmaMan::emitted_light
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &MagmaMan::particle
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &MagmaMan::jump_timer
- [`int alive_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=alive_timer) &MagmaMan::alive_timer
### `Bee`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`bool can_rest`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_rest) &Bee::can_rest
- [`int fly_hang_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fly_hang_timer) &Bee::fly_hang_timer
- [`int targeting_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeting_timer) &Bee::targeting_timer
- [`float wobble_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wobble_x) &Bee::wobble_x
- [`float wobble_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wobble_y) &Bee::wobble_y
### `Ammit`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Ammit::walk_pause_timer
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Ammit::particle
### `ApepPart`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float y_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_pos) &ApepPart::y_pos
- [`float sine_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_angle) &ApepPart::sine_angle
- [`int sync_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sync_timer) &ApepPart::sync_timer
\
or pause timer, used to sync the body parts moving up and down
### `ApepHead`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`ApepPart`](#apeppart)
- [`float distance_traveled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance_traveled) &ApepHead::distance_traveled
- [`int tail_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_uid) &ApepHead::tail_uid
- [`int fx_mouthpiece1_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_mouthpiece1_uid) &ApepHead::fx_mouthpiece1_uid
- [`int fx_mouthpiece2_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_mouthpiece2_uid) &ApepHead::fx_mouthpiece2_uid
### `OsirisHead`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int right_hand_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_hand_uid) &OsirisHead::right_hand_uid
\
right from his perspective
- [`int left_hand_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_hand_uid) &OsirisHead::left_hand_uid
- [`bool moving_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=moving_left) &OsirisHead::moving_left
- [`int targeting_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeting_timer) &OsirisHead::targeting_timer
- [`int invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_timer) &OsirisHead::invincibility_timer
### `OsirisHand`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &OsirisHand::attack_cooldown_timer
### `Alien`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Alien::jump_timer
### `UFO`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int patrol_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=patrol_distance) &UFO::patrol_distance
- [`int attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &UFO::attack_cooldown_timer
- [`bool is_falling`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_falling) &UFO::is_falling
### `Lahamu`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &Lahamu::attack_cooldown_timer
### `YetiQueen`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &YetiQueen::walk_pause_timer
### `YetiKing`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &YetiKing::walk_pause_timer
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &YetiKing::emitted_light
- [`ParticleEmitterInfo particle_fog`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_fog) &YetiKing::particle_fog
- [`ParticleEmitterInfo particle_dust`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_dust) &YetiKing::particle_dust
- [`ParticleEmitterInfo particle_sparkles`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_sparkles) &YetiKing::particle_sparkles
### `Lamassu`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Entity attack_effect_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_effect_entity) &Lamassu::attack_effect_entity
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Lamassu::particle
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Lamassu::emitted_light
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Lamassu::walk_pause_timer
- [`int flight_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flight_timer) &Lamassu::flight_timer
- [`int attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &Lamassu::attack_timer
- [`float attack_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_angle) &Lamassu::attack_angle
### `Olmite`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`bool armor_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=armor_on) &Olmite::armor_on
- [`bool in_stack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_stack) &Olmite::in_stack
\
disables the attack, stun, lock's looking left flag between stack
- [`bool in_stack2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_stack2) &Olmite::in_stack2
\
is set to false couple frame after being detached from stack
- [`int on_top_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=on_top_uid) &Olmite::on_top_uid
- [`float y_offset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_offset) &Olmite::y_offset
- [`int attack_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_cooldown_timer) &Olmite::attack_cooldown_timer
### `Tiamat`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int fx_tiamat_head`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_tiamat_head) &Tiamat::fx_tiamat_head_uid
- [`int fx_tiamat_arm_right1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_tiamat_arm_right1) &Tiamat::fx_tiamat_arm_right1_uid
- [`int fx_tiamat_arm_right2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_tiamat_arm_right2) &Tiamat::fx_tiamat_arm_right2_uid
- [`int frown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frown_timer) &Tiamat::invincibility_timer
- [`int damage_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=damage_timer) &Tiamat::damage_timer
- [`int attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &Tiamat::attack_timer
- [`float tail_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_angle) &Tiamat::tail_angle
- [`float tail_radian`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_radian) &Tiamat::tail_radian
- [`float tail_move_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_move_speed) &Tiamat::tail_move_speed
- [`float right_arm_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_arm_angle) &Tiamat::right_arm_angle
### `GiantFrog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Entity door_front_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door_front_layer) &GiantFrog::door_front_layer
- [`Entity door_back_layer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door_back_layer) &GiantFrog::door_back_layer
- [`Entity platform`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=platform) &GiantFrog::platform
- [`int attack_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_timer) &GiantFrog::attack_timer
- [`int frogs_ejected_in_cycle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frogs_ejected_in_cycle) &GiantFrog::frogs_ejected_in_cycle
- [`int invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_timer) &GiantFrog::invincibility_timer
- [`int mouth_close_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mouth_close_timer) &GiantFrog::mouth_close_timer
- [`bool mouth_open_trigger`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mouth_open_trigger) &GiantFrog::mouth_open_trigger
\
opens the mouth and starts mouth_close_timer, used when detecting grub in the mouth area
### `Frog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int grub_being_eaten_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grub_being_eaten_uid) &Frog::grub_being_eaten_uid
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Frog::jump_timer
- [`bool pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause) &Frog::pause
### `FireFrog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Frog`](#frog)
### `Grub`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float rotation_delta`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_delta) &Grub::rotation_delta
- [`bool drop`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=drop) &Grub::drop
- [`int looking_for_new_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=looking_for_new_direction_timer) &Grub::looking_for_new_direction_timer
\
used when he touches floor/wall/ceiling
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Grub::walk_pause_timer
- [`int turn_into_fly_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=turn_into_fly_timer) &Grub::turn_into_fly_timer
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Grub::particle
### `Tadpole`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int acceleration_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=acceleration_timer) &Tadpole::acceleration_timer
- [`bool player_spotted`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_spotted) &Tadpole::player_spotted
### `GiantFly`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Entity head_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=head_entity) &GiantFly::head_entity
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &GiantFly::particle
- [`float sine_amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_amplitude) &GiantFly::sine_amplitude
- [`float sine_frequency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_frequency) &GiantFly::sine_frequency
- [`float delta_y_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=delta_y_angle) &GiantFly::delta_y_angle
- [`int sine_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_counter) &GiantFly::sine_counter
### `Ghist`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int body_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=body_uid) &Ghist::body_uid
- [`int idle_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=idle_timer) &Ghist::idle_timer
- [`int transparency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=transparency) &Ghist::transparency
- [`int fadeout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fadeout) &Ghist::fadeout
### `JumpDog`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &JumpDog::walk_pause_timer
- [`int squish_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=squish_timer) &JumpDog::squish_timer
### `EggplantMinister`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &EggplantMinister::walk_pause_timer
- [`int squish_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=squish_timer) &EggplantMinister::squish_timer
### `Yama`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`bool message_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=message_shown) &Yama::message_shown
### `Hundun`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float applied_hor_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_hor_velocity) &Hundun::applied_hor_velocity
- [`float applied_ver_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_ver_velocity) &Hundun::applied_ver_velocity
- [`int birdhead_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=birdhead_entity_uid) &Hundun::birdhead_entity_uid
- [`int snakehead_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=snakehead_entity_uid) &Hundun::snakehead_entity_uid
- [`float y_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_level) &Hundun::y_level
- [`int bounce_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bounce_timer) &Hundun::bounce_timer
- [`int fireball_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fireball_timer) &Hundun::fireball_timer
- [`bool birdhead_defeated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=birdhead_defeated) &Hundun::birdhead_defeated
- [`bool snakehead_defeated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=snakehead_defeated) &Hundun::snakehead_defeated
- [`int hundun_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hundun_flags) &Hundun::hundun_flags
\
1:  Will move to the left, 2: Birdhead emerged, 3: Snakehead emerged, 4: Top level arena reached, 5: Birdhead shot last - to alternate the heads shooting fireballs
### `HundunHead`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`float attack_position_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_position_x) &HundunHead::attack_position_x
- [`float attack_position_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack_position_y) &HundunHead::attack_position_y
- [`int egg_crack_effect_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=egg_crack_effect_uid) &HundunHead::egg_crack_effect_uid
\
Posiotion where the head will move on attack
- [`int targeted_player_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=targeted_player_uid) &HundunHead::targeted_player_uid
- [`int looking_for_target_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=looking_for_target_timer) &HundunHead::looking_for_target_timer
\
also cooldown before attack
- [`int invincibility_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invincibility_timer) &HundunHead::invincibility_timer
### `MegaJellyfish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Entity flipper1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipper1) &MegaJellyfish::flipper1
- [`Entity flipper2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipper2) &MegaJellyfish::flipper2
- [`int orb_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=orb_uid) &MegaJellyfish::orb_uid
\
the closest orb, does not gets updated
- [`int tail_bg_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tail_bg_uid) &MegaJellyfish::tail_bg_uid
- [`float applied_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_velocity) &MegaJellyfish::applied_velocity
- [`float wagging_tail_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wagging_tail_counter) &MegaJellyfish::wagging_tail_counter
- [`int flipper_distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipper_distance) &MegaJellyfish::flipper_distance
\
only applies to door-blocking one
- [`int velocity_application_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_application_timer) &MegaJellyfish::velocity_application_timer
### `Scorpion`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Scorpion::walk_pause_timer
- [`int jump_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_cooldown_timer) &Scorpion::jump_cooldown_timer
### `Hermitcrab`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`ENT_TYPE carried_entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carried_entity_type) &Hermitcrab::carried_entity_type
- [`int carried_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carried_entity_uid) &Hermitcrab::carried_entity_uid
- [`int walk_spit_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_spit_timer) &Hermitcrab::walk_spit_timer
- [`bool is_active`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_active) &Hermitcrab::is_active
\
whether it is hidden behind the carried block or not, if true you can damage him
- [`bool is_inactive`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_inactive) &Hermitcrab::is_inactive
- [`bool spawn_new_carried_item`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_new_carried_item) &Hermitcrab::spawn_new_carried_item
\
defaults to true, when toggled to false, a new carried item spawns
### `Necromancer`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`WalkingMonster`](#walkingmonster)
- [`float red_skeleton_spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red_skeleton_spawn_x) &Necromancer::red_skeleton_spawn_x
- [`float red_skeleton_spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red_skeleton_spawn_y) &Necromancer::red_skeleton_spawn_y
- [`int resurrection_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=resurrection_uid) &Necromancer::resurrection_uid
- [`int resurrection_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=resurrection_timer) &Necromancer::resurrection_timer
### `ProtoShopkeeper`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int movement_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=movement_state) &ProtoShopkeeper::movement_state
- [`int walk_pause_explode_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_explode_timer) &ProtoShopkeeper::walk_pause_explode_timer
- [`int walking_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking_speed) &ProtoShopkeeper::walking_speed
\
0 = slow, 4 = fast
### `Beg`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`NPC`](#npc)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &Beg::walk_pause_timer
- [`int disappear_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=disappear_timer) &Beg::disappear_timer
### `Terra`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &Terra::fx_button
- [`float x_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x_pos) &Terra::x_pos
- [`int abuse_speechbubble_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=abuse_speechbubble_timer) &Terra::abuse_speechbubble_timer
### `Critter`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster)
- [`int last_picked_up_by_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_picked_up_by_uid) &Critter::last_picked_up_by_uid
- [`int holding_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=holding_state) &Critter::holding_state
### `CritterBeetle`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`bool pause`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause) &CritterBeetle::pause
\
used when he's getting eaten
### `CritterCrab`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &CritterCrab::walk_pause_timer
- [`bool walking_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walking_left) &CritterCrab::walking_left
### `CritterButterfly`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`int change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &CritterButterfly::change_direction_timer
- [`int vertical_flight_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_flight_direction) &CritterButterfly::vertical_flight_direction
### `CritterLocust`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &CritterLocust::jump_timer
### `CritterSnail`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`float x_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x_direction) &CritterSnail::x_direction
- [`float y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_direction) &CritterSnail::y_direction
- [`float pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_x) &CritterSnail::pos_x
- [`float pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_y) &CritterSnail::pos_y
- [`float rotation_center_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_x) &CritterSnail::rotation_center_x
- [`float rotation_center_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_y) &CritterSnail::rotation_center_y
- [`float rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &CritterSnail::rotation_angle
- [`float rotation_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_speed) &CritterSnail::rotation_speed
### `CritterFish`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`int swim_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=swim_pause_timer) &CritterFish::swim_pause_timer
- [`bool player_in_proximity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_in_proximity) &CritterFish::player_in_proximity
### `CritterPenguin`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &CritterPenguin::walk_pause_timer
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &CritterPenguin::jump_timer
### `CritterFirefly`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`float sine_amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_amplitude) &CritterFirefly::sine_amplitude
- [`float sine_frequency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_frequency) &CritterFirefly::sine_frequency
- [`float sine_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sine_angle) &CritterFirefly::sine_angle
- [`int change_direction_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_direction_timer) &CritterFirefly::change_direction_timer
- [`int sit_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sit_timer) &CritterFirefly::sit_timer
- [`int sit_cooldown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sit_cooldown_timer) &CritterFirefly::sit_cooldown_timer
### `CritterDrone`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &CritterDrone::emitted_light
- [`float applied_hor_momentum`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_hor_momentum) &CritterDrone::applied_hor_momentum
- [`float applied_ver_momentum`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=applied_ver_momentum) &CritterDrone::applied_ver_momentum
- [`int move_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_timer) &CritterDrone::move_timer
### `CritterSlime`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`PowerupCapable`](#powerupcapable) [`Monster`](#monster) [`Critter`](#critter)
- [`float x_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x_direction) &CritterSlime::x_direction
- [`float y_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_direction) &CritterSlime::y_direction
- [`float pos_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_x) &CritterSlime::pos_x
- [`float pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pos_y) &CritterSlime::pos_y
- [`float rotation_center_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_x) &CritterSlime::rotation_center_x
- [`float rotation_center_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_y) &CritterSlime::rotation_center_y
- [`float rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &CritterSlime::rotation_angle
- [`float rotation_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_speed) &CritterSlime::rotation_speed
- [`int walk_pause_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_pause_timer) &CritterSlime::walk_pause_timer
### `Bomb`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float scale_hor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_hor) &Bomb::scale_hor
\
1.25 = default regular bomb, 1.875 = default giant bomb, > 1.25 generates ENT_TYPE_FX_POWEREDEXPLOSION
- [`float scale_ver`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_ver) &Bomb::scale_ver
- [`bool is_big_bomb`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_big_bomb) &Bomb::is_big_bomb
\
is bomb from powerpack
### `Backpack`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool explosion_trigger`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=explosion_trigger) &Backpack::explosion_trigger
\
More like on fire trigger, the explosion happens when the timer reaches > 29
- [`int explosion_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=explosion_timer) &Backpack::explosion_timer
### `Jetpack`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack)
- [`bool flame_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_on) &Jetpack::flame_on
- [`int fuel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fuel) &Jetpack::fuel
### `TeleporterBackpack`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack)
- [`int teleport_number`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teleport_number) &TeleporterBackpack::teleport_number
### `Hoverpack`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack)
- [`bool is_on`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_on) &Hoverpack::is_on
### `Cape`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack)
- [`bool floating_down`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=floating_down) &Cape::floating_down
### `VladsCape`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack) [`Cape`](#cape)
- [`bool can_double_jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=can_double_jump) &VladsCape::can_double_jump
### `Mattock`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int remaining`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=remaining) &Mattock::remaining
### `Gun`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int cooldown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cooldown) &Gun::cooldown
- [`int shots`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shots) &Gun::shots
\
used only for webgun
- [`int shots2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shots2) &Gun::shots2
\
used only for clonegun
- [`int in_chamber`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_chamber) &Gun::in_chamber
\
Only for webgun, uid of the webshot entity
### `Flame`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Flame::emitted_light
### `FlameSize`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Flame`](#flame)
- [`float flame_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_size) &FlameSize::flame_size
\
if changed, gradually goes down |0.03 per frame| to the default size
### `ClimbableRope`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int segment_nr_inverse`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=segment_nr_inverse) &ClimbableRope::segment_nr_inverse
- [`int burn_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=burn_timer) &ClimbableRope::burn_timer
\
entity is killed after 20
- [`Entity above_part`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=above_part) &ClimbableRope::above_part
- [`Entity below_part`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=below_part) &ClimbableRope::below_part
- [`int segment_nr`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=segment_nr) &ClimbableRope::segment_nr
### `Idol`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool trap_triggered`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=trap_triggered) &Idol::trap_triggered
\
if you set it to true for the ice caves or volcano idol, the trap won't trigger
- [`int touch`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=touch) &Idol::touch
\
changes to 0 when first picked up by player and back to -1 if HH picks it up
- [`float spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &Idol::spawn_x
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &Idol::spawn_y
### `Spear`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int sound_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sound_id) &Spear::sound_id
### `JungleSpearCosmetic`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float move_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_x) &JungleSpearCosmetic::move_x
- [`float move_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_y) &JungleSpearCosmetic::move_y
### `WebShot`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool shot`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shot) &WebShot::shot
\
if false, it's attached to the gun
### `HangStrand`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float start_pos_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_pos_y) &HangStrand::start_pos_y
### `HangAnchor`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int spider_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spider_uid) &HangAnchor::spider_uid
### `Arrow`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int flame_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_uid) &Arrow::flame_uid
- [`bool is_on_fire`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_on_fire) &Arrow::is_on_fire
- [`bool is_poisoned`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_poisoned) &Arrow::is_poisoned
- [`bool shot_from_trap`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shot_from_trap) &Arrow::shot_from_trap
### `LightArrow`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Arrow`](#arrow)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &LightArrow::emitted_light
### `LightShot`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &LightShot::emitted_light
### `LightEmitter`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &LightEmitter::emitted_light
### `ScepterShot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightEmitter`](#lightemitter)
- [`float speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speed) &ScepterShot::speed
- [`int idle_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=idle_timer) &ScepterShot::idle_timer
\
short timer before it goes after target
### `SpecialShot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightEmitter`](#lightemitter)
- [`float target_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_x) &SpecialShot::target_x
- [`float target_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=target_y) &SpecialShot::target_y
### `SoundShot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightShot`](#lightshot)
### `Spark`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Flame`](#flame)
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Spark::particle
- [`Entity fx_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_entity) &Spark::fx_entity
- [`float rotation_center_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_x) &Spark::rotation_center_x
- [`float rotation_center_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_center_y) &Spark::rotation_center_y
- [`float rotation_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_angle) &Spark::rotation_angle
- [`float size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=size) &Spark::size
\
slowly goes down to default 1.0, is 0.0 when not on screen
- [`float size_multiply`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=size_multiply) &Spark::size_multiply
\
0.0 when not on screen
- [`float next_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_size) &Spark::next_size
\
width and height will be set to this value  size_multiply next frame
- [`int size_change_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=size_change_timer) &Spark::size_change_timer
\
very short timer before next size change, giving a pulsing effect
### `TiamatShot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightEmitter`](#lightemitter)
### `Fireball`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightShot`](#lightshot) [`SoundShot`](#soundshot)
- [`ParticleEmitterInfo particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle) &Fireball::particle
### `Leaf`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float fade_away_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fade_away_counter) &Leaf::fade_away_counter
\
counts to 100.0 then the leaf fades away
- [`int swing_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=swing_direction) &Leaf::swing_direction
- [`bool fade_away_trigger`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fade_away_trigger) &Leaf::fade_away_trigger
### `AcidBubble`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float speed_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speed_x) &AcidBubble::speed_x
- [`float speed_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speed_y) &AcidBubble::speed_y
- [`float float_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=float_counter) &AcidBubble::float_counter
### `Claw`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int crabman_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=crabman_uid) &Claw::crabman_uid
- [`float spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &Claw::spawn_x
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &Claw::spawn_y
### `StretchChain`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int at_end_of_chain_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=at_end_of_chain_uid) &StretchChain::at_end_of_chain_uid
- [`float dot_offset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dot_offset) &StretchChain::dot_offset
- [`int position_in_chain`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=position_in_chain) &StretchChain::position_in_chain
- [`int inverse_doubled_position_in_chain`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inverse_doubled_position_in_chain) &StretchChain::inverse_doubled_position_in_chain
- [`bool is_dot_hidden`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_dot_hidden) &StretchChain::is_dot_hidden
### `Chest`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool leprechaun`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=leprechaun) &Chest::leprechaun
- [`bool bomb`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bomb) &Chest::bomb
\
size of the bomb is random, if set both true only leprechaun spawns
### `Treasure`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool cashed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cashed) &Treasure::cashed
\
spawns a dust effect and adds money for the total
### `HundunChest`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Treasure`](#treasure)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &HundunChest::timer
### `Boombox`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &Boombox::fx_button
- [`ParticleEmitterInfo music_note1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=music_note1) &Boombox::music_note1
- [`ParticleEmitterInfo music_note2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=music_note2) &Boombox::music_note2
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &Boombox::spawn_y
- [`int station`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=station) &Boombox::station
- [`int station_change_delay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=station_change_delay) &Boombox::station_change_delay
- [`int jump_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_timer) &Boombox::jump_timer
- [`int jump_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump_state) &Boombox::jump_state
### `TV`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &TV::fx_button
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &TV::emitted_light
- [`int station`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=station) &TV::station
### `Telescope`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &Telescope::fx_button
- [`Entity camera_anchor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=camera_anchor) &Telescope::camera_anchor
- [`int looked_through_by_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=looked_through_by_uid) &Telescope::looked_through_by_uid
### `Torch`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int flame_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_uid) &Torch::flame_uid
- [`bool is_lit`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_lit) &Torch::is_lit
### `WallTorch`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Torch`](#torch)
- [`bool dropped_gold`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dropped_gold) &WallTorch::dropped_gold
\
if false, it will drop gold when light up
### `TorchFlame`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Flame`](#flame)
- [`ParticleEmitterInfo smoke_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=smoke_particle) &TorchFlame::smoke_particle
- [`ParticleEmitterInfo flame_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_particle) &TorchFlame::flame_particle
- [`ParticleEmitterInfo warp_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=warp_particle) &TorchFlame::warp_particle
- [`float flame_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_size) &TorchFlame::flame_size
### `LampFlame`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Flame`](#flame)
- [`ParticleEmitterInfo flame_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flame_particle) &LampFlame::flame_particle
### `Bullet`
Derived from [`Entity`](#entity) [`Movable`](#movable)
### `TimedShot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightShot`](#lightshot)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &TimedShot::timer
### `CloneGunShot`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightShot`](#lightshot)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &CloneGunShot::timer
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &CloneGunShot::spawn_y
### `PunishBall`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int attached_to_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attached_to_uid) &PunishBall::attached_to_uid
- [`float x_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x_pos) &PunishBall::x_pos
- [`float y_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y_pos) &PunishBall::y_pos
### `Chain`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int attached_to_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attached_to_uid) &Chain::attached_to_uid
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Chain::timer
### `Container`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ENT_TYPE inside`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inside) &Container::inside
### `Coffin`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ENT_TYPE inside`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inside) &Coffin::inside
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Coffin::timer
### `Fly`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Fly::timer
### `OlmecCannon`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &OlmecCannon::timer
- [`int bombs_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombs_left) &OlmecCannon::bombs_left
### `Landmine`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightEmitter`](#lightemitter)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Landmine::timer
\
explodes at 57, if you set it to 58 will count to overflow
### `UdjatSocket`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &UdjatSocket::fx_button
### `Ushabti`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int wiggle_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wiggle_timer) &Ushabti::wiggle_timer
- [`int shine_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shine_timer) &Ushabti::shine_timer
### `Honey`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int wiggle_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wiggle_timer) &Honey::wiggle_timer
### `GiantClamTop`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int close_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=close_timer) &GiantClamTop::close_timer
- [`int open_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=open_timer) &GiantClamTop::open_timer
### `PlayerGhost`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`LightEmitter`](#lightemitter)
- [`ParticleEmitterInfo sparkles_particle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sparkles_particle) &PlayerGhost::sparkles_particle
- [`PlayerInputs player_inputs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_inputs) &PlayerGhost::player_inputs
- [`Inventory inventory`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inventory) &PlayerGhost::inventory
- [`int shake_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake_timer) &PlayerGhost::shake_timer
- [`int boost_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=boost_timer) &PlayerGhost::boost_timer
### `GhostBreath`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &GhostBreath::timer
- [`bool big_cloud`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=big_cloud) &GhostBreath::big_cloud
### `LaserBeam`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ParticleEmitterInfo sparks`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sparks) &LaserBeam::sparks
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &LaserBeam::emitted_light
### `TreasureHook`
Derived from [`Entity`](#entity) [`Movable`](#movable)
### `AxolotlShot`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int trapped_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=trapped_uid) &AxolotlShot::trapped_uid
- [`float size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=size) &AxolotlShot::size
- [`float swing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=swing) &AxolotlShot::swing
- [`float swing_periodicity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=swing_periodicity) &AxolotlShot::swing_periodicity
- [`float distance_after_capture`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance_after_capture) &AxolotlShot::distance_after_capture
### `TrapPart`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity ceiling`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ceiling) &TrapPart::ceiling
### `SkullDropTrap`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int left_skull_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_skull_uid) &SkullDropTrap::left_skull_uid
- [`int middle_skull_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=middle_skull_uid) &SkullDropTrap::middle_skull_uid
- [`int right_skull_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_skull_uid) &SkullDropTrap::right_skull_uid
- [`int left_skull_drop_time`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_skull_drop_time) &SkullDropTrap::left_skull_drop_time
- [`int middle_skull_drop_time`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=middle_skull_drop_time) &SkullDropTrap::middle_skull_drop_time
- [`int right_skull_drop_time`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_skull_drop_time) &SkullDropTrap::right_skull_drop_time
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &SkullDropTrap::timer
\
counts from 60 3 times, the last time dropping the skulls, then random longer timer for reset
### `FrozenLiquid`
Derived from [`Entity`](#entity) [`Movable`](#movable)
### `Switch`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Switch::timer
### `FlyHead`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int vored_entity_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vored_entity_uid) &FlyHead::vored_entity_uid
### `SnapTrap`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int bait_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bait_uid) &SnapTrap::bait_uid
- [`int reload_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=reload_timer) &SnapTrap::reload_timer
### `EmpressGrave`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`Entity fx_button`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_button) &EmpressGrave::fx_button
- [`Entity ghost`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ghost) &EmpressGrave::ghost
### `Tentacle`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Chain`](#chain)
- [`Entity bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom) &Tentacle::bottom
### `MiniGameShip`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float velocity_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_x) &MiniGameShip::velocity_x
- [`float velocity_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_y) &MiniGameShip::velocity_y
- [`float swing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=swing) &MiniGameShip::swing
- [`float up_down_normal`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=up_down_normal) &MiniGameShip::up_down_normal
### `MiniGameAsteroid`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float spin_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spin_speed) &MiniGameAsteroid::spin_speed
### `Pot`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ENT_TYPE inside`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=inside) &Pot::inside
- [`bool dont_transfer_dmg`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dont_transfer_dmg) &Pot::dont_transfer_dmg
### `CursedPot`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ParticleEmitterInfo smoke`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=smoke) &CursedPot::smoke
- [`ParticleEmitterInfo smoke2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=smoke2) &CursedPot::smoke2
### `CookFire`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool lit`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lit) &CookFire::lit
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &CookFire::emitted_light
- [`ParticleEmitterInfo particles_smoke`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particles_smoke) &CookFire::particles_smoke
- [`ParticleEmitterInfo particles_flames`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particles_flames) &CookFire::particles_flames
- [`ParticleEmitterInfo particles_warp`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particles_warp) &CookFire::particles_warp
### `Orb`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Orb::timer
### `EggSac`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &EggSac::timer
### `Goldbar`
Derived from [`Entity`](#entity) [`Movable`](#movable)
### `Coin`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int nominal_price`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=nominal_price) &Coin::nominal_price
### `RollingItem`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float roll_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=roll_speed) &RollingItem::roll_speed
### `PlayerBag`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int bombs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bombs) &PlayerBag::bombs
- [`int ropes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ropes) &PlayerBag::ropes
### `Powerup`
Derived from [`Entity`](#entity) [`Movable`](#movable)
### `KapalaPowerup`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Powerup`](#powerup)
- [`int amount_of_blood`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=amount_of_blood) &KapalaPowerup::amount_of_blood
### `ParachutePowerup`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Powerup`](#powerup)
- [`int falltime_deploy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=falltime_deploy) &ParachutePowerup::falltime_deploy
\
this gets compared with entity's falling_timer
- [`bool deployed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deployed) &ParachutePowerup::deployed
- [`nil deploy()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deploy) &ParachutePowerup::deploy
### `TrueCrownPowerup`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Powerup`](#powerup)
- [`int timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer) &TrueCrownPowerup::timer
### `AnkhPowerup`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Powerup`](#powerup)
- [`Entity player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player) &AnkhPowerup::player
- [`Entity fx_glow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_glow) &AnkhPowerup::fx_glow
- [`int timer1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer1) &AnkhPowerup::timer1
- [`int timer2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer2) &AnkhPowerup::timer2
- [`int timer3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=timer3) &AnkhPowerup::timer3
- [`bool music_on_off`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=music_on_off) &AnkhPowerup::music_on_off
### `YellowCape`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Backpack`](#backpack) [`Cape`](#cape)
### `Teleporter`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`int teleport_number`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=teleport_number) &Teleporter::teleport_number
### `Boomerang`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`ParticleEmitterInfo trail`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=trail) &Boomerang::trail
- [`float distance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance) &Boomerang::distance
- [`float rotation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation) &Boomerang::rotation
- [`int returns_to_uid`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=returns_to_uid) &Boomerang::returns_to_uid
### `Excalibur`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool in_stone`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=in_stone) &Excalibur::in_stone
### `Shield`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float shake`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shake) &Shield::shake
### `LavaGlow`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`float glow_radius`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=glow_radius) &LavaGlow::glow_radius
### `OlmecFloater`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`bool both_floaters_intact`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=both_floaters_intact) &OlmecFloater::both_floaters_intact
### `Liquid`
Derived from [`Entity`](#entity)
- [`Entity fx_surface`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fx_surface) &Liquid::fx_surface
### `Lava`
Derived from [`Entity`](#entity) [`Liquid`](#liquid)
- [`Illumination emitted_light`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=emitted_light) &Lava::emitted_light
### `ParticleDB`
- [`int id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=id) &ParticleDB::id
- [`int spawn_count_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_count_min) &ParticleDB::spawn_count_min
- [`int spawn_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_count) &ParticleDB::spawn_count
- [`int lifespan_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lifespan_min) &ParticleDB::lifespan_min
- [`int lifespan`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lifespan) &ParticleDB::lifespan
- [`int sheet_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sheet_id) &ParticleDB::sheet_id
- [`int animation_sequence_length`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_sequence_length) &ParticleDB::animation_sequence_length
- [`float spawn_interval`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_interval) &ParticleDB::spawn_interval
- [`float shrink_growth_factor`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shrink_growth_factor) &ParticleDB::shrink_growth_factor
- [`float rotation_speed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rotation_speed) &ParticleDB::rotation_speed
- [`float opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=opacity) &ParticleDB::opacity
- [`float hor_scattering`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_scattering) &ParticleDB::hor_scattering
- [`float ver_scattering`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_scattering) &ParticleDB::ver_scattering
- [`float scale_x_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_x_min) &ParticleDB::scale_x_min
- [`float scale_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_x) &ParticleDB::scale_x
- [`float scale_y_min`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_y_min) &ParticleDB::scale_y_min
- [`float scale_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scale_y) &ParticleDB::scale_y
- [`float hor_deflection_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_deflection_1) &ParticleDB::hor_deflection_1
- [`float ver_deflection_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_deflection_1) &ParticleDB::ver_deflection_1
- [`float hor_deflection_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_deflection_2) &ParticleDB::hor_deflection_2
- [`float ver_deflection_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_deflection_2) &ParticleDB::ver_deflection_2
- [`float hor_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hor_velocity) &ParticleDB::hor_velocity
- [`float ver_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ver_velocity) &ParticleDB::ver_velocity
- [`int red`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=red) &ParticleDB::red
- [`int green`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=green) &ParticleDB::green
- [`int blue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blue) &ParticleDB::blue
- [`bool permanent`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=permanent) &ParticleDB::permanent
- [`bool invisible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=invisible) &ParticleDB::invisible
- [`int get_texture()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture) &ParticleDB::get_texture
- [`bool set_texture(int texture_id)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_texture) &ParticleDB::set_texture
### `ParticleEmitterInfo`
- [`ParticleDB particle_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_type) &ParticleEmitterInfo::particle_type
- [`int particle_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_count) &ParticleEmitterInfo::particle_count
### `PreLoadLevelFilesContext`
- [`nil override_level_files(array<string> levels)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=override_level_files) &PreLoadLevelFilesContext::override_level_files
\
Block all loading `.lvl` files and instead load the specified `.lvl` files. This includes `generic.lvl` so if you need it specify it here.
All `.lvl` files are loaded relative to `Data/Levels`, but they can be completely custom `.lvl` files that ship with your mod so long as they are in said folder.
Use at your own risk, some themes/levels expect a certain level file to be loaded.
- [`nil add_level_files(array<string> levels)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_level_files) &PreLoadLevelFilesContext::add_level_files
\
Load additional levels files other than the ones that would usually be loaded. Stacks with `override_level_files` if that was called first.
All `.lvl` files are loaded relative to `Data/Levels`, but they can be completely custom `.lvl` files that ship with your mod so long as they are in said folder.
### `DoorCoords`
- [`float door1_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door1_x) &DoorCoords::door1_x
- [`float door1_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door1_y) &DoorCoords::door1_y
- [`float door2_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door2_x) &DoorCoords::door2_x
- [`float door2_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door2_y) &DoorCoords::door2_y
### `LevelGenSystem`
- [`int shop_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shop_type) &LevelGenSystem::shop_type
- [`float spawn_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_x) &LevelGenSystem::spawn_x
- [`float spawn_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_y) &LevelGenSystem::spawn_y
- [`int spawn_room_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_room_x) &LevelGenSystem::spawn_room_x
- [`int spawn_room_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_room_y) &LevelGenSystem::spawn_room_y
- [`DoorCoords exits`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=exits) &LevelGenSystem::exit_doors_locations
### `PostRoomGenerationContext`
- [`bool set_room_template(int x, int y, LAYER layer, ROOM_TEMPLATE room_template)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_room_template) &PostRoomGenerationContext::set_room_template
\
Set the room template at the given index and layer, returns `false` if the index is outside of the level.
- [`bool mark_as_machine_room_origin(int x, int y, LAYER layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mark_as_machine_room_origin) &PostRoomGenerationContext::mark_as_machine_room_origin
\
Marks the room as the origin of a machine room, should be the top-left corner of the machine room
Run this after setting the room template for the room, otherwise the machine room will not spawn correctly
- [`bool mark_as_set_room(int x, int y, LAYER layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mark_as_set_room) &PostRoomGenerationContext::mark_as_set_room
\
Marks the room as a set-room, a corresponding `setroomy-x` template must be loaded, else the game will crash
- [`bool unmark_as_set_room(int x, int y, LAYER layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unmark_as_set_room) &PostRoomGenerationContext::unmark_as_set_room
\
Unmarks the room as a set-room
- [`bool set_procedural_spawn_chance(PROCEDURAL_CHANCE chance_id, int inverse_chance)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_procedural_spawn_chance) &PostRoomGenerationContext::set_procedural_spawn_chance
\
Force a spawn chance for this level, has the same restrictions as specifying the spawn chance in the .lvl file.
Note that the actual chance to spawn is `1/inverse_chance` and that is also slightly skewed because of technical reasons.
Returns `false` if the given chance is not defined.
- [`nil set_num_extra_spawns(int extra_spawn_id, int num_spawns_front_layer, int num_spawns_back_layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_num_extra_spawns) &PostRoomGenerationContext::set_num_extra_spawns
\
Change the amount of extra spawns for the given `extra_spawn_id`.
- [`optional<SHORT_TILE_CODE> define_short_tile_code(ShortTileCodeDef short_tile_code_def)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_short_tile_code) &PostRoomGenerationContext::define_short_tile_code
\
Defines a new short tile code, automatically picks an unused character or returns a used one in case of an exact match
Returns `nil` if all possible short tile codes are already in use
- [`nil change_short_tile_code(SHORT_TILE_CODE short_tile_code, ShortTileCodeDef short_tile_code_def)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_short_tile_code) &PostRoomGenerationContext::change_short_tile_code
\
Overrides a specific short tile code, this means it will change for the whole level
### `PreHandleRoomTilesContext`
- [`optional<SHORT_TILE_CODE> get_short_tile_code(int tx, int ty, LAYER layer)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_tile_code) &PreHandleRoomTilesContext::get_short_tile_code
\
Gets the tile code at the specified tile coordinate
Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH`, `0 <= ty < CONST.ROOM_HEIGHT` and `layer` in `{LAYER.FRONT, LAYER.BACK}`
Also returns `nil` if `layer == LAYER.BACK` and the room does not have a back layer
- [`bool set_short_tile_code(int tx, int ty, LAYER layer, SHORT_TILE_CODE short_tile_code)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_short_tile_code) &PreHandleRoomTilesContext::set_short_tile_code
\
Sets the tile code at the specified tile coordinate
Valid coordinates are `0 <= tx < CONST.ROOM_WIDTH`, `0 <= ty < CONST.ROOM_HEIGHT` and `layer` in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`
Also returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
- [`array<tuple<int, int, LAYER>> find_all_short_tile_codes(LAYER layer, SHORT_TILE_CODE short_tile_code)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=find_all_short_tile_codes) find_all_short_tile_codes
\
Finds all places a short tile code is used in the room, `layer` must be in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`
Returns an empty list if `layer == LAYER.BACK` and the room does not have a back layer
- [`bool replace_short_tile_code(LAYER layer, SHORT_TILE_CODE short_tile_code, SHORT_TILE_CODE replacement_short_tile_code)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=replace_short_tile_code) &PreHandleRoomTilesContext::replace_short_tile_code
\
Replaces all instances of `short_tile_code` in the given layer with `replacement_short_tile_code`, `layer` must be in `{LAYER.FRONT, LAYER.BACK, LAYER.BOTH}`
Returns `false` if `layer == LAYER.BACK` and the room does not have a back layer
- [`bool has_back_layer()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=has_back_layer) &PreHandleRoomTilesContext::has_back_layer
\
Check whether the room has a back layer
- [`nil add_empty_back_layer()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_empty_back_layer) &PreHandleRoomTilesContext::add_empty_back_layer
\
Add a back layer filled with all `0` if there is no back layer yet
Does nothing if there already is a backlayer
- [`nil add_copied_back_layer()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_copied_back_layer) &PreHandleRoomTilesContext::add_copied_back_layer
\
Add a back layer that is a copy of the front layer
Does nothing if there already is a backlayer
### `ShortTileCodeDef`
- [`TILE_CODE tile_code`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_code) &ShortTileCodeDef::tile_code
\
Tile code that is used by default when this short tile code is encountered. Defaults to 0.
- [`int chance`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=chance) &ShortTileCodeDef::chance
\
Chance in percent to pick `tile_code` over `alt_tile_code`, ignored if `chance == 0`. Defaults to 100.
- [`TILE_CODE alt_tile_code`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=alt_tile_code) &ShortTileCodeDef::alt_tile_code
\
Alternative tile code, ignored if `chance == 100`. Defaults to 0.
### `QuestsInfo`
- [`int yang_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=yang_state) &QuestsInfo::yang_state
- [`int jungle_sisters_flags`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jungle_sisters_flags) &QuestsInfo::jungle_sisters_flags
- [`int van_horsing_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=van_horsing_state) &QuestsInfo::van_horsing_state
- [`int sparrow_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sparrow_state) &QuestsInfo::sparrow_state
- [`int madame_tusk_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=madame_tusk_state) &QuestsInfo::madame_tusk_state
- [`int beg_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=beg_state) &QuestsInfo::beg_state
### `SaveData`
- [`array<bool, 16> places`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=places) &SaveData::places
- [`array<bool, 78> bestiary`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bestiary) &SaveData::bestiary
- [`array<bool, 38> people`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=people) &SaveData::people
- [`array<bool, 54> items`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=items) &SaveData::items
- [`array<bool, 24> traps`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=traps) &SaveData::traps
- [`last_daily`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=last_daily) &SaveData::last_daily
- [`int characters`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=characters) &SaveData::characters
- [`int shortcuts`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=shortcuts) &SaveData::shortcuts
- [`array<int, 78> bestiary_killed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bestiary_killed) &SaveData::bestiary_killed
- [`array<int, 78> bestiary_killed_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bestiary_killed_by) &SaveData::bestiary_killed_by
- [`array<int, 38> people_killed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=people_killed) &SaveData::people_killed
- [`array<int, 38> people_killed_by`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=people_killed_by) &SaveData::people_killed_by
- [`int plays`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=plays) &SaveData::plays
- [`int deaths`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deaths) &SaveData::deaths
- [`int wins_normal`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins_normal) &SaveData::wins_normal
- [`int wins_hard`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins_hard) &SaveData::wins_hard
- [`int wins_special`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=wins_special) &SaveData::wins_special
- [`int score_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=score_total) &SaveData::score_total
- [`int score_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=score_top) &SaveData::score_top
- [`int deepest_area`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deepest_area) &SaveData::deepest_area
- [`int deepest_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=deepest_level) &SaveData::deepest_level
- [`int time_best`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_best) &SaveData::time_best
- [`int time_total`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_total) &SaveData::time_total
- [`int time_tutorial`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_tutorial) &SaveData::time_tutorial
- [`array<int, 20> character_deaths`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_deaths) &SaveData::character_deaths
- [`array<int, 3> pets_rescued`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pets_rescued) &SaveData::pets_rescued
- [`bool completed_normal`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=completed_normal) &SaveData::completed_normal
- [`bool completed_ironman`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=completed_ironman) &SaveData::completed_ironman
- [`bool completed_hard`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=completed_hard) &SaveData::completed_hard
- [`bool profile_seen`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=profile_seen) &SaveData::profile_seen
- [`bool seeded_unlocked`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seeded_unlocked) &SaveData::seeded_unlocked
- [`int world_last`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=world_last) &SaveData::world_last
- [`int level_last`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_last) &SaveData::level_last
- [`int score_last`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=score_last) &SaveData::score_last
- [`int time_last`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=time_last) &SaveData::time_last
- [`array<int, 9> stickers`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stickers) &SaveData::stickers
- [`array<int, 4> players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players) &SaveData::players
### `CustomSound`
Handle to a loaded sound, can be used to play the sound and receive a `PlayingSound` for more control
It is up to you to not release this as long as any sounds returned by `CustomSound:play()` are still playing
- [`PlayingSound play()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play) play
- [`PlayingSound play(bool paused)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play) play
- [`PlayingSound play(bool paused, SOUND_TYPE sound_type)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play) play
- [`map<VANILLA_SOUND_PARAM, string> get_parameters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameters) get_parameters
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
- [`bool set_looping(SOUND_LOOP_MODE loop_mode)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_looping) &PlayingSound::set_looping
- [`bool set_callback(SoundCallbackFunction callback)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback) set_callback
- [`map<VANILLA_SOUND_PARAM, string> get_parameters()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameters) get_parameters
- [`optional<float> get_parameter(VANILLA_SOUND_PARAM parameter_index)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_parameter) &PlayingSound::get_parameter
- [`bool set_parameter(VANILLA_SOUND_PARAM parameter_index, float value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_parameter) &PlayingSound::set_parameter
### `PlayerSlotSettings`
- [`bool controller_vibration`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=controller_vibration) &PlayerSlotSettings::controller_vibration
- [`bool auto_run_enabled`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=auto_run_enabled) &PlayerSlotSettings::auto_run_enabled
- [`bool controller_right_stick`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=controller_right_stick) &PlayerSlotSettings::controller_right_stick
### `PlayerSlot`
- [`INPUTS buttons_gameplay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons_gameplay) &PlayerSlot::buttons_gameplay
- [`INPUTS buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &PlayerSlot::buttons
- [`InputMapping input_mapping_keyboard`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=input_mapping_keyboard) &PlayerSlot::input_mapping_keyboard
- [`InputMapping input_mapping_controller`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=input_mapping_controller) &PlayerSlot::input_mapping_controller
- [`int player_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_id) &PlayerSlot::player_id
- [`bool is_participating`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_participating) &PlayerSlot::is_participating
### `InputMapping`
- [`int jump`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=jump) &InputMapping::jump
- [`int attack`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attack) &InputMapping::attack
- [`int bomb`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bomb) &InputMapping::bomb
- [`int rope`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rope) &InputMapping::rope
- [`int walk_run`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=walk_run) &InputMapping::walk_run
- [`int use_door_buy`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=use_door_buy) &InputMapping::use_door_buy
- [`int pause_menu`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause_menu) &InputMapping::pause_menu
- [`int journal`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=journal) &InputMapping::journal
- [`int left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left) &InputMapping::left
- [`int right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right) &InputMapping::right
- [`int up`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=up) &InputMapping::up
- [`int down`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=down) &InputMapping::down
### `PlayerInputs`
- [`array<PlayerSlot, MAX_PLAYERS> player_slots`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slots) &PlayerInputs::player_slots
- [`PlayerSlot player_slot_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_1) &PlayerInputs::player_slot_1
- [`PlayerSlot player_slot_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_2) &PlayerInputs::player_slot_2
- [`PlayerSlot player_slot_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_3) &PlayerInputs::player_slot_3
- [`PlayerSlot player_slot_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_4) &PlayerInputs::player_slot_4
- [`array<PlayerSlotSettings, MAX_PLAYERS> player_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_settings) &PlayerInputs::player_settings
- [`PlayerSlotSettings player_slot_1_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_1_settings) &PlayerInputs::player_slot_1_settings
- [`PlayerSlotSettings player_slot_2_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_2_settings) &PlayerInputs::player_slot_2_settings
- [`PlayerSlotSettings player_slot_3_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_3_settings) &PlayerInputs::player_slot_3_settings
- [`PlayerSlotSettings player_slot_4_settings`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_slot_4_settings) &PlayerInputs::player_slot_4_settings
### `GuiDrawContext`
- [`nil draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_line) &GuiDrawContext::draw_line
\
Draws a line on screen
- [`nil draw_rect(float left, float top, float right, float bottom, float thickness, float rounding, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect) draw_rect
\
Draws a rectangle on screen from top-left to bottom-right.
- [`nil draw_rect(AABB rect, float thickness, float rounding, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect) draw_rect
\
Draws a rectangle on screen from top-left to bottom-right.
- [`nil draw_rect_filled(float left, float top, float right, float bottom, float rounding, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled) draw_rect_filled
\
Draws a filled rectangle on screen from top-left to bottom-right.
- [`nil draw_rect_filled(AABB rect, float rounding, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled) draw_rect_filled
\
Draws a filled rectangle on screen from top-left to bottom-right.
- [`nil draw_circle(float x, float y, float radius, float thickness, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle) &GuiDrawContext::draw_circle
\
Draws a circle on screen
- [`nil draw_circle_filled(float x, float y, float radius, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle_filled) &GuiDrawContext::draw_circle_filled
\
Draws a filled circle on screen
- [`nil draw_text(float x, float y, float size, string text, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text) &GuiDrawContext::draw_text
\
Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
- [`nil draw_image(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image) draw_image
\
Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
- [`nil draw_image(IMAGE image, AABB rect, AABB uv_rect, uColor color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image) draw_image
\
Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
- [`nil draw_image_rotated(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image_rotated) draw_image_rotated
\
Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
- [`nil draw_image_rotated(IMAGE image, AABB rect, AABB uv_rect, uColor color, float angle, float px, float py)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image_rotated) draw_image_rotated
\
Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
- [`bool window(string title, float x, float y, float w, float h, bool movable, function callback)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=window) &GuiDrawContext::window
\
Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the
[ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to
autosize in center. Use just a `##Label` as title to hide titlebar.
Important: Keep all your labels unique! If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to
give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text... The window api is probably evolving
still, this is just the first draft. Felt cute, might delete later!
Returns false if the window was closed from the X.
- [`nil win_text(string text)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_text) &GuiDrawContext::win_text
\
Add some text to window, automatically wrapped
- [`nil win_separator()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_separator) &GuiDrawContext::win_separator
\
Add a separator line to window
- [`nil win_inline()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_inline) &GuiDrawContext::win_inline
\
Add next thing on the same line. This is same as `win_sameline(0, -1)`
- [`nil win_sameline(float offset, float spacing)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_sameline) &GuiDrawContext::win_sameline
\
Add next thing on the same line, with an offset
- [`bool win_button(string text)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_button) &GuiDrawContext::win_button
\
Add a button
- [`string win_input_text(string label, string value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_text) &GuiDrawContext::win_input_text
\
Add a text field
- [`int win_input_int(string label, int value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_int) &GuiDrawContext::win_input_int
\
Add an integer field
- [`float win_input_float(string label, float value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_float) &GuiDrawContext::win_input_float
\
Add a float field
- [`int win_slider_int(string label, int value, int min, int max)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_int) &GuiDrawContext::win_slider_int
\
Add an integer slider
- [`int win_drag_int(string label, int value, int min, int max)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_int) &GuiDrawContext::win_drag_int
\
Add an integer dragfield
- [`float win_slider_float(string label, float value, float min, float max)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_float) &GuiDrawContext::win_slider_float
\
Add an float slider
- [`float win_drag_float(string label, float value, float min, float max)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_float) &GuiDrawContext::win_drag_float
\
Add an float dragfield
- [`bool win_check(string label, bool value)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_check) &GuiDrawContext::win_check
\
Add a checkbox
- [`int win_combo(string label, int selected, string opts)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_combo) &GuiDrawContext::win_combo
\
Add a combo box
- [`nil win_pushid(int id)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_pushid) &GuiDrawContext::win_pushid
\
Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
- [`nil win_popid()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_popid) &GuiDrawContext::win_popid
\
Pop unique identifier from the stack. Put after the input.
- [`nil win_image(IMAGE image, int width, int height)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_image) &GuiDrawContext::win_image
\
Draw image to window.
### `VanillaRenderContext`
- [`nil draw_text(const string& text, float x, float y, float scale_x, float scale_y, Color color, int alignment, int fontstyle)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text) &VanillaRenderContext::draw_text
\
Draw text using the built-in renderer. Use in combination with ON.RENDER_✱ events. See vanilla_rendering.lua in the example scripts.
- [`tuple<float, float> draw_text_size(const string& text, float scale_x, float scale_y, int fontstyle)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text_size) &VanillaRenderContext::draw_text_size
\
Measure the provided text using the built-in renderer
- [`nil draw_screen_texture(TEXTURE texture_id, int row, int column, float left, float top, float right, float bottom, Color color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_screen_texture) draw_screen_texture
\
Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU events
- [`nil draw_screen_texture(TEXTURE texture_id, int row, int column, const AABB& rect, Color color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_screen_texture) draw_screen_texture
\
Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU events
- [`nil draw_world_texture(TEXTURE texture_id, int row, int column, float left, float top, float right, float bottom, Color color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_world_texture) draw_world_texture
\
Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
- [`nil draw_world_texture(TEXTURE texture_id, int row, int column, const AABB& rect, Color color)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_world_texture) draw_world_texture
\
Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
### `TextureRenderingInfo`
- [`float x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=x) &TextureRenderingInfo::x
- [`float y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=y) &TextureRenderingInfo::y
- [`float destination_top_left_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_top_left_x) &TextureRenderingInfo::destination_top_left_x
- [`float destination_top_left_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_top_left_y) &TextureRenderingInfo::destination_top_left_y
- [`float destination_top_right_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_top_right_x) &TextureRenderingInfo::destination_top_right_x
- [`float destination_top_right_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_top_right_y) &TextureRenderingInfo::destination_top_right_y
- [`float destination_bottom_left_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_bottom_left_x) &TextureRenderingInfo::destination_bottom_left_x
- [`float destination_bottom_left_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_bottom_left_y) &TextureRenderingInfo::destination_bottom_left_y
- [`float destination_bottom_right_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_bottom_right_x) &TextureRenderingInfo::destination_bottom_right_x
- [`float destination_bottom_right_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=destination_bottom_right_y) &TextureRenderingInfo::destination_bottom_right_y
- [`nil set_destination(const AABB& bbox)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_destination) &TextureRenderingInfo::set_destination
- [`float source_top_left_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_top_left_x) &TextureRenderingInfo::source_top_left_x
- [`float source_top_left_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_top_left_y) &TextureRenderingInfo::source_top_left_y
- [`float source_top_right_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_top_right_x) &TextureRenderingInfo::source_top_right_x
- [`float source_top_right_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_top_right_y) &TextureRenderingInfo::source_top_right_y
- [`float source_bottom_left_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_bottom_left_x) &TextureRenderingInfo::source_bottom_left_x
- [`float source_bottom_left_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_bottom_left_y) &TextureRenderingInfo::source_bottom_left_y
- [`float source_bottom_right_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_bottom_right_x) &TextureRenderingInfo::source_bottom_right_x
- [`float source_bottom_right_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=source_bottom_right_y) &TextureRenderingInfo::source_bottom_right_y
### `TextureDefinition`
Use `TextureDefinition.new()` to get a new instance to this and pass it to define_entity_texture.
`width` and `height` always have to be the size of the image file. They should be divisible by `tile_width` and `tile_height` respectively.
`tile_width` and `tile_height` define the size of a single tile, the image will automatically be divided into these tiles.
Tiles are labeled in sequence starting at the top left, going right and down at the end of the image (you know, like sentences work in the English language). Use those numbers in `Entity::animation_frame`.
`sub_image_offset_x`, `sub_image_offset_y`, `sub_image_width` and `sub_image_height` can be used if only a part of the image should be used. Leave them at zero to ignore this.
- [`string texture_path`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=texture_path) &TextureDefinition::texture_path
- [`int width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &TextureDefinition::width
- [`int height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &TextureDefinition::height
- [`int tile_width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_width) &TextureDefinition::tile_width
- [`int tile_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tile_height) &TextureDefinition::tile_height
- [`int sub_image_offset_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_offset_x) &TextureDefinition::sub_image_offset_x
- [`int sub_image_offset_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_offset_y) &TextureDefinition::sub_image_offset_y
- [`int sub_image_width`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_width) &TextureDefinition::sub_image_width
- [`int sub_image_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sub_image_height) &TextureDefinition::sub_image_height
### `AABB`
Axis-Aligned-Bounding-Box, represents for example a hitbox of an entity or the size of a gui element
- [`AABB()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=AABB) 
\
Create a new axis aligned bounding box - defaults to all zeroes
- [`AABB(const AABB&)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=AABB) 
\
Copy an axis aligned bounding box
- [`AABB(float left_, float top_, float right_, float bottom_)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=AABB) 
\
Create a new axis aligned bounding box by specifying its values
- [`float left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left) &AABB::left
- [`float bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom) &AABB::bottom
- [`float right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right) &AABB::right
- [`float top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top) &AABB::top
- [`bool overlaps_with(const AABB& other)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=overlaps_with) &AABB::overlaps_with
- [`AABB& abs()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=abs) &AABB::abs
\
Fixes the AABB if any of the sides have negative length
- [`AABB& extrude(float amount)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=extrude) &AABB::extrude
\
Grows or shrinks the AABB by the given amount in all directions.
If `amount < 0` and `abs(amount) > right/top - left/bottom` the respective dimension of the AABB will become `0`.
- [`AABB& offset(float off_x, float off_y)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=offset) &AABB::offset
\
Offsets the AABB by the given offset.
- [`float area()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=area) &AABB::area
\
Compute area of the AABB, can be zero if one dimension is zero or negative if one dimension is inverted.
- [`tuple<float, float> center()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=center) &AABB::center
\
Short for `(aabb.left + aabb.right) / 2.0f, (aabb.top + aabb.bottom) / 2.0f`.
- [`float width()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=width) &AABB::width
\
Short for `aabb.right - aabb.left`.
- [`float height()`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=height) &AABB::height
\
Short for `aabb.top - aabb.bottom`.
### `Screen`
- [`float render_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=render_timer) &Screen::render_timer
### `ScreenLogo`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo logo_mossmouth`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=logo_mossmouth) &ScreenLogo::logo_mossmouth
- [`TextureRenderingInfo logo_blitworks`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=logo_blitworks) &ScreenLogo::logo_blitworks
- [`TextureRenderingInfo logo_fmod`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=logo_fmod) &ScreenLogo::logo_fmod
### `ScreenIntro`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo unknown4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown4) &ScreenIntro::unknown4
### `ScreenPrologue`
Derived from [`Screen`](#screen)
- [`STRINGID line1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=line1) &ScreenPrologue::line1
- [`STRINGID line2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=line2) &ScreenPrologue::line2
- [`STRINGID line3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=line3) &ScreenPrologue::line3
### `ScreenTitle`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo logo_spelunky2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=logo_spelunky2) &ScreenTitle::logo_spelunky2
- [`TextureRenderingInfo ana`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ana) &ScreenTitle::ana
- [`TextureRenderingInfo ana_right_eyeball_torch_reflection`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ana_right_eyeball_torch_reflection) &ScreenTitle::ana_right_eyeball_torch_reflection
- [`TextureRenderingInfo ana_left_eyeball_torch_reflection`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ana_left_eyeball_torch_reflection) &ScreenTitle::ana_left_eyeball_torch_reflection
- [`ParticleEmitterInfo particle_torchflame_smoke`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_smoke) &ScreenTitle::particle_torchflame_smoke
- [`ParticleEmitterInfo particle_torchflame_backflames`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_backflames) &ScreenTitle::particle_torchflame_backflames
- [`ParticleEmitterInfo particle_torchflame_flames`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_flames) &ScreenTitle::particle_torchflame_flames
- [`ParticleEmitterInfo particle_torchflame_backflames_animated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_backflames_animated) &ScreenTitle::particle_torchflame_backflames_animated
- [`ParticleEmitterInfo particle_torchflame_flames_animated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_flames_animated) &ScreenTitle::particle_torchflame_flames_animated
- [`ParticleEmitterInfo particle_torchflame_ash`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_ash) &ScreenTitle::particle_torchflame_ash
### `ScreenMenu`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo tunnel_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tunnel_background) &ScreenMenu::tunnel_background
- [`TextureRenderingInfo cthulhu_disc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cthulhu_disc) &ScreenMenu::cthulhu_disc
- [`TextureRenderingInfo tunnel_ring_darkbrown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tunnel_ring_darkbrown) &ScreenMenu::tunnel_ring_darkbrown
- [`TextureRenderingInfo cthulhu_body`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cthulhu_body) &ScreenMenu::cthulhu_body
- [`TextureRenderingInfo tunnel_ring_lightbrown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tunnel_ring_lightbrown) &ScreenMenu::tunnel_ring_lightbrown
- [`TextureRenderingInfo vine_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vine_left) &ScreenMenu::vine_left
- [`TextureRenderingInfo vine_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vine_right) &ScreenMenu::vine_right
- [`TextureRenderingInfo skull_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=skull_left) &ScreenMenu::skull_left
- [`TextureRenderingInfo salamander_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=salamander_right) &ScreenMenu::salamander_right
- [`TextureRenderingInfo left_spear`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_spear) &ScreenMenu::left_spear
- [`TextureRenderingInfo right_spear`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_spear) &ScreenMenu::right_spear
- [`TextureRenderingInfo spear_dangler_related`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spear_dangler_related) &ScreenMenu::spear_dangler_related
- [`TextureRenderingInfo play_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play_scroll) &ScreenMenu::play_scroll
- [`TextureRenderingInfo info_toast`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=info_toast) &ScreenMenu::info_toast
- [`float cthulhu_disc_ring_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cthulhu_disc_ring_angle) &ScreenMenu::cthulhu_disc_ring_angle
- [`float cthulhu_disc_split_progress`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cthulhu_disc_split_progress) &ScreenMenu::cthulhu_disc_split_progress
- [`float cthulhu_disc_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cthulhu_disc_y) &ScreenMenu::cthulhu_disc_y
- [`float cthulhu_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cthulhu_timer) &ScreenMenu::cthulhu_timer
- [`int selected_menu_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_menu_index) &ScreenMenu::selected_menu_index
- [`bool show_toast_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=show_toast_text) &ScreenMenu::show_toast_text
- [`float menu_text_opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=menu_text_opacity) &ScreenMenu::menu_text_opacity
- [`float menu_text_opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=menu_text_opacity) &ScreenMenu::menu_text_opacity
- [`array<float, 6> spear_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spear_position) &ScreenMenu::spear_position
- [`array<SpearDanglerAnimFrames, 6> spear_dangler`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spear_dangler) &ScreenMenu::spear_dangler
- [`float play_scroll_descend_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play_scroll_descend_timer) &ScreenMenu::play_scroll_descend_timer
- [`STRINGID scroll_text`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_text) &ScreenMenu::scroll_text
### `ScreenOptions`
Derived from [`Screen`](#screen)
- [`int selected_menu_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_menu_index) &ScreenOptions::selected_menu_index
- [`TextureRenderingInfo brick_border`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_border) &ScreenOptions::brick_border
- [`float top_bottom_woodpanels_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_bottom_woodpanels_velocity) &ScreenOptions::top_bottom_woodpanels_velocity
- [`float top_bottom_woodpanels_progress`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_bottom_woodpanels_progress) &ScreenOptions::top_bottom_woodpanels_progress
- [`float scroll_unfurl_progress`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_unfurl_progress) &ScreenOptions::scroll_unfurl_progress
- [`float bottom_woodpanel_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel_y) &ScreenOptions::bottom_woodpanel_y
- [`float top_bottom_woodpanels_slide_in_related`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_bottom_woodpanels_slide_in_related) &ScreenOptions::top_bottom_woodpanels_slide_in_related
- [`TextureRenderingInfo bottom_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel) &ScreenOptions::bottom_woodpanel
- [`TextureRenderingInfo top_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel) &ScreenOptions::top_woodpanel
- [`TextureRenderingInfo top_woodpanel_left_scrollhandle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel_left_scrollhandle) &ScreenOptions::top_woodpanel_left_scrollhandle
- [`TextureRenderingInfo top_woodpanel_right_scrollhandle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel_right_scrollhandle) &ScreenOptions::top_woodpanel_right_scrollhandle
- [`STRINGID button_right_caption`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=button_right_caption) &ScreenOptions::button_right_caption
- [`STRINGID button_middle_caption`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=button_middle_caption) &ScreenOptions::button_middle_caption
- [`bool top_woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel_visible) &ScreenOptions::top_woodpanel_visible
- [`bool bottom_woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel_visible) &ScreenOptions::bottom_woodpanel_visible
- [`bool toggle_woodpanel_slidein_animation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toggle_woodpanel_slidein_animation) &ScreenOptions::toggle_woodpanel_slidein_animation
- [`bool capitalize_top_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=capitalize_top_woodpanel) &ScreenOptions::capitalize_top_woodpanel
- [`int current_menu_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=current_menu_1) &ScreenOptions::current_menu_1
- [`int current_menu_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=current_menu_2) &ScreenOptions::current_menu_2
- [`TextureRenderingInfo topleft_woodpanel_esc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc) &ScreenOptions::topleft_woodpanel_esc
- [`TextureRenderingInfo brick_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_background) &ScreenOptions::brick_background
- [`TextureRenderingInfo brick_middlelayer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_middlelayer) &ScreenOptions::brick_middlelayer
- [`TextureRenderingInfo brick_foreground`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_foreground) &ScreenOptions::brick_foreground
- [`TextureRenderingInfo selected_item_rounded_rect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_item_rounded_rect) &ScreenOptions::selected_item_rounded_rect
- [`TextureRenderingInfo selected_item_scarab`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_item_scarab) &ScreenOptions::selected_item_scarab
- [`TextureRenderingInfo item_option_arrow_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_option_arrow_left) &ScreenOptions::item_option_arrow_left
- [`TextureRenderingInfo item_option_arrow_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_option_arrow_right) &ScreenOptions::item_option_arrow_right
- [`TextureRenderingInfo tooltip_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=tooltip_background) &ScreenOptions::tooltip_background
- [`TextureRenderingInfo progressbar_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=progressbar_background) &ScreenOptions::progressbar_background
- [`TextureRenderingInfo progressbar_foreground`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=progressbar_foreground) &ScreenOptions::progressbar_foreground
- [`TextureRenderingInfo progressbar_position_indicator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=progressbar_position_indicator) &ScreenOptions::progressbar_position_indicator
- [`TextureRenderingInfo sectionheader_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sectionheader_background) &ScreenOptions::sectionheader_background
- [`float topleft_woodpanel_esc_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc_slidein_timer) &ScreenOptions::topleft_woodpanel_esc_slidein_timer
- [`float text_fadein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_fadein_timer) &ScreenOptions::text_fadein_timer
- [`float vertical_scroll_effect_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_scroll_effect_timer) &ScreenOptions::vertical_scroll_effect_timer
### `ScreenPlayerProfile`
Derived from [`Screen`](#screen)
### `ScreenLeaderboards`
Derived from [`Screen`](#screen)
### `ScreenSeedInput`
Derived from [`Screen`](#screen)
- [`float bottom_woodpanel_slideup_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel_slideup_timer) &ScreenSeedInput::bottom_woodpanel_slideup_timer
- [`float bottom_woodpanel_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel_y) &ScreenSeedInput::bottom_woodpanel_y
- [`TextureRenderingInfo bottom_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel) &ScreenSeedInput::bottom_woodpanel
- [`STRINGID buttons_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons_text_id) &ScreenSeedInput::buttons_text_id
- [`float topleft_woodpanel_esc_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc_slidein_timer) &ScreenSeedInput::topleft_woodpanel_esc_slidein_timer
- [`STRINGID scroll_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_text_id) &ScreenSeedInput::scroll_text_id
- [`STRINGID start_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_text_id) &ScreenSeedInput::start_text_id
- [`TextureRenderingInfo main_woodpanel_left_border`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_woodpanel_left_border) &ScreenSeedInput::main_woodpanel_left_border
- [`TextureRenderingInfo main_woodpanel_center`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_woodpanel_center) &ScreenSeedInput::main_woodpanel_center
- [`TextureRenderingInfo main_woodpanel_right_border`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_woodpanel_right_border) &ScreenSeedInput::main_woodpanel_right_border
- [`TextureRenderingInfo seed_letter_cutouts`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed_letter_cutouts) &ScreenSeedInput::seed_letter_cutouts
- [`TextureRenderingInfo topleft_woodpanel_esc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc) &ScreenSeedInput::topleft_woodpanel_esc
- [`TextureRenderingInfo start_sidepanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_sidepanel) &ScreenSeedInput::start_sidepanel
- [`float start_sidepanel_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_sidepanel_slidein_timer) &ScreenSeedInput::start_sidepanel_slidein_timer
### `ScreenCharacterSelect`
Derived from [`Screen`](#screen)
- [`float main_background_zoom_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_background_zoom_target) &ScreenCharacterSelect::main_background_zoom_target
- [`float blurred_border_zoom_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurred_border_zoom_target) &ScreenCharacterSelect::blurred_border_zoom_target
- [`float top_bottom_woodpanel_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_bottom_woodpanel_slidein_timer) &ScreenCharacterSelect::top_bottom_woodpanel_slidein_timer
- [`float top_scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_scroll_unfurl_timer) &ScreenCharacterSelect::top_scroll_unfurl_timer
- [`TextureRenderingInfo bottom_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel) &ScreenCharacterSelect::bottom_woodpanel
- [`TextureRenderingInfo top_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel) &ScreenCharacterSelect::top_woodpanel
- [`TextureRenderingInfo left_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_scroll_handle) &ScreenCharacterSelect::left_scroll_handle
- [`TextureRenderingInfo right_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_scroll_handle) &ScreenCharacterSelect::right_scroll_handle
- [`STRINGID left_button_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_button_text_id) &ScreenCharacterSelect::left_button_text_id
- [`STRINGID right_button_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_button_text_id) &ScreenCharacterSelect::right_button_text_id
- [`STRINGID middle_button_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=middle_button_text_id) &ScreenCharacterSelect::middle_button_text_id
- [`bool top_woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel_visible) &ScreenCharacterSelect::top_woodpanel_visible
- [`bool bottom_woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel_visible) &ScreenCharacterSelect::bottom_woodpanel_visible
- [`bool toggle_woodpanel_slidein_animation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toggle_woodpanel_slidein_animation) &ScreenCharacterSelect::toggle_woodpanel_slidein_animation
- [`TextureRenderingInfo mine_entrance_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mine_entrance_background) &ScreenCharacterSelect::mine_entrance_background
- [`TextureRenderingInfo character`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character) &ScreenCharacterSelect::character
- [`TextureRenderingInfo character_shadow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_shadow) &ScreenCharacterSelect::character_shadow
- [`TextureRenderingInfo character_flag`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_flag) &ScreenCharacterSelect::character_flag
- [`TextureRenderingInfo character_left_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_left_arrow) &ScreenCharacterSelect::character_left_arrow
- [`TextureRenderingInfo character_right_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_right_arrow) &ScreenCharacterSelect::character_right_arrow
- [`TextureRenderingInfo mine_entrance_border`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mine_entrance_border) &ScreenCharacterSelect::mine_entrance_border
- [`TextureRenderingInfo mine_entrance_shutter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mine_entrance_shutter) &ScreenCharacterSelect::mine_entrance_shutter
- [`TextureRenderingInfo background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=background) &ScreenCharacterSelect::background
- [`TextureRenderingInfo blurred_border`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurred_border) &ScreenCharacterSelect::blurred_border
- [`TextureRenderingInfo blurred_border2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurred_border2) &ScreenCharacterSelect::blurred_border2
- [`TextureRenderingInfo topleft_woodpanel_esc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc) &ScreenCharacterSelect::topleft_woodpanel_esc
- [`TextureRenderingInfo start_sidepanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_sidepanel) &ScreenCharacterSelect::start_sidepanel
- [`TextureRenderingInfo quick_select_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quick_select_panel) &ScreenCharacterSelect::quick_select_panel
- [`TextureRenderingInfo quick_select_selected_char_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quick_select_selected_char_background) &ScreenCharacterSelect::quick_select_selected_char_background
- [`TextureRenderingInfo quick_select_panel_related`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=quick_select_panel_related) &ScreenCharacterSelect::quick_select_panel_related
- [`array<float, MAX_PLAYERS> player_shutter_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_shutter_timer) &ScreenCharacterSelect::player_shutter_timer
- [`array<float, MAX_PLAYERS> player_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_x) &ScreenCharacterSelect::player_x
- [`array<float, MAX_PLAYERS> player_y`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_y) &ScreenCharacterSelect::player_y
- [`array<array<float, 2>, MAX_PLAYERS> player_arrow_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_arrow_slidein_timer) &ScreenCharacterSelect::player_arrow_slidein_timer
- [`array<bool, MAX_PLAYERS> player_facing_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_facing_left) &ScreenCharacterSelect::player_facing_left
- [`array<bool, MAX_PLAYERS> player_quickselect_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_quickselect_shown) &ScreenCharacterSelect::player_quickselect_shown
- [`array<float, MAX_PLAYERS> player_quickselect_fadein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_quickselect_fadein_timer) &ScreenCharacterSelect::player_quickselect_fadein_timer
- [`array<array<float, 2>, MAX_PLAYERS> player_quickselect_coords`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_quickselect_coords) &ScreenCharacterSelect::player_quickselect_coords
- [`array<float, MAX_PLAYERS> player_quickselect_wiggle_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_quickselect_wiggle_angle) &ScreenCharacterSelect::player_quickselect_wiggle_angle
- [`float topleft_woodpanel_esc_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc_slidein_timer) &ScreenCharacterSelect::topleft_woodpanel_esc_slidein_timer
- [`float start_panel_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_panel_slidein_timer) &ScreenCharacterSelect::start_panel_slidein_timer
- [`float action_buttons_keycap_size`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=action_buttons_keycap_size) &ScreenCharacterSelect::action_buttons_keycap_size
- [`bool not_ready_to_start_yet`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=not_ready_to_start_yet) &ScreenCharacterSelect::not_ready_to_start_yet
- [`int available_mine_entrances`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=available_mine_entrances) &ScreenCharacterSelect::available_mine_entrances
- [`int amount_of_mine_entrances_activated`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=amount_of_mine_entrances_activated) &ScreenCharacterSelect::amount_of_mine_entrances_activated
- [`int buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &ScreenCharacterSelect::buttons
- [`float opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=opacity) &ScreenCharacterSelect::opacity
- [`bool start_pressed`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_pressed) &ScreenCharacterSelect::start_pressed
- [`bool transition_to_game_started`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=transition_to_game_started) &ScreenCharacterSelect::transition_to_game_started
- [`array<FlyingThing, 6> flying_things`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flying_things) &ScreenCharacterSelect::flying_things
- [`int flying_thing_countdown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flying_thing_countdown) &ScreenCharacterSelect::flying_thing_countdown
- [`ParticleEmitterInfo particle_ceilingdust_smoke`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_ceilingdust_smoke) &ScreenCharacterSelect::particle_ceilingdust_smoke
- [`ParticleEmitterInfo particle_ceilingdust_rubble`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_ceilingdust_rubble) &ScreenCharacterSelect::particle_ceilingdust_rubble
- [`ParticleEmitterInfo particle_mist`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_mist) &ScreenCharacterSelect::particle_mist
- [`ParticleEmitterInfo particle_torchflame_smoke1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_smoke1) &ScreenCharacterSelect::particle_torchflame_smoke1
- [`ParticleEmitterInfo particle_torchflame_flames1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_flames1) &ScreenCharacterSelect::particle_torchflame_flames1
- [`ParticleEmitterInfo particle_torchflame_smoke2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_smoke2) &ScreenCharacterSelect::particle_torchflame_smoke2
- [`ParticleEmitterInfo particle_torchflame_flames2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_flames2) &ScreenCharacterSelect::particle_torchflame_flames2
- [`ParticleEmitterInfo particle_torchflame_smoke3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_smoke3) &ScreenCharacterSelect::particle_torchflame_smoke3
- [`ParticleEmitterInfo particle_torchflame_flames3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_flames3) &ScreenCharacterSelect::particle_torchflame_flames3
- [`ParticleEmitterInfo particle_torchflame_smoke4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_smoke4) &ScreenCharacterSelect::particle_torchflame_smoke4
- [`ParticleEmitterInfo particle_torchflame_flames4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=particle_torchflame_flames4) &ScreenCharacterSelect::particle_torchflame_flames4
### `FlyingThing`
- [`TextureRenderingInfo texture_info`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=texture_info) &FlyingThing::texture_info
- [`int entity_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_type) &FlyingThing::entity_type
- [`float spritesheet_column`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spritesheet_column) &FlyingThing::spritesheet_column
- [`float spritesheet_row`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spritesheet_row) &FlyingThing::spritesheet_row
- [`float spritesheet_animation_length`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spritesheet_animation_length) &FlyingThing::spritesheet_animation_length
- [`float velocity_x`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=velocity_x) &FlyingThing::velocity_x
- [`float amplitude`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=amplitude) &FlyingThing::amplitude
- [`float frequency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frequency) &FlyingThing::frequency
- [`float sinewave_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sinewave_angle) &FlyingThing::sinewave_angle
### `ScreenTeamSelect`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo ana_carrying_torch`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ana_carrying_torch) &ScreenTeamSelect::ana_carrying_torch
- [`TextureRenderingInfo scroll_bottom_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_bottom_left) &ScreenTeamSelect::scroll_bottom_left
- [`TextureRenderingInfo scrollend_bottom_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scrollend_bottom_left) &ScreenTeamSelect::scrollend_bottom_left
- [`TextureRenderingInfo four_ropes`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=four_ropes) &ScreenTeamSelect::four_ropes
- [`TextureRenderingInfo unknown4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown4) &ScreenTeamSelect::unknown4
- [`TextureRenderingInfo four_characters`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=four_characters) &ScreenTeamSelect::four_characters
- [`TextureRenderingInfo left_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_arrow) &ScreenTeamSelect::left_arrow
- [`TextureRenderingInfo right_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_arrow) &ScreenTeamSelect::right_arrow
- [`TextureRenderingInfo start_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_panel) &ScreenTeamSelect::start_panel
- [`float start_panel_slide_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=start_panel_slide_timer) &ScreenTeamSelect::start_panel_slide_timer
- [`float pulsating_arrows_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pulsating_arrows_timer) &ScreenTeamSelect::pulsating_arrows_timer
- [`int selected_player`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_player) &ScreenTeamSelect::selected_player
- [`int buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &ScreenTeamSelect::buttons
- [`bool ready`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ready) &ScreenTeamSelect::ready
### `ScreenCamp`
Derived from [`Screen`](#screen)
- [`int buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &ScreenCamp::buttons
### `ScreenLevel`
Derived from [`Screen`](#screen)
- [`int buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &ScreenLevel::buttons
### `ScreenTransition`
Derived from [`Screen`](#screen)
- [`float woodpanel_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_pos) &ScreenTransition::woodpanel_pos
- [`float stats_scroll_horizontal_posaa`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stats_scroll_horizontal_posaa) &ScreenTransition::stats_scroll_horizontal_pos
- [`float stats_scroll_vertical_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stats_scroll_vertical_pos) &ScreenTransition::stats_scroll_vertical_pos
- [`float level_completed_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_completed_pos) &ScreenTransition::level_completed_pos
- [`float stats_scroll_unfurl_targetvalue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stats_scroll_unfurl_targetvalue) &ScreenTransition::stats_scroll_unfurl_targetvalue
- [`TextureRenderingInfo woodpanel1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel1) &ScreenTransition::woodpanel1
- [`TextureRenderingInfo woodpanel2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel2) &ScreenTransition::woodpanel2
- [`TextureRenderingInfo woodpanel3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel3) &ScreenTransition::woodpanel3
- [`TextureRenderingInfo woodpanel_cutout1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout1) &ScreenTransition::woodpanel_cutout1
- [`TextureRenderingInfo woodpanel_cutout2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout2) &ScreenTransition::woodpanel_cutout2
- [`TextureRenderingInfo woodpanel_cutout3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout3) &ScreenTransition::woodpanel_cutout3
- [`TextureRenderingInfo woodplank`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodplank) &ScreenTransition::woodplank
- [`TextureRenderingInfo woodpanel_bottomcutout1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottomcutout1) &ScreenTransition::woodpanel_bottomcutout1
- [`TextureRenderingInfo woodpanel_bottomcutout2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottomcutout2) &ScreenTransition::woodpanel_bottomcutout2
- [`TextureRenderingInfo woodpanel_bottomcutout3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottomcutout3) &ScreenTransition::woodpanel_bottomcutout3
- [`TextureRenderingInfo unknown_all_forced`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown_all_forced) &ScreenTransition::unknown_all_forced
- [`TextureRenderingInfo stats_scroll_top_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stats_scroll_top_bottom) &ScreenTransition::stats_scroll_top_bottom
- [`TextureRenderingInfo killcount_rounded_rect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=killcount_rounded_rect) &ScreenTransition::killcount_rounded_rect
- [`TextureRenderingInfo level_completed_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=level_completed_panel) &ScreenTransition::level_completed_panel
- [`int stats_scroll_state_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stats_scroll_state_1) &ScreenTransition::stats_scroll_state_1
- [`int stats_scroll_state_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=stats_scroll_state_2) &ScreenTransition::stats_scroll_state_2
- [`bool hide_press_to_go_next_level`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hide_press_to_go_next_level) &ScreenTransition::hide_press_to_go_next_level
- [`TextureRenderingInfo mama_tunnel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel) &ScreenTransition::mama_tunnel
- [`TextureRenderingInfo speechbubble`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble) &ScreenTransition::speechbubble
- [`TextureRenderingInfo speechbubble_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_arrow) &ScreenTransition::speechbubble_arrow
- [`float mama_tunnel_fade_targetvalue`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_fade_targetvalue) &ScreenTransition::mama_tunnel_fade_targetvalue
- [`STRINGID mama_tunnel_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_text_id) &ScreenTransition::mama_tunnel_text_id
- [`bool mama_tunnel_choice_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_choice_visible) &ScreenTransition::mama_tunnel_choice_visible
- [`bool mama_tunnel_agree_with_gift`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_agree_with_gift) &ScreenTransition::mama_tunnel_agree_with_gift
- [`bool mama_tunnel_face_invisible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_face_invisible) &ScreenTransition::mama_tunnel_face_invisible
- [`float mama_tunnel_face_transparency`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_face_transparency) &ScreenTransition::mama_tunnel_face_transparency
- [`TextureRenderingInfo mama_tunnel_agree_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_agree_panel) &ScreenTransition::mama_tunnel_agree_panel
- [`TextureRenderingInfo mama_tunnel_agree_panel_indicator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mama_tunnel_agree_panel_indicator) &ScreenTransition::mama_tunnel_agree_panel_indicator
- [`TextureRenderingInfo woodpanel_cutout_big_money1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout_big_money1) &ScreenTransition::woodpanel_cutout_big_money1
- [`TextureRenderingInfo woodpanel_cutout_big_money2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout_big_money2) &ScreenTransition::woodpanel_cutout_big_money2
- [`TextureRenderingInfo woodpanel_cutout_big_money3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout_big_money3) &ScreenTransition::woodpanel_cutout_big_money3
- [`TextureRenderingInfo big_dollar_sign`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=big_dollar_sign) &ScreenTransition::big_dollar_sign
- [`TextureRenderingInfo unknown26`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown26) &ScreenTransition::unknown26
- [`array<int, MAX_PLAYERS> player_stats_scroll_numeric_value`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_stats_scroll_numeric_value) &ScreenTransition::player_stats_scroll_numeric_value
- [`array<TextureRenderingInfo, MAX_PLAYERS> player_secondary_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_secondary_icon) &ScreenTransition::player_secondary_icon
- [`array<TextureRenderingInfo, MAX_PLAYERS> player_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_icon) &ScreenTransition::player_icon
- [`array<int, MAX_PLAYERS> player_secondary_icon_type`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_secondary_icon_type) &ScreenTransition::player_secondary_icon_type
- [`array<int, MAX_PLAYERS> player_icon_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_icon_index) &ScreenTransition::player_icon_index
- [`TextureRenderingInfo hourglasses`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hourglasses) &ScreenTransition::hourglasses
- [`TextureRenderingInfo small_dollar_signs`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=small_dollar_signs) &ScreenTransition::small_dollar_signs
- [`Color this_level_money_color`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=this_level_money_color) &ScreenTransition::this_level_money_color
### `ScreenDeath`
Derived from [`Screen`](#screen)
### `ScreenWin`
Derived from [`Screen`](#screen)
- [`int sequence_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sequence_timer) &ScreenWin::sequence_timer
- [`int frame_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=frame_timer) &ScreenWin::frame_timer
- [`int animation_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_state) &ScreenWin::animation_state
- [`Entity rescuing_ship_entity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rescuing_ship_entity) &ScreenWin::rescuing_ship_entity
### `ScreenCredits`
Derived from [`Screen`](#screen)
### `ScreenScores`
Derived from [`Screen`](#screen)
- [`int animation_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_state) &ScreenScores::animation_state
- [`TextureRenderingInfo woodpanel1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel1) &ScreenScores::woodpanel1
- [`TextureRenderingInfo woodpanel2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel2) &ScreenScores::woodpanel2
- [`TextureRenderingInfo woodpanel3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel3) &ScreenScores::woodpanel3
- [`TextureRenderingInfo woodpanel_cutout`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_cutout) &ScreenScores::woodpanel_cutout
- [`TextureRenderingInfo dollarsign`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dollarsign) &ScreenScores::dollarsign
- [`TextureRenderingInfo hourglass`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hourglass) &ScreenScores::hourglass
- [`int animation_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_timer) &ScreenScores::animation_timer
- [`float woodpanel_slidedown_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_slidedown_timer) &ScreenScores::woodpanel_slidedown_timer
### `ScreenConstellation`
Derived from [`Screen`](#screen)
- [`int sequence_state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=sequence_state) &ScreenConstellation::sequence_state
- [`int animation_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_timer) &ScreenConstellation::animation_timer
- [`float constellation_text_opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=constellation_text_opacity) &ScreenConstellation::constellation_text_opacity
### `ScreenRecap`
Derived from [`Screen`](#screen)
### `ScreenOnlineLoading`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo ouroboros`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ouroboros) &ScreenOnlineLoading::ouroboros
- [`float ouroboros_angle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ouroboros_angle) &ScreenOnlineLoading::ouroboros_angle
### `ScreenOnlineLobby`
Derived from [`Screen`](#screen)
- [`float woodpanels_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanels_slidein_timer) &ScreenOnlineLobby::woodpanels_slidein_timer
- [`float scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_unfurl_timer) &ScreenOnlineLobby::scroll_unfurl_timer
- [`TextureRenderingInfo woodpanel_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottom) &ScreenOnlineLobby::woodpanel_bottom
- [`TextureRenderingInfo woodpanel_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top) &ScreenOnlineLobby::woodpanel_top
- [`TextureRenderingInfo left_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_scroll_handle) &ScreenOnlineLobby::left_scroll_handle
- [`TextureRenderingInfo right_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_scroll_handle) &ScreenOnlineLobby::right_scroll_handle
- [`STRINGID scroll_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_text_id) &ScreenOnlineLobby::scroll_text_id
- [`STRINGID btn_left_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=btn_left_text_id) &ScreenOnlineLobby::btn_left_text_id
- [`STRINGID btn_right_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=btn_right_text_id) &ScreenOnlineLobby::btn_right_text_id
- [`STRINGID btn_center_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=btn_center_text_id) &ScreenOnlineLobby::btn_center_text_id
- [`bool woodpanel_top_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top_visible) &ScreenOnlineLobby::woodpanel_top_visible
- [`bool woodpanel_bottom_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottom_visible) &ScreenOnlineLobby::woodpanel_bottom_visible
- [`bool toggle_panels_slidein`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toggle_panels_slidein) &ScreenOnlineLobby::toggle_panels_slidein
- [`array<OnlineLobbyScreenPlayer, 4> players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players) &ScreenOnlineLobby::players
- [`TextureRenderingInfo background_image`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=background_image) &ScreenOnlineLobby::background_image
- [`TextureRenderingInfo topleft_woodpanel_esc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc) &ScreenOnlineLobby::topleft_woodpanel_esc
- [`float topleft_woodpanel_esc_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=topleft_woodpanel_esc_slidein_timer) &ScreenOnlineLobby::topleft_woodpanel_esc_slidein_timer
- [`float character_walk_offset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_walk_offset) &ScreenOnlineLobby::character_walk_offset
- [`bool character_facing_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character_facing_left) &ScreenOnlineLobby::character_facing_left
- [`int move_direction`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_direction) &ScreenOnlineLobby::move_direction
- [`TextureRenderingInfo character`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=character) &ScreenOnlineLobby::character
- [`TextureRenderingInfo player_ready_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_ready_icon) &ScreenOnlineLobby::player_ready_icon
- [`TextureRenderingInfo arrow_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_left) &ScreenOnlineLobby::arrow_left
- [`TextureRenderingInfo arrow_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_right) &ScreenOnlineLobby::arrow_right
- [`float arrow_left_hor_offset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_left_hor_offset) &ScreenOnlineLobby::arrow_left_hor_offset
- [`float arrow_right_hor_offset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_right_hor_offset) &ScreenOnlineLobby::arrow_right_hor_offset
- [`TextureRenderingInfo platform_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=platform_icon) &ScreenOnlineLobby::platform_icon
- [`int player_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_count) &ScreenOnlineLobby::player_count
- [`bool searching_for_players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=searching_for_players) &ScreenOnlineLobby::searching_for_players
- [`bool show_code_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=show_code_panel) &ScreenOnlineLobby::show_code_panel
- [`float enter_code_woodpanel_bottom_slidein_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_woodpanel_bottom_slidein_pos) &ScreenOnlineLobby::enter_code_woodpanel_bottom_slidein_pos
- [`TextureRenderingInfo enter_code_woodpanel_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_woodpanel_bottom) &ScreenOnlineLobby::enter_code_woodpanel_bottom
- [`STRINGID enter_code_btn_right_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_btn_right_text_id) &ScreenOnlineLobby::enter_code_btn_right_text_id
- [`bool enter_code_woodpanel_top_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_woodpanel_top_visible) &ScreenOnlineLobby::enter_code_woodpanel_top_visible
- [`bool enter_code_woodpanel_bottom_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_woodpanel_bottom_visible) &ScreenOnlineLobby::enter_code_woodpanel_bottom_visible
- [`bool enter_code_toggle_panels_slidein`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_toggle_panels_slidein) &ScreenOnlineLobby::enter_code_toggle_panels_slidein
- [`int selected_character`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_character) &ScreenOnlineLobby::selected_character
- [`int characters_entered_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=characters_entered_count) &ScreenOnlineLobby::characters_entered_count
- [`float enter_code_topleft_woodpanel_esc_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_topleft_woodpanel_esc_slidein_timer) &ScreenOnlineLobby::enter_code_topleft_woodpanel_esc_slidein_timer
- [`STRINGID enter_code_banner_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_banner_text_id) &ScreenOnlineLobby::enter_code_banner_text_id
- [`STRINGID enter_code_OK_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_OK_text_id) &ScreenOnlineLobby::enter_code_OK_text_id
- [`TextureRenderingInfo enter_code_main_woodpanel_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_main_woodpanel_left) &ScreenOnlineLobby::enter_code_main_woodpanel_left
- [`TextureRenderingInfo enter_code_main_woodpanel_center`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_main_woodpanel_center) &ScreenOnlineLobby::enter_code_main_woodpanel_center
- [`TextureRenderingInfo enter_code_main_woodpanel_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_main_woodpanel_right) &ScreenOnlineLobby::enter_code_main_woodpanel_right
- [`TextureRenderingInfo enter_code_banner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_banner) &ScreenOnlineLobby::enter_code_banner
- [`TextureRenderingInfo enter_code_char_cutouts`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_char_cutouts) &ScreenOnlineLobby::enter_code_char_cutouts
- [`TextureRenderingInfo enter_code_pointing_hand`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_pointing_hand) &ScreenOnlineLobby::enter_code_pointing_hand
- [`TextureRenderingInfo enter_code_buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_buttons) &ScreenOnlineLobby::enter_code_buttons
- [`TextureRenderingInfo enter_code_OK_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_OK_panel) &ScreenOnlineLobby::enter_code_OK_panel
- [`float enter_code_OK_panel_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_OK_panel_slidein_timer) &ScreenOnlineLobby::enter_code_OK_panel_slidein_timer
- [`TextureRenderingInfo enter_code_your_code_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_your_code_scroll) &ScreenOnlineLobby::enter_code_your_code_scroll
- [`TextureRenderingInfo enter_code_your_code_scroll_left_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_your_code_scroll_left_handle) &ScreenOnlineLobby::enter_code_your_code_scroll_left_handle
- [`TextureRenderingInfo enter_code_your_code_scroll_right_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_code_your_code_scroll_right_handle) &ScreenOnlineLobby::enter_code_your_code_scroll_right_handle
- [`nil set_code(const string& code)`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_code) &ScreenOnlineLobby::set_code
### `PauseUI`
- [`float menu_slidein_progress`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=menu_slidein_progress) &PauseUI::menu_slidein_progress
- [`TextureRenderingInfo blurred_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurred_background) &PauseUI::blurred_background
- [`TextureRenderingInfo woodpanel_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_left) &PauseUI::woodpanel_left
- [`TextureRenderingInfo woodpanel_middle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_middle) &PauseUI::woodpanel_middle
- [`TextureRenderingInfo woodpanel_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_right) &PauseUI::woodpanel_right
- [`TextureRenderingInfo woodpanel_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top) &PauseUI::woodpanel_top
- [`TextureRenderingInfo scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll) &PauseUI::scroll
- [`TextureRenderingInfo confirmation_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=confirmation_panel) &PauseUI::confirmation_panel
- [`int previously_selected_menu_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=previously_selected_menu_index) &PauseUI::previously_selected_menu_index
- [`int visibility`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=visibility) &PauseUI::visibility
### `JournalUI`
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &JournalUI::state
- [`int page_shown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=page_shown) &JournalUI::page_shown
- [`int current_page`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=current_page) &JournalUI::current_page
- [`int flipping_to_page`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flipping_to_page) &JournalUI::flipping_to_page
- [`int max_page_count`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=max_page_count) &JournalUI::max_page_count
- [`TextureRenderingInfo book_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=book_background) &JournalUI::book_background
- [`TextureRenderingInfo arrow_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_left) &JournalUI::arrow_left
- [`TextureRenderingInfo arrow_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=arrow_right) &JournalUI::arrow_right
- [`TextureRenderingInfo unknown23`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown23) &JournalUI::unknown23
- [`TextureRenderingInfo entire_book`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entire_book) &JournalUI::entire_book
- [`int page_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=page_timer) &JournalUI::page_timer
### `ScreenArenaMenu`
Derived from [`Screen`](#screen)
- [`ScreenZoomAnimation brick_background_animation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_background_animation) &ScreenArenaMenu::brick_background_animation
- [`ScreenZoomAnimation blurry_border_animation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurry_border_animation) &ScreenArenaMenu::blurry_border_animation
- [`float top_woodpanel_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel_slidein_timer) &ScreenArenaMenu::top_woodpanel_slidein_timer
- [`float top_scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_scroll_unfurl_timer) &ScreenArenaMenu::top_scroll_unfurl_timer
- [`TextureRenderingInfo unknown13`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown13) &ScreenArenaMenu::unknown13
- [`TextureRenderingInfo woodpanel_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top) &ScreenArenaMenu::woodpanel_top
- [`TextureRenderingInfo unknown15`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown15) &ScreenArenaMenu::unknown15
- [`TextureRenderingInfo left_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_scroll_handle) &ScreenArenaMenu::left_scroll_handle
- [`TextureRenderingInfo right_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_scroll_handle) &ScreenArenaMenu::right_scroll_handle
- [`STRINGID scroll_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_text_id) &ScreenArenaMenu::scroll_text_id
- [`STRINGID unknown17_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown17_text_id) &ScreenArenaMenu::unknown17_text_id
- [`STRINGID unknown18_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown18_text_id) &ScreenArenaMenu::unknown18_text_id
- [`STRINGID unknown19_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown19_text_id) &ScreenArenaMenu::unknown19_text_id
- [`bool top_woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_woodpanel_visible) &ScreenArenaMenu::top_woodpanel_visible
- [`bool bottom_woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_woodpanel_visible) &ScreenArenaMenu::bottom_woodpanel_visible
- [`bool woodpanels_toggle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanels_toggle) &ScreenArenaMenu::woodpanels_toggle
- [`TextureRenderingInfo brick_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_background) &ScreenArenaMenu::brick_background
- [`TextureRenderingInfo blurry_border`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurry_border) &ScreenArenaMenu::blurry_border
- [`TextureRenderingInfo blurry_border2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blurry_border2) &ScreenArenaMenu::blurry_border2
- [`TextureRenderingInfo characters_drawing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=characters_drawing) &ScreenArenaMenu::characters_drawing
- [`TextureRenderingInfo info_black_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=info_black_background) &ScreenArenaMenu::info_black_background
- [`TextureRenderingInfo main_panel_top_left_corner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_top_left_corner) &ScreenArenaMenu::main_panel_top_left_corner
- [`TextureRenderingInfo main_panel_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_top) &ScreenArenaMenu::main_panel_top
- [`TextureRenderingInfo main_panel_top_right_corner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_top_right_corner) &ScreenArenaMenu::main_panel_top_right_corner
- [`TextureRenderingInfo main_panel_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_left) &ScreenArenaMenu::main_panel_left
- [`TextureRenderingInfo main_panel_center`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_center) &ScreenArenaMenu::main_panel_center
- [`TextureRenderingInfo main_panel_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_right) &ScreenArenaMenu::main_panel_right
- [`TextureRenderingInfo main_panel_bottom_left_corner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_bottom_left_corner) &ScreenArenaMenu::main_panel_bottom_left_corner
- [`TextureRenderingInfo main_panel_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_bottom) &ScreenArenaMenu::main_panel_bottom
- [`TextureRenderingInfo main_panel_bottom_right_corner`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_bottom_right_corner) &ScreenArenaMenu::main_panel_bottom_right_corner
- [`TextureRenderingInfo rules_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rules_scroll) &ScreenArenaMenu::rules_scroll
- [`TextureRenderingInfo black_option_boxes_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=black_option_boxes_left) &ScreenArenaMenu::black_option_boxes_left
- [`TextureRenderingInfo black_option_boxes_center`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=black_option_boxes_center) &ScreenArenaMenu::black_option_boxes_center
- [`TextureRenderingInfo black_option_boxes_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=black_option_boxes_right) &ScreenArenaMenu::black_option_boxes_right
- [`TextureRenderingInfo gold_option_outline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=gold_option_outline) &ScreenArenaMenu::gold_option_outline
- [`TextureRenderingInfo option_icons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=option_icons) &ScreenArenaMenu::option_icons
- [`TextureRenderingInfo option_left_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=option_left_arrow) &ScreenArenaMenu::option_left_arrow
- [`TextureRenderingInfo option_right_arrow`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=option_right_arrow) &ScreenArenaMenu::option_right_arrow
- [`TextureRenderingInfo bottom_left_bricks`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_left_bricks) &ScreenArenaMenu::bottom_left_bricks
- [`TextureRenderingInfo top_left_esc_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_left_esc_panel) &ScreenArenaMenu::top_left_esc_panel
- [`TextureRenderingInfo next_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_panel) &ScreenArenaMenu::next_panel
- [`float center_panels_hor_slide_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=center_panels_hor_slide_position) &ScreenArenaMenu::center_panels_hor_slide_position
- [`float esc_next_panels_slide_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=esc_next_panels_slide_timer) &ScreenArenaMenu::esc_next_panels_slide_timer
- [`float main_panel_vertical_scroll_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=main_panel_vertical_scroll_position) &ScreenArenaMenu::main_panel_vertical_scroll_position
- [`int selected_option_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_option_index) &ScreenArenaMenu::selected_option_index
### `ScreenZoomAnimation`
- [`float zoom_target`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=zoom_target) &ScreenZoomAnimation::zoom_target
### `ScreenArenaStagesSelect`
Derived from [`Screen`](#screen)
- [`float woodenpanel_top_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_top_slidein_timer) &ScreenArenaStagesSelect::woodenpanel_top_slidein_timer
- [`float woodenpanel_top_scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_top_scroll_unfurl_timer) &ScreenArenaStagesSelect::woodenpanel_top_scroll_unfurl_timer
- [`TextureRenderingInfo woodenpanel_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_top) &ScreenArenaStagesSelect::woodenpanel_top
- [`TextureRenderingInfo woodenpanel_top_left_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_top_left_scroll) &ScreenArenaStagesSelect::woodenpanel_top_left_scroll
- [`TextureRenderingInfo woodenpanel_top_right_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_top_right_scroll) &ScreenArenaStagesSelect::woodenpanel_top_right_scroll
- [`STRINGID text_id_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_1) &ScreenArenaStagesSelect::text_id_1
- [`STRINGID text_id_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_2) &ScreenArenaStagesSelect::text_id_2
- [`STRINGID text_id_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_3) &ScreenArenaStagesSelect::text_id_3
- [`STRINGID text_id_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_4) &ScreenArenaStagesSelect::text_id_4
- [`bool woodenpanel_top_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_top_visible) &ScreenArenaStagesSelect::woodenpanel_top_visible
- [`bool woodenpanel_bottom_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_bottom_visible) &ScreenArenaStagesSelect::woodenpanel_bottom_visible
- [`bool woodenpanels_toggle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanels_toggle) &ScreenArenaStagesSelect::woodenpanels_toggle
- [`int buttons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &ScreenArenaStagesSelect::buttons
- [`TextureRenderingInfo brick_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_background) &ScreenArenaStagesSelect::brick_background
- [`TextureRenderingInfo info_black_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=info_black_background) &ScreenArenaStagesSelect::info_black_background
- [`TextureRenderingInfo woodenpanel_center`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodenpanel_center) &ScreenArenaStagesSelect::woodenpanel_center
- [`TextureRenderingInfo blocky_level_representation`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=blocky_level_representation) &ScreenArenaStagesSelect::blocky_level_representation
- [`TextureRenderingInfo theme_indicator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=theme_indicator) &ScreenArenaStagesSelect::theme_indicator
- [`TextureRenderingInfo bricks_bottom_left`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bricks_bottom_left) &ScreenArenaStagesSelect::bricks_bottom_left
- [`TextureRenderingInfo grid_background_row_0`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_0) &ScreenArenaStagesSelect::grid_background_row_0
- [`TextureRenderingInfo grid_background_row_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_1) &ScreenArenaStagesSelect::grid_background_row_1
- [`TextureRenderingInfo grid_background_row_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_2) &ScreenArenaStagesSelect::grid_background_row_2
- [`TextureRenderingInfo grid_background_row_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_3) &ScreenArenaStagesSelect::grid_background_row_3
- [`TextureRenderingInfo grid_background_row_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_4) &ScreenArenaStagesSelect::grid_background_row_4
- [`TextureRenderingInfo grid_background_row_5`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_5) &ScreenArenaStagesSelect::grid_background_row_5
- [`TextureRenderingInfo grid_background_row_6`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_6) &ScreenArenaStagesSelect::grid_background_row_6
- [`TextureRenderingInfo grid_background_row_7`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_row_7) &ScreenArenaStagesSelect::grid_background_row_7
- [`TextureRenderingInfo grid_background_disabled_cross`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_disabled_cross) &ScreenArenaStagesSelect::grid_background_disabled_cross
- [`TextureRenderingInfo grid_background_manipulators`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_background_manipulators) &ScreenArenaStagesSelect::grid_background_manipulators
- [`TextureRenderingInfo unknown21`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown21) &ScreenArenaStagesSelect::unknown21
- [`TextureRenderingInfo grid_disabled_cross`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_disabled_cross) &ScreenArenaStagesSelect::grid_disabled_cross
- [`TextureRenderingInfo grid_yellow_highlighter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_yellow_highlighter) &ScreenArenaStagesSelect::grid_yellow_highlighter
- [`TextureRenderingInfo woodpanel_esc`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_esc) &ScreenArenaStagesSelect::woodpanel_esc
- [`TextureRenderingInfo woodpanel_fight`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_fight) &ScreenArenaStagesSelect::woodpanel_fight
- [`TextureRenderingInfo big_player_drawing`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=big_player_drawing) &ScreenArenaStagesSelect::big_player_drawing
- [`TextureRenderingInfo players_turn_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players_turn_scroll) &ScreenArenaStagesSelect::players_turn_scroll
- [`TextureRenderingInfo players_turn_scroll_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players_turn_scroll_handle) &ScreenArenaStagesSelect::players_turn_scroll_handle
- [`TextureRenderingInfo grid_player_icon`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grid_player_icon) &ScreenArenaStagesSelect::grid_player_icon
### `ScreenArenaItems`
Derived from [`Screen`](#screen)
- [`float woodpanel_top_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top_slidein_timer) &ScreenArenaItems::woodpanel_top_slidein_timer
- [`float woodpanel_top_scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top_scroll_unfurl_timer) &ScreenArenaItems::woodpanel_top_scroll_unfurl_timer
- [`TextureRenderingInfo unknown9`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown9) &ScreenArenaItems::unknown9
- [`TextureRenderingInfo woodpanel_top`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top) &ScreenArenaItems::woodpanel_top
- [`TextureRenderingInfo unknown11`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown11) &ScreenArenaItems::unknown11
- [`TextureRenderingInfo top_scroll_left_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_scroll_left_handle) &ScreenArenaItems::top_scroll_left_handle
- [`TextureRenderingInfo top_scroll_right_handle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=top_scroll_right_handle) &ScreenArenaItems::top_scroll_right_handle
- [`STRINGID scroll_text_id`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_text_id) &ScreenArenaItems::scroll_text_id
- [`STRINGID text_id_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_2) &ScreenArenaItems::text_id_2
- [`STRINGID text_id_3`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_3) &ScreenArenaItems::text_id_3
- [`STRINGID text_id_4`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_4) &ScreenArenaItems::text_id_4
- [`bool woodpanel_top_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_top_visible) &ScreenArenaItems::woodpanel_top_visible
- [`bool woodpanel_bottom_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottom_visible) &ScreenArenaItems::woodpanel_bottom_visible
- [`bool woodpanels_toggle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanels_toggle) &ScreenArenaItems::woodpanels_toggle
- [`TextureRenderingInfo brick_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=brick_background) &ScreenArenaItems::brick_background
- [`TextureRenderingInfo black_background_bottom_right`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=black_background_bottom_right) &ScreenArenaItems::black_background_bottom_right
- [`TextureRenderingInfo woodpanel_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_bottom) &ScreenArenaItems::woodpanel_bottom
- [`TextureRenderingInfo scroll_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_bottom) &ScreenArenaItems::scroll_bottom
- [`TextureRenderingInfo scroll_right_handle_bottom`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_right_handle_bottom) &ScreenArenaItems::scroll_right_handle_bottom
- [`TextureRenderingInfo held_item_crate_on_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=held_item_crate_on_scroll) &ScreenArenaItems::held_item_crate_on_scroll
- [`TextureRenderingInfo held_item_on_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=held_item_on_scroll) &ScreenArenaItems::held_item_on_scroll
- [`TextureRenderingInfo item_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_background) &ScreenArenaItems::item_background
- [`TextureRenderingInfo toggles_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toggles_background) &ScreenArenaItems::toggles_background
- [`TextureRenderingInfo item_selection_gold_outline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_selection_gold_outline) &ScreenArenaItems::item_selection_gold_outline
- [`TextureRenderingInfo item_icons`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_icons) &ScreenArenaItems::item_icons
- [`TextureRenderingInfo item_held_badge`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_held_badge) &ScreenArenaItems::item_held_badge
- [`TextureRenderingInfo item_equipped_badge`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_equipped_badge) &ScreenArenaItems::item_equipped_badge
- [`TextureRenderingInfo item_off_gray_overlay`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=item_off_gray_overlay) &ScreenArenaItems::item_off_gray_overlay
- [`TextureRenderingInfo esc_woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=esc_woodpanel) &ScreenArenaItems::esc_woodpanel
- [`float center_panels_horizontal_slide_position`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=center_panels_horizontal_slide_position) &ScreenArenaItems::center_panels_horizontal_slide_position
- [`float esc_panel_slide_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=esc_panel_slide_timer) &ScreenArenaItems::esc_panel_slide_timer
- [`int selected_item_index`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=selected_item_index) &ScreenArenaItems::selected_item_index
### `ScreenArenaIntro`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players) &ScreenArenaIntro::players
- [`TextureRenderingInfo background_colors`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=background_colors) &ScreenArenaIntro::background_colors
- [`TextureRenderingInfo vertical_lines`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_lines) &ScreenArenaIntro::vertical_lines
- [`TextureRenderingInfo vertical_line_electricity_effect`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=vertical_line_electricity_effect) &ScreenArenaIntro::vertical_line_electricity_effect
- [`TextureRenderingInfo unknown_all_forced`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown_all_forced) &ScreenArenaIntro::unknown_all_forced
- [`TextureRenderingInfo left_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=left_scroll) &ScreenArenaIntro::left_scroll
- [`TextureRenderingInfo right_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=right_scroll) &ScreenArenaIntro::right_scroll
- [`float scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_unfurl_timer) &ScreenArenaIntro::scroll_unfurl_timer
- [`bool waiting`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waiting) &ScreenArenaIntro::waiting
- [`float names_opacity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=names_opacity) &ScreenArenaIntro::names_opacity
- [`float line_electricity_effect_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=line_electricity_effect_timer) &ScreenArenaIntro::line_electricity_effect_timer
- [`int state`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state) &ScreenArenaIntro::state
- [`int countdown`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=countdown) &ScreenArenaIntro::countdown
### `ScreenArenaLevel`
Derived from [`Screen`](#screen)
- [`TextureRenderingInfo get_ready`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ready) &ScreenArenaLevel::get_ready
- [`TextureRenderingInfo get_ready_gray_background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ready_gray_background) &ScreenArenaLevel::get_ready_gray_background
- [`TextureRenderingInfo get_ready_outline`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ready_outline) &ScreenArenaLevel::get_ready_outline
### `ScreenArenaScore`
Derived from [`Screen`](#screen)
- [`float woodpanel_slide_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_slide_timer) &ScreenArenaScore::woodpanel_slide_timer
- [`float scroll_unfurl_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=scroll_unfurl_timer) &ScreenArenaScore::scroll_unfurl_timer
- [`TextureRenderingInfo unknown10`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown10) &ScreenArenaScore::unknown10
- [`TextureRenderingInfo woodpanel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel) &ScreenArenaScore::woodpanel
- [`TextureRenderingInfo unknown_all_forced`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown_all_forced) &ScreenArenaScore::unknown_all_forced
- [`TextureRenderingInfo woodpanel_left_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_left_scroll) &ScreenArenaScore::woodpanel_left_scroll
- [`TextureRenderingInfo woodpanel_right_scroll`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_right_scroll) &ScreenArenaScore::woodpanel_right_scroll
- [`STRINGID text_id_1`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_1) &ScreenArenaScore::text_id_1
- [`STRINGID text_id_2`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=text_id_2) &ScreenArenaScore::text_id_2
- [`bool woodpanel_visible`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_visible) &ScreenArenaScore::woodpanel_visible
- [`bool woodpanel_slide_toggle`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=woodpanel_slide_toggle) &ScreenArenaScore::woodpanel_slide_toggle
- [`int animation_sequence`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_sequence) &ScreenArenaScore::animation_sequence
- [`TextureRenderingInfo background`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=background) &ScreenArenaScore::background
- [`TextureRenderingInfo ok_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ok_panel) &ScreenArenaScore::ok_panel
- [`TextureRenderingInfo ready_panel`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ready_panel) &ScreenArenaScore::ready_panel
- [`TextureRenderingInfo ready_speechbubble_indicator`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ready_speechbubble_indicator) &ScreenArenaScore::ready_speechbubble_indicator
- [`TextureRenderingInfo pillars`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pillars) &ScreenArenaScore::pillars
- [`TextureRenderingInfo bottom_lava`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=bottom_lava) &ScreenArenaScore::bottom_lava
- [`TextureRenderingInfo players`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players) &ScreenArenaScore::players
- [`TextureRenderingInfo player_shadows`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_shadows) &ScreenArenaScore::player_shadows
- [`TextureRenderingInfo unknown24`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown24) &ScreenArenaScore::unknown24
- [`TextureRenderingInfo unknown25`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown25) &ScreenArenaScore::unknown25
- [`TextureRenderingInfo score_counter`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=score_counter) &ScreenArenaScore::score_counter
- [`TextureRenderingInfo unknown27`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unknown27) &ScreenArenaScore::unknown27
- [`TextureRenderingInfo lava_bubbles`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lava_bubbles) &ScreenArenaScore::lava_bubbles
- [`array<bool, MAX_PLAYERS> player_won`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_won) &ScreenArenaScore::player_won
- [`float victory_jump_y_pos`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=victory_jump_y_pos) &ScreenArenaScore::victory_jump_y_pos
- [`float victory_jump_velocity`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=victory_jump_velocity) &ScreenArenaScore::victory_jump_velocity
- [`int animation_frame`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=animation_frame) &ScreenArenaScore::animation_frame
- [`bool squash_and_celebrate`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=squash_and_celebrate) &ScreenArenaScore::squash_and_celebrate
- [`array<bool, MAX_PLAYERS> player_ready`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_ready) &ScreenArenaScore::player_ready
- [`int next_transition_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_transition_timer) &ScreenArenaScore::next_transition_timer
- [`array<float, MAX_PLAYERS> player_bottom_pillar_offset`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_bottom_pillar_offset) &ScreenArenaScore::player_bottom_pillar_offset
- [`array<float, MAX_PLAYERS> player_crushing_pillar_height`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_crushing_pillar_height) &ScreenArenaScore::player_crushing_pillar_height
- [`array<bool, MAX_PLAYERS> player_create_giblets`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=player_create_giblets) &ScreenArenaScore::player_create_giblets
- [`float next_sidepanel_slidein_timer`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=next_sidepanel_slidein_timer) &ScreenArenaScore::next_sidepanel_slidein_timer
## Automatic casting of entities
When using `get_entity()` the returned entity will automatically be of the correct type. It is not necessary to use the `as_<typename>` functions.

To figure out what type of entity you get back, consult the [entity hierarchy list](entities-hierarchy.md)

For reference, the available `as_<typename>` functions are listed below:
- as_acidbubble
- as_alien
- as_altar
- as_ammit
- as_ankhpowerup
- as_anubis
- as_apephead
- as_apeppart
- as_arrow
- as_arrowtrap
- as_axolotl
- as_axolotlshot
- as_backpack
- as_bat
- as_bee
- as_beg
- as_bigspeartrap
- as_bodyguard
- as_bomb
- as_boneblock
- as_boombox
- as_boomerang
- as_boulder
- as_bullet
- as_cape
- as_catmummy
- as_caveman
- as_cavemanshopkeeper
- as_chain
- as_chainedpushblock
- as_chest
- as_cityofgolddoor
- as_clambase
- as_claw
- as_climbablerope
- as_clonegunshot
- as_cobra
- as_coffin
- as_coin
- as_container
- as_conveyorbelt
- as_cookfire
- as_crabman
- as_critter
- as_critterbeetle
- as_critterbutterfly
- as_crittercrab
- as_critterdrone
- as_critterfirefly
- as_critterfish
- as_critterlocust
- as_critterpenguin
- as_critterslime
- as_crittersnail
- as_crocman
- as_crushtrap
- as_cursedpot
- as_decorateddoor
- as_door
- as_drill
- as_eggplantminister
- as_eggsac
- as_eggshipdoor
- as_eggshipdoors
- as_elevator
- as_empressgrave
- as_entity
- as_excalibur
- as_exitdoor
- as_fallingplatform
- as_fireball
- as_firebug
- as_firebugunchained
- as_firefrog
- as_fish
- as_flame
- as_flamesize
- as_floor
- as_fly
- as_flyhead
- as_forcefield
- as_forestsister
- as_frog
- as_frozenliquid
- as_generator
- as_ghist
- as_ghost
- as_ghostbreath
- as_giantclamtop
- as_giantfish
- as_giantfly
- as_giantfrog
- as_goldbar
- as_goldmonkey
- as_grub
- as_gun
- as_hanganchor
- as_hangspider
- as_hangstrand
- as_hermitcrab
- as_honey
- as_horizontalforcefield
- as_hornedlizard
- as_hoverpack
- as_hundun
- as_hundunchest
- as_hundunhead
- as_idol
- as_imp
- as_jetpack
- as_jiangshi
- as_jumpdog
- as_junglespearcosmetic
- as_kapalapowerup
- as_kingu
- as_lahamu
- as_lamassu
- as_lampflame
- as_landmine
- as_laserbeam
- as_lasertrap
- as_lava
- as_lavaglow
- as_lavamander
- as_leaf
- as_leprechaun
- as_lightarrow
- as_lightarrowplatform
- as_lightemitter
- as_lightshot
- as_liquid
- as_lockeddoor
- as_magmaman
- as_mainexit
- as_mantrap
- as_mattock
- as_mech
- as_megajellyfish
- as_minigameasteroid
- as_minigameship
- as_mole
- as_monkey
- as_monster
- as_mosquito
- as_motherstatue
- as_mount
- as_movable
- as_mummy
- as_necromancer
- as_npc
- as_octopus
- as_olmec
- as_olmeccannon
- as_olmecfloater
- as_olmite
- as_orb
- as_osirishand
- as_osirishead
- as_parachutepowerup
- as_pet
- as_pipe
- as_player
- as_playerbag
- as_playerghost
- as_poledeco
- as_pot
- as_powerup
- as_powerupcapable
- as_protoshopkeeper
- as_punishball
- as_pushblock
- as_qilin
- as_quicksand
- as_quillback
- as_regenblock
- as_robot
- as_rockdog
- as_rollingitem
- as_roomowner
- as_scarab
- as_sceptershot
- as_scorpion
- as_shield
- as_shopkeeper
- as_skeleton
- as_skulldroptrap
- as_slidingwallceiling
- as_snaptrap
- as_sorceress
- as_soundshot
- as_spark
- as_sparktrap
- as_spear
- as_specialshot
- as_spider
- as_spikeballtrap
- as_stickytrap
- as_stretchchain
- as_switch
- as_tadpole
- as_teleporter
- as_teleporterbackpack
- as_teleportingborder
- as_telescope
- as_tentacle
- as_tentaclebottom
- as_terra
- as_thinice
- as_tiamat
- as_tiamatshot
- as_timedforcefield
- as_timedpowderkeg
- as_timedshot
- as_torch
- as_torchflame
- as_totemtrap
- as_transferfloor
- as_trappart
- as_treasure
- as_treasurehook
- as_truecrownpowerup
- as_tun
- as_tv
- as_udjatsocket
- as_ufo
- as_unchainedspikeball
- as_ushabti
- as_vampire
- as_vanhorsing
- as_vlad
- as_vladscape
- as_waddler
- as_walkingmonster
- as_walltorch
- as_webgun
- as_webshot
- as_witchdoctor
- as_witchdoctorskull
- as_woodenlogtrap
- as_yama
- as_yang
- as_yellowcape
- as_yetiking
- as_yetiqueen
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
- ...check [entities.txt](game_data/entities.txt)...
- [`LIQUID_STAGNANT_LAVA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.LIQUID_STAGNANT_LAVA) 898
### PARTICLEEMITTER
- [`TITLE_TORCHFLAME_SMOKE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.TITLE_TORCHFLAME_SMOKE) 1
- ...check [particle_emitters.txt](game_data/particle_emitters.txt)...
- [`MINIGAME_BROKENASTEROID_SMOKE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.MINIGAME_BROKENASTEROID_SMOKE) 219
### TILE_CODE
- [`EMPTY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TILE_CODE.EMPTY) 0
- ...check [tile_codes.txt](game_data/tile_codes.txt)...
### ROOM_TEMPLATE
- [`SIDE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE.SIDE) 0
- ...check [room_templates.txt](game_data/room_templates.txt)...
### PROCEDURAL_CHANCE
- [`ARROWTRAP_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PROCEDURAL_CHANCE.ARROWTRAP_CHANCE) 0
- ...check [spawn_chances.txt](game_data/spawn_chances.txt)...
### VANILLA_SOUND
- [`BGM_BGM_TITLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.BGM_BGM_TITLE) BGM/BGM_title
- ...check [vanilla_sounds.txt](game_data/vanilla_sounds.txt)...
- [`FX_FX_DM_BANNER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.FX_FX_DM_BANNER) FX/FX_dm_banner
### VANILLA_SOUND_PARAM
- [`POS_SCREEN_X`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.POS_SCREEN_X) 0
- ...check [vanilla_sound_params.txt](game_data/vanilla_sound_params.txt)...
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
- ...check [textures.txt](game_data/textures.txt)...
- [`DATA_TEXTURES_SHINE_0`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_SHINE_0) 388
- [`DATA_TEXTURES_OLDTEXTURES_AI_0`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_OLDTEXTURES_AI_0) 389
### INPUTS
- [`NONE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.NONE) 0
- [`JUMP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.JUMP) 1
- [`WHIP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.WHIP) 2
- [`BOMB`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.BOMB) 4
- [`ROPE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.ROPE) 8
- [`RUN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.RUN) 16
- [`DOOR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.DOOR) 32
- [`MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.MENU) 64
- [`JOURNAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.JOURNAL) 128
- [`LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.LEFT) 256
- [`RIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.RIGHT) 512
- [`UP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.UP) 1024
- [`DOWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.DOWN) 2048
### ON
- [`LOGO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOGO) ON::LOGO
- [`INTRO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.INTRO) ON::INTRO
- [`PROLOGUE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PROLOGUE) ON::PROLOGUE
- [`TITLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TITLE) ON::TITLE
- [`MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.MENU) ON::MENU
- [`OPTIONS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.OPTIONS) ON::OPTIONS
- [`LEADERBOARD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEADERBOARD) ON::LEADERBOARD
- [`SEED_INPUT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SEED_INPUT) ON::SEED_INPUT
- [`CHARACTER_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CHARACTER_SELECT) ON::CHARACTER_SELECT
- [`TEAM_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TEAM_SELECT) ON::TEAM_SELECT
- [`CAMP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CAMP) ON::CAMP
- [`LEVEL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEVEL) ON::LEVEL
- [`TRANSITION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TRANSITION) ON::TRANSITION
- [`DEATH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH) ON::DEATH
- [`SPACESHIP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPACESHIP) ON::SPACESHIP
- [`WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.WIN) ON::WIN
- [`CREDITS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CREDITS) ON::CREDITS
- [`SCORES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCORES) ON::SCORES
- [`CONSTELLATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CONSTELLATION) ON::CONSTELLATION
- [`RECAP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RECAP) ON::RECAP
- [`ARENA_MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MENU) ON::ARENA_MENU
- [`ARENA_INTRO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_INTRO) ON::ARENA_INTRO
- [`ARENA_MATCH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MATCH) ON::ARENA_MATCH
- [`ARENA_SCORE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_SCORE) ON::ARENA_SCORE
- [`ONLINE_LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOADING) ON::ONLINE_LOADING
- [`ONLINE_LOBBY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOBBY) ON::ONLINE_LOBBY
- [`GUIFRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GUIFRAME) ON::GUIFRAME
\
Params: `GuiDrawContext draw_ctx`\
Runs every frame the game is rendered, thus runs at selected framerate. Drawing functions are only available during this callback through a `GuiDrawContext`
- [`FRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.FRAME) ON::FRAME
\
Runs while playing the game while the player is controllable, not in the base camp or the arena mode
- [`GAMEFRAME`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GAMEFRAME) ON::GAMEFRAME
\
Runs whenever the game engine is actively running. This includes base camp, arena, level transition and death screen
- [`SCREEN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCREEN) ON::SCREEN
\
Runs whenever state.screen changes
- [`START`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.START) ON::START
\
Runs on the first ON.SCREEN of a run
- [`LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOADING) ON::LOADING
- [`RESET`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RESET) ON::RESET
\
Runs when resetting a run
- [`SAVE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SAVE) ON::SAVE
\
Params: `SaveContext save_ctx`\
Runs at the same times as ON.SCREEN, but receives the save_ctx
- [`LOAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOAD) ON::LOAD
\
Params: `LoadContext load_ctx`\
Runs as soon as your script is loaded, including reloads, then never again
- [`PRE_LOAD_LEVEL_FILES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_LEVEL_FILES) ON::PRE_LOAD_LEVEL_FILES
\
Params: `PreLoadLevelFilesContext load_level_ctx`\
Runs right before level files would be loaded
- [`PRE_LEVEL_GENERATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LEVEL_GENERATION) ON::PRE_LEVEL_GENERATION
\
Runs before any level generation, no entities should exist at this point
- [`POST_ROOM_GENERATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_ROOM_GENERATION) ON::POST_ROOM_GENERATION
\
Params: `PostRoomGenerationContext room_gen_ctx`\
Runs right after all rooms are generated before entities are spawned
- [`POST_LEVEL_GENERATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LEVEL_GENERATION) ON::POST_LEVEL_GENERATION
\
Runs right level generation is done, before any entities are updated
- [`PRE_GET_RANDOM_ROOM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_GET_RANDOM_ROOM) ON::PRE_GET_RANDOM_ROOM
\
Params: `int x,::int y, LAYER layer, ROOM_TEMPLATE room_template`\
Return: `string room_data`\
Called when the game wants to get a random room for a given template. Return a string that represents a room template to make the game use that.\
If the size of the string returned does not match with the room templates expected size the room is discarded.\
White spaces at the beginning and end of the string are stripped, not at the beginning and end of each line.
- [`PRE_HANDLE_ROOM_TILES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_HANDLE_ROOM_TILES) ON::PRE_HANDLE_ROOM_TILES
\
Params: `int x, int y, ROOM_TEMPLATE room_template, PreHandleRoomTilesContext room_ctx`\
Return: `bool last_callback` to determine whether callbacks of the same type should be executed after this\
Runs after a random room was selected and right before it would spawn entities for each tile code\
Allows you to modify the rooms content in the front and back layer as well as add a backlayer if not yet existant
- [`SCRIPT_ENABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_ENABLE) ON::SCRIPT_ENABLE
- [`SCRIPT_DISABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_DISABLE) ON::SCRIPT_DISABLE
- [`RENDER_PRE_HUD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_HUD) ON::RENDER_PRE_HUD
\
Params: `VanillaRenderContext render_ctx`\
Runs before the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx
- [`RENDER_POST_HUD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_HUD) ON::RENDER_POST_HUD
\
Params: `VanillaRenderContext render_ctx`\
Runs after the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx
- [`RENDER_PRE_PAUSE_MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_PAUSE_MENU) ON::RENDER_PRE_PAUSE_MENU
\
Params: `VanillaRenderContext render_ctx`\
Runs before the pause menu is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx
- [`RENDER_POST_PAUSE_MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_PAUSE_MENU) ON::RENDER_POST_PAUSE_MENU
\
Params: `VanillaRenderContext render_ctx`\
Runs after the pause menu is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx
- [`RENDER_PRE_DRAW_DEPTH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_DRAW_DEPTH) ON::RENDER_PRE_DRAW_DEPTH
\
Params: `VanillaRenderContext render_ctx, int draw_depth`\
Runs before the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx
### SPAWN_TYPE
- [`LEVEL_GEN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN) SPAWN_TYPE_LEVEL_GEN
\
For any spawn happening during level generation, even if the call happened from the Lua API during a tile code callback.
- [`LEVEL_GEN_TILE_CODE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_TILE_CODE) SPAWN_TYPE_LEVEL_GEN_TILE_CODE
\
Similar to LEVEL_GEN but only triggers on tile code spawns.
- [`LEVEL_GEN_PROCEDURAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_PROCEDURAL) SPAWN_TYPE_LEVEL_GEN_PROCEDURAL
\
Similar to LEVEL_GEN but only triggers on random level spawns, like snakes or bats.
- [`LEVEL_GEN_FLOOR_SPREADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_FLOOR_SPREADING) SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING
\
Only procs during floor spreading, both horizontal and vertical
- [`LEVEL_GEN_GENERAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_GENERAL) SPAWN_TYPE_LEVEL_GEN_GENERAL
\
Covers all spawns during level gen that are not covered by the other two.
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
Some arbitrary constants of the engine
- [`ENGINE_FPS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ENGINE_FPS) 60
\
The framerate at which the engine updates, e.g. at which `ON.GAMEFRAME` and similar are called.\
Independent of rendering framerate, so it does not correlate with the call rate of `ON.GUIFRAME` and similar.
- [`ROOM_WIDTH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ROOM_WIDTH) 10
\
Width of a 1x1 room, both in world coordinates and in tiles.
- [`ROOM_HEIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ROOM_HEIGHT) 8
\
Height of a 1x1 room, both in world coordinates and in tiles.
### WIN_STATE
After setting the WIN_STATE, the exit door on the current level will lead to the chosen ending
- [`NO_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.NO_WIN) 0
- [`TIAMAT_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.TIAMAT_WIN) 1
- [`HUNDUN_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.HUNDUN_WIN) 2
- [`COSMIC_OCEAN_WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.COSMIC_OCEAN_WIN) 3
### VANILLA_TEXT_ALIGNMENT
Used in the `render_ctx:draw_text` and `render_ctx:draw_text_size` functions of the ON.RENDER_PRE/POST_xxx event
- [`LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT.LEFT) 0
- [`CENTER`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT.CENTER) 1
- [`RIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT.RIGHT) 2
### VANILLA_FONT_STYLE
Used in the `render_ctx:draw_text` and `render_ctx:draw_text_size` functions of the ON.RENDER_PRE/POST_xxx event
- [`ITALIC`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_FONT_STYLE.ITALIC) 1
- [`BOLD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_FONT_STYLE.BOLD) 2
### CAUSE_OF_DEATH
- [`DEATH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.DEATH) 0
- [`ENTITY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.ENTITY) 1
- [`LONG_FALL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.LONG_FALL) 2
- [`STILL_FALLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.STILL_FALLING) 3
- [`MISSED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.MISSED) 4
- [`POISONED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.POISONED) 5
### PRNG_CLASS
Determines what class of prng is used, which in turn determines which parts of the game's future prng is affected. See more info at (`PRNG`)[#PRNG]
For example when choosing `PRNG_CLASS.PROCEDURAL_SPAWNS` to generate a random number, random Tiamat spawns will not be affected.
Any integer in the range [0, 9] is a valid class, some are however not documented because of missing information.
- [`PROCEDURAL_SPAWNS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.PROCEDURAL_SPAWNS) PRNG::PROCEDURAL_SPAWNS
- [`PARTICLES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.PARTICLES) PRNG::PARTICLES
- [`ENTITY_VARIATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.ENTITY_VARIATION) PRNG::ENTITY_VARIATION
- [`EXTRA_SPAWNS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.EXTRA_SPAWNS) PRNG::EXTRA_SPAWNS
- [`LEVEL_DECO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.LEVEL_DECO) PRNG::LEVEL_DECO
### REPEAT_TYPE
- [`NO_REPEAT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE.NO_REPEAT) REPEAT_TYPE::NoRepeat
- [`LINEAR`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE.LINEAR) REPEAT_TYPE::Linear
- [`BACK_AND_FORTH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE.BACK_AND_FORTH) REPEAT_TYPE::BackAndForth
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
- [`LIQUID`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.LIQUID) 0x6000
- [`ANY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ANY) 0x0
### FLOOR_SIDE
- [`TOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.TOP) FLOOR_SIDE::TOP
- [`BOTTOM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.BOTTOM) FLOOR_SIDE::BOTTOM
- [`LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.LEFT) FLOOR_SIDE::LEFT
- [`RIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.RIGHT) FLOOR_SIDE::RIGHT
- [`TOP_LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.TOP_LEFT) FLOOR_SIDE::TOP_LEFT
- [`TOP_RIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.TOP_RIGHT) FLOOR_SIDE::TOP_RIGHT
- [`BOTTOM_LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.BOTTOM_LEFT) FLOOR_SIDE::BOTTOM_LEFT
- [`BOTTOM_RIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.BOTTOM_RIGHT) FLOOR_SIDE::BOTTOM_RIGHT
### GHOST_BEHAVIOR
- [`SAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SAD) GHOST_BEHAVIOR::SAD
- [`MEDIUM_SAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.MEDIUM_SAD) GHOST_BEHAVIOR::MEDIUM_SAD
- [`MEDIUM_HAPPY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.MEDIUM_HAPPY) GHOST_BEHAVIOR::MEDIUM_HAPPY
- [`SMALL_ANGRY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_ANGRY) GHOST_BEHAVIOR::SMALL_ANGRY
- [`SMALL_SURPRISED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_SURPRISED) GHOST_BEHAVIOR::SMALL_SURPRISED
- [`SMALL_SAD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_SAD) GHOST_BEHAVIOR::SMALL_SAD
- [`SMALL_HAPPY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_HAPPY) GHOST_BEHAVIOR::SMALL_HAPPY
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
- [`BOTH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.BOTH) -128
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
- [`NONE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.NONE) -1
- [`RESET`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.RESET) -1
- [`DWELLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.DWELLING) 0
- [`JUNGLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.JUNGLE) 1
- [`VOLCANA`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.VOLCANA) 2
- [`TIDE_POOL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.TIDE_POOL) 3
- [`TEMPLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.TEMPLE) 4
- [`ICE_CAVES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.ICE_CAVES) 5
- [`NEO_BABYLON`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.NEO_BABYLON) 6
- [`SUNKEN_CITY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.SUNKEN_CITY) 7
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
### ROOM_TEMPLATE_TYPE
Use in `define_room_template` to declare whether a room template has any special behavior
- [`NONE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.NONE) 0
- [`ENTRANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.ENTRANCE) 1
- [`EXIT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.EXIT) 2
- [`SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.SHOP) 3
- [`MACHINE_ROOM`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.MACHINE_ROOM) 4
### SHOP_TYPE
Determines which kind of shop spawns in the level, if any
- [`GENERAL_STORE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.GENERAL_STORE) 0
- [`CLOTHING_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.CLOTHING_SHOP) 1
- [`WEAPON_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.WEAPON_SHOP) 2
- [`SPECIALTY_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.SPECIALTY_SHOP) 3
- [`HIRED_HAND_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.HIRED_HAND_SHOP) 4
- [`PET_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.PET_SHOP) 5
- [`DICE_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.DICE_SHOP) 6
- [`TUSK_DICE_SHOP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.TUSK_DICE_SHOP) 13
### LEVEL_CONFIG
Use with `get_level_config`
- [`BACK_ROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_CHANCE) 0
- [`BACK_ROOM_INTERCONNECTION_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_INTERCONNECTION_CHANCE) 1
- [`BACK_ROOM_HIDDEN_DOOR_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_HIDDEN_DOOR_CHANCE) 2
- [`BACK_ROOM_HIDDEN_DOOR_CACHE_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_HIDDEN_DOOR_CACHE_CHANCE) 3
- [`MOUNT_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MOUNT_CHANCE) 4
- [`ALTAR_ROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.ALTAR_ROOM_CHANCE) 5
- [`IDOL_ROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.IDOL_ROOM_CHANCE) 6
- [`FLOOR_SIDE_SPREAD_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.FLOOR_SIDE_SPREAD_CHANCE) 7
- [`FLOOR_BOTTOM_SPREAD_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.FLOOR_BOTTOM_SPREAD_CHANCE) 8
- [`BACKGROUND_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACKGROUND_CHANCE) 9
- [`GROUND_BACKGROUND_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.GROUND_BACKGROUND_CHANCE) 10
- [`MACHINE_BIGROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_BIGROOM_CHANCE) 11
- [`MACHINE_WIDEROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_WIDEROOM_CHANCE) 12
- [`MACHINE_TALLROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_TALLROOM_CHANCE) 13
- [`MACHINE_REWARDROOM_CHANCE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_REWARDROOM_CHANCE) 14
- [`MAX_LIQUID_PARTICLES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MAX_LIQUID_PARTICLES) 15
- [`FLAGGED_LIQUID_ROOMS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.FLAGGED_LIQUID_ROOMS) 16
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
### ENT_MORE_FLAG
- [`HIRED_HAND_REVIVED`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.HIRED_HAND_REVIVED) 2
- [`SWIMMING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.SWIMMING) 11
- [`FALLING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.FALLING) 12
- [`CURSED_EFFECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.CURSED_EFFECT) 15
- [`DISABLE_INPUT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.DISABLE_INPUT) 16
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
### SCREEN
- [`LOGO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.LOGO) 0
- [`INTRO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.INTRO) 1
- [`PROLOGUE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.PROLOGUE) 2
- [`TITLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.TITLE) 3
- [`MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.MENU) 4
- [`OPTIONS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.OPTIONS) 5
- [`PLAYER_PROFILE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.PLAYER_PROFILE) 6
- [`LEADERBOARD`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.LEADERBOARD) 7
- [`SEED_INPUT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.SEED_INPUT) 8
- [`CHARACTER_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CHARACTER_SELECT) 9
- [`TEAM_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.TEAM_SELECT) 10
- [`CAMP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CAMP) 11
- [`LEVEL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.LEVEL) 12
- [`TRANSITION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.TRANSITION) 13
- [`DEATH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.DEATH) 14
- [`SPACESHIP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.SPACESHIP) 15
- [`WIN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.WIN) 16
- [`CREDITS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CREDITS) 17
- [`SCORES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.SCORES) 18
- [`CONSTELLATION`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CONSTELLATION) 19
- [`RECAP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.RECAP) 20
- [`ARENA_MENU`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_MENU) 21
- [`ARENA_STAGES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_STAGES) 22
- [`ARENA_ITEMS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_ITEMS) 23
- [`ARENA_SELECT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_SELECT) 24
- [`ARENA_INTRO`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_INTRO) 25
- [`ARENA_LEVEL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_LEVEL) 26
- [`ARENA_SCORE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_SCORE) 27
- [`ONLINE_LOADING`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ONLINE_LOADING) 28
- [`ONLINE_LOBBY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ONLINE_LOBBY) 29
### PAUSEUI_VISIBILITY
- [`INVISIBLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.INVISIBLE) 0
- [`SLIDING_DOWN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.SLIDING_DOWN) 1
- [`VISIBLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.VISIBLE) 2
- [`SLIDING_UP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.SLIDING_UP) 3
### JOURNALUI_STATE
- [`INVISIBLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.INVISIBLE) 0
- [`FADING_IN`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FADING_IN) 1
- [`STABLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.STABLE) 2
- [`FLIPPING_LEFT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FLIPPING_LEFT) 3
- [`FLIPPING_RIGHT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FLIPPING_RIGHT) 4
- [`FADING_OUT`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FADING_OUT) 5
### JOURNALUI_PAGE_SHOWN
- [`PLAYER_PROFILE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.PLAYER_PROFILE) 1
- [`JOURNAL`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.JOURNAL) 2
- [`PLACES`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.PLACES) 3
- [`PEOPLE`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.PEOPLE) 4
- [`BESTIARY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.BESTIARY) 5
- [`ITEMS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.ITEMS) 6
- [`TRAPS`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.TRAPS) 7
- [`STORY`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.STORY) 8
- [`RECAP`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.RECAP) 9
- [`DEATH`](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.DEATH) 10
## Aliases
We use those to clarify what kind of values can be passed and returned from a function, even if the underlying type is really just an integer or a string. This should help to avoid bugs where one would for example just pass a random integer to a function expecting a callback id.
### CallbackId == int;
### Flags == int;
### uColor == int;
### SHORT_TILE_CODE == int;
### STRINGID == int;
## External Function Library
If you use a text editor/IDE that has a Lua linter available you can download [spel2.lua](https://raw.githubusercontent.com/spelunky-fyi/overlunky/main/docs/game_data/spel2.lua), place it in a folder of your choice and specify that folder as a "external function library". For example [VSCode](https://code.visualstudio.com/) with the [Lua Extension](https://marketplace.visualstudio.com/items?itemName=sumneko.lua) offers this feature. This will allow you to get auto-completion of API functions along with linting
