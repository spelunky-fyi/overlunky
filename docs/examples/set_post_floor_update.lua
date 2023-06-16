-- Use FLOOR_GENERIC with textures from different themes that update correctly when destroyed.
-- This lets you use the custom tile code 'floor_generic_tidepool'
-- in the level editor to spawn tidepool floor in dwelling for example...
define_tile_code("floor_generic_tidepool")
set_pre_tile_code_callback(function(x, y, layer)
    local uid = spawn_grid_entity(ENT_TYPE.FLOOR_GENERIC, x, y, layer)
    set_post_floor_update(uid, function(me)
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
    ent:set_post_floor_update(function(me)
        me:set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0)
        for i,v in ipairs(entity_get_items_by(me.uid, ENT_TYPE.DECORATION_GENERIC, MASK.DECORATION)) do
            local deco = get_entity(v)
            deco:set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0)
        end
    end)
end, SPAWN_TYPE.ANY, MASK.FLOOR, ENT_TYPE.FLOOR_QUICKSAND)
