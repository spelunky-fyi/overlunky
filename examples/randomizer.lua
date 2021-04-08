meta.name = "Randomizer"
meta.version = "1.2"
meta.description = "The Randomizer from Overlunky in one Playlunky compatible mod."
meta.author = "Dregu"

-- This is a horrible module example btw, don't take any notes.

register_option_bool("container",  "Container rando +++++++++++++", true)
register_option_bool("door",       "Door rando ++++++++++++++++++", true)
register_option_bool("enemy",      "Enemy rando +++++++++++++++++", true)
register_option_bool("projectile", "Projectile rando ++++++++++++", false)
register_option_bool("trap",       "Trap rando ++++++++++++++++++", true)
register_option_int("add_health", "Max starting health", 20, 4, 99)
register_option_int("add_bombs", "Max starting bombs", 20, 4, 99)
register_option_int("add_ropes", "Max starting ropes", 20, 4, 99)

local container = require "rando.container"
local door = require "rando.door"
local enemy = require "rando.enemy"
local trap = require "rando.trap"
local projectile = require "rando.projectile"

local enable = {
    container = false,
    door = false,
    enemy = false,
    trap = false,
    projectile = false
}

set_callback(function()
    if options.container and not enable.container then
        container.start()
        enable.container = true
    elseif enable.container and not options.container then
        container.stop()
        enable.container = false
    end
    if options.door and not enable.door then
        door.start()
        enable.door = true
    elseif enable.door and not options.door then
        door.stop()
        enable.door = false
    end
    if options.enemy and not enable.enemy then
        enemy.start()
        enable.enemy = true
    elseif enable.enemy and not options.enemy then
        enemy.stop()
        enable.enemy = false
    end
    if options.trap and not enable.trap then
        trap.start()
        enable.trap = true
    elseif enable.trap and not options.trap then
        trap.stop()
        enable.trap = false
    end
    if options.projectile and not enable.projectile then
        projectile.start()
        enable.projectile = true
    elseif enable.projectile and not options.projectile then
        projectile.stop()
        enable.projectile = false
    end
end, ON.GUIFRAME)

set_callback(function()
    for i, player in ipairs(players) do
        player.health = math.random(4, options.add_health)
        player.inventory.bombs = math.random(4, options.add_bombs)
        player.inventory.ropes = math.random(4, options.add_ropes)
    end
end, ON.START)
