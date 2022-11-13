meta.name = 'Options test'
meta.version = 'WIP'
meta.description = 'Test different options.'
meta.author = 'Dregu'

-- initialize some options, some that are not in fact registered options, but this should still work.
-- also, you can write any defaults here before registering the option and this value will be used instead of the registered default.
-- that ought to be useful when saving and loading options (see "Options test 2")
options = { x='world', d='asd', z=5 }

-- options are displayed in alphabetical order by name in the ui
-- use the callback type to organize your options more freely
-- keep the descriptions unique or imgui breaks!

register_option_int('a', 'Number A', 100, 69, 420)
register_option_float('b', 'Float B', 0.5, 0, 1.5)
register_option_bool('c', 'Checkbox C', false)
register_option_string('d', 'String D', '')

-- use a null character \0 separated list, with a double \0\0 at the end
register_option_combo('e', 'Choice E', 'Red\0Green\0Blue\0\0')

-- you can use a table to initialize this too, that way you can easily reference the string later with the index
colors = {'Bulbasaur', 'Charmander', 'Squirtle'}
register_option_combo('f', 'Choice F', table.concat(colors, '\0')..'\0\0', 3)

-- a button needs a callback function
register_option_button('g', 'Button G', function()
    message('Button was clicked at '..tostring(options.g))
end)

-- If you really need to you can pass a second description that is going to be wrapped and thus can be long as heck
tests = {'Chi-Squared', 'McNemar', 'Portmanteau'}
register_option_combo('h', 'Choice H', 'This is a really long description that is probably not necessary because I should\'ve just made the option more clear in the first place. But here we are, having to live with an option that spans probably 4or 5 lines just to explain something that most likely is too complicate for users to touch anyways.', table.concat(tests, '\0')..'\0\0')

-- single custom option with the help of the window api, defaulting to hello, return value saved to options.x
register_option_callback('x', 'hello', function(draw_ctx)
    draw_ctx:win_separator()
    draw_ctx:win_text('Custom options here:')
    return draw_ctx:win_input_text('Custom text X', options.x)
end)

-- neat way to create sections in the options, equally neatly organized in the table
register_option_callback('sect', { slider=150, text='' }, function(draw_ctx)
    local sect = options.sect or {}
    draw_ctx:win_section('Organized stuff here', function()
        draw_ctx:win_indent(20)
        sect.slider = draw_ctx:win_slider_int('Another damn slider', sect.slider or 150, 100, 200)
        sect.text = draw_ctx:win_input_text('More text', sect.text or '')
        draw_ctx:win_indent(-20)
    end)
    return sect -- returning the whole table, nested in options.sect
end)

-- multiple custom options in one callback, saved to the options table or wherever you want
-- also editing conventional options with different kind of interface
customoption = false
register_option_callback('y', false, function(draw_ctx)
    -- not returning anything from this function, but we're setting options.y manually anyway
    options.y = draw_ctx:win_check('Custom check Y', options.y)

    -- this isn't even an option, what is it doing here lol
    customoption = draw_ctx:win_check('Custom checkbox, not even a real option', customoption)

    draw_ctx:window('Your options are', 0, 0, 0, 0, true, function()
        draw_ctx:win_text('This window will also be shown whenever the options are shown.')
        draw_ctx:win_text(inspect(options))
    end)

    draw_ctx:win_section('Hacky stuff behind this', function()
        draw_ctx:win_indent(20)

        -- ok now we're in dangerous territory, because we're using an unknown option 'z'.
        -- this is fine, but make sure you put it in the table yourself (see beginning)
        -- or it might never exist, because it's defined in a section that was never opened
        options.z = draw_ctx:win_slider_int('Custom int Z', options.z or 5, 0, 10)

        options.a = draw_ctx:win_slider_int('A, but slider', options.a or 100, 100, 200)
        if draw_ctx:win_button('Set B to 1.23') then
            options.b = 1.23
        end
        if draw_ctx:win_button('Try to set A to banana') then
            options.a = 'banana'
        end
        draw_ctx:win_indent(-20)
    end)
end)
