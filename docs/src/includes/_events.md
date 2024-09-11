
# Events


> Search script examples for [set_callback](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=set_callback)

These events are used in [set_callback](#set_callback) to call your callback function before or after something happens in the game. The named events related to game screens generally run on the first engine update when changing to said screen. Coming back from the options screen does not fire the parent screen event though (OPTIONS to LEVEL for example). If you need more fine-grained control over the screen events, use `SCREEN`, `PRE_LOAD_SCREEN` and `POST_LOAD_SCREEN` with the help of the `state.screen` variables.

## ON.LOGO


> Search script examples for [ON.LOGO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOGO)

Runs when entering the the mossmouth logo screen.<br/>

## ON.INTRO


> Search script examples for [ON.INTRO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.INTRO)

Runs when entering the intro cutscene.<br/>

## ON.PROLOGUE


> Search script examples for [ON.PROLOGUE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PROLOGUE)

Runs when entering the prologue / poem.<br/>

## ON.TITLE


> Search script examples for [ON.TITLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TITLE)

Runs when entering the title screen.<br/>

## ON.MENU


> Search script examples for [ON.MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.MENU)

Runs when entering the main menu.<br/>

## ON.OPTIONS


> Search script examples for [ON.OPTIONS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.OPTIONS)

Runs when entering the options menu.<br/>

## ON.PLAYER_PROFILE


> Search script examples for [ON.PLAYER_PROFILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PLAYER_PROFILE)

Runs when entering the player profile screen.<br/>

## ON.LEADERBOARD


> Search script examples for [ON.LEADERBOARD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEADERBOARD)

Runs when entering the leaderboard screen.<br/>

## ON.SEED_INPUT


> Search script examples for [ON.SEED_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SEED_INPUT)

Runs when entering the seed input screen of a seeded run.<br/>

## ON.CHARACTER_SELECT


> Search script examples for [ON.CHARACTER_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CHARACTER_SELECT)

Runs when entering the character select screen.<br/>

## ON.TEAM_SELECT


> Search script examples for [ON.TEAM_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TEAM_SELECT)

Runs when entering the team select screen of arena mode.<br/>

## ON.CAMP


> Search script examples for [ON.CAMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CAMP)

Runs when entering the camp, after all entities have spawned, on the first level frame.<br/>

## ON.LEVEL


> Search script examples for [ON.LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEVEL)

Runs when entering any level, after all entities have spawned, on the first level frame.<br/>

## ON.TRANSITION


> Search script examples for [ON.TRANSITION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TRANSITION)

Runs when entering the transition screen, after all entities have spawned.<br/>

## ON.DEATH


> Search script examples for [ON.DEATH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH)

Runs when entering the death screen.<br/>

## ON.SPACESHIP


> Search script examples for [ON.SPACESHIP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPACESHIP)

Runs when entering the olmecship cutscene after [Tiamat](#Tiamat).<br/>

## ON.WIN


> Search script examples for [ON.WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.WIN)

Runs when entering any winning cutscene, including the constellation.<br/>

## ON.CREDITS


> Search script examples for [ON.CREDITS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CREDITS)

Runs when entering the credits screen.<br/>

## ON.SCORES


> Search script examples for [ON.SCORES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCORES)

Runs when entering the final score celebration screen of a normal or hard ending.<br/>

## ON.CONSTELLATION


> Search script examples for [ON.CONSTELLATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CONSTELLATION)

Runs when entering the turning into constellation cutscene after cosmic ocean.<br/>

## ON.RECAP


> Search script examples for [ON.RECAP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RECAP)

Runs when entering the Dear Journal screen after final scores.<br/>

## ON.ARENA_MENU


> Search script examples for [ON.ARENA_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MENU)

Runs when entering the main arena rules menu screen.<br/>

## ON.ARENA_STAGES


> Search script examples for [ON.ARENA_STAGES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_STAGES)

Runs when entering the arena stage selection screen.<br/>

## ON.ARENA_ITEMS


> Search script examples for [ON.ARENA_ITEMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_ITEMS)

Runs when entering the arena item config screen.<br/>

## ON.ARENA_SELECT


> Search script examples for [ON.ARENA_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_SELECT)


## ON.ARENA_INTRO


> Search script examples for [ON.ARENA_INTRO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_INTRO)

Runs when entering the arena VS intro screen.<br/>

## ON.ARENA_MATCH


> Search script examples for [ON.ARENA_MATCH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MATCH)

Runs when entering the arena level screen, after all entities have spawned, on the first level frame, before the get ready go scene.<br/>

## ON.ARENA_SCORE


> Search script examples for [ON.ARENA_SCORE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_SCORE)

Runs when entering the arena scores screen.<br/>

## ON.ONLINE_LOADING


> Search script examples for [ON.ONLINE_LOADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOADING)

Runs when entering the online loading screen.<br/>

## ON.ONLINE_LOBBY


> Search script examples for [ON.ONLINE_LOBBY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOBBY)

Runs when entering the online lobby screen.<br/>

## ON.GUIFRAME


> Search script examples for [ON.GUIFRAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GUIFRAME)

Params: [GuiDrawContext](#GuiDrawContext) draw_ctx<br/>Runs every frame the game is rendered, thus runs at selected framerate. Drawing functions are only available during this callback through a [GuiDrawContext](#GuiDrawContext)<br/>

## ON.FRAME


> Search script examples for [ON.FRAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.FRAME)

Runs while playing the game while the player is controllable, not in the base camp or the arena mode<br/>

## ON.GAMEFRAME


> Search script examples for [ON.GAMEFRAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GAMEFRAME)

Runs whenever the game engine is actively running. This includes base camp, arena, level transition and death screen<br/>

## ON.SCREEN


> Search script examples for [ON.SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCREEN)

Runs whenever state.screen changes<br/>

## ON.START


> Search script examples for [ON.START](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.START)

Runs on the first [ON](#ON).[SCREEN](#SCREEN) of a run<br/>

## ON.LOADING


> Search script examples for [ON.LOADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOADING)

Runs whenever state.loading changes and is > 0. Prefer PRE/POST_LOAD_SCREEN instead though.<br/>

## ON.RESET


> Search script examples for [ON.RESET](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RESET)

Runs when resetting a run<br/>

## ON.SAVE


> Search script examples for [ON.SAVE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SAVE)

Params: [SaveContext](#SaveContext) save_ctx<br/>Runs at the same times as [ON](#ON).[SCREEN](#SCREEN), but receives the save_ctx<br/>

## ON.LOAD


> Search script examples for [ON.LOAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOAD)

Params: [LoadContext](#LoadContext) load_ctx<br/>Runs as soon as your script is loaded, including reloads, then never again<br/>

## ON.PRE_LOAD_LEVEL_FILES


> Search script examples for [ON.PRE_LOAD_LEVEL_FILES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_LEVEL_FILES)

Params: [PreLoadLevelFilesContext](#PreLoadLevelFilesContext) load_level_ctx<br/>Runs right before level files would be loaded<br/>

## ON.PRE_LEVEL_GENERATION


> Search script examples for [ON.PRE_LEVEL_GENERATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LEVEL_GENERATION)

Runs before any level generation, no entities exist at this point. Runs in most screens that have entities. Return true to block normal level generation, i.e. stop any entities from being spawned by [ThemeInfo](#ThemeInfo) functions. Does not block other [ThemeInfo](#ThemeInfo) functions, like spawn_effects though. POST_LEVEL_GENERATION will still run if this callback is blocked.<br/>

## ON.PRE_LOAD_SCREEN


> Search script examples for [ON.PRE_LOAD_SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_SCREEN)

Runs right before loading a new screen based on screen_next. Return true from callback to block the screen from loading.<br/>

## ON.POST_ROOM_GENERATION


> Search script examples for [ON.POST_ROOM_GENERATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_ROOM_GENERATION)

Params: [PostRoomGenerationContext](#PostRoomGenerationContext) room_gen_ctx<br/>Runs right after all rooms are generated before entities are spawned<br/>

## ON.POST_LEVEL_GENERATION


> Search script examples for [ON.POST_LEVEL_GENERATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LEVEL_GENERATION)

Runs right after level generation is done, i.e. after all level gen entities are spawned, before any entities are updated. You can spawn your own entities here, like extra enemies, give items to players etc.<br/>

## ON.POST_LOAD_SCREEN


> Search script examples for [ON.POST_LOAD_SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LOAD_SCREEN)

Runs right after a screen is loaded, before rendering anything<br/>

## ON.PRE_GET_RANDOM_ROOM


> Search script examples for [ON.PRE_GET_RANDOM_ROOM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_GET_RANDOM_ROOM)

Params: int x, int y, [LAYER](#LAYER) layer, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template<br/>Return: `string room_data`<br/>Called when the game wants to get a random room for a given template. Return a string that represents a room template to make the game use that.<br/>If the size of the string returned does not match with the room templates expected size the room is discarded.<br/>White spaces at the beginning and end of the string are stripped, not at the beginning and end of each line.<br/>

## ON.PRE_HANDLE_ROOM_TILES


> Search script examples for [ON.PRE_HANDLE_ROOM_TILES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_HANDLE_ROOM_TILES)

Params: int x, int y, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template, [PreHandleRoomTilesContext](#PreHandleRoomTilesContext) room_ctx<br/>Return: `bool last_callback` to determine whether callbacks of the same type should be executed after this<br/>Runs after a random room was selected and right before it would spawn entities for each tile code<br/>Allows you to modify the rooms content in the front and back layer as well as add a backlayer if not yet existant<br/>

## ON.SCRIPT_ENABLE


> Search script examples for [ON.SCRIPT_ENABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_ENABLE)

Runs when the script is enabled from the UI or when imported by another script while disabled, but not on load.<br/>

## ON.SCRIPT_DISABLE


> Search script examples for [ON.SCRIPT_DISABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_DISABLE)

Runs when the script is disabled from the UI and also right before unloading/reloading.<br/>

## ON.RENDER_PRE_HUD


> Search script examples for [ON.RENDER_PRE_HUD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_HUD)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, [Hud](#Hud) hud<br/>Runs before the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx or edit the Hud values. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_HUD


> Search script examples for [ON.RENDER_POST_HUD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_HUD)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, [Hud](#Hud) hud<br/>Runs after the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx<br/>

## ON.RENDER_PRE_PAUSE_MENU


> Search script examples for [ON.RENDER_PRE_PAUSE_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_PAUSE_MENU)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs before the pause menu is drawn on screen. In this event, you can't really draw textures, because the blurred background is drawn on top of them. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_PAUSE_MENU


> Search script examples for [ON.RENDER_POST_PAUSE_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_PAUSE_MENU)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs after the pause menu is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx<br/>

## ON.RENDER_PRE_BLURRED_BACKGROUND


> Search script examples for [ON.RENDER_PRE_BLURRED_BACKGROUND](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_BLURRED_BACKGROUND)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, float blur<br/>Runs before the blurred background is drawn on screen, behind pause menu or journal book. In this event, you can't really draw textures, because the blurred background is drawn on top of them. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_BLURRED_BACKGROUND


```lua
-- replace journal book background with a piece of paper, drawn on top of the blurred bg
set_callback(function(ctx, blur)
    local src = Quad:new(AABB:new(0.535, 0.21, 0.85, 0.93))
    local dest = Quad:new(AABB:new(-1, 1, 1, -1))
    local col = Color:white()
    col.a = game_manager.journal_ui.opacity --or simply 'blur' to draw behind pause menu too
    ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_TOP_MAIN_0, src, dest, col)
    -- hide real book offscreen, only drawing pages
    game_manager.journal_ui.book_background.y = 2
end, ON.RENDER_POST_BLURRED_BACKGROUND)

-- the previous cb is not called for death screen, default to normal book bg there
set_pre_render_screen(SCREEN.DEATH, function(ctx)
    game_manager.journal_ui.book_background.y = 0
end)

```


> Search script examples for [ON.RENDER_POST_BLURRED_BACKGROUND](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_BLURRED_BACKGROUND)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, float blur<br/>Runs after the blurred background is drawn on screen, behind pause menu or journal book. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx. (blur amount is probably the same as journal opacity)<br/>

## ON.RENDER_PRE_DRAW_DEPTH


> Search script examples for [ON.RENDER_PRE_DRAW_DEPTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_DRAW_DEPTH)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int draw_depth<br/>Runs before the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_DRAW_DEPTH


> Search script examples for [ON.RENDER_POST_DRAW_DEPTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_DRAW_DEPTH)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int draw_depth<br/>Runs right after the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx<br/>

## ON.RENDER_PRE_JOURNAL_PAGE


> Search script examples for [ON.RENDER_PRE_JOURNAL_PAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_JOURNAL_PAGE)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, [JOURNAL_PAGE_TYPE](#JOURNAL_PAGE_TYPE) page_type, [JournalPage](#JournalPage) page<br/>Runs before the journal page is drawn on screen. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_JOURNAL_PAGE


> Search script examples for [ON.RENDER_POST_JOURNAL_PAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_JOURNAL_PAGE)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, [JOURNAL_PAGE_TYPE](#JOURNAL_PAGE_TYPE) page_type, [JournalPage](#JournalPage) page<br/>Runs after the journal page is drawn on screen. In this event, you can draw textures with the draw_screen_texture function of the [VanillaRenderContext](#VanillaRenderContext)<br/>The [JournalPage](#JournalPage) parameter gives you access to the specific fields of the page. Be sure to cast it to the correct type, the following functions are available to do that:<br/>`page:as_journal_page_progress()`<br/>`page:as_journal_page_journalmenu()`<br/>`page:as_journal_page_places()`<br/>`page:as_journal_page_people()`<br/>`page:as_journal_page_bestiary()`<br/>`page:as_journal_page_items()`<br/>`page:as_journal_page_traps()`<br/>`page:as_journal_page_feats()`<br/>`page:as_journal_page_deathcause()`<br/>`page:as_journal_page_deathmenu()`<br/>`page:as_journal_page_playerprofile()`<br/>`page:as_journal_page_lastgameplayed()`<br/>

## ON.RENDER_PRE_LAYER


> Search script examples for [ON.RENDER_PRE_LAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_LAYER)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int rendered_layer<br/>Runs before a layer is rendered, runs for both layers during layer door transitions. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_LAYER


> Search script examples for [ON.RENDER_POST_LAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_LAYER)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int rendered_layer<br/>Runs after a layer is rendered, runs for both layers during layer door transitions. Things drawn here will be part of the layer transition animation<br/>

## ON.RENDER_PRE_LEVEL


> Search script examples for [ON.RENDER_PRE_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_LEVEL)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int camera_layer<br/>Runs before the level is rendered. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_LEVEL


> Search script examples for [ON.RENDER_POST_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_LEVEL)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int camera_layer<br/>Runs after the level is rendered, before hud<br/>

## ON.RENDER_PRE_GAME


> Search script examples for [ON.RENDER_PRE_GAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_GAME)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs before the ingame part of the game is rendered. Return `true` to skip rendering.<br/>

## ON.RENDER_POST_GAME


> Search script examples for [ON.RENDER_POST_GAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_GAME)

Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs after the level and HUD are rendered, before pause menus and blur effects<br/>

## ON.SPEECH_BUBBLE


> Search script examples for [ON.SPEECH_BUBBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPEECH_BUBBLE)

Params: [Entity](#Entity) speaking_entity, string text<br/>Runs before any speech bubble is created, even the one using [say](#say) function<br/>Return: if you don't return anything it will execute the speech bubble function normally with default message<br/>if you return empty string, it will not create the speech bubble at all, if you return string, it will use that instead of the original<br/>The first script to return string (empty or not) will take priority, the rest will receive callback call but the return behavior won't matter<br/>

## ON.TOAST


> Search script examples for [ON.TOAST](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TOAST)

Params: string text<br/>Runs before any toast is created, even the one using [toast](#toast) function<br/>Return: if you don't return anything it will execute the toast function normally with default message<br/>if you return empty string, it will not create the toast at all, if you return string, it will use that instead of the original message<br/>The first script to return string (empty or not) will take priority, the rest will receive callback call but the return behavior won't matter<br/>

## ON.DEATH_MESSAGE


> Search script examples for [ON.DEATH_MESSAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH_MESSAGE)

Params: [STRINGID](#Aliases) id<br/>Runs once after death when the death message journal page is shown. The parameter is the [STRINGID](#Aliases) of the title, like 1221 for BLOWN UP.<br/>

## ON.PRE_LOAD_JOURNAL_CHAPTER


> Search script examples for [ON.PRE_LOAD_JOURNAL_CHAPTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_JOURNAL_CHAPTER)

Params: [JOURNALUI_PAGE_SHOWN](#JOURNALUI_PAGE_SHOWN) chapter<br/>Runs before the journal or any of it's chapter is opened<br/>Return: return true to not load the chapter (or journal as a whole)<br/>

## ON.POST_LOAD_JOURNAL_CHAPTER


> Search script examples for [ON.POST_LOAD_JOURNAL_CHAPTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LOAD_JOURNAL_CHAPTER)

Params: [JOURNALUI_PAGE_SHOWN](#JOURNALUI_PAGE_SHOWN) chapter, array:int pages<br/>Runs after the pages for the journal are prepared, but not yet displayed, `pages` is a list of page numbers that the game loaded, if you want to change it, do the changes (remove pages, add new ones, change order) and return it<br/>All new pages will be created as [JournalPageStory](#JournalPageStory), any custom with page number above 9 will be empty, I recommend using above 99 to be sure not to get the game page, you can later use this to recognise and render your own stuff on that page in the RENDER_POST_JOURNAL_PAGE<br/>Return: return new page array to modify the journal, returning empty array or not returning anything will load the journal normally, any page number that was already loaded will result in the standard game page<br/>When changing the order of game pages make sure that the page that normally is rendered on the left side is on the left in the new order, otherwise you get some messed up result, custom pages don't have this problem. The order is: left, right, left, right ...<br/>

## ON.PRE_GET_FEAT


> Search script examples for [ON.PRE_GET_FEAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_GET_FEAT)

Runs before getting performed status for a [FEAT](#Aliases) when rendering the Feats page in journal.<br/>Return: true to override the vanilla feat with your own. Defaults to Steam GetAchievement.<br/>

## ON.PRE_SET_FEAT


> Search script examples for [ON.PRE_SET_FEAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_SET_FEAT)

Runs before the game sets a vanilla feat performed.<br/>Return: true to block the default behaviour of calling Steam SetAchievement.<br/>

## ON.PRE_UPDATE


> Search script examples for [ON.PRE_UPDATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_UPDATE)

Runs before the State is updated, runs always (menu, settings, camp, game, arena, online etc.) with the game engine, typically 60FPS<br/>Return behavior: return true to stop further PRE_UPDATE callbacks from executing and don't update the state (this will essentially freeze the game engine)<br/>

## ON.POST_UPDATE


> Search script examples for [ON.POST_UPDATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_UPDATE)

Runs right after the State is updated, runs always (menu, settings, camp, game, arena, online etc.) with the game engine, typically 60FPS<br/>

## ON.USER_DATA


> Search script examples for [ON.USER_DATA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.USER_DATA)

Params: [Entity](#Entity) ent<br/>Runs on all changes to [Entity](#Entity).user_data, including after loading saved user_data in the next level and transition. Also runs the first time user_data is set back to nil, but nil won't be saved to bother you on future levels.<br/>

## ON.PRE_LEVEL_CREATION


> Search script examples for [ON.PRE_LEVEL_CREATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LEVEL_CREATION)

Runs right before the front layer is created. Runs in all screens that usually have entities, or when creating a layer manually.<br/>

## ON.POST_LEVEL_CREATION


> Search script examples for [ON.POST_LEVEL_CREATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LEVEL_CREATION)

Runs right after the back layer has been created and you can start spawning entities in it. Runs in all screens that usually have entities, or when creating a layer manually.<br/>

## ON.PRE_LAYER_CREATION


> Search script examples for [ON.PRE_LAYER_CREATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LAYER_CREATION)

Params: [LAYER](#LAYER) layer<br/>Runs right before a layer is created. Runs in all screens that usually have entities, or when creating a layer manually.<br/>

## ON.POST_LAYER_CREATION


> Search script examples for [ON.POST_LAYER_CREATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LAYER_CREATION)

Params: [LAYER](#LAYER) layer<br/>Runs right after a layer has been created and you can start spawning entities in it. Runs in all screens that usually have entities, or when creating a layer manually.<br/>

## ON.PRE_LEVEL_DESTRUCTION


> Search script examples for [ON.PRE_LEVEL_DESTRUCTION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LEVEL_DESTRUCTION)

Runs right before the current level is unloaded and any entities destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.<br/>

## ON.POST_LEVEL_DESTRUCTION


> Search script examples for [ON.POST_LEVEL_DESTRUCTION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LEVEL_DESTRUCTION)

Runs right after the current level has been unloaded and all entities destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.<br/>

## ON.PRE_LAYER_DESTRUCTION


> Search script examples for [ON.PRE_LAYER_DESTRUCTION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LAYER_DESTRUCTION)

Params: [LAYER](#LAYER) layer<br/>Runs right before a layer is unloaded and any entities there destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.<br/>

## ON.POST_LAYER_DESTRUCTION


> Search script examples for [ON.POST_LAYER_DESTRUCTION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LAYER_DESTRUCTION)

Params: [LAYER](#LAYER) layer<br/>Runs right after a layer has been unloaded and any entities there destroyed. Runs in pretty much all screens, even ones without entities. The screen has already changed at this point, meaning the screen being destroyed is in state.screen_last.<br/>

## ON.PRE_PROCESS_INPUT


> Search script examples for [ON.PRE_PROCESS_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_PROCESS_INPUT)

Runs right before the game gets input from various devices and writes to a bunch of buttons-variables. Return true to disable all game input completely.<br/>

## ON.POST_PROCESS_INPUT


> Search script examples for [ON.POST_PROCESS_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_PROCESS_INPUT)

Runs right after the game gets input from various devices and writes to a bunch of buttons-variables. Probably the first chance you have to capture or edit buttons_gameplay or buttons_menu sort of things.<br/>

## ON.PRE_GAME_LOOP


> Search script examples for [ON.PRE_GAME_LOOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_GAME_LOOP)

Runs right before the main engine loop. Return true to block state updates and menu updates, i.e. to pause inside menus.<br/>

## ON.POST_GAME_LOOP


> Search script examples for [ON.POST_GAME_LOOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_GAME_LOOP)

Runs right after the main engine loop.<br/>

## ON.PRE_SAVE_STATE


> Search script examples for [ON.PRE_SAVE_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_SAVE_STATE)

Runs right before the main [StateMemory](#StateMemory) is manually saved to a slot or a custom [SaveState](#SaveState). Slot is 1..4 or -1 on custom [SaveState](#SaveState). Return true to block save.<br/>Params: int slot, [StateMemory](#StateMemory) saved<br/>

## ON.POST_SAVE_STATE


> Search script examples for [ON.POST_SAVE_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_SAVE_STATE)

Runs right after the main [StateMemory](#StateMemory) is manually saved to a slot or a custom [SaveState](#SaveState). Slot is 1..4 or -1 on custom [SaveState](#SaveState).<br/>Params: int slot, [StateMemory](#StateMemory) saved<br/>

## ON.PRE_LOAD_STATE


> Search script examples for [ON.PRE_LOAD_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_STATE)

Runs right before the main [StateMemory](#StateMemory) is manually loaded from a slot or a custom [SaveState](#SaveState). Slot is 1..4 or -1 on custom [SaveState](#SaveState). Return true to block load.<br/>Params: int slot, [StateMemory](#StateMemory) loaded<br/>

## ON.POST_LOAD_STATE


> Search script examples for [ON.POST_LOAD_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LOAD_STATE)

Runs right after the main [StateMemory](#StateMemory) is manually loaded from a slot or a custom [SaveState](#SaveState). Slot is 1..4 or -1 on custom [SaveState](#SaveState).<br/>Params: int slot, [StateMemory](#StateMemory) loaded<br/>

## ON.BLOCKED_UPDATE


> Search script examples for [ON.BLOCKED_UPDATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.BLOCKED_UPDATE)

Runs instead of POST_UPDATE when anything blocks a PRE_UPDATE. Even runs in Playlunky when [Overlunky](#Overlunky) blocks a PRE_UPDATE.<br/>

## ON.BLOCKED_GAME_LOOP


> Search script examples for [ON.BLOCKED_GAME_LOOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.BLOCKED_GAME_LOOP)

Runs instead of POST_GAME_LOOP when anything blocks a PRE_GAME_LOOP. Even runs in Playlunky when [Overlunky](#Overlunky) blocks a PRE_GAME_LOOP.<br/>

## ON.BLOCKED_PROCESS_INPUT


> Search script examples for [ON.BLOCKED_PROCESS_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.BLOCKED_PROCESS_INPUT)

Runs instead of POST_PROCESS_INPUT when anything blocks a PRE_PROCESS_INPUT. Even runs in Playlunky when [Overlunky](#Overlunky) blocks a PRE_PROCESS_INPUT.<br/>
