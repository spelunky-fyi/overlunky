meta.name = 'Options test 2'
meta.version = 'WIP'
meta.description = 'Setting all the options in a single callback for total control, with save and load'
meta.author = 'Dregu'

-- init defaults
options = {
    name = 'Ana Spelunky',
    shoe_size = 9.5,
    checkboxes = {
        a = true,
        b = false,
        c = false,
        d = false,
        e = true
    },
    age = 10,
    debug = true,
}

-- save options to disk
set_callback(function(ctx)
    ctx:save(json.encode(options))
end, ON.SAVE)

-- load options from disk
set_callback(function(ctx)
    local options_json = ctx:load()
    if options_json ~= '' then
        options = json.decode(options_json)
    end
end, ON.LOAD)

-- do everything in one callback
register_option_callback('', options, function(ctx)
    -- now we're in complete control of the order
    options.name = ctx:win_input_text('Name', options.name)
    options.shoe_size = ctx:win_slider_float('Shoe size (US)', options.shoe_size, 1, 20)
    ctx:win_section('Random checkboxes', function()
        options.checkboxes.a = ctx:win_check('A', options.checkboxes.a)
        options.checkboxes.b = ctx:win_check('B', options.checkboxes.b)
        options.checkboxes.c = ctx:win_check('C', options.checkboxes.c)
        options.checkboxes.d = ctx:win_check('D', options.checkboxes.d)
        options.checkboxes.e = ctx:win_check('E', options.checkboxes.e)
        ctx:win_separator()
    end)
    options.age = ctx:win_input_int('Age', options.age)
    options.debug = ctx:win_check('Debug window', options.debug)
    if ctx:win_button('Save options') then
        save_script()
    end

    if options.debug then
        ctx:window('Your options are', 0, 0, 0, 0, true, function()
            ctx:win_text(inspect(options))
        end)
    end

    return options -- actually pointless, we already edited the only copy
end)
