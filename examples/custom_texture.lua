meta = {
	name = "Custom Texture",
	version = "WIP",
	description = "Shows how to add and use custom textures",
	author = "Malacath"
}

-- load a textured just like "floor_cave.png"
local bad_texture_id = nil
do
    local texture_data = TextureData.new()
    texture_data.texture_path = "floor_puke.png"
    texture_data.width = 1536
    texture_data.height = 1536
    texture_data.tile_width = 128
    texture_data.tile_height = 128
    bad_texture_id = define_entity_texture(texture_data)
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
