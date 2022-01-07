meta= {
    name = "Qilin Practise",
    description = "Start with a Qilin at Tiamat to practise flying (press the DOOR/BUY button to reset)",
    author = "Tuquar"
}

local qilin_uid = nil
local start_x = 17.5
local start_y = 88

local function go_to_tiamat()
	warp(1, 1, 14)
end

local function mount_new_qilin()
	-- Spawn qilin and tame it
	x, y, layer = get_position(players[1].uid)
	qilin_uid = spawn_entity(ENT_TYPE.MOUNT_QILIN, x, y, layer, 0, 0)
	qilin = get_entity(qilin_uid)
	qilin:tame(true)
	-- Player mounts qilin
	carry(qilin_uid, players[1].uid)
end

-- check every frame for button press (to reset position)
set_callback(function()
	for i,player in ipairs(players) do
		-- stunned (18), entering (19), loading (20), exiting (21) or dead(22)
		if player.state < 19 and player:is_button_pressed(BUTTON.DOOR) then
			-- reset player on new mount
			qilin = get_entity(qilin_uid)
			if qilin then
				qilin:destroy()
			end
			-- reset any stun
			player:stun(0)
			mount_new_qilin()
			-- tp to just below jump challenge
			move_entity(qilin_uid, start_x, start_y, 0, 0)
		end
	end
end, ON.FRAME)

set_callback(function()
	-- if player is not dead
	if not players[1] then
		return
	end
	players[1].health = 99
	mount_new_qilin()
	if qilin_uid then
		say(qilin_uid, 'Press the DOOR/BUY button to reset', 1, true)
	end
end, ON.LEVEL)

set_callback(function()
	go_to_tiamat()
end, ON.CAMP)

set_callback(function()
	go_to_tiamat()
end, ON.SCRIPT_ENABLE)
