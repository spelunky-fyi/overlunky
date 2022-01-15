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

    -- this mainly sets the music I guess
    state.theme = pick(themes)

    -- size does matter
    state.width = prng:random(2, 8)
    state.height = prng:random(2, 8)

    -- if we called force_custom_theme here, we'd get no procedural level generation to do our own thing
end, ON.PRE_LEVEL_GENERATION)

set_callback(function(ctx)
    if state.screen ~= SCREEN.LEVEL then return end

    -- set fallback theme to whatever we randomized earlier
    customtheme.base_theme = state.theme - 1

    -- force different floor texture
    customtheme.texture_floor = pick(textures_floor)

    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.ARROWTRAP_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CRUSHER_TRAP_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPARKTRAP_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPIKE_BALL_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.BIGSPEARTRAP_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.JUNGLE_SPEAR_TRAP_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.STICKYTRAP_CHANCE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SKULLDROP_CHANCE, 20)

    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SNAKE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPIDER, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.HANGSPIDER, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.GIANTSPIDER, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.BAT, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CAVEMAN, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.HORNEDLIZARD, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MOLE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MANTRAP, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.TIKIMAN, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.WITCHDOCTOR, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MOSQUITO, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MONKEY, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.ROBOT, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FIREBUG, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.IMP, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.LAVAMANDER, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.VAMPIRE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CROCMAN, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.COBRA, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SORCERESS, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CAT, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.NECROMANCER, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.JIANGSHI, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FEMALE_JIANGSHI, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.OCTOPUS, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.HERMITCRAB, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.UFO, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.YETI, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.OLMITE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FROG, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FIREFROG, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.TADPOLE, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.GIANTFLY, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.LEPRECHAUN, 20)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CRABMAN, 20)

    -- force custom theme here to get the level generation from the lvl file, but visuals from our theme
    force_custom_theme(customtheme)
end, ON.POST_ROOM_GENERATION)

set_callback(function()
    if state.screen ~= SCREEN.LEVEL then return end

    -- this is just for hud
    state.world = prng:random(1, 255)
    state.level = prng:random(1, 255)

    -- this is for the transition
    state.theme_next = pick(themes)

    -- this changes the music, even mid level
    state.theme = pick(themes)

    -- camera needs adjustment
    state.camera.bounds_left = 0.5
    state.camera.bounds_right = 10*state.width+4.5
    state.camera.bounds_top = 124.5
    state.camera.bounds_bottom = 120.5-8*state.height
end, ON.POST_LEVEL_GENERATION)
