# Global variables
These variables are always there to use.
### meta


```lua
meta.name = "Awesome Mod"
meta.version = "1.0"
meta.author = "You"
meta.description = [[
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut at nulla porttitor, lobortis magna at, tempus dolor. Cras non ligula tellus. Duis tincidunt sodales velit et ornare. Mauris eu sapien finibus dolor dictum malesuada in non elit.

    Aenean luctus leo ut diam ornare viverra. Nunc condimentum interdum elit, quis porttitor quam finibus ac. Nam mattis, lectus commodo placerat tristique, justo justo congue dui, sed sodales nunc sem ut neque.
]]
-- set this to enable unsafe mode
--meta.unsafe = true

-- rest of your mod goes here

```

#### array&lt;string&gt; meta

> Search script examples for [meta](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=meta)

Table of strings where you should set some script metadata shown in the UI and used by other scripts to find your script.
### state


```lua
if state.time_level > 300 and state.theme == THEME.DWELLING then
    toast("Congratulations for lasting 5 seconds in Dwelling")
end

```

#### [StateMemory](#StateMemory) state

> Search script examples for [state](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=state)

A bunch of [game state](#StateMemory) variables. Your ticket to almost anything that is not an [Entity](#Entity).
### game_manager


```lua
if game_manager.game_props.game_has_focus == false then
    message("Come back soon!")
end
```

#### [GameManager](#GameManager) game_manager

> Search script examples for [game_manager](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_manager)

The [GameManager](#GameManager) gives access to a couple of Screens as well as the pause and journal UI elements
### online


```lua
message = "Currently playing: "
for _, p in pairs(online.online_players) do
    if p.ready_state ~= 0 then
        message = message .. p.player_name .. " "
    end
end
print(message)
```

#### [Online](#Online) online

> Search script examples for [online](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=online)

The [Online](#Online) object has information about the online lobby and its players
### players


```lua
> Make the player invisible, use only in single player only mods

players[1].flags = set_flag(players[1].flags, 1)
```

#### array&lt;[Player](#Player)&gt; players

> Search script examples for [players](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=players)

An array of [Player](#Player) of the current players. This is just a list of existing [Player](#Player) entities in order, i.e., `players[1]` is not guaranteed to be P1 if they have been gibbed for example. See get_player().
### savegame


> Print best time from savegame

```lua
prinspect(savegame.time_best)
```


#### [SaveData](#SaveData) savegame

> Search script examples for [savegame](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=savegame)

Provides a read-only access to the save data, updated as soon as something changes (i.e. before it's written to savegame.sav.)
### options


```lua
register_option_bool("bomb_bag", "BombBag", "Spawn bomb bag at the start of every level", false)

set_callback(function()
    if options.bomb_bag then
        -- Spawn the bomb bag at player location thanks to the LAYER.PLAYER1
        spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_PICKUP_BOMBBAG, 0, 0, LAYER.PLAYER1)
    end
end, ON.LEVEL)
```

#### array&lt;mixed&gt; options

> Search script examples for [options](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=options)

Table of options set in the UI, added with the [register_option_functions](#Option-functions). You can also write your own options in here or override values defined in the register functions/UI before or after they are registered. Check the examples for many different use cases and saving options to disk.
### prng


```lua
> Make it so there is 50% chance that the Ankh will be destroyed

set_callback(function ()
    -- more or less 50% chance
    if prng:random(2) == 1 then
        -- get all Ankh's in a level
        ankhs = get_entities_by(ENT_TYPE.ITEM_PICKUP_ANKH, MASK.ITEM, LAYER.BOTH)
        for _, uid in pairs(ankhs) do
            get_entity(uid):destroy()
        end
    end
end, ON.LEVEL)
```

#### [PRNG](#PRNG) prng

> Search script examples for [prng](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=prng)

The global prng state, calling any function on it will advance the prng state, thus desynchronizing clients if it does not happen on both clients.
# Functions
The game functions like `spawn` use [level coordinates](#get_position). Draw functions use normalized [screen coordinates](#screen_position) from `-1.0 .. 1.0` where `0.0, 0.0` is the center of the screen.

## Callback functions


### clear_callback


> Create three explosions and then clear the interval

```lua
local count = 0 -- this upvalues to the interval
set_interval(function()
  count = count + 1
  spawn(ENT_TYPE.FX_EXPLOSION, 0, 0, LAYER.FRONT, 0, 0)
  if count >= 3 then
    -- calling this without parameters clears the fallback that's calling it
    clear_callback()
  end
end, 60)
```



> Search script examples for [clear_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_callback)

#### nil clear_callback(optional<[CallbackId](#Aliases)> id)

Clear previously added callback `id` or call without arguments inside any callback to clear that callback after it returns.

### clear_entity_callback


> Search script examples for [clear_entity_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_entity_callback)

#### nil clear_entity_callback(int uid, [CallbackId](#Aliases) cb_id)

Clears a callback that is specific to an entity.

### clear_screen_callback


> Search script examples for [clear_screen_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_screen_callback)

#### nil clear_screen_callback(int screen_id, [CallbackId](#Aliases) cb_id)

Clears a callback that is specific to a screen.

### clear_vanilla_sound_callback


> Search script examples for [clear_vanilla_sound_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_vanilla_sound_callback)

#### nil clear_vanilla_sound_callback([CallbackId](#Aliases) id)

Clears a previously set callback

### set_callback


> Search script examples for [set_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback)

#### [CallbackId](#Aliases) set_callback(function cb, int screen)

Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called on an [event](#ON).

### set_global_interval


> Search script examples for [set_global_interval](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_interval)

#### [CallbackId](#Aliases) set_global_interval(function cb, int frames)

Returns unique id for the callback to be used in [clear_callback](#clear_callback). You can also return `false` from your function to clear the callback.
Add global callback function to be called every `frames` engine frames. This timer is never paused or cleared.

### set_global_timeout


> Search script examples for [set_global_timeout](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_global_timeout)

#### [CallbackId](#Aliases) set_global_timeout(function cb, int frames)

Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add global callback function to be called after `frames` engine frames. This timer is never paused or cleared.

### set_interval


> Create three explosions and then clear the interval

```lua
local count = 0 -- this upvalues to the interval
set_interval(function()
  count = count + 1
  spawn(ENT_TYPE.FX_EXPLOSION, 0, 0, LAYER.FRONT, 0, 0)
  if count >= 3 then
    -- calling this without parameters clears the fallback that's calling it
    clear_callback()
  end
end, 60)
```



> Search script examples for [set_interval](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_interval)

#### [CallbackId](#Aliases) set_interval(function cb, int frames)

Returns unique id for the callback to be used in [clear_callback](#clear_callback). You can also return `false` from your function to clear the callback.
Add per level callback function to be called every `frames` engine frames. Timer is paused on pause and cleared on level transition.

### set_on_damage


> Search script examples for [set_on_damage](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_damage)

#### optional&lt;[CallbackId](#Aliases)&gt; set_on_damage(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right before an entity is damaged, return `true` to skip the game's damage handling.
Note that damage_dealer can be nil ! (long fall, ...)
DO NOT CALL `self:damage()` in the callback !
Use this only when no other approach works, this call can be expensive if overused.
Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
The callback signature is bool on_damage([Entity](#Entity) self, [Entity](#Entity) damage_dealer, int damage_amount, float velocity_x, float velocity_y, int stun_amount, int iframes)

### set_on_destroy


> Search script examples for [set_on_destroy](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_destroy)

#### optional&lt;[CallbackId](#Aliases)&gt; set_on_destroy(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right when an entity is destroyed, e.g. as if by `Entity.destroy()` before the game applies any side effects.
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is nil on_destroy([Entity](#Entity) self)

### set_on_kill


> Search script examples for [set_on_kill](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_kill)

#### optional&lt;[CallbackId](#Aliases)&gt; set_on_kill(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right when an entity is eradicated (killing monsters that leave a body behind will not trigger this), before the game applies any side effects.
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is nil on_kill([Entity](#Entity) self, [Entity](#Entity) killer)

### set_on_open


> Search script examples for [set_on_open](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_open)

#### optional&lt;[CallbackId](#Aliases)&gt; set_on_open(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right when a container is opened via up+door, or weapon is shot.
Use this only when no other approach works, this call can be expensive if overused.
Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
The callback signature is nil on_open([Entity](#Entity) entity_self, [Entity](#Entity) opener)

### set_on_player_instagib


> Search script examples for [set_on_player_instagib](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_on_player_instagib)

#### optional&lt;[CallbackId](#Aliases)&gt; set_on_player_instagib(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right when an player/hired hand is crushed/insta-gibbed, return `true` to skip the game's crush handling.
The game's instagib function will be forcibly executed (regardless of whatever you return in the callback) when the entity's health is zero.
This is so that when the entity dies (from other causes), the death screen still gets shown.
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is bool on_player_instagib([Entity](#Entity) self)

### set_post_entity_spawn


> Search script examples for [set_post_entity_spawn](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_entity_spawn)

#### [CallbackId](#Aliases) set_post_entity_spawn(function cb, [SPAWN_TYPE](#SPAWN_TYPE) flags, int mask, variadic_args entity_types)

Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
This is run right after the entity is spawned but before and particular properties are changed, e.g. owner or velocity.
The callback signature is nil post_entity_spawn([Entity](#Entity) ent, [SPAWN_TYPE](#SPAWN_TYPE) spawn_flags)

### set_post_floor_update


```lua
-- Use FLOOR_GENERIC from different themes in your level,
-- with textures that update correctly when destroyed

define_tile_code("floor_generic_tidepool")
set_pre_tile_code_callback(function(x, y, layer)
    local uid = spawn_grid_entity(ENT_TYPE.FLOOR_GENERIC, x, y, layer)
    set_post_update(uid, function(me)
        me:set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TIDEPOOL_0)
        for i,v in ipairs(entity_get_items_by(me.uid, ENT_TYPE.DECORATION_GENERIC, MASK.DECORATION)) do
            local deco = get_entity(v)
            deco:set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TIDEPOOL_0)
        end
    end)
    return true
end, "floor_generic_tidepool")


-- Fix quicksand decorations when not in temple
set_post_entity_spawn(function(ent)
    set_post_floor_update(ent.uid, function(me)
        me:set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0)
        for i,v in ipairs(entity_get_items_by(me.uid, ENT_TYPE.DECORATION_GENERIC, MASK.DECORATION)) do
            local deco = get_entity(v)
            deco:set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0)
        end
    end)
end, SPAWN_TYPE.ANY, MASK.FLOOR, ENT_TYPE.FLOOR_QUICKSAND)

```


> Search script examples for [set_post_floor_update](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_floor_update)

#### optional&lt;[CallbackId](#Aliases)&gt; set_post_floor_update(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right after a floor is updated (by killed neighbor).
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is nil post_floor_update([Entity](#Entity) self)

### set_post_render


> Search script examples for [set_post_render](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_render)

#### optional&lt;[CallbackId](#Aliases)&gt; set_post_render(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right after the entity is rendered.
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is nil post_render([VanillaRenderContext](#VanillaRenderContext) render_ctx, [Entity](#Entity) self)

### set_post_render_screen


> Search script examples for [set_post_render_screen](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_render_screen)

#### optional&lt;[CallbackId](#Aliases)&gt; set_post_render_screen(int screen_id, function fun)

Returns unique id for the callback to be used in [clear_screen_callback](#clear_screen_callback) or `nil` if screen_id is not valid.
Sets a callback that is called right after the screen is drawn.
The callback signature is nil render_screen([Screen](#Screen) self, [VanillaRenderContext](#VanillaRenderContext) render_ctx)

### set_post_statemachine


> Search script examples for [set_post_statemachine](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_statemachine)

#### optional&lt;[CallbackId](#Aliases)&gt; set_post_statemachine(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
`uid` has to be the uid of a `Movable` or else stuff will break.
Sets a callback that is called right after the statemachine, so you can override any values the satemachine might have set (e.g. `animation_frame`).
Use this only when no other approach works, this call can be expensive if overused.
Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
The callback signature is nil statemachine([Entity](#Entity) self)

### set_pre_collision1


> Search script examples for [set_pre_collision1](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_collision1)

#### optional&lt;[CallbackId](#Aliases)&gt; set_pre_collision1(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right before the collision 1 event, return `true` to skip the game's collision handling.
Use this only when no other approach works, this call can be expensive if overused.
Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
The callback signature is bool pre_collision1([Entity](#Entity) entity_self, [Entity](#Entity) collision_entity)

### set_pre_collision2


> Search script examples for [set_pre_collision2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_collision2)

#### optional&lt;[CallbackId](#Aliases)&gt; set_pre_collision2(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right before the collision 2 event, return `true` to skip the game's collision handling.
Use this only when no other approach works, this call can be expensive if overused.
Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
The callback signature is bool pre_collision12([Entity](#Entity) self, [Entity](#Entity) collision_entity)

### set_pre_entity_spawn


> Search script examples for [set_pre_entity_spawn](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_entity_spawn)

#### [CallbackId](#Aliases) set_pre_entity_spawn(function cb, [SPAWN_TYPE](#SPAWN_TYPE) flags, int mask, variadic_args entity_types)

Add a callback for a spawn of specific entity types or mask. Set `mask` to `MASK.ANY` to ignore that.
This is run before the entity is spawned, spawn your own entity and return its uid to replace the intended spawn.
In many cases replacing the intended entity won't have the indended effect or will even break the game, so use only if you really know what you're doing.
The callback signature is optional<int> pre_entity_spawn([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, int layer, [Entity](#Entity) overlay_entity, [SPAWN_TYPE](#SPAWN_TYPE) spawn_flags)

### set_pre_floor_update


> Search script examples for [set_pre_floor_update](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_floor_update)

#### optional&lt;[CallbackId](#Aliases)&gt; set_pre_floor_update(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right before a floor is updated (by killed neighbor), return `true` to skip the game's neighbor update handling.
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is bool pre_floor_update([Entity](#Entity) self)

### set_pre_render


> Search script examples for [set_pre_render](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_render)

#### optional&lt;[CallbackId](#Aliases)&gt; set_pre_render(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
Sets a callback that is called right after the entity is rendered.
Return `true` to skip the original rendering function and all later pre_render callbacks.
Use this only when no other approach works, this call can be expensive if overused.
The callback signature is bool render([VanillaRenderContext](#VanillaRenderContext) render_ctx, [Entity](#Entity) self)

### set_pre_render_screen


> Search script examples for [set_pre_render_screen](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_render_screen)

#### optional&lt;[CallbackId](#Aliases)&gt; set_pre_render_screen(int screen_id, function fun)

Returns unique id for the callback to be used in [clear_screen_callback](#clear_screen_callback) or `nil` if screen_id is not valid.
Sets a callback that is called right before the screen is drawn, return `true` to skip the default rendering.
The callback signature is bool render_screen([Screen](#Screen) self, [VanillaRenderContext](#VanillaRenderContext) render_ctx)

### set_pre_statemachine


> Search script examples for [set_pre_statemachine](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_statemachine)

#### optional&lt;[CallbackId](#Aliases)&gt; set_pre_statemachine(int uid, function fun)

Returns unique id for the callback to be used in [clear_entity_callback](#clear_entity_callback) or `nil` if uid is not valid.
`uid` has to be the uid of a `Movable` or else stuff will break.
Sets a callback that is called right before the statemachine, return `true` to skip the statemachine update.
Use this only when no other approach works, this call can be expensive if overused.
Check [here](https://github.com/spelunky-fyi/overlunky/blob/main/docs/virtual-availability.md) to see whether you can use this callback on the entity type you intend to.
The callback signature is bool statemachine([Entity](#Entity) self)

### set_timeout


> Search script examples for [set_timeout](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_timeout)

#### [CallbackId](#Aliases) set_timeout(function cb, int frames)

Returns unique id for the callback to be used in [clear_callback](#clear_callback).
Add per level callback function to be called after `frames` engine frames. Timer is paused on pause and cleared on level transition.

### set_vanilla_sound_callback


> Search script examples for [set_vanilla_sound_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_vanilla_sound_callback)

#### [CallbackId](#Aliases) set_vanilla_sound_callback([VANILLA_SOUND](#VANILLA_SOUND) name, [VANILLA_SOUND_CALLBACK_TYPE](#VANILLA_SOUND_CALLBACK_TYPE) types, function cb)

Returns unique id for the callback to be used in [clear_vanilla_sound_callback](#clear_vanilla_sound_callback).
Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any properties on the sound. Otherwise you may cause a deadlock.
The callback signature is nil on_vanilla_sound([PlayingSound](#PlayingSound) sound)

## Debug functions


### dump_network


> Search script examples for [dump_network](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=dump_network)

#### nil dump_network()

Hook the sendto and recvfrom functions and start dumping network data to terminal

### get_address


> Search script examples for [get_address](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_address)

#### size_t get_address(string_view address_name)

Get the address for a pattern name

### get_rva


> Search script examples for [get_rva](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_rva)

#### size_t get_rva(string_view address_name)

Get the rva for a pattern name

### raise


> Search script examples for [raise](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=raise)

#### nil raise()

Raise a signal and probably crash the game

## Entity functions


### activate_sparktraps_hack


```lua
activate_sparktraps_hack(true);

-- set random speed, direction and distance for the spark
set_post_entity_spawn(function(ent)

	direction = 1
	if prng:random_chance(2, PRNG_CLASS.ENTITY_VARIATION) then
		direction = -1
	end

	ent.speed = prng:random_float(PRNG_CLASS.ENTITY_VARIATION) * 0.1 * direction
	ent.distance = prng:random_float(PRNG_CLASS.ENTITY_VARIATION) * 10

end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_SPARK)
```


> Search script examples for [activate_sparktraps_hack](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=activate_sparktraps_hack)

#### nil activate_sparktraps_hack(bool activate)

Activate custom variables for speed and distance in the `ITEM_SPARK`
note: because those the variables are custom and game does not initiate then, you need to do it yourself for each spark, recommending `set_post_entity_spawn`
default game values are: speed = -0.015, distance = 3.0

### apply_entity_db


> Search script examples for [apply_entity_db](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=apply_entity_db)

#### nil apply_entity_db(int uid)

Apply changes made in [get_type](#get_type)() to entity instance by uid.

### attach_ball_and_chain


> Search script examples for [attach_ball_and_chain](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attach_ball_and_chain)

#### int attach_ball_and_chain(int uid, float off_x, float off_y)

Spawns and attaches ball and chain to `uid`, the initial position of the ball is at the entity position plus `off_x`, `off_y`

### attach_entity


> Search script examples for [attach_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=attach_entity)

#### nil attach_entity(int overlay_uid, int attachee_uid)

Attaches `attachee` to `overlay`, similar to setting `get_entity(attachee).overlay = get_entity(overlay)`.
However this function offsets `attachee` (so you don't have to) and inserts it into `overlay`'s inventory.

### carry


> Search script examples for [carry](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=carry)

#### nil carry(int mount_uid, int rider_uid)

Make `mount_uid` carry `rider_uid` on their back. Only use this with actual mounts and living things.

### change_waddler_drop


> Search script examples for [change_waddler_drop](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_waddler_drop)

#### nil change_waddler_drop(array<[ENT_TYPE](#ENT_TYPE)> ent_types)

Change [ENT_TYPE](#ENT_TYPE)'s spawned when [Waddler](#Waddler) dies, by default there are 3:
{ITEM_PICKUP_COMPASS, ITEM_CHEST, ITEM_KEY}
Max 255 types
Use empty table as argument to reset to the game default

### drop


> Search script examples for [drop](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=drop)

#### nil drop(int who_uid, int what_uid)

Drop an entity by uid

### enter_door


> Search script examples for [enter_door](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enter_door)

#### nil enter_door(int player_uid, int door_uid)

Calls the enter door function, position doesn't matter, can also enter closed doors (like COG, EW) without unlocking them

### entity_get_items_by


> Search script examples for [entity_get_items_by](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_get_items_by)

#### array&lt;int&gt; entity_get_items_by(int uid, array<[ENT_TYPE](#ENT_TYPE)> entity_types, int mask)

#### array&lt;int&gt; entity_get_items_by(int uid, [ENT_TYPE](#ENT_TYPE) entity_type, int mask)

Gets uids of entities attached to given entity uid. Use `entity_type` and `mask` to filter, set them to 0 to return all attached entities.

### entity_has_item_type


> Search script examples for [entity_has_item_type](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_type)

#### bool entity_has_item_type(int uid, array<[ENT_TYPE](#ENT_TYPE)> entity_types)

#### bool entity_has_item_type(int uid, [ENT_TYPE](#ENT_TYPE) entity_type)

Check if the entity `uid` has some ENT_TYPE `entity_type` in their inventory, can also use table of entity_types

### entity_has_item_uid


> Search script examples for [entity_has_item_uid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_has_item_uid)

#### bool entity_has_item_uid(int uid, int item_uid)

Check if the entity `uid` has some specific `item_uid` by uid in their inventory

### entity_remove_item


> Search script examples for [entity_remove_item](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=entity_remove_item)

#### nil entity_remove_item(int uid, int item_uid)

Remove item by uid from entity

### filter_entities


> Search script examples for [filter_entities](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=filter_entities)

#### array&lt;int&gt; filter_entities(array<int> entities, function predicate)

Returns a list of all uids in `entities` for which `predicate(get_entity(uid))` returns true

### flip_entity


> Search script examples for [flip_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=flip_entity)

#### nil flip_entity(int uid)

Flip entity around by uid. All new entities face right by default.

### force_olmec_phase_0


> Search script examples for [force_olmec_phase_0](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_olmec_phase_0)

#### nil force_olmec_phase_0(bool b)

Forces [Olmec](#Olmec) to stay on phase 0 (stomping)

### get_door_target


> Search script examples for [get_door_target](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_door_target)

#### tuple&lt;int, int, int&gt; get_door_target(int uid)

Get door target `world`, `level`, `theme`

### get_entities_at


> Search script examples for [get_entities_at](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_at)

#### array&lt;int&gt; get_entities_at(array<[ENT_TYPE](#ENT_TYPE)> entity_types, int mask, float x, float y, [LAYER](#LAYER) layer, float radius)

#### array&lt;int&gt; get_entities_at([ENT_TYPE](#ENT_TYPE) entity_type, int mask, float x, float y, [LAYER](#LAYER) layer, float radius)

Get uids of matching entities inside some radius. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types

### get_entities_by


```lua
-- find all cavemen and give them bombs
-- using a type and mask in get_entities_by speeds up the search, cause the api knows which bucket to search in
for i,uid in ipairs(get_entities_by(ENT_TYPE.MONS_CAVEMAN, MASK.MONSTER, LAYER.BOTH)) do
    local x, y, l = get_position(uid)
    spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_BOMB, x, y, l)
end

```


> Search script examples for [get_entities_by](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by)

#### array&lt;int&gt; get_entities_by(array<[ENT_TYPE](#ENT_TYPE)> entity_types, int mask, [LAYER](#LAYER) layer)

#### array&lt;int&gt; get_entities_by([ENT_TYPE](#ENT_TYPE) entity_type, int mask, [LAYER](#LAYER) layer)

Get uids of entities by some conditions. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types

### get_entities_by_type


```lua
local types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
set_callback(function()
    local uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    -- is not the same thing as this, but also works
    local uids2 = get_entities_by_type(types)
    print(tostring(#uids).." == "..tostring(#uids2))
end, ON.LEVEL)

```


> Search script examples for [get_entities_by_type](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_type)

#### array&lt;int&gt; get_entities_by_type(int, int...)

Get uids of entities matching id. This function is variadic, meaning it accepts any number of id's.
You can even pass a table!

### get_entities_overlapping_hitbox


> Search script examples for [get_entities_overlapping_hitbox](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping_hitbox)

#### array&lt;int&gt; get_entities_overlapping_hitbox(array<[ENT_TYPE](#ENT_TYPE)> entity_types, int mask, [AABB](#AABB) hitbox, [LAYER](#LAYER) layer)

#### array&lt;int&gt; get_entities_overlapping_hitbox([ENT_TYPE](#ENT_TYPE) entity_type, int mask, [AABB](#AABB) hitbox, [LAYER](#LAYER) layer)

Get uids of matching entities overlapping with the given hitbox. Set `entity_type` or `mask` to `0` to ignore that, can also use table of entity_types

### get_entity


> Search script examples for [get_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity)

#### [Entity](#Entity) get_entity(int uid)

Get the [Entity](#Entity) behind an uid, converted to the correct type. To see what type you will get, consult the [entity hierarchy list](https://github.com/spelunky-fyi/overlunky/blob/main/docs/entities-hierarchy.md)

### get_entity_name


> Search script examples for [get_entity_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_name)

#### nil get_entity_name([ENT_TYPE](#ENT_TYPE) type, optional<bool> fallback_strategy)

Get localized name of an entity, pass `fallback_strategy` as `true` to fall back to the `ENT_TYPE.` enum name
if the entity has no localized name

### get_entity_type


> Search script examples for [get_entity_type](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_type)

#### [ENT_TYPE](#ENT_TYPE) get_entity_type(int uid)

Get the [ENT_TYPE](#ENT_TYPE)... of the entity by uid

### get_grid_entity_at


> Search script examples for [get_grid_entity_at](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_grid_entity_at)

#### int get_grid_entity_at(float x, float y, [LAYER](#LAYER) layer)

Gets a grid entity, such as floor or spikes, at the given position and layer.

### get_player


> Search script examples for [get_player](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_player)

#### [Player](#Player) get_player(int slot, bool or_ghost = false)

Returns [Player](#Player) (or [PlayerGhost](#PlayerGhost) if `get_player(1, true)`) with this player slot

### get_playerghost


> Search script examples for [get_playerghost](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_playerghost)

#### [PlayerGhost](#PlayerGhost) get_playerghost(int slot)

Returns [PlayerGhost](#PlayerGhost) with this player slot 1..4

### get_type


> Search script examples for [get_type](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_type)

#### [EntityDB](#EntityDB) get_type(int id)

Get the [EntityDB](#EntityDB) behind an [ENT_TYPE](#ENT_TYPE)...

### kill_entity


> Search script examples for [kill_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=kill_entity)

#### nil kill_entity(int uid, optional<bool> destroy_corpse = nullopt)

Kills an entity by uid. `destroy_corpse` defaults to `true`, if you are killing for example a caveman and want the corpse to stay make sure to pass `false`.

### lock_door_at


> Search script examples for [lock_door_at](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lock_door_at)

#### nil lock_door_at(float x, float y)

Try to lock the exit at coordinates

### modify_ankh_health_gain


> Search script examples for [modify_ankh_health_gain](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=modify_ankh_health_gain)

#### nil modify_ankh_health_gain(int max_health, int beat_add_health)

Change how much health the ankh gives you after death, with every beat (the heart beat effect) it will add `beat_add_health` to your health,
`beat_add_health` has to be divisor of `health` and can't be 0, otherwise the function does nothing. Set `health` to 0 to return to the game defaults
If you set `health` above the game max health it will be forced down to the game max

### modify_sparktraps


> Search script examples for [modify_sparktraps](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=modify_sparktraps)

#### nil modify_sparktraps(float angle_increment = 0.015, float distance = 3.0)

Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!

### move_entity


> Search script examples for [move_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_entity)

#### nil move_entity(int uid, float x, float y, float vx, float vy)

#### nil move_entity(int uid, float x, float y, float vx, float vy, [LAYER](#LAYER) layer)

Teleport entity to coordinates with optional velocity

### move_grid_entity


> Search script examples for [move_grid_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=move_grid_entity)

#### nil move_grid_entity(int uid, float x, float y, [LAYER](#LAYER) layer)

Teleport grid entity, the destination should be whole number, this ensures that the collisions will work properly

### pick_up


```lua
-- spawn and equip a jetpack on the player
pick_up(players[1].uid, spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER, 0, 0))

```


> Search script examples for [pick_up](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pick_up)

#### nil pick_up(int who_uid, int what_uid)

Pick up another entity by uid. Make sure you're not already holding something, or weird stuff will happen.

### poison_entity


> Search script examples for [poison_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=poison_entity)

#### nil poison_entity(int entity_uid)

Poisons entity, to cure poison set `poison_tick_timer` to -1

### replace_drop


> Search script examples for [replace_drop](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=replace_drop)

#### nil replace_drop(int drop_id, [ENT_TYPE](#ENT_TYPE) new_drop_entity_type)

Changes a particular drop, e.g. what Van Horsing throws at you (use e.g. replace_drop([DROP](#DROP).VAN_HORSING_DIAMOND, [ENT_TYPE](#ENT_TYPE).ITEM_PLASMACANNON))
Use `0` as type to reset this drop to default, use `-1` as drop_id to reset all to default

### set_blood_multiplication


> Search script examples for [set_blood_multiplication](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_blood_multiplication)

#### nil set_blood_multiplication(int default_multiplier, int vladscape_multiplier)

Sets the multiplication factor for blood droplets upon death (default/no [Vlad](#Vlad)'s cape = 1, with [Vlad](#Vlad)'s cape = 2)
Due to changes in 1.23.x only the [Vlad](#Vlad)'s cape value you provide will be used. The default is automatically [Vlad](#Vlad)'s cape value - 1

### set_contents


> Search script examples for [set_contents](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_contents)

#### nil set_contents(int uid, [ENT_TYPE](#ENT_TYPE) item_entity_type)

Set the contents of [ENT_TYPE](#ENT_TYPE).ITEM_POT, [ENT_TYPE](#ENT_TYPE).ITEM_CRATE or [ENT_TYPE](#ENT_TYPE).ITEM_COFFIN `uid` to ENT_TYPE... `item_entity_type`

### set_cursepot_ghost_enabled


> Search script examples for [set_cursepot_ghost_enabled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_cursepot_ghost_enabled)

#### nil set_cursepot_ghost_enabled(bool enable)

Determines whether the ghost appears when breaking the ghost pot

### set_door


> Search script examples for [set_door](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_door)

#### nil set_door(int uid, int w, int l, int t)

Short for [set_door_target](#set_door_target).

### set_door_target


> Search script examples for [set_door_target](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_door_target)

#### nil set_door_target(int uid, int w, int l, int t)

Make an [ENT_TYPE](#ENT_TYPE).FLOOR_DOOR_EXIT go to world `w`, level `l`, theme `t`

### set_drop_chance


> Search script examples for [set_drop_chance](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_drop_chance)

#### nil set_drop_chance(int dropchance_id, int new_drop_chance)

Alters the drop chance for the provided monster-item combination (use e.g. set_drop_chance([DROPCHANCE](#DROPCHANCE).MOLE_MATTOCK, 10) for a 1 in 10 chance)
Use `-1` as dropchance_id to reset all to default

### set_explosion_mask


> Search script examples for [set_explosion_mask](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_explosion_mask)

#### nil set_explosion_mask(int mask)

Sets which entities are affected by a bomb explosion. Default = [MASK](#MASK).PLAYER | [MASK](#MASK).MOUNT | [MASK](#MASK).MONSTER | [MASK](#MASK).ITEM | [MASK](#MASK).ACTIVEFLOOR | [MASK](#MASK).FLOOR

### set_kapala_blood_threshold


> Search script examples for [set_kapala_blood_threshold](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_kapala_blood_threshold)

#### nil set_kapala_blood_threshold(int threshold)

Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).

### set_kapala_hud_icon


> Search script examples for [set_kapala_hud_icon](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_kapala_hud_icon)

#### nil set_kapala_hud_icon(int icon_index)

Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour).
If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!

### set_max_rope_length


> Search script examples for [set_max_rope_length](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_max_rope_length)

#### nil set_max_rope_length(int length)

Sets the maximum length of a thrown rope (anchor segment not included). Unfortunately, setting this higher than default (6) creates visual glitches in the rope, even though it is fully functional.

### set_olmec_phase_y_level


> Search script examples for [set_olmec_phase_y_level](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_olmec_phase_y_level)

#### nil set_olmec_phase_y_level(int phase, float y)

Sets the Y-level at which [Olmec](#Olmec) changes phases

### set_time_ghost_enabled


> Search script examples for [set_time_ghost_enabled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_time_ghost_enabled)

#### nil set_time_ghost_enabled(bool b)

Determines whether the time ghost appears, including the showing of the ghost toast

### set_time_jelly_enabled


> Search script examples for [set_time_jelly_enabled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_time_jelly_enabled)

#### nil set_time_jelly_enabled(bool b)

Determines whether the time jelly appears in cosmic ocean

### unequip_backitem


> Search script examples for [unequip_backitem](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unequip_backitem)

#### nil unequip_backitem(int who_uid)

Unequips the currently worn backitem

### unlock_door_at


> Search script examples for [unlock_door_at](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=unlock_door_at)

#### nil unlock_door_at(float x, float y)

Try to unlock the exit at coordinates

### waddler_count_entity


> Search script examples for [waddler_count_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_count_entity)

#### int waddler_count_entity([ENT_TYPE](#ENT_TYPE) entity_type)

Returns how many of a specific entity type [Waddler](#Waddler) has stored

### waddler_entity_type_in_slot


> Search script examples for [waddler_entity_type_in_slot](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_entity_type_in_slot)

#### int waddler_entity_type_in_slot(int slot)

Gets the entity type of the item in the provided slot

### waddler_get_entity_meta


> Search script examples for [waddler_get_entity_meta](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_get_entity_meta)

#### int waddler_get_entity_meta(int slot)

Gets the 16-bit meta-value associated with the entity type in the associated slot

### waddler_remove_entity


> Search script examples for [waddler_remove_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_remove_entity)

#### nil waddler_remove_entity([ENT_TYPE](#ENT_TYPE) entity_type, int amount_to_remove = 99)

Removes an entity type from [Waddler](#Waddler)'s storage. Second param determines how many of the item to remove (default = remove all)

### waddler_set_entity_meta


> Search script examples for [waddler_set_entity_meta](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_set_entity_meta)

#### nil waddler_set_entity_meta(int slot, int meta)

Sets the 16-bit meta-value associated with the entity type in the associated slot

### waddler_store_entity


> Search script examples for [waddler_store_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=waddler_store_entity)

#### int waddler_store_entity([ENT_TYPE](#ENT_TYPE) entity_type)

Store an entity type in [Waddler](#Waddler)'s storage. Returns the slot number the item was stored in or -1 when storage is full and the item couldn't be stored.

### worn_backitem


> Search script examples for [worn_backitem](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=worn_backitem)

#### int worn_backitem(int who_uid)

Returns the uid of the currently worn backitem, or -1 if wearing nothing

## Feat functions


### change_feat


> Search script examples for [change_feat](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_feat)

#### nil change_feat([FEAT](#Aliases) feat, bool hidden, string name, string description)

Helper function to set the title and description strings for a [FEAT](#Aliases) with change_string, as well as the hidden state.

### get_feat


> Search script examples for [get_feat](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_feat)

#### tuple&lt;bool, bool, const string, const string&gt; get_feat([FEAT](#Aliases) feat)

Check if the user has performed a feat (Real Steam achievement or a hooked one). Returns: `bool unlocked, bool hidden, string name, string description`

### get_feat_hidden


> Search script examples for [get_feat_hidden](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_feat_hidden)

#### bool get_feat_hidden([FEAT](#Aliases) feat)

Get the visibility of a feat

### set_feat_hidden


> Search script examples for [set_feat_hidden](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_feat_hidden)

#### nil set_feat_hidden([FEAT](#Aliases) feat, bool hidden)

Set the visibility of a feat

## Flag functions


### clr_flag


> Search script examples for [clr_flag](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clr_flag)

#### [Flags](#Aliases) clr_flag([Flags](#Aliases) flags, int bit)

Clears a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.

### get_entity_flags


> Search script examples for [get_entity_flags](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags)

#### int get_entity_flags(int uid)

Get the `flags` field from entity by uid

### get_entity_flags2


> Search script examples for [get_entity_flags2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_flags2)

#### int get_entity_flags2(int uid)

Get the `more_flags` field from entity by uid

### get_level_flags


> Search script examples for [get_level_flags](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_level_flags)

#### int get_level_flags()

Get `state.level_flags`

### set_entity_flags


> Search script examples for [set_entity_flags](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags)

#### nil set_entity_flags(int uid, int flags)

Set the `flags` field from entity by uid

### set_entity_flags2


> Search script examples for [set_entity_flags2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_entity_flags2)

#### nil set_entity_flags2(int uid, int flags)

Set the `more_flags` field from entity by uid

### set_flag


> Search script examples for [set_flag](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_flag)

#### [Flags](#Aliases) set_flag([Flags](#Aliases) flags, int bit)

Set a bit in a number. This doesn't actually change the bit in the entity you pass it, it just returns the new value you can use.

### set_level_flags


> Search script examples for [set_level_flags](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_level_flags)

#### nil set_level_flags(int flags)

Set `state.level_flags`

### test_flag


> Search script examples for [test_flag](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=test_flag)

#### bool test_flag([Flags](#Aliases) flags, int bit)

Returns true if a bit is set in the flags

## Generic functions


### change_poison_timer


> Search script examples for [change_poison_timer](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_poison_timer)

#### nil change_poison_timer(int frames)

Change the amount of frames after the damage from poison is applied

### create_image


> Search script examples for [create_image](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_image)

#### tuple&lt;IMAGE, int, int&gt; create_image(string path)

Create image from file. Returns a tuple containing id, width and height.

### disable_floor_embeds


> Search script examples for [disable_floor_embeds](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=disable_floor_embeds)

#### bool disable_floor_embeds(bool disable)

Disable all crust item spawns, returns whether they were already disabled before the call

### force_journal


> Search script examples for [force_journal](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_journal)

#### nil force_journal(int chapter, int entry)

Force the journal to open on a chapter and entry# when pressing the journal button. Only use even entry numbers. Set chapter to `JOURNALUI_PAGE_SHOWN.JOURNAL` to reset. (This forces the journal toggle to always read from `game_manager.save_related.journal_popup_ui.entry_to_show` etc.)

### get_adventure_seed


> Search script examples for [get_adventure_seed](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_adventure_seed)

#### tuple&lt;int, int&gt; get_adventure_seed()

Get the current adventure seed pair

### get_character_heart_color


> Search script examples for [get_character_heart_color](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_character_heart_color)

#### [Color](#Color) get_character_heart_color([ENT_TYPE](#ENT_TYPE) type_id)

Same as `Player.get_heart_color`

### get_frame


> Search script examples for [get_frame](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_frame)

#### int get_frame()

Get the current global frame count since the game was started. You can use this to make some timers yourself, the engine runs at 60fps.

### get_id


> Search script examples for [get_id](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_id)

#### string get_id()

Get your sanitized script id to be used in import.

### get_level_config


> Search script examples for [get_level_config](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_level_config)

#### int get_level_config([LEVEL_CONFIG](#LEVEL_CONFIG) config)

Gets the value for the specified config

### get_ms


> Search script examples for [get_ms](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_ms)

#### nil get_ms()

Get the current timestamp in milliseconds since the Unix Epoch.

### get_setting


> Search script examples for [get_setting](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_setting)

#### optional&lt;int&gt; get_setting([GAME_SETTING](#GAME_SETTING) setting)

Gets the specified setting, values might need to be interpreted differently per setting

### god


> Search script examples for [god](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=god)

#### nil god(bool g)

Enable/disable godmode for players.

### god_companions


> Search script examples for [god_companions](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=god_companions)

#### nil god_companions(bool g)

Enable/disable godmode for companions.

### grow_chainandblocks


> Search script examples for [grow_chainandblocks](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grow_chainandblocks)

#### bool grow_chainandblocks()

#### bool grow_chainandblocks(int x, int y)

Grow chains from `CHAIN_CEILING` and chain with blocks on it from `CHAINANDBLOCKS_CEILING`, it starts looking for the ceilings from the top left corner of a level
To limit it use the parameters, so if you set x to 10, it will only grow chains from ceilings with x < 10, with y = 10 it's ceilings that have y > (level bound top - 10)

### grow_poles


> Search script examples for [grow_poles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grow_poles)

#### nil grow_poles([LAYER](#LAYER) l, int max_lengh)

#### nil grow_poles([LAYER](#LAYER) l, int max_lengh, [AABB](#AABB) area, bool destroy_broken)

Grow pole from `GROWABLE_CLIMBING_POLE` entities in a level, `area` default is whole level, `destroy_broken` default is false

### grow_vines


> Search script examples for [grow_vines](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=grow_vines)

#### nil grow_vines([LAYER](#LAYER) l, int max_lengh)

#### nil grow_vines([LAYER](#LAYER) l, int max_lengh, [AABB](#AABB) area, bool destroy_broken)

Grow vines from `GROWABLE_VINE` and `VINE_TREE_TOP` entities in a level, `area` default is whole level, `destroy_broken` default is false

### import


> Search script examples for [import](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=import)

#### table import(string id, string version = "", bool optional = false)

Load another script by id "author/name" and import its `exports` table. Returns:

- `table` if the script has exports
- `nil` if the script was found but has no exports
- `false` if the script was not found but optional is set to true
- an error if the script was not found and the optional argument was not set

### is_character_female


> Search script examples for [is_character_female](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_character_female)

#### bool is_character_female([ENT_TYPE](#ENT_TYPE) type_id)

Same as `Player.is_female`

### load_death_screen


> Search script examples for [load_death_screen](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load_death_screen)

#### nil load_death_screen()

Immediately ends the run with the death screen, also calls the save_progress

### load_screen


> Search script examples for [load_screen](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load_screen)

#### nil load_screen()

Immediately load a screen based on state.screen_next and stuff

### lowbias32


> Search script examples for [lowbias32](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lowbias32)

#### int lowbias32(int x)

Some random hash function

### lowbias32_r


> Search script examples for [lowbias32_r](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lowbias32_r)

#### int lowbias32_r(int x)

Reverse of some random hash function

### pause


> Search script examples for [pause](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=pause)

#### nil pause(bool p)

Enable/disable game engine pause.
This is just short for `state.pause == 32`, but that produces an audio bug
I suggest `state.pause == 2`, but that won't run any callback, `state.pause == 16` will do the same but `set_global_interval` will still work

### register_console_command


> Search script examples for [register_console_command](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_console_command)

#### nil register_console_command(string name, function cmd)

Adds a command that can be used in the console.

### rgba


> Search script examples for [rgba](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=rgba)

#### [uColor](#Aliases) rgba(int r, int g, int b, int a)

Converts a color to int to be used in drawing functions. Use values from `0..255`.

### save_progress


> Search script examples for [save_progress](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=save_progress)

#### bool save_progress()

Saves the game to savegame.sav, unless game saves are blocked in the settings. Also runs the [ON](#ON).SAVE callback. Fails and returns false, if you're trying to save too often (2s).

### save_script


> Search script examples for [save_script](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=save_script)

#### bool save_script()

Runs the [ON](#ON).SAVE callback. Fails and returns false, if you're trying to save too often (2s).

### script_enabled


> Search script examples for [script_enabled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=script_enabled)

#### bool script_enabled(string id, string version = "")

Check if another script is enabled by id "author/name". You should probably check this after all the other scripts have had a chance to load.

### seed_prng


> Search script examples for [seed_prng](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=seed_prng)

#### nil seed_prng(int seed)

Seed the game prng.

### set_adventure_seed


> Search script examples for [set_adventure_seed](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_adventure_seed)

#### nil set_adventure_seed(int first, int second)

Set the current adventure seed pair

### set_character_heart_color


> Search script examples for [set_character_heart_color](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_character_heart_color)

#### nil set_character_heart_color([ENT_TYPE](#ENT_TYPE) type_id, [Color](#Color) color)

Same as `Player.set_heart_color`

### set_ending_unlock


```lua
-- change character unlocked by endings to pilot
set_ending_unlock(ENT_TYPE.CHAR_PILOT)

-- change texture of the actual savior in endings to pilot
set_callback(function()
    set_post_entity_spawn(function(ent)
        if state.screen == SCREEN.WIN then
            ent:set_texture(TEXTURE.DATA_TEXTURES_CHAR_PINK_0)
        end
        clear_callback()
    end, SPAWN_TYPE.SYSTEMIC, MASK.PLAYER)
end, ON.WIN)

```


> Search script examples for [set_ending_unlock](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_ending_unlock)

#### nil set_ending_unlock([ENT_TYPE](#ENT_TYPE) type)

Force the character unlocked in either ending to [ENT_TYPE](#ENT_TYPE). Set to 0 to reset to the default guys. Does not affect the texture of the actual savior. (See example)

### set_journal_enabled


> Search script examples for [set_journal_enabled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_journal_enabled)

#### nil set_journal_enabled(bool b)

Enables or disables the journal

### set_seed


> Search script examples for [set_seed](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_seed)

#### nil set_seed(int seed)

Set seed and reset run.

### set_storage_layer


```lua
-- Sets the right layer when using the vanilla tile code if waddler is still happy,
-- otherwise spawns the floor to the left of this tile.
-- Manually spawning FLOOR_STORAGE pre-tilecode doesn't seem to work as expected,
-- so we destroy it post-tilecode.
set_post_tile_code_callback(function(x, y, layer)
    if not test_flag(state.quest_flags, 10) then
        -- Just set the layer and let the vanilla tilecode handle the floor
        set_storage_layer(layer)
    else
        local floor = get_entity(get_grid_entity_at(x, y, layer))
        if floor then
            floor:destroy()
        end
        if get_grid_entity_at(x - 1, y, layer) ~= -1 then
            local left = get_entity(get_grid_entity_at(x - 1, y, layer))
            spawn_grid_entity(left.type.id, x, y, layer)
        end
    end
end, "storage_floor")

-- This fixes a bug in the game that breaks storage on transition.
-- The old storage_uid is not cleared after every level for some reason.
set_callback(function()
    state.storage_uid = -1
end, ON.TRANSITION)

-- Having a waddler is completely optional for storage,
-- but this makes a nice waddler room if he still likes you.
define_tile_code("waddler")
set_pre_tile_code_callback(function(x, y, layer)
    if not test_flag(state.quest_flags, 10) then
        local uid = spawn_roomowner(ENT_TYPE.MONS_STORAGEGUY, x + 0.5, y, layer, ROOM_TEMPLATE.WADDLER)
        set_on_kill(uid, function()
            -- Disable current level storage if you kill waddler
            state.storage_uid = -1
        end)
    end
    return true
end, "waddler")

```


> Search script examples for [set_storage_layer](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_storage_layer)

#### nil set_storage_layer([LAYER](#LAYER) layer)

Set layer to search for storage items on

### show_journal


> Search script examples for [show_journal](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=show_journal)

#### nil show_journal([JOURNALUI_PAGE_SHOWN](#JOURNALUI_PAGE_SHOWN) chapter, int page)

Open the journal on a chapter and page. The main Journal spread is pages 0..1, so most chapters start at 2. Use even page numbers only.

### toggle_journal


> Search script examples for [toggle_journal](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toggle_journal)

#### nil toggle_journal()

Open or close the journal as if pressing the journal button. Will respect visible journal popups and force_journal.

### update_liquid_collision_at


> Search script examples for [update_liquid_collision_at](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=update_liquid_collision_at)

#### nil update_liquid_collision_at(float x, float y, bool add)

Updates the floor collisions used by the liquids, set add to false to remove tile of collision, set to true to add one

### warp


> Search script examples for [warp](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=warp)

#### nil warp(int w, int l, int t)

Warp to a level immediately.

## Input functions


### get_io


> Search script examples for [get_io](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_io)

#### // Access via get_io()

Returns: [ImGuiIO](#ImGuiIO) for raw keyboard, mouse and xinput gamepad stuff. This is kinda bare and might change.
- Note: The clicked/pressed actions only make sense in `ON.GUIFRAME`.
- Note: Lua starts indexing at 1, you need `keysdown[string.byte('A') + 1]` to find the A key.
- Note: Overlunky/etc will eat all keys it is currently configured to use, your script will only get leftovers.
- Note: `gamepad` is basically [XINPUT_GAMEPAD](https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad) but variables are renamed and values are normalized to -1.0..1.0 range.

### mouse_position


> Search script examples for [mouse_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=mouse_position)

#### tuple&lt;float, float&gt; mouse_position()

Current mouse cursor position in screen coordinates.

### return_input


> Search script examples for [return_input](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=return_input)

#### nil return_input(int uid)

Return input previously stolen with [steal_input](#steal_input)

### send_input


> Search script examples for [send_input](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=send_input)

#### nil send_input(int uid, [INPUTS](#INPUTS) buttons)

Send input to entity, has to be previously stolen with [steal_input](#steal_input)

### steal_input


> Search script examples for [steal_input](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=steal_input)

#### nil steal_input(int uid)

Steal input from a [Player](#Player), HiredHand or [PlayerGhost](#PlayerGhost)

## Lighting functions


### create_illumination


> Search script examples for [create_illumination](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_illumination)

#### [Illumination](#Illumination) create_illumination([Color](#Color) color, float size, float x, float y)

#### [Illumination](#Illumination) create_illumination([Color](#Color) color, float size, int uid)

Creates a new [Illumination](#Illumination). Don't forget to continuously call `refresh_illumination`, otherwise your light emitter fades out! Check out the illumination.lua script for an example

### refresh_illumination


> Search script examples for [refresh_illumination](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=refresh_illumination)

#### nil refresh_illumination([Illumination](#Illumination) illumination)

Refreshes an [Illumination](#Illumination), keeps it from fading out

## Message functions


### cancel_speechbubble


> Search script examples for [cancel_speechbubble](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cancel_speechbubble)

#### nil cancel_speechbubble()


### cancel_toast


> Search script examples for [cancel_toast](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=cancel_toast)

#### nil cancel_toast()


### console_prinspect


> Search script examples for [console_prinspect](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=console_prinspect)

#### nil console_prinspect(variadic_args objects)

Prinspect to console

### console_print


> Search script examples for [console_print](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=console_print)

#### nil console_print(string message)

Print a log message to console.

### log_print


> Search script examples for [log_print](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=log_print)

#### nil log_print(string message)

Log to spelunky.log

### lua_print


> Search script examples for [lua_print](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=lua_print)

#### nil lua_print()

Standard lua print function, prints directly to the terminal but not to the game

### message


> Search script examples for [message](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=message)

#### nil message(string message)

Same as `print`

### messpect


> Search script examples for [messpect](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=messpect)

#### nil messpect(variadic_args objects)

Same as `prinspect`

### prinspect


```lua
prinspect(state.level, state.level_next)
local some_stuff_in_a_table = {
    some = state.time_total,
    stuff = state.world
}
prinspect(some_stuff_in_a_table)

```


> Search script examples for [prinspect](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=prinspect)

#### nil prinspect(variadic_args objects)

Prints any type of object by first funneling it through `inspect`, no need for a manual `tostring` or `inspect`.

### print


> Search script examples for [print](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=print)

#### nil print(string message)

Print a log message on screen.

### say


> Search script examples for [say](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=say)

#### nil say(int entity_uid, string message, int sound_type, bool top)

Show a message coming from an entity

### speechbubble_visible


> Search script examples for [speechbubble_visible](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=speechbubble_visible)

#### bool speechbubble_visible()


### toast


> Search script examples for [toast](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast)

#### nil toast(string message)

Show a message that looks like a level feeling.

### toast_visible


> Search script examples for [toast_visible](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=toast_visible)

#### bool toast_visible()


## Movable Behavior functions


### make_custom_behavior


> Search script examples for [make_custom_behavior](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=make_custom_behavior)

#### [CustomMovableBehavior](#CustomMovableBehavior) make_custom_behavior(string_view behavior_name, int state_id, [VanillaMovableBehavior](#VanillaMovableBehavior) base_behavior)

Make a `CustomMovableBehavior`, if `base_behavior` is `nil` you will have to set all of the
behavior functions. If a behavior with `behavior_name` already exists for your script it will
be returned instead.

## Network functions


### udp_listen


> Search script examples for [udp_listen](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=udp_listen)

#### UdpServer udp_listen(string host, in_port_t port, function cb)

Start an UDP server on specified address and run callback when data arrives. Return a string from the callback to reply. Requires unsafe mode.

### udp_send


> Search script examples for [udp_send](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=udp_send)

#### nil udp_send(string host, in_port_t port, string msg)

Send data to specified UDP address. Requires unsafe mode.

## Option functions


### register_option_bool


```lua
register_option_bool("bomb_bag", "BombBag", "Spawn bomb bag at the start of every level", false)

set_callback(function()
    if options.bomb_bag then
        -- Spawn the bomb bag at player location thanks to the LAYER.PLAYER1
        spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_PICKUP_BOMBBAG, 0, 0, LAYER.PLAYER1)
    end
end, ON.LEVEL)
```


> Search script examples for [register_option_bool](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_bool)

#### nil register_option_bool(string name, string desc, string long_desc, bool value)

Add a boolean option that the user can change in the UI. Read with `options.name`, `value` is the default.

### register_option_button


> Search script examples for [register_option_button](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_button)

#### nil register_option_button(string name, string desc, string long_desc, function on_click)

Add a button that the user can click in the UI. Sets the timestamp of last click on value and runs the callback function.

### register_option_callback


> Search script examples for [register_option_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_callback)

#### nil register_option_callback(string name, object value, function on_render)

Add custom options using the window drawing functions. Everything drawn in the callback will be rendered in the options window and the return value saved to `options[name]` or overwriting the whole `options` table if using and empty name. `value` is the default value, and pretty important because anything defined in the callback function will only be defined after the options are rendered. See the example for details.
The callback signature is optional<any> on_render([GuiDrawContext](#GuiDrawContext) draw_ctx)

### register_option_combo


> Search script examples for [register_option_combo](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_combo)

#### nil register_option_combo(string name, string desc, string long_desc, string opts, int value)

Add a combobox option that the user can change in the UI. Read the int index of the selection with `options.name`. Separate `opts` with `\0`,
with a double `\0\0` at the end. `value` is the default index 1..n.

### register_option_float


> Search script examples for [register_option_float](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_float)

#### nil register_option_float(string name, string desc, string long_desc, float value, float min, float max)

Add a float option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
limits, you can override them in the UI with double click.

### register_option_int


> Search script examples for [register_option_int](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_int)

#### nil register_option_int(string name, string desc, string long_desc, int value, int min, int max)

Add an integer option that the user can change in the UI. Read with `options.name`, `value` is the default. Keep in mind these are just soft
limits, you can override them in the UI with double click.

### register_option_string


> Search script examples for [register_option_string](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=register_option_string)

#### nil register_option_string(string name, string desc, string long_desc, string value)

Add a string option that the user can change in the UI. Read with `options.name`, `value` is the default.

## Particle functions


### advance_screen_particles


> Search script examples for [advance_screen_particles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=advance_screen_particles)

#### nil advance_screen_particles([ParticleEmitterInfo](#ParticleEmitterInfo) particle_emitter)

Advances the state of the screen particle emitter (simulates the next positions, ... of all the particles in the emitter). Only used with screen particle emitters. See the `particles.lua` example script for more details.

### extinguish_particles


> Search script examples for [extinguish_particles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=extinguish_particles)

#### nil extinguish_particles([ParticleEmitterInfo](#ParticleEmitterInfo) particle_emitter)

Extinguish a particle emitter (use the return value of `generate_world_particles` or `generate_screen_particles` as the parameter in this function)

### generate_screen_particles


> Search script examples for [generate_screen_particles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=generate_screen_particles)

#### [ParticleEmitterInfo](#ParticleEmitterInfo) generate_screen_particles(int particle_emitter_id, float x, float y)

Generate particles of the specified type at a certain screen coordinate (use e.g. `local emitter = generate_screen_particles(PARTICLEEMITTER.CHARSELECTOR_TORCHFLAME_FLAMES, 0.0, 0.0)`). See the `particles.lua` example script for more details.

### generate_world_particles


> Search script examples for [generate_world_particles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=generate_world_particles)

#### [ParticleEmitterInfo](#ParticleEmitterInfo) generate_world_particles(int particle_emitter_id, int uid)

Generate particles of the specified type around the specified entity uid (use e.g. `local emitter = generate_world_particles(PARTICLEEMITTER.PETTING_PET, players[1].uid)`). You can then decouple the emitter from the entity with `emitter.entity_uid = -1` and freely move it around. See the `particles.lua` example script for more details.

### get_particle_type


> Search script examples for [get_particle_type](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_particle_type)

#### [ParticleDB](#ParticleDB) get_particle_type(int id)

Get the [ParticleDB](#ParticleDB) details of the specified ID

### render_screen_particles


> Search script examples for [render_screen_particles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=render_screen_particles)

#### nil render_screen_particles([ParticleEmitterInfo](#ParticleEmitterInfo) particle_emitter)

Renders the particles to the screen. Only used with screen particle emitters. See the `particles.lua` example script for more details.

## Position functions


### distance


> Search script examples for [distance](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=distance)

#### float distance(int uid_a, int uid_b)

Calculate the tile distance of two entities by uid

### draw_text_size


```lua
-- draw text
set_callback(function(draw_ctx)
    -- get a random color
    local color = math.random(0, 0xffffffff)
    -- zoom the font size based on frame
    local size = (get_frame() % 199)+1
    local text = 'Awesome!'
    -- calculate size of text
    local w, h = draw_text_size(size, text)
    -- draw to the center of screen
    draw_ctx:draw_text(0-w/2, 0-h/2, size, text, color)
end, ON.GUIFRAME)

```


> Search script examples for [draw_text_size](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text_size)

#### tuple&lt;float, float&gt; draw_text_size(float size, string text)

Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.

### fix_liquid_out_of_bounds


```lua
-- call this in ON.FRAME if needed in your custom level
set_callback(fix_liquid_out_of_bounds, ON.FRAME)

```


> Search script examples for [fix_liquid_out_of_bounds](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=fix_liquid_out_of_bounds)

#### nil fix_liquid_out_of_bounds()

Removes all liquid that is about to go out of bounds, which crashes the game.

### game_position


> Search script examples for [game_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=game_position)

#### tuple&lt;float, float&gt; game_position(float x, float y)

Get the game coordinates at the screen position (`x`, `y`)

### get_bounds


```lua
-- Draw the level boundaries
set_callback(function(draw_ctx)
    local xmin, ymax, xmax, ymin = get_bounds()
    local sx, sy = screen_position(xmin, ymax) -- top left
    local sx2, sy2 = screen_position(xmax, ymin) -- bottom right
    draw_ctx:draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)

```


> Search script examples for [get_bounds](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_bounds)

#### tuple&lt;float, float, float, float&gt; get_bounds()

Basically gets the absolute coordinates of the area inside the unbreakable bedrock walls, from wall to wall. Every solid entity should be
inside these boundaries. The order is: top left x, top left y, bottom right x, bottom right y

### get_camera_position


> Search script examples for [get_camera_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_camera_position)

#### tuple&lt;float, float&gt; get_camera_position()

Gets the current camera position in the level

### get_hitbox


> Search script examples for [get_hitbox](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_hitbox)

#### [AABB](#AABB) get_hitbox(int uid, optional<float> extrude, optional<float> offsetx, optional<float> offsety)

Gets the hitbox of an entity, use `extrude` to make the hitbox bigger/smaller in all directions and `offset` to offset the hitbox in a given direction

### get_position


> Search script examples for [get_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_position)

#### tuple&lt;float, float, int&gt; get_position(int uid)

Get position `x, y, layer` of entity by uid. Use this, don't use `Entity.x/y` because those are sometimes just the offset to the entity
you're standing on, not real level coordinates.

### get_render_hitbox


> Search script examples for [get_render_hitbox](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_render_hitbox)

#### [AABB](#AABB) get_render_hitbox(int uid, optional<float> extrude, optional<float> offsetx, optional<float> offsety)

Same as `get_hitbox` but based on `get_render_position`

### get_render_position


> Search script examples for [get_render_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_render_position)

#### tuple&lt;float, float, int&gt; get_render_position(int uid)

Get interpolated render position `x, y, layer` of entity by uid. This gives smooth hitboxes for 144Hz master race etc...

### get_velocity


> Search script examples for [get_velocity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_velocity)

#### tuple&lt;float, float&gt; get_velocity(int uid)

Get velocity `vx, vy` of an entity by uid. Use this, don't use `Entity.velocityx/velocityy` because those are relative to `Entity.overlay`.

### get_window_size


> Search script examples for [get_window_size](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_window_size)

#### tuple&lt;int, int&gt; get_window_size()

Gets the resolution (width and height) of the screen

### get_zoom_level


> Search script examples for [get_zoom_level](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_zoom_level)

#### float get_zoom_level()

Get the current set zoom level

### position_is_valid


> Search script examples for [position_is_valid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=position_is_valid)

#### bool position_is_valid(float x, float y, [LAYER](#LAYER) layer, [POS_TYPE](#POS_TYPE) flags)

Check if position satifies the given [POS_TYPE](#POS_TYPE) flags, to be used in a custom is_valid function procedural for spawns.

### screen_aabb


> Search script examples for [screen_aabb](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_aabb)

#### [AABB](#AABB) screen_aabb([AABB](#AABB) box)

Convert an `AABB` to a screen `AABB` that can be directly passed to draw functions

### screen_distance


> Search script examples for [screen_distance](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_distance)

#### float screen_distance(float x)

Translate a distance of `x` tiles to screen distance to be be used in drawing functions

### screen_position


> Search script examples for [screen_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=screen_position)

#### tuple&lt;float, float&gt; screen_position(float x, float y)

Translate an entity position to screen position to be used in drawing functions

### set_camp_camera_bounds_enabled


> Search script examples for [set_camp_camera_bounds_enabled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_camp_camera_bounds_enabled)

#### nil set_camp_camera_bounds_enabled(bool b)

Enables or disables the default position based camp camera bounds, to set them manually yourself

### zoom


> Search script examples for [zoom](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=zoom)

#### nil zoom(float level)

Set the zoom level used in levels and shops. 13.5 is the default.

## Room functions


### define_room_template


> Search script examples for [define_room_template](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_room_template)

#### int define_room_template(string room_template, [ROOM_TEMPLATE_TYPE](#ROOM_TEMPLATE_TYPE) type)

Define a new room remplate to use with `set_room_template`

### get_room_index


> Search script examples for [get_room_index](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_index)

#### tuple&lt;int, int&gt; get_room_index(float x, float y)

Transform a position to a room index to be used in `get_room_template` and `PostRoomGenerationContext.set_room_template`

### get_room_pos


> Search script examples for [get_room_pos](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_pos)

#### tuple&lt;float, float&gt; get_room_pos(int x, int y)

Transform a room index into the top left corner position in the room

### get_room_template


> Search script examples for [get_room_template](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_template)

#### optional&lt;int&gt; get_room_template(int x, int y, [LAYER](#LAYER) layer)

Get the room template given a certain index, returns `nil` if coordinates are out of bounds

### get_room_template_name


> Search script examples for [get_room_template_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_room_template_name)

#### string_view get_room_template_name(int room_template)

For debugging only, get the name of a room template, returns `'invalid'` if room template is not defined

### is_machine_room_origin


> Search script examples for [is_machine_room_origin](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_machine_room_origin)

#### bool is_machine_room_origin(int x, int y)

Get whether a room is the origin of a machine room

### is_room_flipped


> Search script examples for [is_room_flipped](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_room_flipped)

#### bool is_room_flipped(int x, int y)

Get whether a room is flipped at the given index, returns `false` if coordinates are out of bounds

### set_room_template_size


> Search script examples for [set_room_template_size](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_room_template_size)

#### bool set_room_template_size(int room_template, int width, int height)

Set the size of room template in tiles, the template must be of type `ROOM_TEMPLATE_TYPE.MACHINE_ROOM`.

### spawn_roomowner


```lua
-- spawns waddler selling pets
-- all the aggro etc mechanics from a normal shop still apply
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_roomowner(ENT_TYPE.MONS_STORAGEGUY, x+1, y, l, ROOM_TEMPLATE.SHOP)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_DOG, x-1, y, l), owner)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_CAT, x-2, y, l), owner)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_HAMSTER, x-3, y, l), owner)

-- use in a tile code to add shops to custom levels
-- this may spawn some shop related decorations too
define_tile_code("pet_shop_boys")
set_pre_tile_code_callback(function(x, y, layer)
    local owner = spawn_roomowner(ENT_TYPE.MONS_YANG, x, y, layer, ROOM_TEMPLATE.SHOP)
    -- another dude for the meme, this has nothing to do with the shop
    spawn_on_floor(ENT_TYPE.MONS_BODYGUARD, x+1, y, layer)
    add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_HAMSTER, x+2, y, layer), owner)
    return true
end, "pet_shop_boys")

```


> Search script examples for [spawn_roomowner](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_roomowner)

#### int spawn_roomowner([ENT_TYPE](#ENT_TYPE) owner_type, float x, float, y, [LAYER](#LAYER) layer, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template = -1)

Spawn a [RoomOwner](#RoomOwner) (or a few other like CavemanShopkeeper) in the coordinates and make them own the room, optionally changing the room template. Returns the [RoomOwner](#RoomOwner) uid.

## Shop functions


### add_item_to_shop


> Search script examples for [add_item_to_shop](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_item_to_shop)

#### nil add_item_to_shop(int item_uid, int shop_owner_uid)

Adds entity as shop item, has to be movable (haven't tested many)

### change_diceshop_prizes


> Search script examples for [change_diceshop_prizes](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_diceshop_prizes)

#### nil change_diceshop_prizes(array<[ENT_TYPE](#ENT_TYPE)> ent_types)

Change [ENT_TYPE](#ENT_TYPE)'s spawned in dice shops (Madame Tusk as well), by default there are 25:
{ITEM_PICKUP_BOMBBAG, ITEM_PICKUP_BOMBBOX, ITEM_PICKUP_ROPEPILE, ITEM_PICKUP_COMPASS, ITEM_PICKUP_PASTE, ITEM_PICKUP_PARACHUTE, ITEM_PURCHASABLE_CAPE, ITEM_PICKUP_SPECTACLES, ITEM_PICKUP_CLIMBINGGLOVES, ITEM_PICKUP_PITCHERSMITT,
ENT_TYPE_ITEM_PICKUP_SPIKESHOES, ENT_TYPE_ITEM_PICKUP_SPRINGSHOES, ITEM_MACHETE, ITEM_BOOMERANG, ITEM_CROSSBOW, ITEM_SHOTGUN, ITEM_FREEZERAY, ITEM_WEBGUN, ITEM_CAMERA, ITEM_MATTOCK, ITEM_PURCHASABLE_JETPACK, ITEM_PURCHASABLE_HOVERPACK,
ITEM_TELEPORTER, ITEM_PURCHASABLE_TELEPORTER_BACKPACK, ITEM_PURCHASABLE_POWERPACK}
Min 6, Max 255, if you want less then 6 you need to write some of them more then once (they will have higher "spawn chance")
If you use this function in the level with diceshop in it, you have to update `item_ids` in the [ITEM_DICE_PRIZE_DISPENSER](#PrizeDispenser)
Use empty table as argument to reset to the game default

### is_inside_active_shop_room


> Search script examples for [is_inside_active_shop_room](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_inside_active_shop_room)

#### bool is_inside_active_shop_room(float x, float y, [LAYER](#LAYER) layer)

Checks whether a coordinate is inside a room containing an active shop. This function checks whether the shopkeeper is still alive.

### is_inside_shop_zone


> Search script examples for [is_inside_shop_zone](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=is_inside_shop_zone)

#### bool is_inside_shop_zone(float x, float y, [LAYER](#LAYER) layer)

Checks whether a coordinate is inside a shop zone, the rectangle where the camera zooms in a bit. Does not check if the shop is still active!

### spawn_shopkeeper


```lua
-- spawns a shopkeeper selling a shotgun next to you
-- converts the current room to a ROOM_TEMPLATE.SHOP with shop music and zoom effect
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_shopkeeper(x+1, y, l)
add_item_to_shop(spawn_on_floor(ENT_TYPE.ITEM_SHOTGUN, x-1, y, l), owner)

-- spawns a shopkeeper selling a puppy next to you
-- also converts the room to a shop, but after the shopkeeper is spawned
-- this enables the safe zone for moving items, but disables shop music and zoom for whatever reason
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_shopkeeper(x+1, y, l, ROOM_TEMPLATE.SIDE)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_DOG, x-1, y, l), owner)
local ctx = PostRoomGenerationContext:new()
local rx, ry = get_room_index(x, y)
ctx:set_room_template(rx, ry, l, ROOM_TEMPLATE.SHOP)

```


> Search script examples for [spawn_shopkeeper](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_shopkeeper)

#### int spawn_shopkeeper(float x, float, y, [LAYER](#LAYER) layer, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template = [ROOM_TEMPLATE](#ROOM_TEMPLATE).SHOP)

Spawn a [Shopkeeper](#Shopkeeper) in the coordinates and make the room their shop. Returns the [Shopkeeper](#Shopkeeper) uid. Also see spawn_roomowner.

## Sound functions


### create_sound


> Search script examples for [create_sound](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=create_sound)

#### optional&lt;[CustomSound](#CustomSound)&gt; create_sound(string path)

Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found

### get_sound


> Search script examples for [get_sound](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_sound)

#### optional&lt;[CustomSound](#CustomSound)&gt; get_sound(string path_or_vanilla_sound)

Gets an existing sound, either if a file at the same path was already loaded or if it is already loaded by the game

### play_sound


> Search script examples for [play_sound](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=play_sound)

#### [SoundMeta](#SoundMeta) play_sound([VANILLA_SOUND](#VANILLA_SOUND) sound, int source_uid)


## Spawn functions


### change_altar_damage_spawns


> Search script examples for [change_altar_damage_spawns](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_altar_damage_spawns)

#### nil change_altar_damage_spawns(array<[ENT_TYPE](#ENT_TYPE)> ent_types)

Change [ENT_TYPE](#ENT_TYPE)'s spawned when you damage the altar, by default there are 6:
{MONS_BAT, MONS_BEE, MONS_SPIDER, MONS_JIANGSHI, MONS_FEMALE_JIANGSHI, MONS_VAMPIRE}
Max 255 types
Use empty table as argument to reset to the game default

### change_sunchallenge_spawns


> Search script examples for [change_sunchallenge_spawns](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_sunchallenge_spawns)

#### nil change_sunchallenge_spawns(array<[ENT_TYPE](#ENT_TYPE)> ent_types)

Change [ENT_TYPE](#ENT_TYPE)'s spawned by `FLOOR_SUNCHALLENGE_GENERATOR`, by default there are 4:
{MONS_WITCHDOCTOR, MONS_VAMPIRE, MONS_SORCERESS, MONS_NECROMANCER}
Because of the game logic number of entity types has to be a power of 2: (1, 2, 4, 8, 16, 32), if you want say 30 types, you need to write two entities two times (they will have higher "spawn chance")
Use empty table as argument to reset to the game default

### default_spawn_is_valid


> Search script examples for [default_spawn_is_valid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=default_spawn_is_valid)

#### bool default_spawn_is_valid(float x, float y, [LAYER](#LAYER) layer)

Default function in spawn definitions to check whether a spawn is valid or not

### define_extra_spawn


> Search script examples for [define_extra_spawn](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_extra_spawn)

#### int define_extra_spawn(function do_spawn, function is_valid, int num_spawns_frontlayer, int num_spawns_backlayer)

Define a new extra spawn, these are semi-guaranteed level gen spawns with a fixed upper bound.
The function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
Use for example when you can spawn only on the ceiling, under water or inside a shop.
Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
To change the number of spawns use `PostRoomGenerationContext::set_num_extra_spawns` during `ON.POST_ROOM_GENERATION`
No name is attached to the extra spawn since it is not modified from level files, instead every call to this function will return a new uniqe id.

### define_procedural_spawn


> Search script examples for [define_procedural_spawn](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_procedural_spawn)

#### [PROCEDURAL_CHANCE](#PROCEDURAL_CHANCE) define_procedural_spawn(string procedural_spawn, function do_spawn, function is_valid)

Define a new procedural spawn, the function `nil do_spawn(x, y, layer)` contains your code to spawn the thing, whatever it is.
The function `bool is_valid(x, y, layer)` determines whether the spawn is legal in the given position and layer.
Use for example when you can spawn only on the ceiling, under water or inside a shop.
Set `is_valid` to `nil` in order to use the default rule (aka. on top of floor and not obstructed).
If a user disables your script but still uses your level mod nothing will be spawned in place of your procedural spawn.

### door


> Search script examples for [door](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=door)

#### int door(float x, float y, [LAYER](#LAYER) layer, int w, int l, int t)

Short for [spawn_door](#spawn_door).

### get_missing_extra_spawns


> Search script examples for [get_missing_extra_spawns](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_missing_extra_spawns)

#### tuple&lt;int, int&gt; get_missing_extra_spawns(int extra_spawn_chance_id)

Use to query whether any of the requested spawns could not be made, usually because there were not enough valid spaces in the level.
Returns missing spawns in the front layer and missing spawns in the back layer in that order.
The value only makes sense after level generation is complete, aka after `ON.POST_LEVEL_GENERATION` has run.

### get_procedural_spawn_chance


> Search script examples for [get_procedural_spawn_chance](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_procedural_spawn_chance)

#### int get_procedural_spawn_chance([PROCEDURAL_CHANCE](#PROCEDURAL_CHANCE) chance_id)

Get the inverse chance of a procedural spawn for the current level.
A return value of 0 does not mean the chance is infinite, it means the chance is zero.

### layer_door


> Search script examples for [layer_door](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=layer_door)

#### nil layer_door(float x, float y)

Short for [spawn_layer_door](#spawn_layer_door).

### set_ghost_spawn_times


> Search script examples for [set_ghost_spawn_times](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_ghost_spawn_times)

#### nil set_ghost_spawn_times(int normal = 10800, int cursed = 9000)

Determines when the ghost appears, either when the player is cursed or not

### spawn


```lua
-- spawn a jetpack next to the player
spawn(ENT_TYPE.ITEM_JETPACK, 1, 0, LAYER.PLAYER, 0, 0)

```


> Search script examples for [spawn](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn)

#### int spawn([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer, float vx, float vy)

Short for [spawn_entity](#spawn_entity).

### spawn_apep


> Search script examples for [spawn_apep](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_apep)

#### int spawn_apep(float x, float y, [LAYER](#LAYER) layer, bool right)

Spawns apep with the choice if it going left or right, if you want the game to choose use regular spawn functions with `ENT_TYPE.MONS_APEP_HEAD`

### spawn_companion


> Search script examples for [spawn_companion](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_companion)

#### int spawn_companion([ENT_TYPE](#ENT_TYPE) companion_type, float x, float y, [LAYER](#LAYER) layer)

Spawn a companion (hired hand, player character, eggplant child)

### spawn_critical


> Search script examples for [spawn_critical](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_critical)

#### int spawn_critical([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer, float vx, float vy)

Short for [spawn_entity_nonreplaceable](#spawn_entity_nonreplaceable).

### spawn_door


> Search script examples for [spawn_door](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_door)

#### int spawn_door(float x, float y, [LAYER](#LAYER) layer, int w, int l, int t)

Spawn a door to another world, level and theme and return the uid of spawned entity.
Uses level coordinates with [LAYER](#LAYER).FRONT and [LAYER](#LAYER).BACK, but player-relative coordinates with [LAYER](#LAYER).PLAYERn

### spawn_entity


```lua
-- spawn megajelly on top of player using absolute coordinates on level start
set_callback(function()
    local x, y, layer = get_position(players[1].uid)
    spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
end, ON.LEVEL)

-- spawn clover next to player using player-relative coordinates
set_callback(function()
    spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 1, 0, LAYER.PLAYER1, 0, 0)
end, ON.LEVEL)

```


> Search script examples for [spawn_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity)

#### int spawn_entity([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer, float vx, float vy)

Spawn an entity in position with some velocity and return the uid of spawned entity.
Uses level coordinates with [[LAYER](#LAYER).FRONT](#LAYER) and [LAYER](#LAYER).BACK, but player-relative coordinates with [LAYER](#LAYER).PLAYERn.

### spawn_entity_nonreplaceable


> Search script examples for [spawn_entity_nonreplaceable](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_nonreplaceable)

#### int spawn_entity_nonreplaceable([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer, float vx, float vy)

Same as `spawn_entity` but does not trigger any pre-entity-spawn callbacks, so it will not be replaced by another script

### spawn_entity_over


> Search script examples for [spawn_entity_over](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_over)

#### int spawn_entity_over([ENT_TYPE](#ENT_TYPE) entity_type, int over_uid, float x, float y)

Spawn an entity of `entity_type` attached to some other entity `over_uid`, in offset `x`, `y`

### spawn_entity_snapped_to_floor


> Search script examples for [spawn_entity_snapped_to_floor](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_entity_snapped_to_floor)

#### int spawn_entity_snapped_to_floor([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer)

Spawns an entity directly on the floor below the tile at the given position.
Use this to avoid the little fall that some entities do when spawned during level gen callbacks.

### spawn_grid_entity


> Search script examples for [spawn_grid_entity](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_grid_entity)

#### int spawn_grid_entity([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer)

Spawn a grid entity, such as floor or traps, that snaps to the grid.

### spawn_layer_door


> Search script examples for [spawn_layer_door](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_layer_door)

#### nil spawn_layer_door(float x, float y)

Spawn a door to backlayer.

### spawn_liquid


> Search script examples for [spawn_liquid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_liquid)

#### nil spawn_liquid([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y)

#### nil spawn_liquid([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, float velocityx, float velocityy, int liquid_flags, int amount, float blobs_separation)

Spawn liquids, always spawns in the front layer, will have fun effects if `entity_type` is not a liquid (only the short version, without velocity etc.).
Don't overuse this, you are still restricted by the liquid pool sizes and thus might crash the game.
`liquid_flags` - not much known about, 2 - will probably crash the game, 3 - pause_physics, 6-12 is probably agitation, surface_tension etc. set to 0 to ignore
`amount` - it will spawn amount x amount (so 1 = 1, 2 = 4, 3 = 6 etc.), `blobs_separation` is optional

### spawn_mushroom


> Search script examples for [spawn_mushroom](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_mushroom)

#### int spawn_mushroom(float x, float y, [LAYER](#LAYER) l)

#### int spawn_mushroom(float x, float y, [LAYER](#LAYER) l, int height)

Spawns and grows mushroom, height relates to the trunk, without it, it will roll the game default 3-5 height
Regardless, if there is not enough space, it will spawn shorter one or if there is no space even for the smallest one, it will just not spawn at all
Returns uid of the base or -1 if it wasn't able to spawn

### spawn_on_floor


> Search script examples for [spawn_on_floor](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_on_floor)

#### int spawn_on_floor([ENT_TYPE](#ENT_TYPE) entity_type, float x, float y, [LAYER](#LAYER) layer)

Short for [spawn_entity_snapped_to_floor](#spawn_entity_snapped_to_floor).

### spawn_over


> Search script examples for [spawn_over](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_over)

#### int spawn_over([ENT_TYPE](#ENT_TYPE) entity_type, int over_uid, float x, float y)

Short for [spawn_entity_over](#spawn_entity_over)

### spawn_player


> Search script examples for [spawn_player](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_player)

#### nil spawn_player(int player_slot, float x, float y)

Spawn a player in given location, if player of that slot already exist it will spawn clone, the game may crash as this is very unexpected situation
If you want to respawn a player that is a ghost, set in his inventory `health` to above 0, and `time_of_death` to 0 and call this function, the ghost entity will be removed automatically

### spawn_playerghost


> Search script examples for [spawn_playerghost](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_playerghost)

#### int spawn_playerghost([ENT_TYPE](#ENT_TYPE) char_type, float x, float y, [LAYER](#LAYER) layer)

Spawn the [PlayerGhost](#PlayerGhost) entity, it will not move and not be connected to any player, you can then use [steal_input](#steal_input) and send_input to controll it
or change it's `player_inputs` to the `input` of real player so he can control it directly

### spawn_tree


> Search script examples for [spawn_tree](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_tree)

#### int spawn_tree(float x, float y, [LAYER](#LAYER) layer)

#### int spawn_tree(float x, float y, [LAYER](#LAYER) layer, int height)

Spawns and grows a tree

### spawn_unrolled_player_rope


> Search script examples for [spawn_unrolled_player_rope](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=spawn_unrolled_player_rope)

#### int spawn_unrolled_player_rope(float x, float y, [LAYER](#LAYER) layer, [TEXTURE](#TEXTURE) texture)

#### int spawn_unrolled_player_rope(float x, float y, [LAYER](#LAYER) layer, [TEXTURE](#TEXTURE) texture, int max_length)

Spawns an already unrolled rope as if created by player

## String functions


### add_custom_name


> Search script examples for [add_custom_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_custom_name)

#### nil add_custom_name(int uid, string name)

Adds custom name to the item by uid used in the shops
This is better alternative to `add_string` but instead of changing the name for entity type, it changes it for this particular entity

### add_string


> Search script examples for [add_string](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=add_string)

#### [STRINGID](#Aliases) add_string(string str)

Add custom string, currently can only be used for names of shop items (Entitydb->description)
Returns [STRINGID](#Aliases) of the new string

### change_string


> Search script examples for [change_string](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=change_string)

#### nil change_string([STRINGID](#Aliases) id, string str)

Change string at the given id (don't use stringid diretcly for vanilla string, use `hash_to_stringid` first)
This edits custom string and in game strings but changing the language in settings will reset game strings

### clear_custom_name


> Search script examples for [clear_custom_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clear_custom_name)

#### nil clear_custom_name(int uid)

Clears the name set with `add_custom_name`

### enum_get_name


> Search script examples for [enum_get_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=enum_get_name)

#### string enum_get_name(table enum, int value)

Return the name of an unknown number in an enum table

### get_character_name


> Search script examples for [get_character_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_character_name)

#### string get_character_name([ENT_TYPE](#ENT_TYPE) type_id)

Same as `Player.get_name`

### get_character_short_name


> Search script examples for [get_character_short_name](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_character_short_name)

#### string get_character_short_name([ENT_TYPE](#ENT_TYPE) type_id)

Same as `Player.get_short_name`

### get_string


> Search script examples for [get_string](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_string)

#### const string get_string([STRINGID](#Aliases) string_id)

Get string behind [STRINGID](#Aliases) (don't use stringid diretcly for vanilla string, use `hash_to_stringid` first)
Will return the string of currently choosen language

### hash_to_stringid


> Search script examples for [hash_to_stringid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=hash_to_stringid)

#### [STRINGID](#Aliases) hash_to_stringid(int hash)

Convert the hash to stringid
Check [strings00_hashed.str](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/strings00_hashed.str) for the hash values, or extract assets with modlunky and check those.

### set_level_string


```lua
-- set the level string shown in hud, journal and game over
-- also change the one used in transitions for consistency
set_callback(function()
    if state.screen_next == SCREEN.LEVEL then
        local level_str = "test" .. tostring(state.level_count)
        set_level_string(level_str)
        change_string(hash_to_stringid(0xda7c0c5b), F"{level_str} COMPLETED!")
    end
end, ON.PRE_LOAD_SCREEN)

```


> Search script examples for [set_level_string](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_level_string)

#### nil set_level_string(string str)

Set the level number shown in the hud and journal to any string. This is reset to the default "%d-%d" automatically just before PRE_LOAD_SCREEN to a level or main menu, so use in PRE_LOAD_SCREEN, POST_LEVEL_GENERATION or similar for each level.
Use "%d-%d" to reset to default manually. Does not affect the "...COMPLETED!" message in transitions or lines in "Dear Journal", you need to edit them separately with `change_string`.

## Texture functions


### define_texture


> Search script examples for [define_texture](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_texture)

#### [TEXTURE](#TEXTURE) define_texture([TextureDefinition](#TextureDefinition) texture_data)

Defines a new texture that can be used in [Entity](#Entity)::set_texture
If a texture with the same definition already exists the texture will be reloaded from disk.

### get_texture


> Search script examples for [get_texture](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture)

#### optional&lt;[TEXTURE](#TEXTURE)&gt; get_texture([TextureDefinition](#TextureDefinition) texture_data)

Gets a texture with the same definition as the given, if none exists returns `nil`

### get_texture_definition


> Search script examples for [get_texture_definition](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_texture_definition)

#### [TextureDefinition](#TextureDefinition) get_texture_definition([TEXTURE](#TEXTURE) texture_id)

Gets a `TextureDefinition` for equivalent to the one used to define the texture with `id`

### reload_texture


> Search script examples for [reload_texture](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=reload_texture)

#### nil reload_texture(string texture_path)

Reloads a texture from disk, use this only as a development tool for example in the console
Note that `define_texture` will also reload the texture if it already exists

### reset_lut


> Search script examples for [reset_lut](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=reset_lut)

#### nil reset_lut([LAYER](#LAYER) layer)

Same as `set_lut(nil, layer)`

### set_lut


> Search script examples for [set_lut](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_lut)

#### nil set_lut(optional<[TEXTURE](#TEXTURE)> texture_id, [LAYER](#LAYER) layer)

Force the LUT texture for the given layer (or both) until it is reset
Pass `nil` in the first parameter to reset

## Theme functions


### force_co_subtheme


> Search script examples for [force_co_subtheme](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_co_subtheme)

#### nil force_co_subtheme(int subtheme)

Forces the theme of the next cosmic ocean level(s) (use e.g. `force_co_subtheme(COSUBTHEME.JUNGLE)`. Use `COSUBTHEME.RESET` to reset to default random behaviour)

### force_custom_subtheme


> Search script examples for [force_custom_subtheme](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_custom_subtheme)

#### nil force_custom_subtheme()

Force current subtheme used in the CO theme. You can pass a [CustomTheme](#CustomTheme), [ThemeInfo](#ThemeInfo) or [THEME](#THEME). Not to be confused with force_co_subtheme.

### force_custom_theme


> Search script examples for [force_custom_theme](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_custom_theme)

#### nil force_custom_theme()

Force a theme in PRE_LOAD_LEVEL_FILES, POST_ROOM_GENERATION or PRE_LEVEL_GENERATION to change different aspects of the levelgen. You can pass a [CustomTheme](#CustomTheme), [ThemeInfo](#ThemeInfo) or [THEME](#THEME).

### get_co_subtheme


> Search script examples for [get_co_subtheme](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_co_subtheme)

#### int get_co_subtheme()

Gets the sub theme of the current cosmic ocean level, returns `COSUBTHEME.NONE` if the current level is not a CO level.

## Tile code functions


### define_tile_code


> Search script examples for [define_tile_code](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=define_tile_code)

#### [TILE_CODE](#TILE_CODE) define_tile_code(string tile_code)

Define a new tile code, to make this tile code do anything you have to use either `set_pre_tile_code_callback` or `set_post_tile_code_callback`.
If a user disables your script but still uses your level mod nothing will be spawned in place of your tile code.

### get_short_tile_code


> Search script examples for [get_short_tile_code](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_tile_code)

#### optional&lt;int&gt; get_short_tile_code([ShortTileCodeDef](#ShortTileCodeDef) short_tile_code_def)

Gets a short tile code based on definition, returns `nil` if it can't be found

### get_short_tile_code_definition


> Search script examples for [get_short_tile_code_definition](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_short_tile_code_definition)

#### optional&lt;[ShortTileCodeDef](#ShortTileCodeDef)&gt; get_short_tile_code_definition([SHORT_TILE_CODE](#Aliases) short_tile_code)

Gets the definition of a short tile code (if available), will vary depending on which file is loaded

### set_post_tile_code_callback


> Search script examples for [set_post_tile_code_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_post_tile_code_callback)

#### [CallbackId](#Aliases) set_post_tile_code_callback(function cb, string tile_code)

Add a callback for a specific tile code that is called after the game handles the tile code.
Use this to affect what the game or other scripts spawned in this position.
This is received even if a previous pre-tile-code-callback has returned true
The callback signature is nil post_tile_code(float x, float y, int layer, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template)

### set_pre_tile_code_callback


> Search script examples for [set_pre_tile_code_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_pre_tile_code_callback)

#### [CallbackId](#Aliases) set_pre_tile_code_callback(function cb, string tile_code)

Add a callback for a specific tile code that is called before the game handles the tile code.
Return true in order to stop the game or scripts loaded after this script from handling this tile code.
For example, when returning true in this callback set for `"floor"` then no floor will spawn in the game (unless you spawn it yourself)
The callback signature is bool pre_tile_code(float x, float y, int layer, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template)

## Deprecated functions

<aside class='warning'>These functions still exist but their usage is discouraged, they all have alternatives mentioned here so please use those!</aside>

### on_frame

Use `set_callback(function, ON.FRAME)` instead

### on_camp

Use `set_callback(function, ON.CAMP)` instead

### on_level

Use `set_callback(function, ON.LEVEL)` instead

### on_start

Use `set_callback(function, ON.START)` instead

### on_transition

Use `set_callback(function, ON.TRANSITION)` instead

### on_death

Use `set_callback(function, ON.DEATH)` instead

### on_win

Use `set_callback(function, ON.WIN)` instead

### on_screen

Use `set_callback(function, ON.SCREEN)` instead

### on_guiframe

Use `set_callback(function, ON.GUIFRAME)` instead

### load_script


> Search script examples for [load_script](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=load_script)

`nil load_script()`<br/>
Same as import().

### read_prng


> Search script examples for [read_prng](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_prng)

`array<int> read_prng()`<br/>
Read the game prng state. Use [prng](#PRNG):get_pair() instead.

### force_dark_level


```lua
-- forces any level to be dark, even bosses
set_callback(function()
    state.level_flags = set_flag(state.level_flags, 18)
end, ON.POST_ROOM_GENERATION)

```


> Search script examples for [force_dark_level](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=force_dark_level)

`nil force_dark_level(bool g)`<br/>
Set level flag 18 on post room generation instead, to properly force every level to dark

### get_entities


> Search script examples for [get_entities](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities)

#### array&lt;int&gt; get_entities()

Use `get_entities_by(0, MASK.ANY, LAYER.BOTH)` instead

### get_entities_by_mask


> Search script examples for [get_entities_by_mask](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_mask)

#### array&lt;int&gt; get_entities_by_mask(int mask)

Use `get_entities_by(0, mask, LAYER.BOTH)` instead

### get_entities_by_layer


> Search script examples for [get_entities_by_layer](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_by_layer)

#### array&lt;int&gt; get_entities_by_layer([LAYER](#LAYER) layer)

Use `get_entities_by(0, MASK.ANY, layer)` instead

### get_entities_overlapping


> Search script examples for [get_entities_overlapping](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entities_overlapping)

#### array&lt;int&gt; get_entities_overlapping(array<[ENT_TYPE](#ENT_TYPE)> entity_types, int mask, float sx, float sy, float sx2, float sy2, [LAYER](#LAYER) layer)

#### array&lt;int&gt; get_entities_overlapping([ENT_TYPE](#ENT_TYPE) entity_type, int mask, float sx, float sy, float sx2, float sy2, [LAYER](#LAYER) layer)

Use `get_entities_overlapping_hitbox` instead

### get_entity_ai_state


> Search script examples for [get_entity_ai_state](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=get_entity_ai_state)

#### int get_entity_ai_state(int uid)

As the name is misleading. use entity `move_state` field instead

### set_arrowtrap_projectile


> Search script examples for [set_arrowtrap_projectile](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_arrowtrap_projectile)

#### nil set_arrowtrap_projectile([ENT_TYPE](#ENT_TYPE) regular_entity_type, [ENT_TYPE](#ENT_TYPE) poison_entity_type)

Use `replace_drop(DROP.ARROWTRAP_WOODENARROW, new_arrow_type)` and `replace_drop(DROP.POISONEDARROWTRAP_WOODENARROW, new_arrow_type)` instead

### set_camera_position


> Search script examples for [set_camera_position](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_camera_position)

#### nil set_camera_position(float cx, float cy)

this doesn't actually work at all. See State -> [Camera](#Camera) the for proper camera handling

### setflag


> Search script examples for [setflag](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=setflag)

`nil setflag()`<br/>

### clrflag


> Search script examples for [clrflag](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=clrflag)

`nil clrflag()`<br/>

### testflag


> Search script examples for [testflag](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=testflag)

`nil testflag()`<br/>

### read_input


> Search script examples for [read_input](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_input)

`INPUTS read_input(int uid)`<br/>
Use `players[1].input.buttons_gameplay` for only the inputs during the game, or `.buttons` for all the inputs, even during the pause menu
Of course, you can get the player by other mean, it doesn't need to be the `players` table
You can only read inputs from actual players, HH don't have any inputs

### read_stolen_input


> Search script examples for [read_stolen_input](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=read_stolen_input)

`INPUTS read_stolen_input(int uid)`<br/>
Read input that has been previously stolen with [steal_input](#steal_input)
Use `state.player_inputs.player_slots[player_slot].buttons_gameplay` for only the inputs during the game, or `.buttons` for all the inputs, even during the pause menu

### generate_particles


> Search script examples for [generate_particles](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=generate_particles)

#### [ParticleEmitterInfo](#ParticleEmitterInfo) generate_particles(int particle_emitter_id, int uid)

Use `generate_world_particles`

### draw_line


> Search script examples for [draw_line](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_line)

`nil draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color)`<br/>
Use `GuiDrawContext.draw_line` instead

### draw_rect


> Search script examples for [draw_rect](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect)

`nil draw_rect(float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color)`<br/>
Use `GuiDrawContext.draw_rect` instead

### draw_rect_filled


> Search script examples for [draw_rect_filled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_rect_filled)

`nil draw_rect_filled(float x1, float y1, float x2, float y2, float rounding, uColor color)`<br/>
Use `GuiDrawContext.draw_rect_filled` instead

### draw_circle


> Search script examples for [draw_circle](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle)

`nil draw_circle(float x, float y, float radius, float thickness, uColor color)`<br/>
Use `GuiDrawContext.draw_circle` instead

### draw_circle_filled


> Search script examples for [draw_circle_filled](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_circle_filled)

`nil draw_circle_filled(float x, float y, float radius, uColor color)`<br/>
Use `GuiDrawContext.draw_circle_filled` instead

### draw_text


> Search script examples for [draw_text](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_text)

`nil draw_text(float x, float y, float size, string text, uColor color)`<br/>
Use `GuiDrawContext.draw_text` instead

### draw_image


> Search script examples for [draw_image](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image)

`nil draw_image(IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)`<br/>
Use `GuiDrawContext.draw_image` instead

### draw_image_rotated


> Search script examples for [draw_image_rotated](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=draw_image_rotated)

`nil draw_image_rotated(IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)`<br/>
Use `GuiDrawContext.draw_image_rotated` instead

### window


> Search script examples for [window](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=window)

`nil window(string title, float x, float y, float w, float h, bool movable, function callback)`<br/>
Use `GuiDrawContext.window` instead

### win_text


> Search script examples for [win_text](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_text)

`nil win_text(string text)`<br/>
Use `GuiDrawContext.win_text` instead

### win_separator


> Search script examples for [win_separator](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_separator)

`nil win_separator()`<br/>
Use `GuiDrawContext.win_separator` instead

### win_inline


> Search script examples for [win_inline](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_inline)

`nil win_inline()`<br/>
Use `GuiDrawContext.win_inline` instead

### win_sameline


> Search script examples for [win_sameline](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_sameline)

`nil win_sameline(float offset, float spacing)`<br/>
Use `GuiDrawContext.win_sameline` instead

### win_button


> Search script examples for [win_button](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_button)

`bool win_button(string text)`<br/>
Use `GuiDrawContext.win_button` instead

### win_input_text


> Search script examples for [win_input_text](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_text)

`string win_input_text(string label, string value)`<br/>
Use `GuiDrawContext.win_input_text` instead

### win_input_int


> Search script examples for [win_input_int](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_int)

`int win_input_int(string label, int value)`<br/>
Use `GuiDrawContext.win_input_int` instead

### win_input_float


> Search script examples for [win_input_float](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_input_float)

`float win_input_float(string label, float value)`<br/>
Use `GuiDrawContext.win_input_float` instead

### win_slider_int


> Search script examples for [win_slider_int](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_int)

`int win_slider_int(string label, int value, int min, int max)`<br/>
Use `GuiDrawContext.win_slider_int` instead

### win_drag_int


> Search script examples for [win_drag_int](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_int)

`int win_drag_int(string label, int value, int min, int max)`<br/>
Use `GuiDrawContext.win_drag_int` instead

### win_slider_float


> Search script examples for [win_slider_float](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_slider_float)

`float win_slider_float(string label, float value, float min, float max)`<br/>
Use `GuiDrawContext.win_slider_float` instead

### win_drag_float


> Search script examples for [win_drag_float](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_drag_float)

`float win_drag_float(string label, float value, float min, float max)`<br/>
Use `GuiDrawContext.win_drag_float` instead

### win_check


> Search script examples for [win_check](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_check)

`bool win_check(string label, bool value)`<br/>
Use `GuiDrawContext.win_check` instead

### win_combo


> Search script examples for [win_combo](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_combo)

`int win_combo(string label, int selected, string opts)`<br/>
Use `GuiDrawContext.win_combo` instead

### win_pushid


> Search script examples for [win_pushid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_pushid)

`nil win_pushid(int id)`<br/>
Use `GuiDrawContext.win_pushid` instead

### win_popid


> Search script examples for [win_popid](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_popid)

`nil win_popid()`<br/>
Use `GuiDrawContext.win_popid` instead

### win_image


> Search script examples for [win_image](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=win_image)

`nil win_image(IMAGE image, int width, int height)`<br/>
Use `GuiDrawContext.win_image` instead
