local module = {}

function module.hello()
    if #players > 0 then
        message("Hello from another module and here's a jetpack")
        spawn(ENT_TYPE.ITEM_JETPACK, 0, 0, LAYER.PLAYER1, 0, 0)
    end
end

return module
