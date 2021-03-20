meta.name = "Require test"
meta.version = "WIP"
meta.description = "Test requiring modules."
meta.author = "Dregu"

foo = require "testlib.mod"
bar = require "anothermod"
foo.hello()

set_callback(function()
    bar.hello()
end, ON.LEVEL)
