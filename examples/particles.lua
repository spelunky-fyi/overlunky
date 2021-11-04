meta.name = "Particles"
meta.version = "WIP"
meta.description = "Demonstrates how the particle emitters work"
meta.author = "Zappatic"

-------------------------------------
-- WORLD COORDINATE PARTICLE EMITTERS
-------------------------------------
-- These emitters are by default linked to a specific entity, via its uid.

-- Change the hearts particle emitter to be permanent (otherwise it would only show 3 hearts and stop)
get_particle_type(PARTICLEEMITTER.PETTING_PET).permanent = true
set_callback(function(render_ctx)
    -- create the hearts particle emitter and link it to the player
    local world_emitter = generate_world_particles(PARTICLEEMITTER.PETTING_PET, players[1].uid)
end, ON.LEVEL)

-- If you want to put the particle emitter in a specific spot in the level you can disconnect it from its uid, like so:
-- world_emitter.entity_uid = -1
-- Now, you can manipulate the x and y coordinate of the emitter and it will stay in that location in the level

--------------------------------------
-- SCREEN COORDINATE PARTICLE EMITTERS
--------------------------------------
-- These emitters stay on the same position on the screen, regardless of where you are in the level.

-- The following example will put a particle emitter on the heart, making it look like it's on fire
-- For screen particle emitters, we have to do more:
-- First you have to create the emitter, and be sure to keep track of it
-- Then, every frame, you have to advance it (simulate the particles to the next positions, ...)
-- as well as render it to the screen.
emitter = nil
set_callback(function(render_ctx)
    if state.screen == SCREEN.LEVEL and emitter == nil then
        emitter = generate_screen_particles(PARTICLEEMITTER.CHARSELECTOR_TORCHFLAME_FLAMES, -0.93, 0.94)
    end
end, ON.SCREEN)

set_post_render_screen(SCREEN.LEVEL, function(screen, render_ctx)
    if emitter ~= nil then
        advance_screen_particles(emitter)
        render_screen_particles(emitter)
    end
end)

-- For screen particle emitters it is very important that you only generate them once and keep track of them, for however many you need.
-- Each generate call allocates memory in the game, so if you generate a new emitter every frame, you will very quickly run out of memory!
-- In the example above, even if you quit the level to the main menu and then come back to the level, the same originally generated emitter
-- will be used, which is what you want.
