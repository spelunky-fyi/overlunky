-- Write a data file
-- Data will be written to Mods/Data/[scriptname.lua or Mod Name]/timestamp.txt
local f = io.open_data(tostring(os.time()) .. ".txt", "w")
if f then
    f:write("hello world at " .. os.date())
    f:close()
end

-- List all files in data dir and read them out
for _, v in pairs(list_data_dir()) do
    local f = io.open_data(v)
    if f then
        print(v .. ": " .. f:read("a"))
    end
end
