
# Enums

Enums are like numbers but in text that's easier to remember.

```lua
set_callback(function()
    if state.theme == THEME.COSMIC_OCEAN then
        x, y, l = get_position(players[1].uid)
        spawn(ENT_TYPE.ITEM_JETPACK, x, y, l, 0, 0)
    end
end, ON.LEVEL)
```

## BEG


> Search script examples for [BEG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG)

[Beg](#Beg) quest states

Name | Data | Description
---- | ---- | -----------
[QUEST_NOT_STARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.QUEST_NOT_STARTED) | 0 | 
[ALTAR_DESTROYED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.ALTAR_DESTROYED) | 1 | 
[SPAWNED_WITH_BOMBBAG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.SPAWNED_WITH_BOMBBAG) | 2 | 
[BOMBBAG_THROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.BOMBBAG_THROWN) | 3 | 
[SPAWNED_WITH_TRUECROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.SPAWNED_WITH_TRUECROWN) | 4 | 
[TRUECROWN_THROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BEG.TRUECROWN_THROWN) | 5 | 

## BUTTON


> Search script examples for [BUTTON](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON)



Name | Data | Description
---- | ---- | -----------
[JUMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.JUMP) | 1 | 
[WHIP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.WHIP) | 2 | 
[BOMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.BOMB) | 4 | 
[ROPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.ROPE) | 8 | 
[RUN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.RUN) | 16 | 
[DOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=BUTTON.DOOR) | 32 | 

## CAUSE_OF_DEATH


> Search script examples for [CAUSE_OF_DEATH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH)



Name | Data | Description
---- | ---- | -----------
[DEATH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.DEATH) | 0 | 
[ENTITY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.ENTITY) | 1 | 
[LONG_FALL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.LONG_FALL) | 2 | 
[STILL_FALLING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.STILL_FALLING) | 3 | 
[MISSED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.MISSED) | 4 | 
[POISONED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CAUSE_OF_DEATH.POISONED) | 5 | 

## CHAR_STATE


> Search script examples for [CHAR_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE)



Name | Data | Description
---- | ---- | -----------
[FLAILING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.FLAILING) | 0 | 
[STANDING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.STANDING) | 1 | 
[SITTING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.SITTING) | 2 | 
[HANGING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.HANGING) | 4 | 
[DUCKING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.DUCKING) | 5 | 
[CLIMBING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.CLIMBING) | 6 | 
[PUSHING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.PUSHING) | 7 | 
[JUMPING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.JUMPING) | 8 | 
[FALLING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.FALLING) | 9 | 
[DROPPING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.DROPPING) | 10 | 
[ATTACKING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.ATTACKING) | 12 | 
[THROWING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.THROWING) | 17 | 
[STUNNED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.STUNNED) | 18 | 
[ENTERING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.ENTERING) | 19 | 
[LOADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.LOADING) | 20 | 
[EXITING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.EXITING) | 21 | 
[DYING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CHAR_STATE.DYING) | 22 | 

## CONST


> Search script examples for [CONST](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST)

Some arbitrary constants of the engine

Name | Data | Description
---- | ---- | -----------
[ENGINE_FPS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ENGINE_FPS) | 60 | The framerate at which the engine updates, e.g. at which `ON.GAMEFRAME` and similar are called.<br/>Independent of rendering framerate, so it does not correlate with the call rate of `ON.GUIFRAME` and similar.<br/>
[ROOM_WIDTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ROOM_WIDTH) | 10 | Width of a 1x1 room, both in world coordinates and in tiles.<br/>
[ROOM_HEIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.ROOM_HEIGHT) | 8 | Height of a 1x1 room, both in world coordinates and in tiles.<br/>
[MAX_TILES_VERT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.MAX_TILES_VERT) | g_level_max_y | Maximum number of working floor tiles in vertical axis, 126 (0-125 coordinates)<br/>Floors spawned above or below will not have any collision<br/>
[MAX_TILES_HORIZ](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.MAX_TILES_HORIZ) | g_level_max_x | Maximum number of working floor tiles in horizontal axis, 86 (0-85 coordinates)<br/>Floors spawned above or below will not have any collision<br/>
[NOF_DRAW_DEPTHS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.NOF_DRAW_DEPTHS) | 53 | Number of draw_depths, 53 (0-52)<br/>
[MAX_PLAYERS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=CONST.MAX_PLAYERS) | 4 | Just the max number of players in multiplayer<br/>

## COSUBTHEME


> Search script examples for [COSUBTHEME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME)

Parameter to force_co_subtheme

Name | Data | Description
---- | ---- | -----------
[NONE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.NONE) | -1 | 
[RESET](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.RESET) | -1 | 
[DWELLING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.DWELLING) | 0 | 
[JUNGLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.JUNGLE) | 1 | 
[VOLCANA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.VOLCANA) | 2 | 
[TIDE_POOL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.TIDE_POOL) | 3 | 
[TEMPLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.TEMPLE) | 4 | 
[ICE_CAVES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.ICE_CAVES) | 5 | 
[NEO_BABYLON](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.NEO_BABYLON) | 6 | 
[SUNKEN_CITY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=COSUBTHEME.SUNKEN_CITY) | 7 | 

## DROP


> Search script examples for [DROP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROP)


Name | Data | Description
---- | ---- | -----------
[ALTAR_DICE_CLIMBINGGLOVES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROP.ALTAR_DICE_CLIMBINGGLOVES) | 0 | 
...see [drops.cpp](https:github.com/spelunky-fyi/overlunky/blob/main/src/game_api/drops.cpp) for a list of possible drops... |  | 
[YETI_PITCHERSMITT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROP.YETI_PITCHERSMITT) | 85 | 

## DROPCHANCE


> Search script examples for [DROPCHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROPCHANCE)


Name | Data | Description
---- | ---- | -----------
[BONEBLOCK_SKELETONKEY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROPCHANCE.BONEBLOCK_SKELETONKEY) | 0 | 
...see [drops.cpp](https:github.com/spelunky-fyi/overlunky/blob/main/src/game_api/drops.cpp) for a list of possible dropchances... |  | 
[YETI_PITCHERSMITT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DROPCHANCE.YETI_PITCHERSMITT) | 10 | 

## DYNAMIC_TEXTURE


> Search script examples for [DYNAMIC_TEXTURE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE)



Name | Data | Description
---- | ---- | -----------
[INVISIBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.INVISIBLE) | DYNAMIC_TEXTURE::INVISIBLE | 
[BACKGROUND](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.BACKGROUND) | DYNAMIC_TEXTURE::BACKGROUND | 
[FLOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.FLOOR) | DYNAMIC_TEXTURE::FLOOR | 
[DOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.DOOR) | DYNAMIC_TEXTURE::DOOR | 
[DOOR_LAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.DOOR_LAYER) | DYNAMIC_TEXTURE::DOOR_LAYER | 
[BACKGROUND_DECORATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.BACKGROUND_DECORATION) | DYNAMIC_TEXTURE::BACKGROUND_DECORATION | 
[KALI_STATUE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.KALI_STATUE) | DYNAMIC_TEXTURE::KALI_STATUE | 
[COFFIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=DYNAMIC_TEXTURE.COFFIN) | DYNAMIC_TEXTURE::COFFIN | 

## ENT_FLAG


> Search script examples for [ENT_FLAG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG)



Name | Data | Description
---- | ---- | -----------
[INVISIBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INVISIBLE) | 1 | 
[INDESTRUCTIBLE_OR_SPECIAL_FLOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INDESTRUCTIBLE_OR_SPECIAL_FLOOR) | 2 | 
[SOLID](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.SOLID) | 3 | 
[PASSES_THROUGH_OBJECTS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PASSES_THROUGH_OBJECTS) | 4 | 
[PASSES_THROUGH_EVERYTHING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PASSES_THROUGH_EVERYTHING) | 5 | 
[TAKE_NO_DAMAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.TAKE_NO_DAMAGE) | 6 | 
[THROWABLE_OR_KNOCKBACKABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.THROWABLE_OR_KNOCKBACKABLE) | 7 | 
[IS_PLATFORM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.IS_PLATFORM) | 8 | 
[CLIMBABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.CLIMBABLE) | 9 | 
[NO_GRAVITY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.NO_GRAVITY) | 10 | 
[INTERACT_WITH_WATER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INTERACT_WITH_WATER) | 11 | 
[STUNNABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.STUNNABLE) | 12 | 
[COLLIDES_WALLS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.COLLIDES_WALLS) | 13 | 
[INTERACT_WITH_SEMISOLIDS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INTERACT_WITH_SEMISOLIDS) | 14 | 
[CAN_BE_STOMPED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.CAN_BE_STOMPED) | 15 | 
[POWER_STOMPS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.POWER_STOMPS) | 16 | 
[FACING_LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.FACING_LEFT) | 17 | 
[PICKUPABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PICKUPABLE) | 18 | 
[USABLE_ITEM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.USABLE_ITEM) | 19 | 
[ENABLE_BUTTON_PROMPT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.ENABLE_BUTTON_PROMPT) | 20 | 
[INTERACT_WITH_WEBS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.INTERACT_WITH_WEBS) | 21 | 
[LOCKED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.LOCKED) | 22 | 
[SHOP_ITEM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.SHOP_ITEM) | 23 | 
[SHOP_FLOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.SHOP_FLOOR) | 24 | 
[PASSES_THROUGH_PLAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PASSES_THROUGH_PLAYER) | 25 | 
[PAUSE_AI_AND_PHYSICS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.PAUSE_AI_AND_PHYSICS) | 28 | 
[DEAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.DEAD) | 29 | 
[HAS_BACKITEM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_FLAG.HAS_BACKITEM) | 32 | 

## ENT_MORE_FLAG


> Search script examples for [ENT_MORE_FLAG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG)



Name | Data | Description
---- | ---- | -----------
[HIRED_HAND_REVIVED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.HIRED_HAND_REVIVED) | 2 | 
[SWIMMING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.SWIMMING) | 11 | 
[HIT_GROUND](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.HIT_GROUND) | 12 | 
[HIT_WALL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.HIT_WALL) | 13 | 
[FALLING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.FALLING) | 14 | 
[CURSED_EFFECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.CURSED_EFFECT) | 15 | 
[ELIXIR_BUFF](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.ELIXIR_BUFF) | 16 | 
[DISABLE_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_MORE_FLAG.DISABLE_INPUT) | 17 | 

## ENT_TYPE


> Search script examples for [ENT_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE)


Name | Data | Description
---- | ---- | -----------
[FLOOR_BORDERTILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.FLOOR_BORDERTILE) | 1 | 
...check [entities.txt](game_data/entities.txt)... |  | 
[LIQUID_COARSE_LAVA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ENT_TYPE.LIQUID_COARSE_LAVA) | 915 | 

## FLOOR_SIDE


> Search script examples for [FLOOR_SIDE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE)



Name | Data | Description
---- | ---- | -----------
[TOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.TOP) | FLOOR_SIDE::TOP | 
[BOTTOM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.BOTTOM) | FLOOR_SIDE::BOTTOM | 
[LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.LEFT) | FLOOR_SIDE::LEFT | 
[RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.RIGHT) | FLOOR_SIDE::RIGHT | 
[TOP_LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.TOP_LEFT) | FLOOR_SIDE::TOP_LEFT | 
[TOP_RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.TOP_RIGHT) | FLOOR_SIDE::TOP_RIGHT | 
[BOTTOM_LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.BOTTOM_LEFT) | FLOOR_SIDE::BOTTOM_LEFT | 
[BOTTOM_RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=FLOOR_SIDE.BOTTOM_RIGHT) | FLOOR_SIDE::BOTTOM_RIGHT | 

## GAMEPAD


> Search script examples for [GAMEPAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD)



Name | Data | Description
---- | ---- | -----------
[UP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.UP) | 0x0001 | 
[DOWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.DOWN) | 0x0002 | 
[LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.LEFT) | 0x0004 | 
[RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.RIGHT) | 0x0008 | 
[START](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.START) | 0x0010 | 
[BACK](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.BACK) | 0x0020 | 
[LEFT_THUMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.LEFT_THUMB) | 0x0040 | 
[RIGHT_THUMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.RIGHT_THUMB) | 0x0080 | 
[LEFT_SHOULDER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.LEFT_SHOULDER) | 0x0100 | 
[RIGHT_SHOULDER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.RIGHT_SHOULDER) | 0x0200 | 
[A](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.A) | 0x1000 | 
[B](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.B) | 0x2000 | 
[X](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.X) | 0x4000 | 
[Y](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD.Y) | 0x8000 | 

## GAMEPAD_FLAG


> Search script examples for [GAMEPAD_FLAG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG)



Name | Data | Description
---- | ---- | -----------
[UP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.UP) | 1 | 
[DOWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.DOWN) | 2 | 
[LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.LEFT) | 3 | 
[RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.RIGHT) | 4 | 
[START](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.START) | 5 | 
[BACK](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.BACK) | 6 | 
[LEFT_THUMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.LEFT_THUMB) | 7 | 
[RIGHT_THUMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.RIGHT_THUMB) | 8 | 
[LEFT_SHOULDER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.LEFT_SHOULDER) | 9 | 
[RIGHT_SHOULDER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.RIGHT_SHOULDER) | 10 | 
[A](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.A) | 13 | 
[B](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.B) | 14 | 
[X](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.X) | 15 | 
[Y](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAMEPAD_FLAG.Y) | 16 | 

## GAME_SETTING


> Search script examples for [GAME_SETTING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAME_SETTING)


Name | Data | Description
---- | ---- | -----------
[DAMSEL_STYLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAME_SETTING.DAMSEL_STYLE) | 0 | 
...check [game_settings.txt](game_data/game_settings.txt)... |  | 
[CROSSPROGRESS_AUTOSYNC](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GAME_SETTING.CROSSPROGRESS_AUTOSYNC) | 47 | 

## GHOST_BEHAVIOR


> Search script examples for [GHOST_BEHAVIOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR)



Name | Data | Description
---- | ---- | -----------
[SAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SAD) | GHOST_BEHAVIOR::SAD | 
[MEDIUM_SAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.MEDIUM_SAD) | GHOST_BEHAVIOR::MEDIUM_SAD | 
[MEDIUM_HAPPY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.MEDIUM_HAPPY) | GHOST_BEHAVIOR::MEDIUM_HAPPY | 
[SMALL_ANGRY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_ANGRY) | GHOST_BEHAVIOR::SMALL_ANGRY | 
[SMALL_SURPRISED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_SURPRISED) | GHOST_BEHAVIOR::SMALL_SURPRISED | 
[SMALL_SAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_SAD) | GHOST_BEHAVIOR::SMALL_SAD | 
[SMALL_HAPPY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=GHOST_BEHAVIOR.SMALL_HAPPY) | GHOST_BEHAVIOR::SMALL_HAPPY | 

## HUNDUNFLAGS


> Search script examples for [HUNDUNFLAGS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS)



Name | Data | Description
---- | ---- | -----------
[WILLMOVELEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.WILLMOVELEFT) | 1 | 
[BIRDHEADEMERGED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.BIRDHEADEMERGED) | 2 | 
[SNAKEHEADEMERGED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.SNAKEHEADEMERGED) | 4 | 
[TOPLEVELARENAREACHED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.TOPLEVELARENAREACHED) | 8 | 
[BIRDHEADSHOTLAST](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=HUNDUNFLAGS.BIRDHEADSHOTLAST) | 16 | 

## INPUTS


> Search script examples for [INPUTS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS)



Name | Data | Description
---- | ---- | -----------
[NONE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.NONE) | 0 | 
[JUMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.JUMP) | 1 | 
[WHIP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.WHIP) | 2 | 
[BOMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.BOMB) | 4 | 
[ROPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.ROPE) | 8 | 
[RUN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.RUN) | 16 | 
[DOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.DOOR) | 32 | 
[MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.MENU) | 64 | 
[JOURNAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.JOURNAL) | 128 | 
[LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.LEFT) | 256 | 
[RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.RIGHT) | 512 | 
[UP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.UP) | 1024 | 
[DOWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUTS.DOWN) | 2048 | 

## INPUT_FLAG


> Search script examples for [INPUT_FLAG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG)



Name | Data | Description
---- | ---- | -----------
[JUMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.JUMP) | 1 | 
[WHIP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.WHIP) | 2 | 
[BOMB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.BOMB) | 3 | 
[ROPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.ROPE) | 4 | 
[RUN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.RUN) | 5 | 
[DOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.DOOR) | 6 | 
[MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.MENU) | 7 | 
[JOURNAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.JOURNAL) | 8 | 
[LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.LEFT) | 9 | 
[RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.RIGHT) | 10 | 
[UP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.UP) | 11 | 
[DOWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=INPUT_FLAG.DOWN) | 12 | 

## JOURNALUI_PAGE_SHOWN


> Search script examples for [JOURNALUI_PAGE_SHOWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN)



Name | Data | Description
---- | ---- | -----------
[PLAYER_PROFILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.PLAYER_PROFILE) | 1 | 
[JOURNAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.JOURNAL) | 2 | 
[PLACES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.PLACES) | 3 | 
[PEOPLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.PEOPLE) | 4 | 
[BESTIARY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.BESTIARY) | 5 | 
[ITEMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.ITEMS) | 6 | 
[TRAPS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.TRAPS) | 7 | 
[STORY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.STORY) | 8 | 
[FEATS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.FEATS) | 9 | 
[RECAP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.RECAP) | 10 | 
[DEATH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_PAGE_SHOWN.DEATH) | 11 | 

## JOURNALUI_STATE


> Search script examples for [JOURNALUI_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE)



Name | Data | Description
---- | ---- | -----------
[INVISIBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.INVISIBLE) | 0 | 
[FADING_IN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FADING_IN) | 1 | 
[STABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.STABLE) | 2 | 
[FLIPPING_LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FLIPPING_LEFT) | 3 | 
[FLIPPING_RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FLIPPING_RIGHT) | 4 | 
[FADING_OUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNALUI_STATE.FADING_OUT) | 5 | 

## JOURNAL_PAGE_TYPE


> Search script examples for [JOURNAL_PAGE_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE)



Name | Data | Description
---- | ---- | -----------
[PROGRESS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.PROGRESS) | JournalPageType::Progress | 
[JOURNAL_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.JOURNAL_MENU) | JournalPageType::JournalMenu | 
[PLACES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.PLACES) | JournalPageType::Places | 
[PEOPLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.PEOPLE) | JournalPageType::People | 
[BESTIARY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.BESTIARY) | JournalPageType::Bestiary | 
[ITEMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.ITEMS) | JournalPageType::Items | 
[TRAPS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.TRAPS) | JournalPageType::Traps | 
[STORY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.STORY) | JournalPageType::Story | 
[FEATS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.FEATS) | JournalPageType::Feats | 
[DEATH_CAUSE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.DEATH_CAUSE) | JournalPageType::DeathCause | 
[DEATH_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.DEATH_MENU) | JournalPageType::DeathMenu | 
[RECAP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.RECAP) | JournalPageType::Recap | 
[PLAYER_PROFILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.PLAYER_PROFILE) | JournalPageType::PlayerProfile | 
[LAST_GAME_PLAYED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JOURNAL_PAGE_TYPE.LAST_GAME_PLAYED) | JournalPageType::LastGamePlayed | 

## JUNGLESISTERS


> Search script examples for [JUNGLESISTERS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS)

Jungle sister quest flags (angry = -1)

Name | Data | Description
---- | ---- | -----------
[PARSLEY_RESCUED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.PARSLEY_RESCUED) | 1 | 
[PARSNIP_RESCUED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.PARSNIP_RESCUED) | 2 | 
[PARMESAN_RESCUED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.PARMESAN_RESCUED) | 3 | 
[WARNING_ONE_WAY_DOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.WARNING_ONE_WAY_DOOR) | 4 | 
[GREAT_PARTY_HUH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.GREAT_PARTY_HUH) | 5 | 
[I_WISH_BROUGHT_A_JACKET](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=JUNGLESISTERS.I_WISH_BROUGHT_A_JACKET) | 6 | 

## LAYER


> Search script examples for [LAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER)



Name | Data | Description
---- | ---- | -----------
[FRONT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.FRONT) | 0 | 
[BACK](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.BACK) | 1 | 
[PLAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER) | -1 | 
[PLAYER1](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER1) | -1 | 
[PLAYER2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER2) | -2 | 
[PLAYER3](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER3) | -3 | 
[PLAYER4](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.PLAYER4) | -4 | 
[BOTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LAYER.BOTH) | -128 | 

## LEVEL_CONFIG


> Search script examples for [LEVEL_CONFIG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG)

Use with `get_level_config`

Name | Data | Description
---- | ---- | -----------
[BACK_ROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_CHANCE) | 0 | 
[BACK_ROOM_INTERCONNECTION_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_INTERCONNECTION_CHANCE) | 1 | 
[BACK_ROOM_HIDDEN_DOOR_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_HIDDEN_DOOR_CHANCE) | 2 | 
[BACK_ROOM_HIDDEN_DOOR_CACHE_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACK_ROOM_HIDDEN_DOOR_CACHE_CHANCE) | 3 | 
[MOUNT_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MOUNT_CHANCE) | 4 | 
[ALTAR_ROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.ALTAR_ROOM_CHANCE) | 5 | 
[IDOL_ROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.IDOL_ROOM_CHANCE) | 6 | 
[FLOOR_SIDE_SPREAD_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.FLOOR_SIDE_SPREAD_CHANCE) | 7 | 
[FLOOR_BOTTOM_SPREAD_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.FLOOR_BOTTOM_SPREAD_CHANCE) | 8 | 
[BACKGROUND_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.BACKGROUND_CHANCE) | 9 | 
[GROUND_BACKGROUND_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.GROUND_BACKGROUND_CHANCE) | 10 | 
[MACHINE_BIGROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_BIGROOM_CHANCE) | 11 | 
[MACHINE_WIDEROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_WIDEROOM_CHANCE) | 12 | 
[MACHINE_TALLROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_TALLROOM_CHANCE) | 13 | 
[MACHINE_REWARDROOM_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MACHINE_REWARDROOM_CHANCE) | 14 | 
[MAX_LIQUID_PARTICLES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.MAX_LIQUID_PARTICLES) | 15 | 
[FLAGGED_LIQUID_ROOMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LEVEL_CONFIG.FLAGGED_LIQUID_ROOMS) | 16 | 

## LIQUID_POOL


> Search script examples for [LIQUID_POOL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LIQUID_POOL)



Name | Data | Description
---- | ---- | -----------
[WATER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LIQUID_POOL.WATER) | 1 | 
[COARSE_WATER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LIQUID_POOL.COARSE_WATER) | 2 | 
[LAVA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LIQUID_POOL.LAVA) | 3 | 
[COARSE_LAVA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LIQUID_POOL.COARSE_LAVA) | 4 | 
[STAGNANT_LAVA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=LIQUID_POOL.STAGNANT_LAVA) | 5 | 

## MASK


> Search script examples for [MASK](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK)



Name | Data | Description
---- | ---- | -----------
[PLAYER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.PLAYER) | 0x1 | All CHAR_* entities, only `Player` type<br/>
[MOUNT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.MOUNT) | 0x2 | All MOUNT_* entities, only `Mount` type<br/>
[MONSTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.MONSTER) | 0x4 | All MONS_* entities, various types, all `Movable`<br/>
[ITEM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ITEM) | 0x8 | All ITEM_* entities except: ITEM_POWERUP_*, ITEM_ROPE, ITEM_CLIMBABLE_ROPE, ITEM_UNROLLED_ROPE, ITEM_RUBBLE, ITEM_FLAMETHROWER_FIREBALL, ITEM_CURSING_CLOUD<br/>Also includes: FX_JETPACKFLAME, FX_OLMECPART_FLOATER, FX_SMALLFLAME, FX_TELEPORTSHADOW<br/>Various types, all `Movable`<br/>
[EXPLOSION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.EXPLOSION) | 0x10 | Only: FX_EXPLOSION, FX_POWEREDEXPLOSION, FX_MODERNEXPLOSION<br/>All `Explosion` type<br/>
[ROPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ROPE) | 0x20 | Only: ITEM_ROPE, ITEM_CLIMBABLE_ROPE, ITEM_UNROLLED_ROPE<br/>Various types, all `Movable`<br/>
[FX](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.FX) | 0x40 | All FX_* entities except: FX_COMPASS, FX_SPECIALCOMPASS, FX_EXPLOSION, FX_POWEREDEXPLOSION, FX_MODERNEXPLOSION, FX_JETPACKFLAME, FX_OLMECPART_FLOATER, FX_SMALLFLAME, FX_TELEPORTSHADOW, FX_LEADER_FLAG, FX_PLAYERINDICATOR, FX_PLAYERINDICATORPORTRAIT<br/>Also includes: DECORATION_CHAINANDBLOCKS_CHAINDECORATION, DECORATION_SLIDINGWALL_CHAINDECORATION, ITEM_RUBBLE, ITEM_FLAMETHROWER_FIREBALL, ITEM_CURSING_CLOUD<br/>Various types, all `Movable`<br/>
[ACTIVEFLOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ACTIVEFLOOR) | 0x80 | All ACTIVEFLOOR_* entities<br/>Various types, all `Movable`<br/>
[FLOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.FLOOR) | 0x100 | All FLOOR_* and FLOORSTYLED_* entities<br/>Various types, all `Floor`<br/>
[DECORATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.DECORATION) | 0x200 | All DECORATION_* entities except: DECORATION_CHAINANDBLOCKS_CHAINDECORATION, DECORATION_SLIDINGWALL_CHAINDECORATION, DECORATION_PALACE_PORTRAIT<br/>Also includes: EMBED_GOLD, ENT_TYPE_EMBED_GOLD_BIG<br/>Various types, all `Entity`<br/>
[BG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.BG) | 0x400 | All MIDBG* entities and most of the BG_* entities<br/>does not include: a lot .. check [default_flags_more_flags.txt](internal/entity.type.default_flags_more_flags.txt) for full list<br/>Also includes: DECORATION_PALACE_PORTRAIT<br/>Various types, all `Entity`<br/>
[SHADOW](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.SHADOW) | 0x800 | All the BG_* entities excluded from `BG` (MASK.BG &#124 MASK.SHADOW) will get you all BG_* entities plus one extra decoration mentioned above<br/>Various types, all `Entity`<br/>
[LOGICAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.LOGICAL) | 0x1000 | All LOGICAL_* entities<br/>Also includes: ITEM_POWERUP_*, FX_COMPASS, FX_SPECIALCOMPASS, FX_LEADER_FLAG, FX_PLAYERINDICATOR, FX_PLAYERINDICATORPORTRAIT<br/>Various types, all `Entity`<br/>
[WATER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.WATER) | 0x2000 | Only: LIQUID_WATER, LIQUID_COARSE_WATER, LIQUID_IMPOSTOR_LAKE<br/>Various types, all `Entity`<br/>
[LAVA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.LAVA) | 0x4000 | Only: LIQUID_LAVA, LIQUID_STAGNANT_LAVA, LIQUID_IMPOSTOR_LAVA, LIQUID_COARSE_LAVA<br/>Various types, all `Entity`<br/>
[LIQUID](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.LIQUID) | 0x6000 | Short for ([MASK](#MASK).WATER &#124 [MASK](#MASK).LAVA)<br/>
[ANY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=MASK.ANY) | 0x0 | Value of 0, treated by all the functions as ANY mask<br/>

## ON


> Search script examples for [ON](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON)



Name | Data | Description
---- | ---- | -----------
[LOGO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOGO) | ON::LOGO | 
[INTRO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.INTRO) | ON::INTRO | 
[PROLOGUE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PROLOGUE) | ON::PROLOGUE | 
[TITLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TITLE) | ON::TITLE | 
[MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.MENU) | ON::MENU | 
[OPTIONS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.OPTIONS) | ON::OPTIONS | 
[PLAYER_PROFILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PLAYER_PROFILE) | ON::PLAYER_PROFILE | 
[LEADERBOARD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEADERBOARD) | ON::LEADERBOARD | 
[SEED_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SEED_INPUT) | ON::SEED_INPUT | 
[CHARACTER_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CHARACTER_SELECT) | ON::CHARACTER_SELECT | 
[TEAM_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TEAM_SELECT) | ON::TEAM_SELECT | 
[CAMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CAMP) | ON::CAMP | 
[LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LEVEL) | ON::LEVEL | 
[TRANSITION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TRANSITION) | ON::TRANSITION | 
[DEATH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH) | ON::DEATH | 
[SPACESHIP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPACESHIP) | ON::SPACESHIP | 
[WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.WIN) | ON::WIN | 
[CREDITS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CREDITS) | ON::CREDITS | 
[SCORES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCORES) | ON::SCORES | 
[CONSTELLATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.CONSTELLATION) | ON::CONSTELLATION | 
[RECAP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RECAP) | ON::RECAP | 
[ARENA_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MENU) | ON::ARENA_MENU | 
[ARENA_STAGES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_STAGES) | ON::ARENA_STAGES | 
[ARENA_ITEMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_ITEMS) | ON::ARENA_ITEMS | 
[ARENA_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_SELECT) | ON::ARENA_SELECT | 
[ARENA_INTRO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_INTRO) | ON::ARENA_INTRO | 
[ARENA_MATCH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_MATCH) | ON::ARENA_MATCH | 
[ARENA_SCORE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ARENA_SCORE) | ON::ARENA_SCORE | 
[ONLINE_LOADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOADING) | ON::ONLINE_LOADING | 
[ONLINE_LOBBY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.ONLINE_LOBBY) | ON::ONLINE_LOBBY | 
[GUIFRAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GUIFRAME) | ON::GUIFRAME | Params: [GuiDrawContext](#GuiDrawContext) draw_ctx<br/>Runs every frame the game is rendered, thus runs at selected framerate. Drawing functions are only available during this callback through a [GuiDrawContext](#GuiDrawContext)<br/>
[FRAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.FRAME) | ON::FRAME | Runs while playing the game while the player is controllable, not in the base camp or the arena mode<br/>
[GAMEFRAME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.GAMEFRAME) | ON::GAMEFRAME | Runs whenever the game engine is actively running. This includes base camp, arena, level transition and death screen<br/>
[SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCREEN) | ON::SCREEN | Runs whenever state.screen changes<br/>
[START](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.START) | ON::START | Runs on the first [ON](#ON).[SCREEN](#SCREEN) of a run<br/>
[LOADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOADING) | ON::LOADING | Runs whenever state.loading changes and is > 0. Prefer PRE/POST_LOAD_SCREEN instead though.<br/>
[RESET](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RESET) | ON::RESET | Runs when resetting a run<br/>
[SAVE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SAVE) | ON::SAVE | Params: [SaveContext](#SaveContext) save_ctx<br/>Runs at the same times as [ON](#ON).[SCREEN](#SCREEN), but receives the save_ctx<br/>
[LOAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.LOAD) | ON::LOAD | Params: [LoadContext](#LoadContext) load_ctx<br/>Runs as soon as your script is loaded, including reloads, then never again<br/>
[PRE_LOAD_LEVEL_FILES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_LEVEL_FILES) | ON::PRE_LOAD_LEVEL_FILES | Params: [PreLoadLevelFilesContext](#PreLoadLevelFilesContext) load_level_ctx<br/>Runs right before level files would be loaded<br/>
[PRE_LEVEL_GENERATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LEVEL_GENERATION) | ON::PRE_LEVEL_GENERATION | Runs before any level generation, no entities should exist at this point<br/>
[PRE_LOAD_SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_SCREEN) | ON::PRE_LOAD_SCREEN | Runs right before loading a new screen based on screen_next. Return true from callback to block the screen from loading.<br/>
[POST_ROOM_GENERATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_ROOM_GENERATION) | ON::POST_ROOM_GENERATION | Params: [PostRoomGenerationContext](#PostRoomGenerationContext) room_gen_ctx<br/>Runs right after all rooms are generated before entities are spawned<br/>
[POST_LEVEL_GENERATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LEVEL_GENERATION) | ON::POST_LEVEL_GENERATION | Runs right after level generation is done, before any entities are updated<br/>
[POST_LOAD_SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LOAD_SCREEN) | ON::POST_LOAD_SCREEN | Runs right after a screen is loaded, before rendering anything<br/>
[PRE_GET_RANDOM_ROOM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_GET_RANDOM_ROOM) | ON::PRE_GET_RANDOM_ROOM | Params: int x, int y, [LAYER](#LAYER) layer, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template<br/>Return: `string room_data`<br/>Called when the game wants to get a random room for a given template. Return a string that represents a room template to make the game use that.<br/>If the size of the string returned does not match with the room templates expected size the room is discarded.<br/>White spaces at the beginning and end of the string are stripped, not at the beginning and end of each line.<br/>
[PRE_HANDLE_ROOM_TILES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_HANDLE_ROOM_TILES) | ON::PRE_HANDLE_ROOM_TILES | Params: int x, int y, [ROOM_TEMPLATE](#ROOM_TEMPLATE) room_template, [PreHandleRoomTilesContext](#PreHandleRoomTilesContext) room_ctx<br/>Return: `bool last_callback` to determine whether callbacks of the same type should be executed after this<br/>Runs after a random room was selected and right before it would spawn entities for each tile code<br/>Allows you to modify the rooms content in the front and back layer as well as add a backlayer if not yet existant<br/>
[SCRIPT_ENABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_ENABLE) | ON::SCRIPT_ENABLE | 
[SCRIPT_DISABLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SCRIPT_DISABLE) | ON::SCRIPT_DISABLE | 
[RENDER_PRE_HUD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_HUD) | ON::RENDER_PRE_HUD | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs before the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx<br/>
[RENDER_POST_HUD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_HUD) | ON::RENDER_POST_HUD | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs after the HUD is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx<br/>
[RENDER_PRE_PAUSE_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_PAUSE_MENU) | ON::RENDER_PRE_PAUSE_MENU | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs before the pause menu is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx<br/>
[RENDER_POST_PAUSE_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_PAUSE_MENU) | ON::RENDER_POST_PAUSE_MENU | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx<br/>Runs after the pause menu is drawn on screen. In this event, you can draw textures with the `draw_screen_texture` function of the render_ctx<br/>
[RENDER_PRE_DRAW_DEPTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_PRE_DRAW_DEPTH) | ON::RENDER_PRE_DRAW_DEPTH | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int draw_depth<br/>Runs before the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx<br/>
[RENDER_POST_DRAW_DEPTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_DRAW_DEPTH) | ON::RENDER_POST_DRAW_DEPTH | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, int draw_depth<br/>Runs right after the entities of the specified draw_depth are drawn on screen. In this event, you can draw textures with the `draw_world_texture` function of the render_ctx<br/>
[RENDER_POST_JOURNAL_PAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.RENDER_POST_JOURNAL_PAGE) | ON::RENDER_POST_JOURNAL_PAGE | Params: [VanillaRenderContext](#VanillaRenderContext) render_ctx, [JOURNAL_PAGE_TYPE](#JOURNAL_PAGE_TYPE) page_type, [JournalPage](#JournalPage) page<br/>Runs after the journal page is drawn on screen. In this event, you can draw textures with the draw_screen_texture function of the [VanillaRenderContext](#VanillaRenderContext)<br/>The [JournalPage](#JournalPage) parameter gives you access to the specific fields of the page. Be sure to cast it to the correct type, the following functions are available to do that:<br/>`page:as_journal_page_progress()`<br/>`page:as_journal_page_journalmenu()`<br/>`page:as_journal_page_places()`<br/>`page:as_journal_page_people()`<br/>`page:as_journal_page_bestiary()`<br/>`page:as_journal_page_items()`<br/>`page:as_journal_page_traps()`<br/>`page:as_journal_page_story()`<br/>`page:as_journal_page_feats()`<br/>`page:as_journal_page_deathcause()`<br/>`page:as_journal_page_deathmenu()`<br/>`page:as_journal_page_recap()`<br/>`page:as_journal_page_playerprofile()`<br/>`page:as_journal_page_lastgameplayed()`<br/>
[SPEECH_BUBBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.SPEECH_BUBBLE) | ON::SPEECH_BUBBLE | Params: [Entity](#Entity) speaking_entity, string text<br/>Runs before any speech bubble is created, even the one using [say](#say) function<br/>Return: if you don't return anything it will execute the speech bubble function normally with default message<br/>if you return empty string, it will not create the speech bubble at all, if you return string, it will use that instead of the original<br/>The first script to return string (empty or not) will take priority, the rest will receive callback call but the return behavior won't matter<br/>
[TOAST](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.TOAST) | ON::TOAST | Params: string text<br/>Runs before any toast is created, even the one using [toast](#toast) function<br/>Return: if you don't return anything it will execute the toast function normally with default message<br/>if you return empty string, it will not create the toast at all, if you return string, it will use that instead of the original message<br/>The first script to return string (empty or not) will take priority, the rest will receive callback call but the return behavior won't matter<br/>
[DEATH_MESSAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.DEATH_MESSAGE) | ON::DEATH_MESSAGE | Params: [STRINGID](#Aliases) id<br/>Runs once after death when the death message journal page is shown. The parameter is the [STRINGID](#Aliases) of the title, like 1221 for BLOWN UP.<br/>
[PRE_LOAD_JOURNAL_CHAPTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_LOAD_JOURNAL_CHAPTER) | ON::PRE_LOAD_JOURNAL_CHAPTER | Params: [JOURNALUI_PAGE_SHOWN](#JOURNALUI_PAGE_SHOWN) chapter<br/>Runs before the journal or any of it's chapter is opened<br/>Return: return true to not load the chapter (or journal as a whole)<br/>
[POST_LOAD_JOURNAL_CHAPTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.POST_LOAD_JOURNAL_CHAPTER) | ON::POST_LOAD_JOURNAL_CHAPTER | Params: [JOURNALUI_PAGE_SHOWN](#JOURNALUI_PAGE_SHOWN) chapter, array:int pages<br/>Runs after the pages for the journal are prepared, but not yet displayed, `pages` is a list of page numbers that the game loaded, if you want to change it, do the changes (remove pages, add new ones, change order) and return it<br/>All new pages will be created as [JournalPageStory](#JournalPageStory), any custom with page number above 9 will be empty, I recommend using above 99 to be sure not to get the game page, you can later use this to recognise and render your own stuff on that page in the RENDER_POST_JOURNAL_PAGE<br/>Return: return new page array to modify the journal, returning empty array or not returning anything will load the journal normally, any page number that was aready loaded will result in the standard game page<br/>When changing the order of game pages make sure that the page that normally is rendered on the left side is on the left in the new order, otherwise you get some messed up result, custom pages don't have this problem. The order is: left, right, left, right ...<br/>
[PRE_GET_FEAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_GET_FEAT) | ON::PRE_GET_FEAT | Runs before getting performed status for a [FEAT](#Aliases) when rendering the Feats page in journal.<br/>Return: true to override the vanilla feat with your own. Defaults to Steam GetAchievement.<br/>
[PRE_SET_FEAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ON.PRE_SET_FEAT) | ON::PRE_SET_FEAT | Runs before the game sets a vanilla feat performed.<br/>Return: true to block the default behaviour of calling Steam SetAchievement.<br/>

## PARTICLEEMITTER


> Search script examples for [PARTICLEEMITTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER)


Name | Data | Description
---- | ---- | -----------
[TITLE_TORCHFLAME_SMOKE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.TITLE_TORCHFLAME_SMOKE) | 1 | 
...check [particle_emitters.txt](game_data/particle_emitters.txt)... |  | 
[MINIGAME_BROKENASTEROID_SMOKE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PARTICLEEMITTER.MINIGAME_BROKENASTEROID_SMOKE) | 219 | 

## PAUSEUI_VISIBILITY


> Search script examples for [PAUSEUI_VISIBILITY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY)



Name | Data | Description
---- | ---- | -----------
[INVISIBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.INVISIBLE) | 0 | 
[SLIDING_DOWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.SLIDING_DOWN) | 1 | 
[VISIBLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.VISIBLE) | 2 | 
[SLIDING_UP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PAUSEUI_VISIBILITY.SLIDING_UP) | 3 | 

## POS_TYPE


> Search script examples for [POS_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE)



Name | Data | Description
---- | ---- | -----------
[FLOOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.FLOOR) | POS_TYPE::FLOOR | On top of solid floor<br/>
[CEILING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.CEILING) | POS_TYPE::CEILING | Below solid ceiling<br/>
[AIR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.AIR) | POS_TYPE::AIR | Is a non-solid tile (no need to explicitly add this to everything)<br/>
[WALL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.WALL) | POS_TYPE::WALL | Next to a wall<br/>
[ALCOVE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.ALCOVE) | POS_TYPE::ALCOVE | Has a floor, ceiling and exactly one wall<br/>
[PIT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.PIT) | POS_TYPE::PIT | Has a floor, two walls and no ceiling<br/>
[HOLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.HOLE) | POS_TYPE::HOLE | Air pocket surrounded by floors<br/>
[WATER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.WATER) | POS_TYPE::WATER | Is in water (otherwise assumed not in water)<br/>
[LAVA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.LAVA) | POS_TYPE::LAVA | Is in lava (otherwise assumed not in lava)<br/>
[SAFE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.SAFE) | POS_TYPE::SAFE | Avoid hazards, like certain traps, shops and any special floor<br/>
[EMPTY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.EMPTY) | POS_TYPE::EMPTY | Has nothing but decoration and background in it<br/>
[SOLID](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.SOLID) | POS_TYPE::SOLID | Is inside solid floor or activefloor<br/>
[DEFAULT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.DEFAULT) | POS_TYPE::DEFAULT | FLOOR | SAFE | EMPTY<br/>
[WALL_LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.WALL_LEFT) | POS_TYPE::WALL_LEFT | Next to a wall on the left<br/>
[WALL_RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=POS_TYPE.WALL_RIGHT) | POS_TYPE::WALL_RIGHT | Next to a wall on the right<br/>

## PRNG_CLASS


> Search script examples for [PRNG_CLASS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS)

Determines what class of prng is used, which in turn determines which parts of the game's future prng is affected. See more info at [PRNG](#PRNG)<br/>For example when choosing `PRNG_CLASS.PROCEDURAL_SPAWNS` to generate a random number, random Tiamat spawns will not be affected.<br/>Any integer in the range [0, 9] is a valid class, some are however not documented because of missing information.

Name | Data | Description
---- | ---- | -----------
[PROCEDURAL_SPAWNS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.PROCEDURAL_SPAWNS) | PRNG::PROCEDURAL_SPAWNS | 
[PARTICLES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.PARTICLES) | PRNG::PARTICLES | 
[ENTITY_VARIATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.ENTITY_VARIATION) | PRNG::ENTITY_VARIATION | 
[EXTRA_SPAWNS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.EXTRA_SPAWNS) | PRNG::EXTRA_SPAWNS | 
[LEVEL_DECO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PRNG_CLASS.LEVEL_DECO) | PRNG::LEVEL_DECO | 

## PROCEDURAL_CHANCE


> Search script examples for [PROCEDURAL_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PROCEDURAL_CHANCE)


Name | Data | Description
---- | ---- | -----------
[ARROWTRAP_CHANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=PROCEDURAL_CHANCE.ARROWTRAP_CHANCE) | 0 | 
...check [spawn_chances.txt](game_data/spawn_chances.txt)... |  | 

## REPEAT_TYPE


> Search script examples for [REPEAT_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE)



Name | Data | Description
---- | ---- | -----------
[NO_REPEAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE.NO_REPEAT) | REPEAT_TYPE::NoRepeat | 
[LINEAR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE.LINEAR) | REPEAT_TYPE::Linear | 
[BACK_AND_FORTH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=REPEAT_TYPE.BACK_AND_FORTH) | REPEAT_TYPE::BackAndForth | 

## ROOM_TEMPLATE


> Search script examples for [ROOM_TEMPLATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE)


Name | Data | Description
---- | ---- | -----------
[SIDE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE.SIDE) | 0 | 
...check [room_templates.txt](game_data/room_templates.txt)... |  | 

## ROOM_TEMPLATE_TYPE


> Search script examples for [ROOM_TEMPLATE_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE)

Use in `define_room_template` to declare whether a room template has any special behavior

Name | Data | Description
---- | ---- | -----------
[NONE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.NONE) | 0 | 
[ENTRANCE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.ENTRANCE) | 1 | 
[EXIT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.EXIT) | 2 | 
[SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.SHOP) | 3 | 
[MACHINE_ROOM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=ROOM_TEMPLATE_TYPE.MACHINE_ROOM) | 4 | 

## SCREEN


> Search script examples for [SCREEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN)



Name | Data | Description
---- | ---- | -----------
[LOGO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.LOGO) | 0 | 
[INTRO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.INTRO) | 1 | 
[PROLOGUE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.PROLOGUE) | 2 | 
[TITLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.TITLE) | 3 | 
[MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.MENU) | 4 | 
[OPTIONS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.OPTIONS) | 5 | 
[PLAYER_PROFILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.PLAYER_PROFILE) | 6 | 
[LEADERBOARD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.LEADERBOARD) | 7 | 
[SEED_INPUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.SEED_INPUT) | 8 | 
[CHARACTER_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CHARACTER_SELECT) | 9 | 
[TEAM_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.TEAM_SELECT) | 10 | 
[CAMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CAMP) | 11 | 
[LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.LEVEL) | 12 | 
[TRANSITION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.TRANSITION) | 13 | 
[DEATH](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.DEATH) | 14 | 
[SPACESHIP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.SPACESHIP) | 15 | 
[WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.WIN) | 16 | 
[CREDITS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CREDITS) | 17 | 
[SCORES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.SCORES) | 18 | 
[CONSTELLATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.CONSTELLATION) | 19 | 
[RECAP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.RECAP) | 20 | 
[ARENA_MENU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_MENU) | 21 | 
[ARENA_STAGES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_STAGES) | 22 | 
[ARENA_ITEMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_ITEMS) | 23 | 
[ARENA_SELECT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_SELECT) | 24 | 
[ARENA_INTRO](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_INTRO) | 25 | 
[ARENA_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_LEVEL) | 26 | 
[ARENA_SCORE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ARENA_SCORE) | 27 | 
[ONLINE_LOADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ONLINE_LOADING) | 28 | 
[ONLINE_LOBBY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SCREEN.ONLINE_LOBBY) | 29 | 

## SHAPE


> Search script examples for [SHAPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHAPE)



Name | Data | Description
---- | ---- | -----------
[RECTANGLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHAPE.RECTANGLE) | SHAPE::RECTANGLE | 
[CIRCLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHAPE.CIRCLE) | SHAPE::CIRCLE | 

## SHOP_TYPE


> Search script examples for [SHOP_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE)

Determines which kind of shop spawns in the level, if any

Name | Data | Description
---- | ---- | -----------
[GENERAL_STORE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.GENERAL_STORE) | 0 | 
[CLOTHING_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.CLOTHING_SHOP) | 1 | 
[WEAPON_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.WEAPON_SHOP) | 2 | 
[SPECIALTY_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.SPECIALTY_SHOP) | 3 | 
[HIRED_HAND_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.HIRED_HAND_SHOP) | 4 | 
[PET_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.PET_SHOP) | 5 | 
[DICE_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.DICE_SHOP) | 6 | 
[HEDJET_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.HEDJET_SHOP) | 8 | 
[CURIO_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.CURIO_SHOP) | 9 | 
[CAVEMAN_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.CAVEMAN_SHOP) | 10 | 
[TURKEY_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.TURKEY_SHOP) | 11 | 
[GHIST_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.GHIST_SHOP) | 12 | 
[TUSK_DICE_SHOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SHOP_TYPE.TUSK_DICE_SHOP) | 13 | 

## SOUND_LOOP_MODE


> Search script examples for [SOUND_LOOP_MODE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE)

Paramater to `PlayingSound:set_looping()`, specifies what type of looping this sound should do

Name | Data | Description
---- | ---- | -----------
[OFF](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.OFF) | 0 | 
[LOOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.LOOP) | 1 | 
[BIDIRECTIONAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_LOOP_MODE.BIDIRECTIONAL) | 2 | 

## SOUND_TYPE


> Search script examples for [SOUND_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE)

Third parameter to `CustomSound:play()`, specifies which group the sound will be played in and thus how the player controls its volume

Name | Data | Description
---- | ---- | -----------
[SFX](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE.SFX) | 0 | 
[MUSIC](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SOUND_TYPE.MUSIC) | 1 | 

## SPARROW


> Search script examples for [SPARROW](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW)

Sparrow quest states

Name | Data | Description
---- | ---- | -----------
[ANGRY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.ANGRY) | -2 | 
[DEAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.DEAD) | -1 | 
[QUEST_NOT_STARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.QUEST_NOT_STARTED) | 0 | 
[THIEF_STATUS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.THIEF_STATUS) | 1 | 
[FINISHED_LEVEL_WITH_THIEF_STATUS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FINISHED_LEVEL_WITH_THIEF_STATUS) | 2 | 
[FIRST_HIDEOUT_SPAWNED_ROPE_THROW](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FIRST_HIDEOUT_SPAWNED_ROPE_THROW) | 3 | 
[FIRST_ENCOUNTER_ROPES_THROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FIRST_ENCOUNTER_ROPES_THROWN) | 4 | 
[TUSK_IDOL_STOLEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.TUSK_IDOL_STOLEN) | 5 | 
[SECOND_HIDEOUT_SPAWNED_NEOBAB](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.SECOND_HIDEOUT_SPAWNED_NEOBAB) | 6 | 
[SECOND_ENCOUNTER_INTERACTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.SECOND_ENCOUNTER_INTERACTED) | 7 | 
[MEETING_AT_TUSK_BASEMENT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.MEETING_AT_TUSK_BASEMENT) | 8 | 
[FINAL_REWARD_THROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPARROW.FINAL_REWARD_THROWN) | 9 | 

## SPAWN_TYPE


> Search script examples for [SPAWN_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE)



Name | Data | Description
---- | ---- | -----------
[LEVEL_GEN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN) | SPAWN_TYPE_LEVEL_GEN | For any spawn happening during level generation, even if the call happened from the Lua API during a tile code callback.<br/>
[LEVEL_GEN_TILE_CODE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_TILE_CODE) | SPAWN_TYPE_LEVEL_GEN_TILE_CODE | Similar to LEVEL_GEN but only triggers on tile code spawns.<br/>
[LEVEL_GEN_PROCEDURAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_PROCEDURAL) | SPAWN_TYPE_LEVEL_GEN_PROCEDURAL | Similar to LEVEL_GEN but only triggers on random level spawns, like snakes or bats.<br/>
[LEVEL_GEN_FLOOR_SPREADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_FLOOR_SPREADING) | SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING | Only procs during floor spreading, both horizontal and vertical<br/>
[LEVEL_GEN_GENERAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.LEVEL_GEN_GENERAL) | SPAWN_TYPE_LEVEL_GEN_GENERAL | Covers all spawns during level gen that are not covered by the other two.<br/>
[SCRIPT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.SCRIPT) | SPAWN_TYPE_SCRIPT | Runs for any spawn happening through a call from the Lua API, also during level generation.<br/>
[SYSTEMIC](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.SYSTEMIC) | SPAWN_TYPE_SYSTEMIC | Covers all other spawns, such as items from crates or the player throwing bombs.<br/>
[ANY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=SPAWN_TYPE.ANY) | SPAWN_TYPE_ANY | Covers all of the above.<br/>

## TEXTURE


> Search script examples for [TEXTURE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE)


Name | Data | Description
---- | ---- | -----------
[DATA_TEXTURES_PLACEHOLDER_0](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_PLACEHOLDER_0) | 0 | 
...check [textures.txt](game_data/textures.txt)... |  | 
[DATA_TEXTURES_SHINE_0](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_SHINE_0) | 388 | 
[DATA_TEXTURES_OLDTEXTURES_AI_0](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TEXTURE.DATA_TEXTURES_OLDTEXTURES_AI_0) | 389 | 

## THEME


> Search script examples for [THEME](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME)



Name | Data | Description
---- | ---- | -----------
[DWELLING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.DWELLING) | 1 | 
[JUNGLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.JUNGLE) | 2 | 
[VOLCANA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.VOLCANA) | 3 | 
[OLMEC](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.OLMEC) | 4 | 
[TIDE_POOL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TIDE_POOL) | 5 | 
[TEMPLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TEMPLE) | 6 | 
[ICE_CAVES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ICE_CAVES) | 7 | 
[NEO_BABYLON](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.NEO_BABYLON) | 8 | 
[SUNKEN_CITY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.SUNKEN_CITY) | 9 | 
[COSMIC_OCEAN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.COSMIC_OCEAN) | 10 | 
[CITY_OF_GOLD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.CITY_OF_GOLD) | 11 | 
[DUAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.DUAT) | 12 | 
[ABZU](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ABZU) | 13 | 
[TIAMAT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.TIAMAT) | 14 | 
[EGGPLANT_WORLD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.EGGPLANT_WORLD) | 15 | 
[HUNDUN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.HUNDUN) | 16 | 
[BASE_CAMP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.BASE_CAMP) | 17 | 
[ARENA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME.ARENA) | 18 | 

## THEME_OVERRIDE


> Search script examples for [THEME_OVERRIDE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE)

Overrides for different [CustomTheme](#CustomTheme) functions. Warning: We WILL change these, especially the unknown ones, and even the known ones if they turn out wrong in testing.

Name | Data | Description
---- | ---- | -----------
[BASE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.BASE) | THEME_OVERRIDE::BASE | 
[UNKNOWN_V1](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V1) | THEME_OVERRIDE::UNKNOWN_V1 | 
[INIT_FLAGS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.INIT_FLAGS) | THEME_OVERRIDE::INIT_FLAGS | 
[INIT_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.INIT_LEVEL) | THEME_OVERRIDE::INIT_LEVEL | 
[UNKNOWN_V4](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V4) | THEME_OVERRIDE::UNKNOWN_V4 | 
[UNKNOWN_V5](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V5) | THEME_OVERRIDE::UNKNOWN_V5 | 
[SPECIAL_ROOMS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPECIAL_ROOMS) | THEME_OVERRIDE::SPECIAL_ROOMS | 
[UNKNOWN_V7](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V7) | THEME_OVERRIDE::UNKNOWN_V7 | 
[UNKNOWN_V8](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V8) | THEME_OVERRIDE::UNKNOWN_V8 | 
[VAULT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.VAULT) | THEME_OVERRIDE::VAULT | 
[COFFIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.COFFIN) | THEME_OVERRIDE::COFFIN | 
[FEELING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.FEELING) | THEME_OVERRIDE::FEELING | 
[UNKNOWN_V12](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V12) | THEME_OVERRIDE::UNKNOWN_V12 | 
[SPAWN_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_LEVEL) | THEME_OVERRIDE::SPAWN_LEVEL | 
[SPAWN_BORDER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_BORDER) | THEME_OVERRIDE::SPAWN_BORDER | 
[POST_PROCESS_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.POST_PROCESS_LEVEL) | THEME_OVERRIDE::POST_PROCESS_LEVEL | 
[SPAWN_TRAPS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_TRAPS) | THEME_OVERRIDE::SPAWN_TRAPS | 
[POST_PROCESS_ENTITIES](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.POST_PROCESS_ENTITIES) | THEME_OVERRIDE::POST_PROCESS_ENTITIES | 
[SPAWN_PROCEDURAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_PROCEDURAL) | THEME_OVERRIDE::SPAWN_PROCEDURAL | 
[SPAWN_BACKGROUND](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_BACKGROUND) | THEME_OVERRIDE::SPAWN_BACKGROUND | 
[SPAWN_LIGHTS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_LIGHTS) | THEME_OVERRIDE::SPAWN_LIGHTS | 
[SPAWN_TRANSITION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_TRANSITION) | THEME_OVERRIDE::SPAWN_TRANSITION | 
[POST_TRANSITION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.POST_TRANSITION) | THEME_OVERRIDE::POST_TRANSITION | 
[SPAWN_PLAYERS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_PLAYERS) | THEME_OVERRIDE::SPAWN_PLAYERS | 
[SPAWN_EFFECTS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_EFFECTS) | THEME_OVERRIDE::SPAWN_EFFECTS | 
[LVL_FILE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.LVL_FILE) | THEME_OVERRIDE::LVL_FILE | 
[THEME_ID](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.THEME_ID) | THEME_OVERRIDE::THEME_ID | 
[BASE_ID](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.BASE_ID) | THEME_OVERRIDE::BASE_ID | 
[ENT_FLOOR_SPREADING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.ENT_FLOOR_SPREADING) | THEME_OVERRIDE::ENT_FLOOR_SPREADING | 
[ENT_FLOOR_SPREADING2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.ENT_FLOOR_SPREADING2) | THEME_OVERRIDE::ENT_FLOOR_SPREADING2 | 
[UNKNOWN_V30](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V30) | THEME_OVERRIDE::UNKNOWN_V30 | 
[TRANSITION_MODIFIER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.TRANSITION_MODIFIER) | THEME_OVERRIDE::TRANSITION_MODIFIER | 
[UNKNOWN_V32](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V32) | THEME_OVERRIDE::UNKNOWN_V32 | 
[ENT_BACKWALL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.ENT_BACKWALL) | THEME_OVERRIDE::ENT_BACKWALL | 
[ENT_BORDER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.ENT_BORDER) | THEME_OVERRIDE::ENT_BORDER | 
[ENT_CRITTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.ENT_CRITTER) | THEME_OVERRIDE::ENT_CRITTER | 
[GRAVITY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.GRAVITY) | THEME_OVERRIDE::GRAVITY | 
[PLAYER_DAMAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.PLAYER_DAMAGE) | THEME_OVERRIDE::PLAYER_DAMAGE | 
[UNKNOWN_V38](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V38) | THEME_OVERRIDE::UNKNOWN_V38 | 
[TEXTURE_BACKLAYER_LUT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.TEXTURE_BACKLAYER_LUT) | THEME_OVERRIDE::TEXTURE_BACKLAYER_LUT | 
[BACKLAYER_LIGHT_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.BACKLAYER_LIGHT_LEVEL) | THEME_OVERRIDE::BACKLAYER_LIGHT_LEVEL | 
[LOOP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.LOOP) | THEME_OVERRIDE::LOOP | 
[VAULT_LEVEL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.VAULT_LEVEL) | THEME_OVERRIDE::VAULT_LEVEL | 
[GET_UNKNOWN1_OR_2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.GET_UNKNOWN1_OR_2) | THEME_OVERRIDE::GET_UNKNOWN1_OR_2 | 
[TEXTURE_DYNAMIC](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.TEXTURE_DYNAMIC) | THEME_OVERRIDE::TEXTURE_DYNAMIC | 
[PRE_TRANSITION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.PRE_TRANSITION) | THEME_OVERRIDE::PRE_TRANSITION | 
[LEVEL_HEIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.LEVEL_HEIGHT) | THEME_OVERRIDE::LEVEL_HEIGHT | 
[UNKNOWN_V47](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V47) | THEME_OVERRIDE::UNKNOWN_V47 | 
[SPAWN_DECORATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_DECORATION) | THEME_OVERRIDE::SPAWN_DECORATION | 
[SPAWN_DECORATION2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_DECORATION2) | THEME_OVERRIDE::SPAWN_DECORATION2 | 
[SPAWN_EXTRA](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.SPAWN_EXTRA) | THEME_OVERRIDE::SPAWN_EXTRA | 
[UNKNOWN_V51](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=THEME_OVERRIDE.UNKNOWN_V51) | THEME_OVERRIDE::UNKNOWN_V51 | 

## TILE_CODE


> Search script examples for [TILE_CODE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TILE_CODE)


Name | Data | Description
---- | ---- | -----------
[EMPTY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TILE_CODE.EMPTY) | 0 | 
...check [tile_codes.txt](game_data/tile_codes.txt)... |  | 

## TUSK


> Search script examples for [TUSK](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK)

Madame Tusk quest states

Name | Data | Description
---- | ---- | -----------
[ANGRY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.ANGRY) | -2 | 
[DEAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.DEAD) | -1 | 
[QUEST_NOT_STARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.QUEST_NOT_STARTED) | 0 | 
[DICE_HOUSE_SPAWNED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.DICE_HOUSE_SPAWNED) | 1 | 
[HIGH_ROLLER_STATUS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.HIGH_ROLLER_STATUS) | 2 | 
[PALACE_WELCOME_MESSAGE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=TUSK.PALACE_WELCOME_MESSAGE) | 3 | 

## VANHORSING


> Search script examples for [VANHORSING](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING)

Van Horsing quest states

Name | Data | Description
---- | ---- | -----------
[QUEST_NOT_STARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.QUEST_NOT_STARTED) | 0 | 
[JAILCELL_SPAWNED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.JAILCELL_SPAWNED) | 1 | 
[FIRST_ENCOUNTER_DIAMOND_THROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.FIRST_ENCOUNTER_DIAMOND_THROWN) | 2 | 
[SPAWNED_IN_VLADS_CASTLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.SPAWNED_IN_VLADS_CASTLE) | 3 | 
[SHOT_VLAD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.SHOT_VLAD) | 4 | 
[TEMPLE_HIDEOUT_SPAWNED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.TEMPLE_HIDEOUT_SPAWNED) | 5 | 
[SECOND_ENCOUNTER_COMPASS_THROWN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.SECOND_ENCOUNTER_COMPASS_THROWN) | 6 | 
[TUSK_CELLAR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANHORSING.TUSK_CELLAR) | 7 | 

## VANILLA_FONT_STYLE


> Search script examples for [VANILLA_FONT_STYLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_FONT_STYLE)

Used in the `render_ctx:draw_text` and `render_ctx:draw_text_size` functions of the ON.RENDER_PRE/POST_xxx event<br/>There are more styles, we just didn't name them all

Name | Data | Description
---- | ---- | -----------
[NORMAL](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_FONT_STYLE.NORMAL) | 0 | 
[ITALIC](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_FONT_STYLE.ITALIC) | 1 | 
[BOLD](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_FONT_STYLE.BOLD) | 2 | 

## VANILLA_SOUND


> Search script examples for [VANILLA_SOUND](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND)


Name | Data | Description
---- | ---- | -----------
[BGM_BGM_TITLE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.BGM_BGM_TITLE) | BGM/BGM_title | 
...check [vanilla_sounds.txt](game_data/vanilla_sounds.txt)... |  | 
[FX_FX_DM_BANNER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND.FX_FX_DM_BANNER) | FX/FX_dm_banner | 

## VANILLA_SOUND_CALLBACK_TYPE


> Search script examples for [VANILLA_SOUND_CALLBACK_TYPE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE)

Bitmask parameter to `set_vanilla_sound_callback()`

Name | Data | Description
---- | ---- | -----------
[CREATED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.CREATED) | FMODStudio::EventCallbackType::Created | Params: `PlayingSound vanilla_sound`<br/>
[DESTROYED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.DESTROYED) | FMODStudio::EventCallbackType::Destroyed | Params: `PlayingSound vanilla_sound`<br/>
[STARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.STARTED) | FMODStudio::EventCallbackType::Started | Params: `PlayingSound vanilla_sound`<br/>
[RESTARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.RESTARTED) | FMODStudio::EventCallbackType::Restarted | Params: `PlayingSound vanilla_sound`<br/>
[STOPPED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.STOPPED) | FMODStudio::EventCallbackType::Stopped | Params: `PlayingSound vanilla_sound`<br/>
[START_FAILED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_CALLBACK_TYPE.START_FAILED) | FMODStudio::EventCallbackType::StartFailed | Params: `PlayingSound vanilla_sound`<br/>

## VANILLA_SOUND_PARAM


> Search script examples for [VANILLA_SOUND_PARAM](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM)


Name | Data | Description
---- | ---- | -----------
[POS_SCREEN_X](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.POS_SCREEN_X) | 0 | 
...check [vanilla_sound_params.txt](game_data/vanilla_sound_params.txt)... |  | 
[CURRENT_LAYER2](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_SOUND_PARAM.CURRENT_LAYER2) | 37 | 

## VANILLA_TEXT_ALIGNMENT


> Search script examples for [VANILLA_TEXT_ALIGNMENT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT)

Used in the `render_ctx:draw_text` and `render_ctx:draw_text_size` functions of the ON.RENDER_PRE/POST_xxx event

Name | Data | Description
---- | ---- | -----------
[LEFT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT.LEFT) | 0 | 
[CENTER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT.CENTER) | 1 | 
[RIGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=VANILLA_TEXT_ALIGNMENT.RIGHT) | 2 | 

## WIN_STATE


> Search script examples for [WIN_STATE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE)

After setting the [WIN_STATE](#WIN_STATE), the exit door on the current level will lead to the chosen ending

Name | Data | Description
---- | ---- | -----------
[NO_WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.NO_WIN) | 0 | 
[TIAMAT_WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.TIAMAT_WIN) | 1 | 
[HUNDUN_WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.HUNDUN_WIN) | 2 | 
[COSMIC_OCEAN_WIN](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WIN_STATE.COSMIC_OCEAN_WIN) | 3 | 

## WORLD_SHADER


> Search script examples for [WORLD_SHADER](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER)



Name | Data | Description
---- | ---- | -----------
[COLOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.COLOR) | WorldShader::Colors | Renders a solid color<br/>
[TEXTURE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.TEXTURE) | WorldShader::Texture | Renders a texture without applying the given color<br/>
[TEXTURE_ALPHA_COLOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.TEXTURE_ALPHA_COLOR) | WorldShader::TextureAlphaColor | Renders a texture by interpreting its red channel as alpha and applying the given color<br/>
[TEXTURE_COLOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.TEXTURE_COLOR) | WorldShader::TextureColor | The default shader to be used, just renders a texture with transparancy and the given color<br/>
[TEXTURE_COLORS_WARP](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.TEXTURE_COLORS_WARP) | WorldShader::TextureColorsWarp | Renders the texture, with "gamma correction" of the color channels and multiplying everything by the input color alpha only<br/>
[DEFERRED_COLOR_TRANSPARENT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_COLOR_TRANSPARENT) | WorldShader::DeferredColorTransparent | Basically same as COLOR but goes through the deferred pipeline<br/>
[DEFERRED_TEXTURE_COLOR](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR) | WorldShader::DeferredTextureColor | Basically same as TEXTURE_COLOR but goes through the deferred pipeline<br/>
[DEFERRED_TEXTURE_COLOR_POISONED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_POISONED) | WorldShader::DeferredTextureColor_Poisoned | Same as DEFERRED_TEXTURE_COLOR but applies poison color effect<br/>
[DEFERRED_TEXTURE_COLOR_CURSED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_CURSED) | WorldShader::DeferredTextureColor_Cursed | Same as DEFERRED_TEXTURE_COLOR but applies cursed color effect<br/>
[DEFERRED_TEXTURE_COLOR_POISONED_CURSED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_POISONED_CURSED) | WorldShader::DeferredTextureColor_PoisonedCursed | Same as DEFERRED_TEXTURE_COLOR but applies poisoned and cursed color effect<br/>
[DEFERRED_TEXTURE_COLOR_TRANSPARENT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_TRANSPARENT) | WorldShader::DeferredTextureColor_Transparent | Basically same as DEFERRED_TEXTURE_COLOR<br/>
[DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED) | WorldShader::DeferredTextureColor_TransparentCorrected | Same as DEFERRED_TEXTURE_COLOR_TRANSPARENT but applies gamma correction to alpha channel<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE) | WorldShader::DeferredTextureColor_Emissive | Same as DEFERRED_TEXTURE_COLOR but renders to the emissive channel<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW) | WorldShader::DeferredTextureColor_EmissiveGlow | Same as DEFERRED_TEXTURE_COLOR but renders to the emissive channel with glow<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY) | WorldShader::DeferredTextureColor_EmissiveGlowHeavy | Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW but renders to the emissive channel with heavy glow<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS) | WorldShader::DeferredTextureColor_EmissiveGlowBrightness | Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY but renders glow on top of the texture<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW) | WorldShader::DeferredTextureColor_EmissiveColorizedGlow | Same as DEFERRED_TEXTURE_COLOR but renders heavy glow behind the texture<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW) | WorldShader::DeferredTextureColor_EmissiveColorizedGlow_DynamicGlow | Basically same as DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW<br/>
[DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=WORLD_SHADER.DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION) | WorldShader::DeferredTextureColor_EmissiveColorizedGlow_Saturation | Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW but renders texture as solid color<br/>

## YANG


> Search script examples for [YANG](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG)

[Yang](#Yang) quest states

Name | Data | Description
---- | ---- | -----------
[ANGRY](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.ANGRY) | -1 | 
[QUEST_NOT_STARTED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.QUEST_NOT_STARTED) | 0 | 
[TURKEY_PEN_SPAWNED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.TURKEY_PEN_SPAWNED) | 2 | 
[BOTH_TURKEYS_DELIVERED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.BOTH_TURKEYS_DELIVERED) | 3 | 
[TURKEY_SHOP_SPAWNED](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.TURKEY_SHOP_SPAWNED) | 4 | 
[ONE_TURKEY_BOUGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.ONE_TURKEY_BOUGHT) | 5 | 
[TWO_TURKEYS_BOUGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.TWO_TURKEYS_BOUGHT) | 6 | 
[THREE_TURKEYS_BOUGHT](https://github.com/spelunky-fyi/overlunky/search?l=Lua&q=YANG.THREE_TURKEYS_BOUGHT) | 7 | 
