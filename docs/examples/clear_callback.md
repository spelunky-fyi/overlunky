> Create three explosions and then clear the interval

```lua
local count = 0 -- this upvalues to the interval
set_interval(function()
  count = count + 1
  spawn(ENT_TYPE.FX_EXPLOSION, 0, 0, LAYER.FRONT, 0, 0)
  if count >= 3 then
    -- calling this without parameters clears the fallback that's calling it
    clear_callback()
  end
end, 60)
```
