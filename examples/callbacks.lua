meta.name = 'Callback test'
meta.version = '1.2.3'
meta.description = 'Tests callbacks.'
meta.author = 'Dregu'

frame = 1
frame2 = 1
id = -1
id2 = -1
id3 = -1

set_callback(function() message("ON.CAMP") end, ON.CAMP)
set_callback(function()
  frame = 1
  message("ON.LEVEL")
end, ON.LEVEL)
set_callback(function()
  frame2 = 1
  message("ON.START")
end, ON.START)
set_callback(function() message("ON.TRANSITION") end, ON.TRANSITION)
set_callback(function() message("ON.DEATH") end, ON.DEATH)
set_callback(function() message("ON.WIN") end, ON.WIN)
set_callback(function() message("ON.CONSTELLATION") end, ON.CONSTELLATION)
set_callback(function() message("ON.CREDITS") end, ON.CREDITS)
set_callback(function() message("ON.SPACESHIP") end, ON.SPACESHIP)
set_callback(function() message("ON.SCORES") end, ON.SCORES)
set_callback(function() message("ON.RECAP") end, ON.RECAP)
set_callback(function() message("ON.SCREEN") end, ON.SCREEN)
set_callback(function()
  if state.time_level > frame then
    frame = state.time_level + 60
    message("ON.FRAME "..tostring(state.time_level))
  end
end, ON.FRAME)
set_callback(function()
  id = set_global_interval(function()
    message("global interval "..tostring(id))
  end, 180)
end, ON.START)
set_callback(function()
  id = set_interval(function()
    message("level interval "..tostring(id2))
  end, 180)
end, ON.LEVEL)
function timeout()
  message("timeout "..tostring(id3))
  set_timeout(timeout, math.random(90, 120))
end
function on_camp() message("on_camp") end
function on_level() 
  message("on_level")
  set_timeout(timeout, 60)
end
function on_start()
  message("on_start")
  set_global_timeout(function()
    message("hello it's me global_timeout from the past")
  end, 1200)
end
function on_transition() message("on_transition") end
function on_death() message("on_death") end
function on_win() message("on_win") end
function on_screen() message("on_screen") end
function on_frame()
  if state.time_total > frame2 then
    frame2 = state.time_total + 45
    message("on_frame "..tostring(state.time_total))
  end
end
