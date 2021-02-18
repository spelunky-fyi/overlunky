# Overlunky Lua API
- Everything here is still changing, don't be sad if your scripts break next week!
- This doc doesn't have a lot of examples, that's why we have [examples/](https://github.com/spelunky-fyi/overlunky/tree/main/examples).
- This doc is for the HEAD version. If you're using an exe release from the past, you might find some things here don't work.
- You can find changes to and earlier versions of this doc [here](https://github.com/spelunky-fyi/overlunky/commits/main/docs/script-api.md).
## Lua libraries
The following Lua libraries and their functions are available. You can read more about them in the [Lua documentation](https://www.lua.org/manual/5.3/manual.html#6).
### `math`
### `base`
### `string`
### `table`
## Global variables
These variables are always there to use.
### `state`
A bunch of [game state](#statememory) variables
Example:
```
if state.time_level > 300 and state.theme == THEME.DWELLING then
    toast("Congratilations for lasting 10 seconds in Dwelling")
end
```
### `players`
An array of [Movables](#movable) of the current players. Pro tip: You need `players[1].uid` in most entity functions.
### `meta`
Table of strings where you should set some script metadata shown in the UI.
- `meta.name` Script name
- `meta.version` Version
- `meta.description` Short description of the script
- `meta.author` Your name
### `options`
Table of options set in the UI, added with the [register_option_functions](#register_option_int).
## Event functions
Define these in your script to be called on an event. For example:
```
function on_level()
    toast("Welcome to the level")
end
```
### `on_guiframe`
Runs on every screen frame. You need this to use draw functions.
### `on_frame`
Runs on every game engine frame.
### `on_camp`
Runs on entering the camp.
### `on_level`
Runs on the start of every level.
### `on_start`
Runs on the start of first level.
### `on_transition`
Runs on the start of level transition.
### `on_death`
Runs on the death screen.
### `on_win`
Runs on any ending cutscene.
### `on_screen`
Runs on any [screen change](#on).
## Functions
Note: The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen.
### `message`
#### Params: `string message`
Print a log message on screen.
### `set_interval`
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called every `frames` game frames. Timer is paused on pause and cleared on level transition.
### `set_timeout`
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called after `frames` frames. Timer is paused on pause and cleared on level transition.
### `set_global_interval`
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called every `frames` frames. This timer is never paused or cleared.
### `set_global_timeout`
#### Params: `function cb, int frames`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called after `frames` frames. This timer is never paused or cleared.
### `set_callback`
#### Params: `function cb, int screen`
#### Returns: `int` unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called on an [event](#on).
### `clear_callback`
#### Params: `int id`
Clear previously added callback `id`
### `toast`
#### Params: `string message`
Show a message that looks like a level feeling.
### `say`
#### Params: `int entity_id, string message, int unk_type, bool top`
Show a message coming from an entity
### `register_option_int`
#### Params: `string name, string desc, int value, int min, int max`
Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default.
### `register_option_bool`
#### Params: `string name, string desc, bool value`
Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.
### `spawn_entity`
#### Params: `int id, float x, float y, int layer, float vx, float vy`
#### Returns: `int`
Spawn an entity in position with some velocity and return the uid of spawned entity.
### `spawn`
#### Params: `int id, float x, float y, int layer, float vx, float vy`
#### Returns: `int`
Short for [spawn_entity](#spawn_entity).
### `spawn_door`
#### Params: `float x, float y, int layer, int w, int l, int t`
#### Returns: `int`
Spawn a door to another world, level and theme and return the uid of spawned entity.
### `door`
#### Params: `float x, float y, int layer, int w, int l, int t`
#### Returns: `int`
Short for [spawn_door](#spawn_door).
### `spawn_layer_door`
#### Params: `float x, float y`
Spawn a door to backlayer
### `layer_door`
#### Params: `float x, float y`
Short for [spawn_layer_door](#spawn_layer_door).
### `god`
#### Params: `bool g`
Enable/disable godmode
### `force_dark_level`
#### Params: `bool g`
Try to force next levels to be dark
### `zoom`
#### Params: `float level`
Set the zoom level used in levels and shops. 13.5 is the default.
### `pause`
#### Params: `bool p`
Enable/disable game engine pause
### `move_entity`
#### Params: `int id, float x, float y, float vx, float vy`
Teleport entity to coordinates with optional velocity
### `set_door_target`
#### Params: `int id, int w, int l, int t`
Make an ENT_TYPE.FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`
### `set_door`
#### Params: `int id, int w, int l, int t`
Short for [set_door_target](#set_door_target).
### `set_contents`
#### Params: `int id, int item`
Set the contents of ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CRATE or ENT_TYPE.ITEM_COFFIN `id` to ENT_TYPE... `item`
### `get_entity`
#### Params: `int id`
#### Returns: `Entity`
Get the [Entity](#entity) behind an uid
### `get_type`
#### Params: `int id`
#### Returns: `EntityDB`
Get the [EntityDB](#entitydb) behind an uid.
### `get_entities`
#### Returns: `array<int>`
Get uids of all entities currently loaded
### `get_entities_by`
#### Params: `int type, int mask, int layer`
#### Returns: `array<int>`
Get uids of entities by some conditions. Set `type` or `mask` to `0` to ignore that.
### `get_entities_by_type`
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
### `get_entities_by_mask`
#### Params: `int mask`
#### Returns: `array<int>`
Get uids of entities by some search_flags
### `get_entities_by_layer`
#### Params: `int layer`
#### Returns: `array<int>`
Get uids of entities by layer. `0` for main level, `1` for backlayer, `-1` for layer of the player.
### `get_entities_at`
#### Params: `int type, int mask, float x, float y, int layer, float r`
#### Returns: `array<int>`
Get uids of matching entities inside some radius. Set `type` or `mask` to `0` to ignore that.
### `get_entity_flags`
#### Params: `int id`
#### Returns: `int`
Get the `flags` field from entity by uid
### `set_entity_flags`
#### Params: `int id, int flags`
Set the `flags` field from entity by uid
### `get_entity_flags2`
#### Params: `int id`
#### Returns: `int`
Get the `more_flags` field from entity by uid
### `set_entity_flags2`
#### Params: `int id, int flags`
Set the `more_flags` field from entity by uid
### `get_entity_ai_state`
#### Params: `int id`
#### Returns: `int`
Get the `move_state` field from entity by uid
### `get_hud_flags`
#### Returns: `int`
Get `state.flags`
### `set_hud_flags`
#### Params: `int flags`
Set `state.flags`
### `get_entity_type`
#### Params: `int id`
#### Returns: `int`
Get the ENT_TYPE... for entity by uid
### `get_zoom_level`
#### Returns: `float`
Get the current set zoom level
### `screen_position`
#### Params: `float x, float y`
#### Returns: `pair<float, float>`
Translate an entity position to screen position to be used in drawing functions
### `screen_distance`
#### Params: `float x`
#### Returns: `float`
Translate a distance of `x` tiles to screen distance to be be used in drawing functions
### `get_position`
#### Params: `int id`
#### Returns: `tuple<float, float, int>`
Get position `x, y, layer` of entity by uid. Use this, don't use `Movable.x/y` because those are sometimes just the offset to the entity
you're standing on.
### `entity_remove_item`
#### Params: `int id, int item`
Remove item by uid from entity
### `spawn_entity_over`
#### Params: `int id, int over, float x, float y`
#### Returns: `int`
Spawn an entity by `id` attached to some other entity `over`, in offset `x`, `y`
### `entity_has_item_uid`
#### Params: `int id, int item`
#### Returns: `bool`
Check if the entity `id` has some specific `item` by uid in their inventory
### `entity_has_item_type`
#### Params: `int id, int type`
#### Returns: `bool`
Check if the entity `id` has some ENT_TYPE `type` in their inventory
### `kill_entity`
#### Params: `int id`
Kills an entity by uid.
### `apply_entity_db`
#### Params: `int id`
Apply changes made in [get_type](#get_type)() to entity instance by uid.
### `lock_door_at`
#### Params: `float x, float y`
Try to lock the exit at coordinates
### `unlock_door_at`
#### Params: `float x, float y`
Try to unlock the exit at coordinates
### `get_frame`
#### Returns: `int`
Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.
### `carry`
#### Params: `int mount, int rider`
Make `mount` carry `rider` on their back. Only use this with actual mounts and living things.
### `distance`
#### Params: `int a, int b`
Calculate the tile distance of two entities by uid
### `set_flag`
#### Params: `int flags, int bit`
Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### `setflag`
### `clr_flag`
#### Params: `int flags, int bit`
Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.
### `clrflag`
### `test_flag`
#### Params: `int flags, int bit`
Returns true if a bit is set in the flags
### `testflag`
### `rgba`
#### Params: `int r, int g, int b, int a`
Converts a color to int to be used in drawing functions. Use values from `0..255`.
### `draw_line`
#### Params: `float x1, float y1, float x2, float y2, float thickness, int color`
Draws a line on screen
### `draw_rect`
#### Params: `float x1, float y1, float x2, float y2, float thickness, float rounding, int color`
Draws rectangle on screen from top-left to bottom-right.
### `draw_circle`
#### Params: `float x, float y, float radius, float thickness, int color`
Draws a circle on screen
### `draw_text`
#### Params: `float x, float y, string text, int color`
Draws text on screen
## Types
Using the api through these directly is kinda dangerous, but such is life. I got pretty bored writing this doc generator at this point, so you can find the variable types in the [.hpp files](https://github.com/spelunky-fyi/overlunky/tree/main/src/game_api). They're mostly just ints and floats.
### Color
- `r` &Color::r
- `g` &Color::g
- `b` &Color::b
- `a` &Color::a
### Inventory
- `money` &Inventory::money
- `bombs` &Inventory::bombs
- `ropes` &Inventory::ropes
- `kills_level` &Inventory::kills_level
- `kills_total` &Inventory::kills_total
### EntityDB
- `id` &EntityDB::id
- `search_flags` &EntityDB::search_flags
- `width` &EntityDB::width
- `height` &EntityDB::height
- `friction` &EntityDB::friction
- `elasticity` &EntityDB::elasticity
- `weight` &EntityDB::weight
- `acceleration` &EntityDB::acceleration
- `max_speed` &EntityDB::max_speed
- `sprint_factor` &EntityDB::sprint_factor
- `jump` &EntityDB::jump
- `damage` &EntityDB::damage
- `life` &EntityDB::life
### Entity
- `type` &Entity::type
- `overlay` &Entity::overlay
- `flags` &Entity::flags
- `more_flags` &Entity::more_flags
- `uid` &Entity::uid
- `animation` &Entity::animation
- `x` &Entity::x
- `y` &Entity::y
- `width` &Entity::w
- `height` &Entity::h
- `topmost` &Entity::topmost
- `topmost_mount` &Entity::topmost_mount
- `as_movable` &Entity::as<Movable>
- `as_door` &Entity::as<Door>
- `as_container` &Entity::as<Container>
- `as_mattock` &Entity::as<Mattock>
- `as_mount` &Entity::as<Mount>
### Movable
- `movex` &Movable::movex
- `movey` &Movable::movey
- `buttons` &Movable::buttons
- `stand_counter` &Movable::stand_counter
- `owner_uid` &Movable::owner_uid
- `last_owner_uid` &Movable::last_owner_uid
- `idle_counter` &Movable::idle_counter
- `standing_on_uid` &Movable::standing_on_uid
- `velocityx` &Movable::velocityx
- `velocityy` &Movable::velocityy
- `holding_uid` &Movable::holding_uid
- `state` &Movable::state
- `last_state` &Movable::last_state
- `move_state` &Movable::move_state
- `health` &Movable::health
- `some_state` &Movable::some_state
- `color` &Movable::color
- `hitboxx` &Movable::hitboxx
- `hitboxy` &Movable::hitboxy
- `offsetx` &Movable::offsetx
- `offsety` &Movable::offsety
- `airtime` &Movable::airtime
### Player
- `inventory` &Player::inventory_ptr
### Container
- `inside` &Container::inside
### StateMemory
- `screen_last` &StateMemory::screen_last
- `screen` &StateMemory::screen
- `screen_next` &StateMemory::screen_next
- `ingame` &StateMemory::ingame
- `playing` &StateMemory::playing
- `pause` &StateMemory::pause
- `width` &StateMemory::w
- `height` &StateMemory::h
- `kali_favor` &StateMemory::kali_favor
- `kali_status` &StateMemory::kali_status
- `kali_altars_destroyed` &StateMemory::kali_altars_destroyed
- `time_total` &StateMemory::time_total
- `world` &StateMemory::world
- `world_next` &StateMemory::world_next
- `level` &StateMemory::level
- `level_next` &StateMemory::level_next
- `theme` &StateMemory::theme
- `theme_next` &StateMemory::theme_next
- `shoppie_aggro` &StateMemory::shoppie_aggro
- `shoppie_aggro_next` &StateMemory::shoppie_aggro_levels
- `merchant_aggro` &StateMemory::merchant_aggro
- `kills_npc` &StateMemory::kills_npc
- `level_count` &StateMemory::level_count
- `journal_flags` &StateMemory::journal_flags
- `time_last_level` &StateMemory::time_last_level
- `time_level` &StateMemory::time_level
- `hud_flags` &StateMemory::hud_flags
- `loading` &StateMemory::loading
- `reset` &StateMemory::reset
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
- `FLOOR_BORDERTILE` 1
- ...blah blah read your entities.txt...
- `LIQUID_STAGNANT_LAVA` 898
### THEME
- `DWELLING` 1
- `JUNGLE` 2
- `VOLCANA` 3
- `OLMEC` 4
- `TIDE_POOL` 5
- `TEMPLE` 6
- `ICE_CAVES` 7
- `NEO_BABYLON` 8
- `SUNKEN_CITY` 9
- `COSMIC_OCEAN` 10
- `CITY_OF_GOLD` 11
- `DUAT` 12
- `ABZU` 13
- `TIAMAT` 14
- `EGGPLANT_WORLD` 15
- `HUNDUN` 16
- `BASE_CAMP` 17
### ON
- `LOGO` 0
- `INTRO` 1
- `PROLOGUE` 2
- `TITLE` 3
- `MENU` 4
- `OPTIONS` 5
- `LEADERBOARD` 7
- `SEED_INPUT` 8
- `CHARACTER_SELECT` 9
- `TEAM_SELECT` 10
- `CAMP` 11
- `LEVEL` 12
- `TRANSITION` 13
- `DEATH` 14
- `SPACESHIP` 15
- `WIN` 16
- `CREDITS` 17
- `SCORES` 18
- `CONSTELLATION` 19
- `RECAP` 20
- `GUIFRAME` 100
- `FRAME` 101
- `SCREEN` 102
- `START` 103
- `LOADING` 104
- `RESET` 105
### LAYER
- `FRONT` 0
- `BACK` 1
- `CURRENT` -1
