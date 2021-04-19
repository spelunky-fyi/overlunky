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

-- a button needs a callback function
register_option_button('g', 'Button G', function()
    message('Button was clicked at '..tostring(options.g))
end)

-- If you really need to you can pass a second description that is going to be wrapped and thus can be long as heck
tests = {'Chi-Squared', 'McNemar', 'Portmanteau'}
register_option_combo('h', 'Choice H', 'This is a really long description that is probably not necessary because I should\'ve just made the option more clear in the first place. But here we are, having to live with an option that spans probably 4or 5 lines just to explain something that most likely is too complicate for users to touch anyways.', table.concat(tests, '\0')..'\0\0')

-- just print these out real quick
set_callback(function()
    message('Options: A:'..tostring(options.a)..', B:'..tostring(options.b)..', C:'..tostring(options.c)..', D:'..tostring(options.d)..', E:'..tostring(options.e)..', F:'..colors[options.f]..', G:'..tostring(options.g)..', H:'..tostring(options.h))
end, ON.FRAME)
