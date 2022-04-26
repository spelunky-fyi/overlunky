activate_sparktraps_hack(true);

-- set random speed, direction and distance for the spark
set_post_entity_spawn(function(ent)

	direction = 1
	if prng:random_chance(2, PRNG_CLASS.ENTITY_VARIATION) then
		direction = -1
	end

	ent.speed = prng:random_float(PRNG_CLASS.ENTITY_VARIATION) * 0.1 * direction
	ent.distance = prng:random_float(PRNG_CLASS.ENTITY_VARIATION) * 10

end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_SPARK)