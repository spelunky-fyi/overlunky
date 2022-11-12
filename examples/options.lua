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
register_option_combo('f', 'Choice F', table.concat(colors, '\0')..'\0\0', 3)

-- a button needs a callback function
register_option_button('g', 'Button G', function()
    message('Button was clicked at '..tostring(options.g))
end)

-- If you really need to you can pass a second description that is going to be wrapped and thus can be long as heck
tests = {'Chi-Squared', 'McNemar', 'Portmanteau'}
register_option_combo('h', 'Choice H', 'This is a really long description that is probably not necessary because I should\'ve just made the option more clear in the first place. But here we are, having to live with an option that spans probably 4or 5 lines just to explain something that most likely is too complicate for users to touch anyways.', table.concat(tests, '\0')..'\0\0')

-- single custom option with the help of the window api, return value saved to options.x
register_option_callback('x', function(draw_ctx)
    draw_ctx:win_separator()
    draw_ctx:win_text('Custom options here:')
    return draw_ctx:win_input_text('Custom text X', options.x or '')
end)

-- multiple custom options in one callback, saved to the options table or wherever you want
-- also editing conventional options with different kind of interface
customoption = false
additionaloption = false
register_option_callback('y', function(draw_ctx)
    options.z = draw_ctx:win_slider_int('Custom int Z', options.z or 5, 0, 10)
    customoption = draw_ctx:win_check('Custom checkbox', customoption)

    draw_ctx:window('Additional options', 0, 0, 0, 0, true, function()
        draw_ctx:win_text('This window will also be shown whenever the options are shown.')
        additionaloption = draw_ctx:win_check('Additional checkbox', additionaloption)
    end)

    draw_ctx:win_section('Fancy stuff behind this', function()
        draw_ctx:win_indent(20)
        options.a = draw_ctx:win_slider_int('A, but slider', options.a or 100, 100, 200)
        if draw_ctx:win_button('Set B to 1.23') then
            options.b = 1.23
        end
        if draw_ctx:win_button('Try to set A to banana') then
            options.a = 'banana'
        end
        draw_ctx:win_indent(-20)
    end)
    -- not returning anything, options.y will be nil but we have set a bunch of others in here
end)

-- just print these out real quick
set_global_interval(function()
    message('Options: A:'..tostring(options.a)..', B:'..tostring(options.b)..', C:'..tostring(options.c)..', D:'..tostring(options.d)..', E:'..tostring(options.e)..', F:'..colors[options.f]..', G:'..tostring(options.g)..', H:'..tostring(options.h)..', X: '..tostring(options.x)..', Y: '..tostring(options.y)..', Z: '..tostring(options.z)..' '..tostring(customoption))
end, 15)
