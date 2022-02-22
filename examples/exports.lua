meta.name = "Exports"
meta.author = "Dregu"
meta.version = "1.0"
meta.descrtiption = "Exports functions for imports.lua and other scripts to use"

local function some_local(str)
    print(str)
end

exports = {
    foo = function(test)
        some_local(test)
    end,
    bar = function()
        return F"{state.world}-{state.level}"
    end
}
