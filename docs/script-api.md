# Overlunky Lua API
- Everything here is still changing, don't be sad if your scripts break next week!
- This doc doesn't have a lot of examples, that's why we have [examples/](https://github.com/spelunky-fyi/overlunky/tree/main/examples).
- This doc and the examples are written for a person who already knows [how to program in Lua](http://lua-users.org/wiki/TutorialDirectory).
- This doc is up to date for the [WHIP build](https://github.com/spelunky-fyi/overlunky/releases/tag/whip). If you're using an official release from the past, you might find some things here don't work.
- You can find changes to and earlier versions of this doc [here](https://github.com/spelunky-fyi/overlunky/commits/main/docs/script-api.md).
## Lua libraries
The following Lua libraries and their functions are available. You can read more about them in the [Lua documentation](https://www.lua.org/manual/5.3/manual.html#6).
### `math`
### `base`
### `string`
### `table`
## Global variables
These variables are always there to use.
### [`meta`](/spelunky-fyi/overlunky/search?l=Lua&q=meta)
Table of strings where you should set some script metadata shown in the UI.
- `meta.name` Script name
- `meta.version` Version
- `meta.description` Short description of the script
- `meta.author` Your name
### [`state`](/spelunky-fyi/overlunky/search?l=Lua&q=state)
A bunch of [game state](#statememory) variables
Example:
```
if state.time_level > 300 and state.theme == THEME.DWELLING then
    toast("Congratulations for lasting 5 seconds in Dwelling")
end
```
### [`players`](/spelunky-fyi/overlunky/search?l=Lua&q=players)
An array of [Player](#player) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
### [`options`](/spelunky-fyi/overlunky/search?l=Lua&q=options)
Table of options set in the UI, added with the [register_option_functions](#register_option_int).
## Event functions
Define these in your script to be called on an event. For example:
```
function on_level()
    toast("Welcome to the level")
end
```
### [`on_guiframe`](/spelunky-fyi/overlunky/search?l=Lua&q=on_guiframe)
Runs on every screen frame. You need this to use draw functions.
### [`on_frame`](/spelunky-fyi/overlunky/search?l=Lua&q=on_frame)
Runs on every game engine frame.
### [`on_camp`](/spelunky-fyi/overlunky/search?l=Lua&q=on_camp)
Runs on entering the camp.
### [`on_level`](/spelunky-fyi/overlunky/search?l=Lua&q=on_level)
Runs on the start of every level.
### [`on_start`](/spelunky-fyi/overlunky/search?l=Lua&q=on_start)
Runs on the start of first level.
### [`on_transition`](/spelunky-fyi/overlunky/search?l=Lua&q=on_transition)
Runs on the start of level transition.
### [`on_death`](/spelunky-fyi/overlunky/search?l=Lua&q=on_death)
Runs on the death screen.
### [`on_win`](/spelunky-fyi/overlunky/search?l=Lua&q=on_win)
Runs on any ending cutscene.
### [`on_screen`](/spelunky-fyi/overlunky/search?l=Lua&q=on_screen)
Runs on any [screen change](#on).
## Functions
Note: The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen.
### [`message`](/spelunky-fyi/overlunky/search?l=Lua&q=message)
#### Params: `string message`
Print a log message on screen.
### [`set_interval`](/spelunky-fyi/overlunky/search?l=Lua&q=set_interval)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called every `frames` game frames. Timer is paused on pause and cleared on level transition.
### [`set_timeout`](/spelunky-fyi/overlunky/search?l=Lua&q=set_timeout)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called after `frames` frames. Timer is paused on pause and cleared on level transition.
### [`set_global_interval`](/spelunky-fyi/overlunky/search?l=Lua&q=set_global_interval)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called every `frames` frames. This timer is never paused or cleared.
### [`set_global_timeout`](/spelunky-fyi/overlunky/search?l=Lua&q=set_global_timeout)
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called after `frames` frames. This timer is never paused or cleared.
### [`set_callback`](/spelunky-fyi/overlunky/search?l=Lua&q=set_callback)
#### Params: `function cb, int screen`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called on an [event](#on).
### [`clear_callback`](/spelunky-fyi/overlunky/search?l=Lua&q=clear_callback)
#### Params: `int id`
Clear previously added callback `id`
### [`require`](/spelunky-fyi/overlunky/search?l=Lua&q=require)
#### Params: `string id`
Load another script by id "author/name"
### [`toast`](/spelunky-fyi/overlunky/search?l=Lua&q=toast)
#### Params: `string message`
Show a message that looks like a level feeling.
### [`say`](/spelunky-fyi/overlunky/search?l=Lua&q=say)
#### Params: `int entity_id, string message, int unk_type, bool top`
Show a message coming from an entity
### [`register_option_int`](/spelunky-fyi/overlunky/search?l=Lua&q=register_option_int)
#### Params: `string name, string desc, int value, int min, int max`
Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft limits, you can override them in the UI with double click.
### [`register_option_float`](/spelunky-fyi/overlunky/search?l=Lua&q=register_option_float)
#### Params: `string name, string desc, float value, float min, float max`
Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft limits, you can override them in the UI with double click.
### [`register_option_bool`](/spelunky-fyi/overlunky/search?l=Lua&q=register_option_bool)
#### Params: `string name, string desc, bool value`
Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
### [`register_option_string`](/spelunky-fyi/overlunky/search?l=Lua&q=register_option_string)
#### Params: `string name, string desc, string value`
Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.
### [`register_option_combo`](/spelunky-fyi/overlunky/search?l=Lua&q=register_option_combo)
#### Params: `string name, string desc, string opts`
Add a combobox option that the user can change in the UI. Read with `options.name`, `value` is the default. Separate list options with `\\0`. Return the index of the selection as int.
### [`spawn_entity`](/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity)
#### Params: `int id, float x, float y, int layer, float vx, float vy`
#### Returns: `int`
Spawn an entity in position with some velocity and return the uid of spawned entity.
Uses level coordinates with [LAYER.FRONT](#layer) and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn.
Example:
```
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
### [`spawn`](/spelunky-fyi/overlunky/search?l=Lua&q=spawn)
#### Params: `int id, float x, float y, int layer, float vx, float vy`
#### Returns: `int`
Short for [spawn_entity](#spawn_entity).
### [`spawn_door`](/spelunky-fyi/overlunky/search?l=Lua&q=spawn_door)
#### Params: `float x, float y, int layer, int w, int l, int t`
#### Returns: `int`
Spawn a door to another world, level and theme and return the uid of spawned entity.
Uses level coordinates with LAYER.FRONT and LAYER.BACK, but player-relative coordinates with LAYER.PLAYERn
### [`door`](/spelunky-fyi/overlunky/search?l=Lua&q=door)
#### Params: `float x, float y, int layer, int w, int l, int t`
#### Returns: `int`
Short for [spawn_door](#spawn_door).
### [`spawn_layer_door`](/spelunky-fyi/overlunky/search?l=Lua&q=spawn_layer_door)
#### Params: `float x, float y`
Spawn a door to backlayer
### [`layer_door`](/spelunky-fyi/overlunky/search?l=Lua&q=layer_door)
#### Params: `float x, float y`
Short for [spawn_layer_door](#spawn_layer_door).
### [`god`](/spelunky-fyi/overlunky/search?l=Lua&q=god)
#### Params: `bool g`
Enable/disable godmode
### [`force_dark_level`](/spelunky-fyi/overlunky/search?l=Lua&q=force_dark_level)
#### Params: `bool g`
Try to force next levels to be dark
### [`zoom`](/spelunky-fyi/overlunky/search?l=Lua&q=zoom)
#### Params: `float level`
Set the zoom level used in levels and shops. 13.5 is the default.
### [`pause`](/spelunky-fyi/overlunky/search?l=Lua&q=pause)
#### Params: `bool p`
Enable/disable game engine pause
### [`move_entity`](/spelunky-fyi/overlunky/search?l=Lua&q=move_entity)
#### Params: `int id, float x, float y, float vx, float vy`
Teleport entity to coordinates with optional velocity
### [`set_door_target`](/spelunky-fyi/overlunky/search?l=Lua&q=set_door_target)
#### Params: `int id, int w, int l, int t`
Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
### [`set_door`](/spelunky-fyi/overlunky/search?l=Lua&q=set_door)
#### Params: `int id, int w, int l, int t`
Short for [set_door_target](#set_door_target).
### [`set_contents`](/spelunky-fyi/overlunky/search?l=Lua&q=set_contents)
#### Params: `int id, int item`
Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `id` to ENT_TYPE... `item`
### [`get_entity`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entity)
#### Params: `int id`
#### Returns: `Entity`
Get the [Entity](#entity) behind an uid
### [`get_type`](/spelunky-fyi/overlunky/search?l=Lua&q=get_type)
#### Params: `int id`
#### Returns: `EntityDB`
Get the [EntityDB](#entitydb) behind an uid.
### [`get_entities`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entities)
#### Returns: `array<int>`
Get uids of all entities currently loaded
### [`get_entities_by`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by)
#### Params: `int type, int mask, int layer`
#### Returns: `array<int>`
Get uids of entities by some conditions. Set `type` or `mask` to `0` to ignore that.
### [`get_entities_by_type`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_type)
#### Params: `int, int...`
#### Returns: `array<int>`
Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
You can even pass a table! Example:
```
types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
function on_level()
    uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    -- is not the same thing as this, but also works
    uids2 = get_entities_by_type(types)
    message(tostring(#uids).." == "..tostring(#uids2))
end
```
### [`get_entities_by_mask`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_mask)
#### Params: `int mask`
#### Returns: `array<int>`
Get uids of entities by some search_flags
### [`get_entities_by_layer`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_layer)
#### Params: `int layer`
#### Returns: `array<int>`
Get uids of entities by layer. `0` for main level, `1` for backlayer, `-1` for layer of the player.
### [`get_entities_at`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_at)
#### Params: `int type, int mask, float x, float y, int layer, float r`
#### Returns: `array<int>`
Get uids of matching entities inside some radius. Set `type` or `mask` to `0` to ignore that.
### [`get_entity_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags)
#### Params: `int id`
#### Returns: `int`
Get the `flags` field from entity by uid
### [`set_entity_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags)
#### Params: `int id, int flags`
Set the `flags` field from entity by uid
### [`get_entity_flags2`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags2)
#### Params: `int id`
#### Returns: `int`
Get the `more_flags` field from entity by uid
### [`set_entity_flags2`](/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags2)
#### Params: `int id, int flags`
Set the `more_flags` field from entity by uid
### [`get_entity_ai_state`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_ai_state)
#### Params: `int id`
#### Returns: `int`
Get the `move_state` field from entity by uid
### [`get_hud_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=get_hud_flags)
#### Returns: `int`
Get `state.flags`
### [`set_hud_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=set_hud_flags)
#### Params: `int flags`
Set `state.flags`
### [`get_entity_type`](/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_type)
#### Params: `int id`
#### Returns: `int`
Get the ENT_TYPE... for entity by uid
### [`get_zoom_level`](/spelunky-fyi/overlunky/search?l=Lua&q=get_zoom_level)
#### Returns: `float`
Get the current set zoom level
### [`screen_position`](/spelunky-fyi/overlunky/search?l=Lua&q=screen_position)
#### Params: `float x, float y`
#### Returns: `pair<float, float>`
Translate an entity position to screen position to be used in drawing functions
### [`screen_distance`](/spelunky-fyi/overlunky/search?l=Lua&q=screen_distance)
#### Params: `float x`
#### Returns: `float`
Translate a distance of `x` tiles to screen distance to be be used in drawing functions
### [`get_position`](/spelunky-fyi/overlunky/search?l=Lua&q=get_position)
Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
you're standing on, not real level coordinates.
### [`entity_remove_item`](/spelunky-fyi/overlunky/search?l=Lua&q=entity_remove_item)
#### Params: `int id, int item`
Remove item by uid from entity
### [`spawn_entity_over`](/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_over)
#### Params: `int id, int over, float x, float y`
#### Returns: `int`
Spawn an entity by `id` attached to some other entity `over`, in offset `x`, `y`
### [`entity_has_item_uid`](/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_uid)
#### Params: `int id, int item`
#### Returns: `bool`
Check if the entity `id` has some specific `item` by uid in their inventory
### [`entity_has_item_type`](/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_type)
#### Params: `int id, int type`
#### Returns: `bool`
Check if the entity `id` has some ENT_TYPE `type` in their inventory
### [`kill_entity`](/spelunky-fyi/overlunky/search?l=Lua&q=kill_entity)
#### Params: `int id`
Kills an entity by uid.
### [`apply_entity_db`](/spelunky-fyi/overlunky/search?l=Lua&q=apply_entity_db)
#### Params: `int id`
Apply changes made in [get_type](#get_type)() to entity instance by uid.
### [`lock_door_at`](/spelunky-fyi/overlunky/search?l=Lua&q=lock_door_at)
#### Params: `float x, float y`
Try to lock the exit at coordinates
### [`unlock_door_at`](/spelunky-fyi/overlunky/search?l=Lua&q=unlock_door_at)
#### Params: `float x, float y`
Try to unlock the exit at coordinates
### [`get_frame`](/spelunky-fyi/overlunky/search?l=Lua&q=get_frame)
#### Returns: `int`
Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
### [`carry`](/spelunky-fyi/overlunky/search?l=Lua&q=carry)
#### Params: `int mount, int rider`
Make `mount` carry `rider` on their back. Only use this with actual mounts and living things.
### [`flip_entity`](/spelunky-fyi/overlunky/search?l=Lua&q=flip_entity)
#### Params: `int id`
Flip entity around by uid. All new entities face right by default.
### [`distance`](/spelunky-fyi/overlunky/search?l=Lua&q=distance)
#### Params: `int a, int b`
Calculate the tile distance of two entities by uid
### [`get_bounds`](/spelunky-fyi/overlunky/search?l=Lua&q=get_bounds)
#### Returns: `float`, `float`, `float`, `float`
Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
inside these boundaries. The order is: top left x, top left y, bottom right x, bottom right y Example:
```
-- Draw the level boundaries
set_callback(function()
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(xmin, ymin) -- top left
    sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)
```
### [`get_camera_position`](/spelunky-fyi/overlunky/search?l=Lua&q=get_camera_position)
#### Returns: `pair<float, float>`
Gets the current camera position in the level
### [`set_camera_position`](/spelunky-fyi/overlunky/search?l=Lua&q=set_camera_position)
#### Params: `float cx, float cy`
Sets the current camera position in the level.
Note: The camera will still try to follow the player and this doesn't actually work at all.
### [`set_flag`](/spelunky-fyi/overlunky/search?l=Lua&q=set_flag)
#### Params: `int flags, int bit`
Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### [`setflag`](/spelunky-fyi/overlunky/search?l=Lua&q=setflag)
### [`clr_flag`](/spelunky-fyi/overlunky/search?l=Lua&q=clr_flag)
#### Params: `int flags, int bit`
Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### [`clrflag`](/spelunky-fyi/overlunky/search?l=Lua&q=clrflag)
### [`test_flag`](/spelunky-fyi/overlunky/search?l=Lua&q=test_flag)
#### Params: `int flags, int bit`
Returns true if a bit is set in the flags
### [`testflag`](/spelunky-fyi/overlunky/search?l=Lua&q=testflag)
### [`rgba`](/spelunky-fyi/overlunky/search?l=Lua&q=rgba)
#### Params: `int r, int g, int b, int a`
Converts a color to int to be used in drawing functions. Use values from `0..255`.
### [`draw_line`](/spelunky-fyi/overlunky/search?l=Lua&q=draw_line)
#### Params: `float x1, float y1, float x2, float y2, float thickness, int color`
Draws a line on screen
### [`draw_rect`](/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect)
#### Params: `float x1, float y1, float x2, float y2, float thickness, float rounding, int color`
Draws a rectangle on screen from top-left to bottom-right.
### [`draw_rect_filled`](/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled)
#### Params: `float x1, float y1, float x2, float y2, float rounding, int color`
Draws a filled rectangle on screen from top-left to bottom-right.
### [`draw_circle`](/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle)
#### Params: `float x, float y, float radius, float thickness, int color`
Draws a circle on screen
### [`draw_circle_filled`](/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle_filled)
#### Params: `float x, float y, float radius, int color`
Draws a filled circle on screen
### [`draw_text`](/spelunky-fyi/overlunky/search?l=Lua&q=draw_text)
#### Params: `float x, float y, string text, int color`
Draws text on screen
## Types
Using the api through these directly is kinda dangerous, but such is life. I got pretty bored writing this doc generator at this point, so you can find the variable types in the [source files](https://github.com/spelunky-fyi/overlunky/tree/main/src/game_api). They're mostly just ints and floats. Example:
```
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
- [`r`](/spelunky-fyi/overlunky/search?l=Lua&q=r) &Color::r
- [`g`](/spelunky-fyi/overlunky/search?l=Lua&q=g) &Color::g
- [`b`](/spelunky-fyi/overlunky/search?l=Lua&q=b) &Color::b
- [`a`](/spelunky-fyi/overlunky/search?l=Lua&q=a) &Color::a
### `Inventory`
- [`money`](/spelunky-fyi/overlunky/search?l=Lua&q=money) &Inventory::money
- [`bombs`](/spelunky-fyi/overlunky/search?l=Lua&q=bombs) &Inventory::bombs
- [`ropes`](/spelunky-fyi/overlunky/search?l=Lua&q=ropes) &Inventory::ropes
- [`kills_level`](/spelunky-fyi/overlunky/search?l=Lua&q=kills_level) &Inventory::kills_level
- [`kills_total`](/spelunky-fyi/overlunky/search?l=Lua&q=kills_total) &Inventory::kills_total
### `EntityDB`
- [`id`](/spelunky-fyi/overlunky/search?l=Lua&q=id) &EntityDB::id
- [`search_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=search_flags) &EntityDB::search_flags
- [`width`](/spelunky-fyi/overlunky/search?l=Lua&q=width) &EntityDB::width
- [`height`](/spelunky-fyi/overlunky/search?l=Lua&q=height) &EntityDB::height
- [`friction`](/spelunky-fyi/overlunky/search?l=Lua&q=friction) &EntityDB::friction
- [`elasticity`](/spelunky-fyi/overlunky/search?l=Lua&q=elasticity) &EntityDB::elasticity
- [`weight`](/spelunky-fyi/overlunky/search?l=Lua&q=weight) &EntityDB::weight
- [`acceleration`](/spelunky-fyi/overlunky/search?l=Lua&q=acceleration) &EntityDB::acceleration
- [`max_speed`](/spelunky-fyi/overlunky/search?l=Lua&q=max_speed) &EntityDB::max_speed
- [`sprint_factor`](/spelunky-fyi/overlunky/search?l=Lua&q=sprint_factor) &EntityDB::sprint_factor
- [`jump`](/spelunky-fyi/overlunky/search?l=Lua&q=jump) &EntityDB::jump
- [`damage`](/spelunky-fyi/overlunky/search?l=Lua&q=damage) &EntityDB::damage
- [`life`](/spelunky-fyi/overlunky/search?l=Lua&q=life) &EntityDB::life
### `Entity`
- [`type`](/spelunky-fyi/overlunky/search?l=Lua&q=type) &Entity::type
- [`overlay`](/spelunky-fyi/overlunky/search?l=Lua&q=overlay) &Entity::overlay
- [`flags`](/spelunky-fyi/overlunky/search?l=Lua&q=flags) &Entity::flags
- [`more_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=more_flags) &Entity::more_flags
- [`uid`](/spelunky-fyi/overlunky/search?l=Lua&q=uid) &Entity::uid
- [`animation`](/spelunky-fyi/overlunky/search?l=Lua&q=animation) &Entity::animation
- [`x`](/spelunky-fyi/overlunky/search?l=Lua&q=x) &Entity::x
- [`y`](/spelunky-fyi/overlunky/search?l=Lua&q=y) &Entity::y
- [`width`](/spelunky-fyi/overlunky/search?l=Lua&q=width) &Entity::w
- [`height`](/spelunky-fyi/overlunky/search?l=Lua&q=height) &Entity::h
- [`topmost`](/spelunky-fyi/overlunky/search?l=Lua&q=topmost) &Entity::topmost
- [`topmost_mount`](/spelunky-fyi/overlunky/search?l=Lua&q=topmost_mount) &Entity::topmost_mount
- [`as_movable`](/spelunky-fyi/overlunky/search?l=Lua&q=as_movable) &Entity::as&lt;Movable&gt;
- [`as_door`](/spelunky-fyi/overlunky/search?l=Lua&q=as_door) &Entity::as&lt;Door&gt;
- [`as_container`](/spelunky-fyi/overlunky/search?l=Lua&q=as_container) &Entity::as&lt;Container&gt;
- [`as_mattock`](/spelunky-fyi/overlunky/search?l=Lua&q=as_mattock) &Entity::as&lt;Mattock&gt;
- [`as_mount`](/spelunky-fyi/overlunky/search?l=Lua&q=as_mount) &Entity::as&lt;Mount&gt;
- [`as_player`](/spelunky-fyi/overlunky/search?l=Lua&q=as_player) &Entity::as&lt;Player&gt;
- [`as_monster`](/spelunky-fyi/overlunky/search?l=Lua&q=as_monster) &Entity::as&lt;Monster&gt;
### `Movable`
Derived from [`Entity`](#entity)
- [`movex`](/spelunky-fyi/overlunky/search?l=Lua&q=movex) &Movable::movex
- [`movey`](/spelunky-fyi/overlunky/search?l=Lua&q=movey) &Movable::movey
- [`buttons`](/spelunky-fyi/overlunky/search?l=Lua&q=buttons) &Movable::buttons
- [`stand_counter`](/spelunky-fyi/overlunky/search?l=Lua&q=stand_counter) &Movable::stand_counter
- [`owner_uid`](/spelunky-fyi/overlunky/search?l=Lua&q=owner_uid) &Movable::owner_uid
- [`last_owner_uid`](/spelunky-fyi/overlunky/search?l=Lua&q=last_owner_uid) &Movable::last_owner_uid
- [`idle_counter`](/spelunky-fyi/overlunky/search?l=Lua&q=idle_counter) &Movable::idle_counter
- [`standing_on_uid`](/spelunky-fyi/overlunky/search?l=Lua&q=standing_on_uid) &Movable::standing_on_uid
- [`velocityx`](/spelunky-fyi/overlunky/search?l=Lua&q=velocityx) &Movable::velocityx
- [`velocityy`](/spelunky-fyi/overlunky/search?l=Lua&q=velocityy) &Movable::velocityy
- [`holding_uid`](/spelunky-fyi/overlunky/search?l=Lua&q=holding_uid) &Movable::holding_uid
- [`state`](/spelunky-fyi/overlunky/search?l=Lua&q=state) &Movable::state
- [`last_state`](/spelunky-fyi/overlunky/search?l=Lua&q=last_state) &Movable::last_state
- [`move_state`](/spelunky-fyi/overlunky/search?l=Lua&q=move_state) &Movable::move_state
- [`health`](/spelunky-fyi/overlunky/search?l=Lua&q=health) &Movable::health
- [`stun_timer`](/spelunky-fyi/overlunky/search?l=Lua&q=stun_timer) &Movable::stun_timer
- [`stun_state`](/spelunky-fyi/overlunky/search?l=Lua&q=stun_state) &Movable::stun_state
- [`some_state`](/spelunky-fyi/overlunky/search?l=Lua&q=some_state) &Movable::some_state
- [`color`](/spelunky-fyi/overlunky/search?l=Lua&q=color) &Movable::color
- [`hitboxx`](/spelunky-fyi/overlunky/search?l=Lua&q=hitboxx) &Movable::hitboxx
- [`hitboxy`](/spelunky-fyi/overlunky/search?l=Lua&q=hitboxy) &Movable::hitboxy
- [`offsetx`](/spelunky-fyi/overlunky/search?l=Lua&q=offsetx) &Movable::offsetx
- [`offsety`](/spelunky-fyi/overlunky/search?l=Lua&q=offsety) &Movable::offsety
- [`airtime`](/spelunky-fyi/overlunky/search?l=Lua&q=airtime) &Movable::airtime
### `Monster`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`sol::base_classes`](/spelunky-fyi/overlunky/search?l=Lua&q=sol::base_classes) sol::bases&lt;Entity
### `Player`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`inventory`](/spelunky-fyi/overlunky/search?l=Lua&q=inventory) &Player::inventory_ptr
### `Mount`
Derived from [`Entity`](#entity) [`Movable`](#movable) [`Monster`](#monster)
- [`carry`](/spelunky-fyi/overlunky/search?l=Lua&q=carry) &Mount::carry
- [`tame`](/spelunky-fyi/overlunky/search?l=Lua&q=tame) &Mount::tame
### `Container`
Derived from [`Entity`](#entity) [`Movable`](#movable)
- [`inside`](/spelunky-fyi/overlunky/search?l=Lua&q=inside) &Container::inside
- [`timer`](/spelunky-fyi/overlunky/search?l=Lua&q=timer) &Container::timer
### `StateMemory`
- [`screen_last`](/spelunky-fyi/overlunky/search?l=Lua&q=screen_last) &StateMemory::screen_last
- [`screen`](/spelunky-fyi/overlunky/search?l=Lua&q=screen) &StateMemory::screen
- [`screen_next`](/spelunky-fyi/overlunky/search?l=Lua&q=screen_next) &StateMemory::screen_next
- [`ingame`](/spelunky-fyi/overlunky/search?l=Lua&q=ingame) &StateMemory::ingame
- [`playing`](/spelunky-fyi/overlunky/search?l=Lua&q=playing) &StateMemory::playing
- [`pause`](/spelunky-fyi/overlunky/search?l=Lua&q=pause) &StateMemory::pause
- [`width`](/spelunky-fyi/overlunky/search?l=Lua&q=width) &StateMemory::w
- [`height`](/spelunky-fyi/overlunky/search?l=Lua&q=height) &StateMemory::h
- [`kali_favor`](/spelunky-fyi/overlunky/search?l=Lua&q=kali_favor) &StateMemory::kali_favor
- [`kali_status`](/spelunky-fyi/overlunky/search?l=Lua&q=kali_status) &StateMemory::kali_status
- [`kali_altars_destroyed`](/spelunky-fyi/overlunky/search?l=Lua&q=kali_altars_destroyed) &StateMemory::kali_altars_destroyed
- [`time_total`](/spelunky-fyi/overlunky/search?l=Lua&q=time_total) &StateMemory::time_total
- [`world`](/spelunky-fyi/overlunky/search?l=Lua&q=world) &StateMemory::world
- [`world_next`](/spelunky-fyi/overlunky/search?l=Lua&q=world_next) &StateMemory::world_next
- [`level`](/spelunky-fyi/overlunky/search?l=Lua&q=level) &StateMemory::level
- [`level_next`](/spelunky-fyi/overlunky/search?l=Lua&q=level_next) &StateMemory::level_next
- [`theme`](/spelunky-fyi/overlunky/search?l=Lua&q=theme) &StateMemory::theme
- [`theme_next`](/spelunky-fyi/overlunky/search?l=Lua&q=theme_next) &StateMemory::theme_next
- [`shoppie_aggro`](/spelunky-fyi/overlunky/search?l=Lua&q=shoppie_aggro) &StateMemory::shoppie_aggro
- [`shoppie_aggro_next`](/spelunky-fyi/overlunky/search?l=Lua&q=shoppie_aggro_next) &StateMemory::shoppie_aggro_levels
- [`merchant_aggro`](/spelunky-fyi/overlunky/search?l=Lua&q=merchant_aggro) &StateMemory::merchant_aggro
- [`kills_npc`](/spelunky-fyi/overlunky/search?l=Lua&q=kills_npc) &StateMemory::kills_npc
- [`level_count`](/spelunky-fyi/overlunky/search?l=Lua&q=level_count) &StateMemory::level_count
- [`journal_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=journal_flags) &StateMemory::journal_flags
- [`time_last_level`](/spelunky-fyi/overlunky/search?l=Lua&q=time_last_level) &StateMemory::time_last_level
- [`time_level`](/spelunky-fyi/overlunky/search?l=Lua&q=time_level) &StateMemory::time_level
- [`hud_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=hud_flags) &StateMemory::hud_flags
- [`loading`](/spelunky-fyi/overlunky/search?l=Lua&q=loading) &StateMemory::loading
- [`quest_flags`](/spelunky-fyi/overlunky/search?l=Lua&q=quest_flags) &StateMemory::quest_flags
## Enums
Enums are like numbers but in text that's easier to remember. Example:
```
set_callback(function()
    if state.theme == THEME.COSMIC_OCEAN then
        x, y, l = get_position(players[1].uid)
        spawn(ENT_TYPE.ITEM_JETPACK, x, y, l, 0, 0)
    end
end, ON.LEVEL)
```
### ENT_TYPE
- [`FLOOR_BORDERTILE`](/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.FLOOR_BORDERTILE) 1
- ...blah blah read your entities.txt...
- [`LIQUID_STAGNANT_LAVA`](/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.LIQUID_STAGNANT_LAVA) 898
### THEME
- [`DWELLING`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.DWELLING) 1
- [`JUNGLE`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.JUNGLE) 2
- [`VOLCANA`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.VOLCANA) 3
- [`OLMEC`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.OLMEC) 4
- [`TIDE_POOL`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TIDE_POOL) 5
- [`TEMPLE`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TEMPLE) 6
- [`ICE_CAVES`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ICE_CAVES) 7
- [`NEO_BABYLON`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.NEO_BABYLON) 8
- [`SUNKEN_CITY`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.SUNKEN_CITY) 9
- [`COSMIC_OCEAN`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.COSMIC_OCEAN) 10
- [`CITY_OF_GOLD`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.CITY_OF_GOLD) 11
- [`DUAT`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.DUAT) 12
- [`ABZU`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ABZU) 13
- [`TIAMAT`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TIAMAT) 14
- [`EGGPLANT_WORLD`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.EGGPLANT_WORLD) 15
- [`HUNDUN`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.HUNDUN) 16
- [`BASE_CAMP`](/spelunky-fyi/overlunky/search?l=Lua&q=THEME.BASE_CAMP) 17
### ON
- [`LOGO`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOGO) 0
- [`INTRO`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.INTRO) 1
- [`PROLOGUE`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.PROLOGUE) 2
- [`TITLE`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.TITLE) 3
- [`MENU`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.MENU) 4
- [`OPTIONS`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.OPTIONS) 5
- [`LEADERBOARD`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEADERBOARD) 7
- [`SEED_INPUT`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.SEED_INPUT) 8
- [`CHARACTER_SELECT`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.CHARACTER_SELECT) 9
- [`TEAM_SELECT`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.TEAM_SELECT) 10
- [`CAMP`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.CAMP) 11
- [`LEVEL`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEVEL) 12
- [`TRANSITION`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.TRANSITION) 13
- [`DEATH`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH) 14
- [`SPACESHIP`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPACESHIP) 15
- [`WIN`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.WIN) 16
- [`CREDITS`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.CREDITS) 17
- [`SCORES`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCORES) 18
- [`CONSTELLATION`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.CONSTELLATION) 19
- [`RECAP`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.RECAP) 20
- [`GUIFRAME`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.GUIFRAME) 100
- [`FRAME`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.FRAME) 101
- [`SCREEN`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCREEN) 102
- [`START`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.START) 103
- [`LOADING`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOADING) 104
- [`RESET`](/spelunky-fyi/overlunky/search?l=Lua&q=ON.RESET) 105
### LAYER
- [`FRONT`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.FRONT) 0
- [`BACK`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.BACK) 1
- [`PLAYER`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER) -1
- [`PLAYER1`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER1) -1
- [`PLAYER2`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER2) -2
- [`PLAYER3`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER3) -3
- [`PLAYER4`](/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER4) -4
