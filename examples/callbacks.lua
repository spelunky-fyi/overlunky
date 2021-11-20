meta.name = 'Callback test'
meta.version = 'WIP'
meta.description = 'Tests callbacks.'
meta.author = 'Dregu'

register_option_bool("no_on_frame", "Disable intervals", "Disable ON.FRAME, set_interval and timeout messages", false)

frame = 1
id = -1
id2 = -1
id3 = -1
count = 0

set_callback(function() message("ON.LOGO") end, ON.LOGO)
set_callback(function() message("ON.INTRO") end, ON.INTRO)
set_callback(function() message("ON.PROLOGUE") end, ON.PROLOGUE)
set_callback(function() message("ON.TITLE") end, ON.TITLE)
set_callback(function() message("ON.MENU") end, ON.MENU)
set_callback(function() message("ON.OPTIONS") end, ON.OPTIONS)
set_callback(function() message("ON.LEADERBOARD") end, ON.LEADERBOARD)
set_callback(function() message("ON.SEED_INPUT") end, ON.SEED_INPUT)
set_callback(function() message("ON.CHARACTER_SELECT") end, ON.CHARACTER_SELECT)
set_callback(function() message("ON.TEAM_SELECT") end, ON.TEAM_SELECT)
set_callback(function() message("ON.CAMP") end, ON.CAMP)
set_callback(function() message("ON.LEVEL")
                        frame = 1
                        id3 = set_timeout(timeout, 60) end, ON.LEVEL)
set_callback(function() message("ON.TRANSITION") end, ON.TRANSITION)
set_callback(function() message("ON.DEATH") end, ON.DEATH)
set_callback(function() message("ON.SPACESHIP") end, ON.SPACESHIP)
set_callback(function() message("ON.WIN") end, ON.WIN)
set_callback(function() message("ON.CREDITS") end, ON.CREDITS)
set_callback(function() message("ON.SCORES") end, ON.SCORES)
set_callback(function() message("ON.CONSTELLATION") end, ON.CONSTELLATION)
set_callback(function() message("ON.RECAP") end, ON.RECAP)
set_callback(function() message("ON.ARENA_MENU") end, ON.ARENA_MENU)
set_callback(function() message("ON.ARENA_INTRO") end, ON.ARENA_INTRO)
set_callback(function() message("ON.ARENA_MATCH") end, ON.ARENA_MATCH)
set_callback(function() message("ON.ARENA_SCORE") end, ON.ARENA_SCORE)
set_callback(function() message("ON.ONLINE_LOADING") end, ON.ONLINE_LOADING)
set_callback(function() message("ON.ONLINE_LOBBY") end, ON.ONLINE_LOBBY)
--set_callback(function(draw_ctx) message("ON.GUIFRAME ") end, ON.GUIFRAME)
set_callback(function()
  if not options.no_on_frame and state.time_level > frame then
    frame = state.time_level + 60
    message(F"ON.FRAME {state.time_level}")
  end
end, ON.FRAME)
-- ON.GAMEFRAME
set_callback(function() message(F"ON.SCREEN {state.screen}") end, ON.SCREEN)
set_callback(function() message("ON.START") end, ON.START)
set_callback(function() message(F"ON.LOADING {state.loading}") end, ON.LOADING)
set_callback(function() message("ON.RESET") end, ON.RESET)
--set_callback(function(save_ctx) message("ON.SAVE") end, ON.SAVE) -- same as ON.SCREEN
set_callback(function() message("ON.LOAD") end, ON.LOAD)
set_callback(function() message("ON.SCRIPT_ENABLE") end, ON.SCRIPT_ENABLE)
set_callback(function() message("ON.SCRIPT_DISABLE") end, ON.SCRIPT_DISABLE)
set_callback(function() message("ON.SPEECH_BUBBLE") end, ON.SPEECH_BUBBLE)
set_callback(function() message("ON.TOAST") end, ON.TOAST)

set_callback(function()
  id = set_global_interval(function()
    message(F"[global interval]: {id}")
    count = count + 1
    if count > 5 then
      message("[global interval]: this has run for long enough!")
      clear_callback(id)
    end
  end, 180) -- 180 = about 3s

end, ON.CAMP)

set_callback(function()
  id2 = set_interval(function()
    if not options.no_on_frame then
      message(F"[level interval]: {id2}")
    end
  end, 180) -- 180 = about 3s
end, ON.LEVEL)

function timeout()
  if not options.no_on_frame then
    message(F"[timeout]: {id3}")
  end
  id3 = set_timeout(timeout, math.random(90, 120))
end

set_global_timeout(function()
  message("you loaded the script two seconds ago didn't you")
end, 120)
