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
    -- 2.0 to not to be abstracted by the level illumination, you probably want different value for backlayer
    following_light_emitter.brightness = 2.0

    -- Creates a light emitter at a stationary position
    x, y, layer = get_position(players[1].uid)
    static_light_emitter = create_illumination(Color:blue(), 10.0, x, y)
    -- 2.0 to not to be abstracted by the level illumination, you probably want different value for backlayer
    static_light_emitter.brightness = 2.0
end, ON.LEVEL)

set_callback(function()
    -- need to reset at the end of a level, so we don't access it as it's no longer valid
    static_light_emitter = nil
    following_light_emitter = nil
end, ON.PRE_LEVEL_DESTRUCTION)


set_callback(function()
    if following_light_emitter ~= nil then
        -- In order for light emitters not to fade out, you have to "refresh" them each frame. If you do not do this,
        -- the brightness will be reduced with -0.05 per frame.
        refresh_illumination(following_light_emitter)
    end
    if static_light_emitter ~= nil then
        refresh_illumination(static_light_emitter)
    end
end, ON.PRE_UPDATE)
