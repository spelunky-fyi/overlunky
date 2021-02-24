-- This script tests the metadata parser.
-- When a script is first loaded in disabled state, only the lines containing metadata should be executed.
-- It is not a good example what to do!

-- Seriously, don't do this
meta = {
                            name = "Metadata test",
    version = "1.0" }

                                                     meta.author=            'Dregu'

-- If you came here looking for examples, this is what I think you should actually do, because it's concise and easy to read for people:
--meta.name = "Metadata test"
--meta.version = "0.2"
--meta.author = "Dregu"
--meta.description = "This script tests the metadata parser."

message("Don't print this before I hit Enable")

meta.description = "You can put these after your real code too, but don't be an idiot."
