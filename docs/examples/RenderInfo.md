> For using a custom normal map:

```lua
set_post_entity_spawn(function(ent)
  -- Doesn't really make sense with this texture, you can use your custom normal texture id here
  ent.rendering_info:set_normal_map_texture(TEXTURE.DATA_TEXTURES_FLOORSTYLED_GOLD_NORMAL_0)
  ent.rendering_info.shader = 30 -- Make sure to set the shader to one that uses normal map
end, SPAWN_TYPE.LEVEL_GEN, MASK.FLOOR, ENT_TYPE.FLOORSTYLED_MINEWOOD)
```

> Note: if using set_texture_num, make sure to have used set_second_texture/set_third_texture before, since not doing so can lead to crashes
