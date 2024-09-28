--If you want the locked door to look like the closed exit door at Hundun

function close_hundun_door(door)
    door:unlock(false)
    for _, uid in pairs(get_entities_overlapping_grid(door.x, door.y, door.layer)) do
        ent = get_entity(uid)
        if ent.type.id == ENT_TYPE.BG_DOOR then
            ent:set_texture(TEXTURE.DATA_TEXTURES_DECO_EGGPLANT_0)
            return
        end
    end
end

