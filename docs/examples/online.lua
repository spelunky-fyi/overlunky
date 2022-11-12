message = "Currently playing: "
for _, p in pairs(online.online_players) do
    if p.ready_state ~= 0 then
        message = message .. p.player_name .. " "
    end
end
print(message)
