meta.name = "Vlad's cape multi-jump"
meta.version = "WIP"
meta.description = "Unlimited jumping with Vlad's cape"
meta.author = "Zappatic"

-- give unlimited jumps to all vlad's capes
-- note that this disables floating!
set_callback(function()
    vladscapes = get_entities_by_type(ENT_TYPE.ITEM_VLADS_CAPE)
    for i, cape_uid in ipairs(vladscapes) do
        get_entity(cape_uid):as_vlads_cape().can_double_jump = true
    end
end, ON.GAMEFRAME)
