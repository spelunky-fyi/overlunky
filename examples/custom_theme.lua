meta.name = "Custom theme test"
meta.version = "WIP"
meta.author = "Dregu"
meta.description = "Testing custom themes..."

local lvls = {
    "dwellingarea.lvl",
    "junglearea.lvl",
    "volcanoarea.lvl",
    "tidepoolarea.lvl",
    "templearea.lvl",
    "icecavesarea.lvl",
    "babylonarea.lvl",
    "sunkencityarea.lvl"
}

local themes = {
    THEME.DWELLING,
    THEME.JUNGLE,
    THEME.VOLCANA,
    THEME.TIDE_POOL,
    THEME.TEMPLE,
    THEME.ICE_CAVES,
    THEME.NEO_BABYLON,
    THEME.SUNKEN_CITY,
    THEME.CITY_OF_GOLD
}

local textures_floor = {
    TEXTURE.DATA_TEXTURES_FLOOR_CAVE_0,
    TEXTURE.DATA_TEXTURES_FLOOR_JUNGLE_0,
    TEXTURE.DATA_TEXTURES_FLOOR_VOLCANO_0,
    TEXTURE.DATA_TEXTURES_FLOOR_TIDEPOOL_0,
    TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0,
    TEXTURE.DATA_TEXTURES_FLOOR_ICE_0,
    TEXTURE.DATA_TEXTURES_FLOOR_BABYLON_0,
    TEXTURE.DATA_TEXTURES_FLOOR_SUNKEN_0,
    TEXTURE.DATA_TEXTURES_FLOOR_EGGPLANT_0,
    TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0
}

local borders = {
    ENT_TYPE.FLOOR_BORDERTILE,
    ENT_TYPE.FLOOR_BORDERTILE_METAL,
    ENT_TYPE.FLOOR_BORDERTILE_OCTOPUS
}

names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

theme_names = {}
for i,v in pairs(THEME) do
  theme_names[v] = i
end

local function pick(from)
    return from[prng:random(#from)]
end

local customtheme

set_callback(function(ctx)
    if state.screen ~= SCREEN.LEVEL then return end

    -- this sets which procedural level generation to use
    ctx:override_level_files({"generic.lvl", pick(lvls)})

    -- this could use completely custom level format for kaizo, it doesn't actually need to load lvl-files
end, ON.PRE_LOAD_LEVEL_FILES)

set_callback(function()
    if state.screen ~= SCREEN.LEVEL then return end
    -- initialize a theme we can poke
    if not customtheme then
        customtheme = CustomTheme:new()
    end

    -- not sure what this affects in this point
    state.theme = pick(themes)

    -- size doesn't really matter in custom theme, when game doesn't try to force its logic
    state.width = prng:random(2, 6)
    state.height = prng:random(2, 6)

    -- set fallback theme to whatever we randomized earlier
    -- background and default procedural spawns are taken from here
    customtheme.base_theme = state.theme - 1

    -- force different floor textures
    customtheme.textures[CUSTOM_TEXTURE.FLOOR] = pick(textures_floor)
    customtheme.textures[CUSTOM_TEXTURE.COFFIN] = TEXTURE.DATA_TEXTURES_COFFINS_5
    customtheme.border_floor = pick(borders)

end, ON.PRE_LEVEL_GENERATION)

set_callback(function(ctx)
    if state.screen ~= SCREEN.LEVEL then return end

    -- force custom theme here to get the normal level generation from the lvl file, but visuals from our theme
    force_custom_theme(customtheme)

end, ON.POST_ROOM_GENERATION)

set_callback(function()
    if state.screen ~= SCREEN.LEVEL then return end

    -- this is just for hud
    state.world = prng:random(1, 255)
    state.level = prng:random(1, 255)

    -- this is for the transition floor
    state.theme_next = pick(themes)

    -- this changes the music, even mid level
    state.theme = pick(themes)
end, ON.POST_LEVEL_GENERATION)

set_callback(function()
    -- TODO: something is forcing the ice caves pit camera, annoying in closed levels
    state.camera.bounds_bottom = 120.5-8*state.height
end, ON.LEVEL)
