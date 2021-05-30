meta.name = 'Choose Cosmic Ocean subtheme'
meta.version = '1'
meta.description = 'Change the subtheme of the next Cosmic Ocean levels'
meta.author = 'Zappatic'

previous_choice = 1
subtheme_names = {'Reset default behaviour', 'Dwelling', 'Jungle', 'Volcana', 'Tidepool', 'Temple', 'Icecaves', 'Neo Babylon', 'Sunken City'}
subtheme_themeids = {COSUBTHEME.RESET, COSUBTHEME.DWELLING, COSUBTHEME.JUNGLE, COSUBTHEME.VOLCANA, COSUBTHEME.TIDEPOOL, COSUBTHEME.TEMPLE, COSUBTHEME.ICECAVES, COSUBTHEME.NEOBABYLON, COSUBTHEME.SUNKENCITY}

register_option_combo('subtheme', 'Theme', table.concat(subtheme_names, '\0')..'\0\0')

set_callback(function()
    choice = options.subtheme
    if previous_choice ~= choice then
        force_co_subtheme(subtheme_themeids[choice])
        if choice == 1 then
            message("Change to default behaviour")
        else
            message("Change cosmic ocean subtheme to "..subtheme_names[choice])
        end
        previous_choice = choice
    end
end, ON.GUIFRAME)
