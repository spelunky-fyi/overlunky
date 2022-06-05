-- forces any level transition to immediately go to constellation ending with custom text
set_callback(function()
    if state.screen_next == SCREEN.TRANSITION then
      if state.level_count == 0 then state.level_count = 1 end -- no /0
      state.win_state = WIN_STATE.COSMIC_OCEAN_WIN
      state.screen_next = SCREEN.CONSTELLATION
      state.world_next = 8
      state.level_next = 99
      state.theme_next = THEME.COSMIC_OCEAN
      state.level_gen.themes[THEME.COSMIC_OCEAN].sub_theme = state.level_gen.themes[state.theme]
      state:force_current_theme(THEME.COSMIC_OCEAN)
      set_global_interval(function()
        if state.screen_constellation.sequence_state == 2 then
          state.screen_constellation.constellation_text = "Lol u stars now"
          clear_callback()
        end
      end, 1)
    end
  end, ON.PRE_LOAD_SCREEN)
