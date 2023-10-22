set_camera_layer_control_enabled(false)

g_current_timer = nil
-- default load_time 36
function change_layer(layer_to, load_time)
    
    if state.camera_layer == layer_to then
        return
    end
    if g_current_timer ~= nil then
        clear_callback(g_current_timer)
        g_current_timer = nil
    end
    -- if we don't want the load time, we can just change the actual layer
    if load_time == nil or load_time == 0 then
        state.camera_layer = layer_to
        return
    end
    
    state.layer_transition_timer = load_time
    state.transition_to_layer = layer_to
    -- actual layer change after time delay
    set_timeout(function() state.camera_layer = layer_to end, load_time)
end
