> Very basic example how to use a CustomTheme in the procedural levels. Search the examples or check [Eternal Flame of Gehenna](https://spelunky.fyi/mods/m/gehenna/) for custom levels.
```lua
-- create a new theme based on dwelling
local my_theme = CustomTheme:new()
-- get some ember effects from volcana
my_theme:override(THEME_OVERRIDE.SPAWN_EFFECTS, THEME.VOLCANA)
-- change level size
my_theme:post(THEME_OVERRIDE.INIT_LEVEL, function()
    state.width = 5
    state.height = 15
end)
-- set floor textures to eggy
my_theme.textures[DYNAMIC_TEXTURE.FLOOR] = TEXTURE.DATA_TEXTURES_FLOOR_EGGPLANT_0
set_callback(function(ctx)
    -- use the level gen from ice caves
    ctx:override_level_files({"generic.lvl", "icecavesarea.lvl"})
    -- force our theme instead of the default
    force_custom_theme(my_theme)
end, ON.PRE_LOAD_LEVEL_FILES)
```

> You can call theme functions from other themes, for example to make all growable tile codes work in your theme:
```lua
local custom = CustomTheme:new()
custom:post(THEME_OVERRIDE.SPAWN_LEVEL, function()
    state.level_gen.themes[THEME.VOLCANA]:spawn_traps()
    state.level_gen.themes[THEME.TIDE_POOL]:spawn_traps()
    state.level_gen.themes[THEME.JUNGLE]:spawn_traps()
end)
```
