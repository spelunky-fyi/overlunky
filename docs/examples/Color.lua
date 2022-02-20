-- make a semi transparent red color and print it in different formats
local color = Color:red()
color.a = 0.5
local r, g, b, a = color:get_rgba()
prinspect(r, g, b, a) -- 255, 0, 0, 128
prinspect(color.r, color.g, color.b, color.a) -- 1.0, 0.0, 0.0, 0.5
prinspect(string.format("%x"), color:get_ucolor()) -- 800000ff
