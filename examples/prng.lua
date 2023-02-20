meta.name = "PRNG viewer"
meta.version = "WIP"
meta.description = ""
meta.author = "Dregu"

local t = {}
for i=0,10 do
    t[i] = { a="", b="" }
end

set_callback(function(ctx)
    ctx:window('PRNG viewer', 0, 0, 0, 0, true, function(ctx, pos, size)
        do
            local a, b = get_adventure_seed()
            local name = "Adventure Seed"
            t[10].a = ctx:win_input_text(F"{name}##SA", string.format("%016X", a))
            t[10].b = ctx:win_input_text(F"##SB", string.format("%016X", b))
            local na = tonumber(t[10].a, 16)
            local nb = tonumber(t[10].b, 16)
            if na ~= a or nb ~= b then
                set_adventure_seed(na, nb)
            end
        end
        for i=0,9 do
            ctx:win_separator()
            local a, b = prng:get_pair(i+1)
            local name = table.concat(enum_get_names(PRNG_CLASS, i), ", ") or "???"
            t[i].a = ctx:win_input_text(F"{i}: {name}##A{i}", string.format("%016X", a))
            t[i].b = ctx:win_input_text(F"##B{i}", string.format("%016X", b))
            local na = tonumber(t[i].a, 16)
            local nb = tonumber(t[i].b, 16)
            if na ~= a or nb ~= b then
                prng:set_pair(i+1, na, nb)
            end
        end
    end)
end, ON.GUIFRAME)
