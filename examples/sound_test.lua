meta.name = 'Sound test'
meta.version = 'WIP'
meta.description = 'Tests sound stuff'
meta.author = 'Malacath-92'

test_sound = create_sound('stupid.mp3', false)
count = 0

set_callback(function()
    if test_sound ~= nil then
        interval_id = set_interval(function()
            message('Playing sound...')
            test_sound:play(false)
    
            count = count + 1
            if count > 4 then
                message('Destroying sound...')
                test_sound = nil
                clear_callback(interval_id)
            end
        end, 60 * 5)
    else
        message('Sound was not found...')
    end
end, ON.CAMP)