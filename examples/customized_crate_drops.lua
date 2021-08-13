meta.name = "Customized Crate Drops"
meta.version = "WIP"
meta.description = "Makes all crates drop items in the color of the opening player"
meta.author = "Malacath"

set_post_entity_spawn(function(crate)
    local function purplify_drop(crate, killer_or_opener)
        if killer_or_opener.get_heart_color then
            -- declaration is seperate from definition because otherwise it can not become an upvalue of the function
            local cb

            cb = set_post_entity_spawn(function(ent)
                ent.color = killer_or_opener:get_heart_color()
                clear_callback(cb)
            end, SPAWN_TYPE.ANY, MASK.ANY, crate.inside)
        end
    end
    set_on_kill(crate.uid, purplify_drop)
    set_on_open(crate.uid, purplify_drop)
end, SPAWN_TYPE.ANY, MASK.ANY, ENT_TYPE.ITEM_CRATE)
