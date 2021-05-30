meta = {
	name = "Custom Texture",
	version = "WIP",
	description = "Shows how to add and use custom textures",
	author = "Malacath"
}

-- load a textured just like "floor_cave.png"
local bad_texture_id = nil
do
    local texture_def = get_texture_definition(TEXTURE.DATA_TEXTURES_FLOOR_CAVE_0)
    texture_def.texture_path = "floor_puke.png"
    bad_texture_id = define_texture(texture_def)
end

-- replace random floor tiles with the new texture
-- this looks like shit, but it proves that both textures still exist
set_post_tile_code_callback(function(x, y, l)
    local ents = get_entities_overlapping(0, MASK.FLOOR, x - 0.45, y - 0.45, x + 0.45, y + 0.45, l);
    for _, uid in pairs(ents) do
        if math.random() > 0.5 then
            local ent = get_entity(uid)
            ent:set_texture(bad_texture_id)
        end
    end
end, "floor")
