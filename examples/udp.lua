meta = {
    name = "UDP echo example",
    description = "Opens UDP server on a random port and echoes sent messages, closes the server on 'gg'",
    author = "Dregu",
    unsafe = true
}

options = {
    host = "127.0.0.1",
    port = 0
}

function init()
    deinit()

    server = {
        -- Save messages to queue instead of printing directly, cause that doesn't work from the server thread
        queue = {},

        -- Open server on an ephemeral random port, push messages to queue and echo back to sender
        socket = udp_listen(options.host, options.port, function(msg, src)
            msg = msg:gsub("%s*$", "")
            table.insert(server.queue, { msg = msg, src = src })
            if msg == "gg" then
                return "bye!\n"
            else
                return "echo: " .. msg .. "\n"
            end
        end)
    }

    -- If port was opened successfully, start checking the message queue
    if server.socket:open() then
        print(F "Listening on {server.socket.port}, please send some UDP datagrams or 'gg' to close")
        server.inter = set_global_interval(function()
            for _, msg in pairs(server.queue) do
                print(F "Received: '{msg.msg}' from {msg.src}")
                if msg.msg == "gg" then
                    server.socket:close()
                    clear_callback()
                    print("Server is now closed, have a nice day")
                end
            end
            server.queue = {}
        end, 1)
    else
        print(F "Failed to open server: {server.socket:error()}")
    end
end

function deinit()
    if server then
        server.socket:close()
        if server.inter then clear_callback(server.inter) end
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
        ctx:win_text(server.socket:error())
    end
    if server and server.socket:open() then
        ctx:win_text(F "Listening on {server.socket.host}:{server.socket.port}\nTry sending something with udp_send:")
        if ctx:win_button("Send 'Hello World!'") then udp_send(server.socket.host, server.socket.port, "Hello World!") end
        ctx:win_inline()
        if ctx:win_button("Send 'gg'") then udp_send(server.socket.host, server.socket.port, "gg") end
    else
        ctx:win_text("Stopped")
    end
end)
