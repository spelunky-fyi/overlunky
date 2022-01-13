meta.name = "Illumination"
meta.version = "WIP"
meta.description = "Demonstrates how to create custom illumination"
meta.author = "Zappatic"


-- This script creates two light emitters at the start of each level, a red one that follows the player around
-- and a blue one that stays stationary, wherever the player spawned.

static_light_emitter = nil
following_light_emitter = nil

set_callback(function()
    -- Create a light emitter linked to a uid (so it follows it around)
    following_light_emitter = create_illumination(Color:red(), 10.0, players[1].uid)

    -- Creates a light emitter at a stationary position
    x, y, layer = get_position(players[1].uid)
    static_light_emitter = create_illumination(Color:blue(), 10.0, x, y)
end, ON.LEVEL)


set_callback(function()
    if following_light_emitter ~= nil then
        -- In order for light emitters not to fade out, you have to "refresh" them each frame. If you do not do this,
        -- the brightness will be reduced with -0.05 per frame.
        -- Unfortunately, exiting from the pause menu prevents this callback from running for 1 frame, so to prevent
        -- the brightness from reducing a tiny bit each time the player pauses, it is recommended to force the brightness
        -- here as well.
        refresh_illumination(following_light_emitter)
        following_light_emitter.brightness = 2.0
    end
    if static_light_emitter ~= nil then
        refresh_illumination(static_light_emitter)
        static_light_emitter.brightness = 2.0
    end
end, ON.FRAME)
