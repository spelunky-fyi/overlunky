activate_tiamat_position_hack(true);

set_post_entity_spawn(function(ent)

	-- make them same as in the game, but relative to the tiamat entity
	ent.attack_x = ent.x - 1
	ent.attack_y = ent.y + 2

end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_TIAMAT)