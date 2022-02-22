meta.name = "Imports"
meta.author = "Dregu"
meta.version = "1.0"
meta.description = "Imports the stuff from exports.lua"

local stuff = import("dregu/exports", "1.0") -- version optional
set_callback(function()
  stuff.foo("hello")
  print(stuff.bar())
end, ON.LEVEL)
