meta.name = "Vanilla rendering"
meta.version = "WIP"
meta.description = "Demonstrates drawing of in-game text and textures"
meta.author = "Zappatic"

white = Color:white()
red = Color:red()
green = Color:green()
blue = Color:blue()
black = Color:black()
fuchsia = Color:fuchsia()

font_scale = 0.0006

-- On way to render text that isn't changing is to create TextRenderingInfo for it, this saves some resources compared to drawing a text directly
textRender = TextRenderingInfo:new("Bordered, shadowed text", 0.0008, 0.0008, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)

set_callback(function(render_ctx)
    local y = 0.9
    local scale = font_scale 
    for i = 1, 7 do
        render_ctx:draw_text("Text scale "..tostring(scale), 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
        y = y - 0.1
        scale = scale + 0.0001
    end
    y = y + 0.05
    
    local x = -0.28
    for i = 1, 7 do
        render_ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_HUD_0, 4, i-1, x, y, x + 0.08, y - (0.08 * (16.0/9.0)), white)
        x = x + 0.08
    end
    y = y - 0.20

    scale = 0.0008
    render_ctx:draw_text("Left aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07
    render_ctx:draw_text("Center aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07
    render_ctx:draw_text("Right aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07

    red_width, _ = render_ctx:draw_text_size("Red ", scale, scale, VANILLA_FONT_STYLE.ITALIC)
    green_width, _ = render_ctx:draw_text_size("Green ", scale, scale, VANILLA_FONT_STYLE.ITALIC)
    blue_width, _ = render_ctx:draw_text_size("Blue ", scale, scale, VANILLA_FONT_STYLE.ITALIC)
    space_width = 0.01
    total_width = red_width + green_width + blue_width + (space_width * 2.0)

    render_ctx:draw_text("Red", (total_width / 2.0) * -1.0, y, scale, scale, red, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    render_ctx:draw_text("Green", ((total_width / 2.0) * -1.0) + red_width + space_width, y, scale, scale, green, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    render_ctx:draw_text("Blue", ((total_width / 2.0) * -1.0) + red_width + space_width + green_width + space_width, y, scale, scale, blue, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07

    render_ctx:draw_text("Bold text", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
    y = y - 0.07

    -- "Bordered, shadowed text"
    textRender.x = 0.0035
    textRender.y = y - 0.0035
    render_ctx:draw_text(textRender, black) -- shadow
    textRender.x = 0.0
    textRender.y = y
    render_ctx:draw_text(textRender, white) -- normal text
    
    -- because this text is rendered centered, we have to calc the top left relative to the centerpoint we provided to render_ctx:draw_text
    width, height = textRender:text_size()
    borderBox = AABB:new(0.0 - (width / 2.0), y - (height / 2.0), 0.0 - (width / 2.0) + width, y - (height / 2.0) + height)
    render_ctx:draw_screen_rect(borderBox, 2, fuchsia) -- border
    
    y = y - 0.12

    -- jump = \u{83}
    -- attack = \u{84}
    -- bomb = \u{85}
    -- rope = \u{86}
    -- pause = \u{89}
    -- journal = \u{8A}
    -- use door/buy = \u{88}
    -- walk/run = \u{87}
    -- left = \u{8B}
    -- right = \u{8C}
    -- up = \u{8D}
    -- down = \u{8E}
    -- enter/confirm = \u{8F}
    -- if you combine multiple keys in one string, it might mess up, e.g. "\u{83} and \u{89}"
    local special_characters = "Press \u{83} to jump"
    scale = 0.0013
    render_ctx:draw_text(special_characters, 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)

end, ON.RENDER_POST_HUD)

set_callback(function(render_ctx)
    render_ctx:draw_text("Rendered below the HUD", 0.95, 0.86, 0.0006, 0.0006, red, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.ITALIC)
end, ON.RENDER_PRE_HUD)

-- draw a crown on the player's head
set_callback(function(render_ctx, draw_depth)
    if #players < 1 then return end
    -- the event is PRE draw depth, so if we want to draw it in front of the player, we have to
    -- draw it 'pre' the next draw_depth, which is one closer towards draw_depth 1 (so subtract 1 instead of add 1)
    if draw_depth == players[1].type.draw_depth - 1 then
        x, y, l = get_render_position(players[1].uid)
        -- reposition the crown on top of the head, starting from the center point of the player: bit to the left, bit upwards
        x = x - 0.5
        y = y + 1.15
        rect = Quad:new(AABB:new(x, y, x + 1.0, y - 1.0))
        render_ctx:draw_world_texture(TEXTURE.DATA_TEXTURES_ITEMS_0, 1, 14, rect, white, WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW)
    end
end, ON.RENDER_PRE_DRAW_DEPTH)

-- move Ana on the title screen
set_pre_render_screen(SCREEN.TITLE, function(screen, render_ctx)
    s = screen:as_screen_title()
    s.ana:set_destination(AABB:new(-0.1, 0, 0.15, -0.4))
    s.particle_torchflame_flames.x = 0.05
    s.particle_torchflame_flames.y = 0.0
    s.particle_torchflame_backflames.x = 0.05
    s.particle_torchflame_backflames.y = 0.0
    s.particle_torchflame_smoke.x = 0.0
    s.particle_torchflame_smoke.y = 0.0
    s.particle_torchflame_backflames_animated.x = 0.05
    s.particle_torchflame_backflames_animated.y = 0.0
    s.particle_torchflame_flames_animated.x = 0.05
    s.particle_torchflame_flames_animated.y = 0.0
    s.particle_torchflame_ash.x = 0.05
    s.particle_torchflame_ash.y = 0.0
end)

-- drawing text on top of the screen should be done in the post_render_screen callback:
set_post_render_screen(SCREEN.TITLE, function(screen, render_ctx)
    render_ctx:draw_text("Ana", 0.0, 0.0, 0.0006, 0.0006, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
end)

-- put the journal in the bottom right corner
set_pre_render_screen(SCREEN.LEVEL, function(screen, render_ctx)
    -- the journal (and pause screen) UI is available through the game manager, it's not a property of a particular screen
    if game_manager.journal_ui.state > 0 then -- checks if the journal is visible
        game_manager.journal_ui.entire_book:set_destination(AABB:new(0, 0, 0.95, -0.95))
    end
end)

set_callback(function(render_ctx, page_type, page)
    if page_type == JOURNAL_PAGE_TYPE.JOURNAL_MENU then
        render_ctx:draw_text("Text on the journal menu", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.PROGRESS then
        render_ctx:draw_text("Text on the progress page", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.PLACES then
        -- If you draw when the page_type equals PLACES, then it will be drawn on every place page
        -- If you want to draw on a specific page, compare the page_number of the current page to the page you want to render, in this case page 3
        -- The page number is a zero-based index, and you have to count the journal progress and journal menu page as well, so Jungle is page 3
        -- In this case, we don't have to cast the page with as_journal_page_places() because page_number is available in the base class JournalPage
        if page.page_number == 3 then
            render_ctx:draw_text("Text on the Jungle place page", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
        end
    elseif page_type == JOURNAL_PAGE_TYPE.PEOPLE then
        render_ctx:draw_text("Text on all people pages", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.BESTIARY then
        render_ctx:draw_text("Text on all bestiary pages", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
        -- hide all main images in bestiary (as if they were undiscovered)
        local p = page:as_journal_page_bestiary();
        p.show_main_image = false
    elseif page_type == JOURNAL_PAGE_TYPE.ITEMS then
        render_ctx:draw_text("Text on all items pages", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.TRAPS then
        render_ctx:draw_text("Text on all traps pages", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.STORY then
        render_ctx:draw_text("Text on all story pages", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.FEATS then
        render_ctx:draw_text("Text on all feats pages", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.DEATH_CAUSE then
        render_ctx:draw_text("Text on the deathcause page", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.DEATH_MENU then
        render_ctx:draw_text("Text on the death menu", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.RECAP then
        render_ctx:draw_text("Text on the recap page", 0.0, 0.0, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.PLAYER_PROFILE then
        render_ctx:draw_text("Text on the player profile page", 0.0, 0.0, 0.0018, 0.0018, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    elseif page_type == JOURNAL_PAGE_TYPE.LAST_GAME_PLAYED then
        render_ctx:draw_text("Text on the last game played page", 0.0, 0.2, 0.0018, 0.0018, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    end
end, ON.RENDER_POST_JOURNAL_PAGE)
