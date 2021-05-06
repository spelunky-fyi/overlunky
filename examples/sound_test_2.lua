meta.name = 'Event test'
meta.version = 'WIP'
meta.description = 'Tests vanilla sound stuff'
meta.author = 'Malacath-92'

message('Loaded event test mod...')

-- Thanks @Trixelized
pre_fart = create_sound('pre_fart.wav')
fart = create_sound('fart.wav')

-- Fart for Roffy
set_vanilla_sound_callback(VANILLA_SOUND.PLAYER_WHIP1, VANILLA_SOUND_CALLBACK_TYPE.STARTED, function(whip_sound)
    if whip_sound:get_parameter(VANILLA_SOUND_PARAM.PLAYER_CHARACTER) == 4 then
        whip_sound:stop()
        pre_fart:play()
    end
end)
set_vanilla_sound_callback(VANILLA_SOUND.PLAYER_WHIP2, VANILLA_SOUND_CALLBACK_TYPE.STARTED, function(whip_sound)
    if whip_sound:get_parameter(VANILLA_SOUND_PARAM.PLAYER_CHARACTER) == 4 then
        whip_sound:stop()
        fart:play()
    end
end)

-- Lower pitch for Valerie
set_vanilla_sound_callback(VANILLA_SOUND.PLAYER_WHIP1, VANILLA_SOUND_CALLBACK_TYPE.CREATED, function(whip_sound)
    if whip_sound:get_parameter(VANILLA_SOUND_PARAM.PLAYER_CHARACTER) == 13 then
        whip_sound:set_pitch(0.5)
    end
end)
set_vanilla_sound_callback(VANILLA_SOUND.PLAYER_WHIP2, VANILLA_SOUND_CALLBACK_TYPE.CREATED, function(whip_sound)
    if whip_sound:get_parameter(VANILLA_SOUND_PARAM.PLAYER_CHARACTER) == 13 then
        whip_sound:set_pitch(0.5)
    end
end)

test_sound = get_sound(VANILLA_SOUND.PLAYER_WHIP1)
if test_sound ~= nil then
    test_sound:play()
    for _, name in pairs(test_sound:get_parameters()) do
        message(name)
    end
end
count = 0

set_callback(function()
    if test_sound ~= nil then
        interval_id = set_interval(function()
            message('Playing sound...')
            playing_sound = test_sound:play(true)
            if count == 0 then
                message('Panning all left...')
                if not playing_sound:set_pan(-1.0) then
                    message('Failed as expected...')
                end
            elseif count == 1 then
                message('Panning all right...')
                if not playing_sound:set_pan(1.0) then
                    message('Failed as expected...')
                end
            elseif count == 2 then
                message('High pitch...')
                playing_sound:set_pitch(2.0)
            elseif count == 3 then
                message('Low pitch...')
                playing_sound:set_pitch(0.5)
            elseif count == 4 then
                message('Low volume...')
                playing_sound:set_volume(0.25)
            elseif count == 5 then
                message('Negative volume...')
                playing_sound:set_volume(-1.0)
            elseif count == 6 then
                message('Muted...')
                if not playing_sound:set_mute(true) then
                    message('Failed as expected...')
                end
            elseif count == 7 then
                message('With callback...')
                playing_sound:set_callback(function()
                    message("Sound ended...")
                end)
            elseif count == 8 then
                message('With looping...')
                if not playing_sound:set_looping(SOUND_LOOP_MODE.LOOP) then
                    message('Failed as expected...')
                end
            end
            playing_sound:set_pause(false)
    
            count = count + 1
            if count > 8 then
                message('Destroying sound...')
                clear_callback(interval_id)
                playing_sound:stop()
                test_sound = nil
            end
        end, 60 * 5)
    else
        message('Event was not found...')
    end
end, ON.CAMP)
