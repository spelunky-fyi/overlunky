meta.name = 'ImGuiIO input and hotkeys'
meta.version = 'WIP'
meta.description = 'Examples on imgui keyboard, mouse and gamepad input, also set_hotkey.'
meta.author = 'Dregu'

set_callback(function()
  local iio = get_io()

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
      print(F "Clicked {x},{y} and it's fair game")
    end
  end

  -- Wheel
  if iio.mousewheel ~= 0 and not iio.wantmouse then
    local dir = 'up'
    if iio.mousewheel < 0 then dir = 'down' end
    print(F 'Wheel going {dir}')
  end

  -- Draw raw gamepad data if one is connected
  if iio.gamepad.enabled then
    draw_circle_filled(iio.gamepad.rx, iio.gamepad.ry, 0.02, 0x660000ff)
  end

  -- if someone is already using keyboard for input, we probably shouldn't
  -- also true when OL has a hotkey that matches to this event
  if iio.wantkeyboard then
    return
  end

  -- capture keyboard input from game and ui while holding ctrl + alt
  if iio.modifierdown(KEY.OL_MOD_CTRL | KEY.OL_MOD_ALT) then
    -- if we set this early, it will unregister the games raw input for the next bit,
    -- disabling game keys, and also ui keys
    iio.wantkeyboard = true
  end

  -- capture single hotkey combo
  if iio.keypressed(KEY.OL_MOD_CTRL | KEY.OL_MOD_ALT | KEY.A) then
    print(iio.keystring(KEY.OL_MOD_CTRL | KEY.OL_MOD_ALT | KEY.A))
    -- this is too late to try to get exclusive input for this key combo, it has already passed through the ui, and it's also too late to disable the rawinput
    --io.wantkeyboard = true
  end
end, ON.GUIFRAME)

-- XInput Gamepad
set_callback(function()
  local iio = get_io()
  if #players < 1 then return end
  if not iio.gamepad.enabled then return end

  local x, y, l = get_position(players[1].uid)

  -- Fly around with right stick
  if math.abs(iio.gamepad.rx) > 0.1 then
    players[1].velocityx = iio.gamepad.rx / 5
    players[1].velocityy = 0.01
    players[1].falling_timer = 0
  end
  if math.abs(iio.gamepad.ry) > 0.1 then
    players[1].velocityy = iio.gamepad.ry / 5
    players[1].falling_timer = 0
  end

  -- Throw bombs when holding triggers
  if iio.gamepad.lt > 0.5 and (get_frame() % 4) == 0 then
    spawn(ENT_TYPE.ITEM_BOMB, x - 0.2, y, l, -1, 0.06)
  end
  if iio.gamepad.rt > 0.5 and (get_frame() % 4) == 0 then
    spawn(ENT_TYPE.ITEM_BOMB, x + 0.2, y, l, 1, 0.06)
  end

  -- Check if a button is down
  -- Get your button flags from https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad#members for now...
  if (iio.gamepad.buttons & 0x100) > 0 then print("Holding L1") end
end, ON.POST_UPDATE)

-- create some global hotkeys
function hotkey_handler(key) print(get_io().keystring(key)) end

-- will be suppressed by inactive window or active input fields in this tool instance
set_hotkey(hotkey_handler, KEY.Z | KEY.OL_MOD_CTRL, HOTKEY_TYPE.NORMAL)

-- won't be suppressed by inactive window or input fields
set_hotkey(hotkey_handler, KEY.X | KEY.OL_MOD_ALT, HOTKEY_TYPE.GLOBAL | HOTKEY_TYPE.INPUT)


-- smart key handler that reacts to named key presses, but also
-- opens the key picker when any key is still unbound,
-- checks if ui is already capturing input
-- and tells overlunky to ignore picked keys
keys = {}
function key_handler(ctx, name)
  if not keys[name] or keys[name] == -1 then
    keys[name] = ctx:key_picker(F "Pick key for: {name}", KEY_TYPE.ANY)
    -- key picker will return -1 until a key has been released
    if keys[name] ~= -1 then
      print(F "Picked {keys[name]} {key_name(keys[name])} {name}")
      if get_bucket().overlunky then
        get_bucket().overlunky.ignore_keycodes:clear()
        for _, v in pairs(keys) do
          get_bucket().overlunky.ignore_keycodes:insert(nil, v)
        end
      end
    end
  elseif not get_io().wantkeyboard then
    return get_io().keypressed(keys[name])
  end
end

set_callback(function(ctx)
  if key_handler(ctx, "first") then print("First action with " .. key_name(keys.first)) end
  if key_handler(ctx, "second") then print("Second action with " .. key_name(keys.second)) end
  if key_handler(ctx, "third") then print("Third action with " .. key_name(keys.third)) end
end, ON.GUIFRAME)
