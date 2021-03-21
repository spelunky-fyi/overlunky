meta.name = 'Options test'
meta.version = 'WIP'
meta.description = 'Test different options.'
meta.author = 'Dregu'

-- options are displayed in alphabetical order by name in the ui
-- keep the descriptions unique or imgui breaks!

register_option_int('a', 'Number A', 100, 69, 420)
register_option_float('b', 'Float B', 0.5, 0, 1.5)
register_option_bool('c', 'Checkbox C', false)
register_option_string('d', 'String D', '')

-- use a null character \0 separated list, with a double \0\0 at the end
register_option_combo('e', 'Choice E', 'Red\0Green\0Blue\0\0')

-- you can use a table to initialize this too, that way you can easily reference the string later with the index
colors = {'Bulbasaur', 'Charmander', 'Squirtle'}
register_option_combo('f', 'Choice F', table.concat(colors, '\0')..'\0\0')

-- just print these out real quick
set_callback(function()
    message('Options: A:'..tostring(options.a)..', B:'..tostring(options.b)..', C:'..tostring(options.c)..', D:'..tostring(options.d)..', E:'..tostring(options.e)..', F:'..colors[options.f])
end, ON.FRAME)
