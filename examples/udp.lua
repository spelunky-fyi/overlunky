meta = {
    name = "UDP echo example",
    description = "Opens UDP server on a random port and echoes sent messages, closes the server on 'gg'",
    author = "Dregu",
    unsafe = true
}

options = {
    host = "127.0.0.1", -- can also be just "localhost"
    port = 0
}

function init()
    deinit()

    -- Bind/Open udp server on the given host:port
    server = UdpServer:new(options.host, options.port)

    -- If port was opened successfully, start checking the message queue
    if server:is_open() then
        print(F "Listening on {server:address()}, please send some UDP datagrams or 'gg' to close")
        server_inter = set_global_interval(function()
            repeat
                ret = server:read(function(msg, src)
                    print(F "Received: '{msg}' from {src}")
                    server:send(msg, src) -- echo
                    if msg == "gg" then
                        server:close()
                        clear_callback()
                        server_inter = nil
                        ret = -1 -- to stop te loop
                        print("Server is now closed, have a nice day")
                    end
                end)
            until ret == -1
        end, 1)
    else
        print(F "Failed to open server: {server:last_error_str()}")
    end
end

function deinit()
    if server then
        server:close()
        if server_inter then clear_callback(server_inter) end
        server = nil
    end
end

set_callback(init, ON.LOAD)
set_callback(init, ON.SCRIPT_ENABLE)
set_callback(deinit, ON.SCRIPT_DISABLE)

register_option_callback("x", nil, function(ctx)
    options.host = ctx:win_input_text("Host", options.host)
    options.port = ctx:win_input_int("Port", options.port)
    if options.port < 0 then options.port = 0 end
    if options.port > 65535 then options.port = 65535 end
    if ctx:win_button("Start server") then init() end
    ctx:win_inline()
    if ctx:win_button("Stop server") then deinit() end
    if server then
        ctx:win_text(server:last_error_str())
    end
    if server and server:is_open() then
        ctx:win_text(F "Listening on {server:address()}\nTry sending something with udp_send:")
        if ctx:win_button("Send 'Hello World!'") then udp_send(server:address(), "Hello World!") end
        ctx:win_inline()
        if ctx:win_button("Send 'gg'") then udp_send(server:address(), "gg") end
    else
        ctx:win_text("Stopped")
    end
end)
