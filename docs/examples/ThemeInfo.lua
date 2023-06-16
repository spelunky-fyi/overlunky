-- When hell freezes over: Examples for hooking ThemeInfo virtuals

state.level_gen.themes[THEME.VOLCANA]:set_pre_texture_dynamic(function(theme, id)
    -- change volcana floor to ice floor
    if id == DYNAMIC_TEXTURE.FLOOR then
        return TEXTURE.DATA_TEXTURES_FLOOR_ICE_0
    end
end)

state.level_gen.themes[THEME.VOLCANA]:set_pre_spawn_effects(function(theme)
    -- run the effects function from another theme to get cool ice particle effects
    for i=1,50 do
        state.level_gen.themes[THEME.ICE_CAVES]:spawn_effects()
    end
    -- then we run this to fix the weird camera bounds set by ice caves
    state.level_gen.themes[THEME.DWELLING]:spawn_effects()
    -- don't spawn volcanic effects
    return true
end)

-- make players cold
state.level_gen.themes[THEME.VOLCANA]:set_post_spawn_players(function(theme)
    for i,p in pairs(get_local_players()) do
        spawn_over(ENT_TYPE.LOGICAL_FROST_BREATH, p.uid, 0, 0)
    end
end)

-- make vlads bluish
state.level_gen.themes[THEME.VOLCANA]:set_pre_texture_backlayer_lut(function(theme)
    return TEXTURE.DATA_TEXTURES_LUT_ICECAVES_0
end)
