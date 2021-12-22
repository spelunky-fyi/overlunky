meta.name = 'Raw input test'
meta.version = 'WIP'
meta.description = 'Test raw keyboard, mouse and gamepad input.'
meta.author = 'Dregu'

local iio = get_io()
local prev_keys = ''

set_callback(function()
  -- Show currently pressed keys. Note: Overlunky will eat all keys configured as hotkeys. Modifiers Ctrl, Alt and Super probably don't even right.
  local keys = ''
  for i,v in ipairs(iio.keysdown) do
    if v then keys = keys .. string.char(i-1) end
  end
  if keys ~= prev_keys then
    print(keys)
  end
  prev_keys = keys

  -- Press S to realize S was pressed, ignoring when inputting text in OL
  if iio.keypressed('S') then
    if iio.wantkeyboard then
      print("S pressed in an input field, plz ignore")
    else
      print("S pressed and it's fair game")
    end
  end

  -- Capture mouse clicks
  if iio.mouseclicked[1] then
    local x, y = mouse_position()
    if iio.wantmouse then
      print("Clicked inside Overlunky window, plz ignore")
    else
      print(F"Clicked {x},{y} and it's fair game")
    end
  end

  -- Wheel
  if iio.mousewheel ~= 0 and not iio.wantmouse then
    local dir = 'up'
    if iio.mousewheel < 0 then dir = 'down' end
    print(F'Wheel going {dir}')
  end

  -- Draw raw gamepad data
  draw_circle_filled(iio.gamepad.rx, iio.gamepad.ry, 0.02, 0x660000ff)
end, ON.GUIFRAME)

-- XInput Gamepad
set_callback(function()
  if #players < 1 then return end
  local x, y, l = get_position(players[1].uid)

  -- Fly around with right stick
  if math.abs(iio.gamepad.rx) > 0.1 then
    players[1].velocityx = iio.gamepad.rx/5
    players[1].velocityy = 0.01
    players[1].falling_timer = 0
  end
  if math.abs(iio.gamepad.ry) > 0.1 then
    players[1].velocityy = iio.gamepad.ry/5
    players[1].falling_timer = 0
  end

  -- Throw bombs when holding triggers
  if iio.gamepad.lt > 0.5 and (get_frame() % 4) == 0 then
    spawn(ENT_TYPE.ITEM_BOMB, x-0.2, y, l, -1, 0.06)
  end
  if iio.gamepad.rt > 0.5 and (get_frame() % 4) == 0 then
    spawn(ENT_TYPE.ITEM_BOMB, x+0.2, y, l, 1, 0.06)
  end

  -- Check if a button is down
  -- Get your button flags from https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad#members for now...
  if (iio.gamepad.buttons & 0x100) > 0 then print("Holding L1") end
end, ON.FRAME)
