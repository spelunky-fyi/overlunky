meta.name = "Jetpack fuel test"
meta.version = "WIP"
meta.description = "Give unlimited jetpack fuel"
meta.author = "Zappatic"

-- give unlimited fuel to jetpacks
set_callback(function()
    for i, player in ipairs(players) do
        player:set_jetpack_fuel(255)
    end
end, ON.GAMEFRAME)
