meta.name = 'Mole Particles'
meta.version = 'WIP'
meta.description = 'Recolor mole particles when in non-dwelling floors to match the floor theme.'
meta.author = 'JayTheBusinessGoose'

local function particle_color_components_for_texture(texture)
    if texture == TEXTURE.DATA_TEXTURES_FLOOR_SURFACE_0 then
        return .4, .51, .68
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_BABYLON_0 then
        return .21, .73, .69
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_SUNKEN_0 then
        return .17, .64, .43
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_CAVE_0 then
        return nil
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_VOLCANO_0 then
        return .59, .43, .52
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_JUNGLE_0 then
        return .55, .66, .18
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_TIDEPOOL_0 then
        return .18, .78, .74
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0 then
        return .75, .57, .36
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_ICE_0 then
        return .32, .44, .69
    elseif texture == TEXTURE.DATA_TEXTURES_FLOOR_EGGPLANT_0 then
        return .63, .26, .87
    end
end

local function particle_color_for_texture(texture)
    local r, g, b = particle_color_components_for_texture(texture)
    if not r or not g or not b then return nil end

    return Color:new(r, g, b, 1)
end

local function fix_particles(particles, layer)
    for _, particle in pairs(particles) do
        -- Only check particles that were just created on this frame. Others will be colored properly since these
        -- particles do not move.
        if particle.lifetime == 1 then
            local x, y = particle.x, particle.y
            local grid_uid = get_grid_entity_at(math.floor(x + .5), math.floor(y + .5), layer)
            if grid_uid ~= -1 then
                local grid_ent = get_entity(grid_uid)
                local texture = grid_ent:get_texture()
                local color = particle_color_for_texture(texture)
                if color then
                    particle.color = color:get_ucolor()
                end
            end
        end
    end
end

local function configure_mole(mole_ent)
    local mole_uid = mole_ent.uid
    local particle_callback
    local level_callback
    local function clear_callbacks()
        if particle_callback then
            clear_callback(particle_callback)
        end
        if level_callback then
            clear_callback(level_callback)
        end
        level_callback = nil
        particle_callback = nil
    end
    particle_callback = set_callback(function(_, draw_depth)
        if mole_ent.uid ~= mole_uid then
            clear_callbacks()
            return
        end
        -- This is the draw depth that the particles are rendered at, change the color just before
        -- they render to make sure they are recolored on the first frame.
        if draw_depth ~= 10 then return end
        local particle_emitter = mole_ent.burrowing_particle
        fix_particles(particle_emitter.emitted_particles, LAYER.FRONT)
        fix_particles(particle_emitter.emitted_particles_back_layer, LAYER.BACK)
    end, ON.RENDER_PRE_DRAW_DEPTH)
    level_callback = set_callback(function()
        clear_callbacks()
    end, ON.PRE_LOAD_LEVEL_FILES)
end

set_post_entity_spawn(function(mole_ent)
    configure_mole(mole_ent)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_MOLE)