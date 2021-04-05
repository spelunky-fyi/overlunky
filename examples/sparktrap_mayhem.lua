meta.name = "Sparktrap mayhem"
meta.version = "WIP"
meta.description = "Every 4 seconds, spark traps change their behaviour"
meta.author = "Zappatic"

phase = 0
counter = 0

speed = 0.015
rotation_direction = 1
distance = 3.0

speeds = { 0.005, 0.015, 0.03, 0.06, 0.12, 0.24 }

set_callback(function()
    if phase == 0 then -- distance sinewave
        if counter > 360 then counter = 0 end
        distance = 2 + math.sin((math.rad(counter*5)))
    elseif phase == 1 then -- flip rotation
        rotation_direction = rotation_direction * -1
        phase = 3
    elseif phase == 2 then -- random speed
        speed = speeds[math.random(6)] * rotation_direction
        phase = 0
    elseif phase == 3 then -- random fixed distance
        if counter % 15 == 0 then
            distance = math.random(3)
            speed = 0.015 * rotation_direction
        end
    end
    modify_sparktraps(speed, distance)
    counter = counter + 1
end, ON.GUIFRAME)


set_callback(function()
    set_interval(function()
        phase = math.random(4) - 1
    end, 240)
end, ON.LEVEL)


