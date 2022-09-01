Used to store static common data for an ENT_TYPE. You can also clone entity types with the copy constructor to create new custom entities with different common properties. [This tool](https://dregu.github.io/Spelunky2ls/animation.html) can be helpful when messing with the animations. The default values are also listed in [entities.json](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/entities.json).

> When cloning an entity type, remember to save it in the script for as long as you need it. Otherwise the memory will be freed immediately, which eventually leads to a crash when used or overwritten by other stuff:

```lua
-- Create a special fast snake type with weird animation
special_snake = EntityDB:new(ENT_TYPE.MONS_SNAKE)
special_snake.max_speed = 1
special_snake.acceleration = 2
special_snake.animations[2].num_tiles = 1

set_post_entity_spawn(function(snake)
    -- 50% chance to make snakes special
    if prng:random_chance(2, PRNG_CLASS.PROCEDURAL_SPAWNS) then
        -- Assign custom type
        snake.type = special_snake
        -- This is only really needed if types are changed during the level
        snake.current_animation = special_snake.animations[2]
    end
end, SPAWN_TYPE.ANY, MASK.MONSTER, ENT_TYPE.MONS_SNAKE)
```

> You can also use Entity.user_data to store the custom type:

```lua
-- Custom player who is buffed a bit every level
set_callback(function()
    -- Doing this to include HH
    for i,v in ipairs(get_entities_by_mask(MASK.PLAYER)) do
        local player = get_entity(v)

        -- Create new custom type on the first level, based on the original type
        if not player.user_data then
            player.user_data = {}
            player.user_data.type = EntityDB:new(player.type.id)
        end

        -- Set the player entity type to the custom type every level
        player.type = player.user_data.type

        -- Buff the player every subsequent level
        if state.level_count > 0 then
            player.type.max_speed = player.type.max_speed * 1.1
            player.type.acceleration = player.type.acceleration * 1.1
            player.type.jump = player.type.jump * 1.1
        end
    end
end, ON.POST_LEVEL_GENERATION)
```

> Illegal bad example, don't do this:

```lua
set_callback(function()
    -- Nobody owns the new type and the memory is freed immediately, eventually leading to a crash
    players[1].type = EntityDB:new(players[1].type)
    players[1].type.max_speed = 2
end, ON.POST_LEVEL_GENERATION)
```
