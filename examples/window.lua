meta.name = "Window test"
meta.version = "WIP"
meta.description = ""
meta.author = "Dregu"

local inputtext = ''
local inputnumber = 0
local inputslider = 6
local inputdrag = 9
local inputcombo = 1
local comboopts = { 'one', 'two', 'three' }
local inputcheck = false

local widgetopen = false
local closebutton = false

local seedinput = ''
local widgetopen2 = false
local closebutton2 = false

-- add a simple button to options to open our cluttered test windows
register_option_button('open', 'Open test window', function()
    widgetopen = true
end)
register_option_button('open2', 'Open seed dialog', function()
    widgetopen2 = true
end)

set_callback(function()
    if closebutton then
        closebutton = false
        widgetopen = false
        message('Window was closed with a button')
    end
    if widgetopen then
        -- create a new window and test most of the widgets
        -- we'll put this one top center and movable
        widgetopen = window('Test script window widget thing', -0.2, 1, 0.4, 0.5, true, function()
            -- open another window from this window
            if win_button('Open seed dialog') then
                widgetopen2 = true
            end

            -- all the input widgets return the current value always, and expect you to keep feeding it back, or it will just revert to default when you blur the input
            local stupidexample = win_input_text('Not like this', '') -- this will just always be empty

            -- you need to circulate your value through the widget like this
            inputtext = win_input_text('Write text##texthereplease', inputtext)
            inputnumber = win_input_int('Write number', inputnumber)
            inputslider = win_slider_int('Select number##slider', inputslider, 1, 10)
            inputdrag = win_drag_float('Select another number##drag', inputdrag, 1, 10)
            inputcombo = win_combo('Combo thing', inputcombo, table.concat(comboopts, '\0')..'\0\0')
            inputcheck = win_check('Check this out', inputcheck)
            win_separator()
            win_text('Click here:')
            win_inline()
            if win_button('Submit') then
                message(inputtext..' '..tostring(inputnumber)..' '..tostring(inputslider)..' '..tostring(inputdrag)..' '..comboopts[inputcombo]..' '..tostring(inputcheck))
            end

            -- long text is wrapped
            win_text('Sed non justo eu orci lacinia mollis. Morbi finibus luctus massa eu gravida. Maecenas ornare tincidunt pellentesque. Aliquam vel nulla eu mauris pulvinar consequat sed vitae nisi. Nulla facilisi. Nulla venenatis mollis mauris, sed blandit nisi auctor facilisis. In tempor magna eleifend, ornare sem in, egestas massa. Donec pulvinar orci ac ligula rhoncus, a hendrerit elit interdum. Maecenas sed orci quis justo imperdiet finibus.')

            -- remember to use unique labels on identical inputs
            if win_button('Test##FirstTest') then
                message('First button')
            elseif win_button('Test##SecondTest') then
                message('Second button')
            end

            -- or with pushid
            a = {}
            for i=1,5 do
                win_pushid(i)
                a[i] = win_button('A')
                if i < 5 then
                    win_inline()
                end
                win_popid()
            end
            for i=1,5 do
                if a[i] then
                    message('Clicked A number '..tostring(i))
                end
            end

            closebutton = win_button('Close window')
        end)
        if not widgetopen then
            message('Window was closed from the X')
        end
    end

    -- another smaller thing opened from the larger thing
    if closebutton2 then
        closebutton2 = false
        widgetopen2 = false
    end
    if widgetopen2 then
        -- non movable prompt in the center of your screen
        widgetopen2 = window('Enter seed', 0, 0, 0, 0, false, function()
            seedinput = win_input_text('Seed', seedinput)
            local seed = tonumber(seedinput, 16)
            if win_button('Set seed') and seed then
                closebutton2 = true
                set_seed(seed)
            end
        end)
    end
end, ON.GUIFRAME)
