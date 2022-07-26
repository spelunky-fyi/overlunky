meta.name = "Seed Finder Examples"
meta.version = "WIP"
meta.description = "This adds some simple example scripts to the Scriptable Seed Finder"
meta.author = "Dregu"

finder = import("dregu/seedfinder")

finder.add({
    goal = "1-2 Shop Jetpack",
    deepest_world = 1,
    deepest_level = 2,
    success = function()
        for i,v in ipairs(get_entities_by(ENT_TYPE.ITEM_PURCHASABLE_JETPACK, MASK.ITEM, LAYER.FRONT)) do
            finder.uid = v
            return true
        end
    end
})

finder.add({
    goal = "Crust Jetpack",
    success = function()
        for i,v in ipairs(get_entities_by(ENT_TYPE.ITEM_JETPACK, MASK.ITEM, LAYER.FRONT)) do
            finder.uid = v
            return true
        end
    end
})

finder.add({
    goal = "Crust teleporter near the entrance",
    deepest_world = 1,
    deepest_level = 1,
    success = function()
        for i,v in ipairs(get_entities_by(ENT_TYPE.ITEM_TELEPORTER, MASK.ITEM, LAYER.FRONT)) do
            if distance(players[1].uid, v) < 10 then
                finder.uid = v
                return true
            end
        end
    end
})

finder.add({
    goal = "Custom [container] item",

    -- GuiDrawContext and a custom options table are passed here for you to populate
    -- This function is then called to show your custom options
    options = function(ctx, opt)
        local layers = {LAYER.FRONT, LAYER.BACK, LAYER.BOTH}
        opt.item = ctx:win_input_int("ENT_TYPE", opt.item or ENT_TYPE.ITEM_PLASMACANNON)
        ctx:win_inline()
        ctx:win_text(enum_get_name(ENT_TYPE, opt.item) or "")

        opt.distance = ctx:win_input_int("Max distance", opt.distance or 1000)

        opt.layer = ctx:win_combo("Layer", opt.layer or 3, "Front\0Back\0Both\0\0")
        opt.real_layer = layers[opt.layer] or LAYER.BOTH
    end,

    -- The custom options table is also passed to the success and reset functions
    success = function(opt)
        for i,v in ipairs(get_entities_by(opt.item, MASK.ITEM, opt.real_layer)) do
            if distance(players[1].uid, v) < opt.distance then
                finder.uid = v
                return true
            end
        end
        for i,v in ipairs(get_entities_by(ENT_TYPE.CONTAINER, MASK.ITEM, opt.real_layer)) do
            local ent = get_entity(v)
            if distance(players[1].uid, v) < opt.distance and ent.inside == opt.item then
                finder.uid = v
                return true
            end
        end
    end
})
