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
        return get_entities_by(ENT_TYPE.ITEM_PURCHASABLE_JETPACK, MASK.ITEM, LAYER.FRONT)[1] ~= nil
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
