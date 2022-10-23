meta.name = "Custom Journal"
meta.version = "EX"
meta.description = "Shows few way's to modify the journal"
meta.author = 'Mr Auto'

set_callback(function(chapter)

    if chapter == JOURNALUI_PAGE_SHOWN.FEATS then
        -- disable the feats in journal
        return true
    end

end, ON.PRE_LOAD_JOURNAL_CHAPTER)


set_callback(function(chapter, pages)

    if chapter == JOURNALUI_PAGE_SHOWN.BESTIARY then
        
        -- add more empty pages to the bestiary, total number of pages should always be multiply of 2
        -- otherwise it may crash or load in a page from different chapter
        table.insert(pages, 100)
        table.insert(pages, 101)
        return pages
    end
    
    if chapter == JOURNALUI_PAGE_SHOWN.PEOPLE then
    
        -- remove Roffy from the journal (yep, hes gone now) and replace he's page with an empty one
        table.remove(pages, 4)
        table.insert(pages, 4, 110)
        return pages
    end

end, ON.POST_LOAD_JOURNAL_CHAPTER)

string_ids = -- optimizations: hash_to_stringid only needs to be called once (probably doesn't matter that much)
{
    places_header = hash_to_stringid(0x4f7bcb96),
    people_header = hash_to_stringid(0xe1e4ca91),
    bestiary_header = hash_to_stringid(0x29b133af),
    items_header = hash_to_stringid(0x22712e240),
    traps_header = hash_to_stringid(0x2f3b54fb),
    feats_header = hash_to_stringid(0xff0b67b3),
    page_number = hash_to_stringid(0x00d28a8e),
    defeated = hash_to_stringid(0x039e2f38),
    killed_by = hash_to_stringid(0xa217f155),
    killed = hash_to_stringid(0xfc17292a),
    N_A = hash_to_stringid(0x9a293191),
}

function setup_page(x, y, render_ctx, page_type, page_number)

    local side_multiply = 1
    local aligment = VANILLA_TEXT_ALIGNMENT.RIGHT
    
    if x > 0 then -- check page side
        side_multiply = -1
        aligment = VANILLA_TEXT_ALIGNMENT.LEFT
    end
    
    if page_number ~= nil then -- draw page number in the corner if needed
        
        -- format the "Entry {number}"
        text = string.format(get_string(string_ids.page_number), page_number)
        -- it's a little bit off, but that's the best i could do by eye, not sure if the font is right anyway
        render_ctx:draw_text(text, x + 0.644 * side_multiply, 0.7137 + 0.005 * side_multiply, 0.00093, 0.0005, Color:new(), aligment, VANILLA_FONT_STYLE.ITALIC)
    end
    
    if page_type >= JOURNAL_PAGE_TYPE.PLACES and page_type <= JOURNAL_PAGE_TYPE.TRAPS then
        -- draw the background for the elements on the page
        dest = AABB:new(-3.0, 0.888, 1.0, -0.888)
        if x > 0 then
            dest = AABB:new(-1.0, 0.888, 3.0, -0.888)
        end
        render_ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_TOP_ENTRY_0, 0, 0, dest, Color:white())
        -- you could potentially draw empty page with just the bow at the top, but that's requires the use of draw_screen_texture with `Quad& source` etc.
        
        header_text = ""
        if page_type == JOURNAL_PAGE_TYPE.BESTIARY then
        
            -- draw background for the Defeated/Killed By Numbers
            dest = Quad:new(AABB:new(-0.898, 0.558, -0.32, 0.053))
            text_x = -0.82
            if x > 0 then -- check page side
                dest = Quad:new(AABB:new(0.26, 0.558, 0.855, 0.053))
                text_x = 0.34
            end
            render_ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_ELEMENTS_0, Quad:new(AABB:new(0.0, 0.35, 0.6, 1.0)), dest, Color:white())
            
            -- draw the "Defeated" and "Killed By" texts
            render_ctx:draw_text(get_string(string_ids.defeated), text_x, 0.4957, 0.0014, 0.0007, Color:new(), VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
            render_ctx:draw_text(get_string(string_ids.killed_by), text_x, 0.2748, 0.0014, 0.0007, Color:new(), VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
            
            header_text = get_string(string_ids.bestiary_header)
        elseif page_type == JOURNAL_PAGE_TYPE.PLACES then
            header_text = get_string(string_ids.places_header)
        elseif page_type == JOURNAL_PAGE_TYPE.PEOPLE then
            -- should have similar stuff as BESTIARY, but i din't want to write extra logic for character pages etc.
            header_text = get_string(string_ids.people_header)
        elseif page_type == JOURNAL_PAGE_TYPE.ITEMS then
            header_text = get_string(string_ids.items_header)
        elseif page_type == JOURNAL_PAGE_TYPE.TRAPS then
            header_text = get_string(string_ids.traps_header)
        elseif page_type == JOURNAL_PAGE_TYPE.FEATS then
            header_text = get_string(string_ids.feats_header)
        end
        -- draw the header/title on the red bow
        render_ctx:draw_text(header_text, -1.0 * side_multiply, 0.74, 0.0022, 0.0011, Color:white(), VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
    end
end


background_texture_def = TextureDefinition:new()
background_texture_def.texture_path = "floor_puke.png"
background_texture_def.width = 1536
background_texture_def.height = 1536
background_texture_def.tile_width = 256
background_texture_def.tile_height = 256

fire_dragon_def = TextureDefinition:new()
fire_dragon_def.texture_path = "loading.png"
fire_dragon_def.width = 255
fire_dragon_def.height = 255
fire_dragon_def.tile_width = 255
fire_dragon_def.tile_height = 255


background_texture = define_texture(background_texture_def)
fire_dragon_texture = define_texture(fire_dragon_def)


set_callback(function(render_ctx, page_type, page)

    x = page.background.x
    y = page.background.y
    
    if page.page_number == 100 then
        -- be aware that page_type here, for custom pages is always gonna be JOURNAL_PAGE_TYPE.STORY
        setup_page(x, y, render_ctx, JOURNAL_PAGE_TYPE.BESTIARY, 79)
        
        -- draw the name right side page would have the same parameters, just x as negative
        render_ctx:draw_text("FIRE DRAGON", 0.24, -0.12, 0.0023, 0.0011, Color:white(), VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
        
        -- draw the N/A text for the values of "Defeated" and "Killed By"
        N_A_text = get_string(string_ids.N_A)
        render_ctx:draw_text(N_A_text, 0.57, 0.41, 0.0023, 0.0011, Color:white(), VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
        render_ctx:draw_text(N_A_text, 0.57, 0.185, 0.0023, 0.0011, Color:white(), VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
        
        -- draw the monster background and logo
        icon_position = AABB:new(-0.4, 0.03, 0.28, 0.61)
        render_ctx:draw_screen_texture(background_texture, 0, 0, icon_position, Color:new(0.3,0,0,1))
        render_ctx:draw_screen_texture(fire_dragon_texture, 0, 0, icon_position, Color:white())
        
    elseif page.page_number == 101 then
        -- empty page (mostly to render the other half of the BESTIARY header)
        setup_page(x, y, render_ctx, JOURNAL_PAGE_TYPE.BESTIARY, 80)
    
    elseif page.page_number == 110 then
        -- setup empty page to replace rofly
        setup_page(x, y, render_ctx, JOURNAL_PAGE_TYPE.PEOPLE, 4)
    end

end, ON.RENDER_POST_JOURNAL_PAGE)
