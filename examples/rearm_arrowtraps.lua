meta.name = "Rearm arrowtraps"
meta.version = "WIP"
meta.description = "Every ten seconds, all arrow traps (plain and poison) get rearmed"
meta.author = "Zappatic"

set_callback(function()
    set_interval(function()
        arrowtraps = get_entities_by_type(ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP)
        for i, trap_id in ipairs(arrowtraps) do
            trap = get_entity(trap_id):as_arrowtrap()
            trap:rearm()
        end
        message("Rearmed "..tostring(#arrowtraps).." arrow traps")
    end, 600)
end, ON.LEVEL)


