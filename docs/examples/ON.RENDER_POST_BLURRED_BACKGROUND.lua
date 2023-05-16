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
