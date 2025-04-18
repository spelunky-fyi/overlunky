#include "game_patches_lua.hpp"

#include <sol/sol.hpp>

#include "game_patches.hpp"

namespace NGamePatches
{
void register_usertypes(sol::state& lua)
{
    /// Sets the amount of blood drops in the Kapala needed to trigger a health increase (default = 7).
    lua["set_kapala_blood_threshold"] = set_kapala_blood_threshold;
    /// Sets the hud icon for the Kapala (0-6 ; -1 for default behaviour).
    /// If you set a Kapala treshold greater than 7, make sure to set the hud icon in the range 0-6, or other icons will appear in the hud!
    lua["set_kapala_hud_icon"] = set_kapala_hud_icon;
    /// Changes characteristics of (all) sparktraps: speed, rotation direction and distance from center
    /// Speed: expressed as the amount that should be added to the angle every frame (use a negative number to go in the other direction)
    /// Distance from center: if you go above 3.0 the game might crash because a spark may go out of bounds!
    lua["modify_sparktraps"] = modify_sparktraps;
    /// Activate custom variables for speed and distance in the `ITEM_SPARK`
    /// note: because those the variables are custom and game does not initiate them, you need to do it yourself for each spark, recommending `set_post_entity_spawn`
    /// default game values are: speed = -0.015, distance = 3.0
    lua["activate_sparktraps_hack"] = activate_sparktraps_hack;
    /// Set layer to search for storage items on
    lua["set_storage_layer"] = set_storage_layer;
    /// Sets the Y-level at which Olmec changes phases
    lua["set_olmec_phase_y_level"] = set_olmec_phase_y_level;
    /// Forces Olmec to stay on phase 0 (stomping)
    lua["force_olmec_phase_0"] = force_olmec_phase_0;
    /// Determines when the ghost appears, either when the player is cursed or not
    lua["set_ghost_spawn_times"] = set_ghost_spawn_times;
    /// Determines whether the ghost appears when breaking the ghost pot
    lua["set_cursepot_ghost_enabled"] = set_cursepot_ghost_enabled;
    /// Determines whether the time ghost appears, including the showing of the ghost toast
    lua["set_time_ghost_enabled"] = set_time_ghost_enabled;
    /// Determines whether the time jelly appears in cosmic ocean
    lua["set_time_jelly_enabled"] = set_time_jelly_enabled;
    /// Enables or disables the default position based camp camera bounds, to set them manually yourself
    lua["set_camp_camera_bounds_enabled"] = set_camp_camera_bounds_enabled;
    /// Sets which entities are affected by a bomb explosion. Default = MASK.PLAYER | MASK.MOUNT | MASK.MONSTER | MASK.ITEM | MASK.ACTIVEFLOOR | MASK.FLOOR
    lua["set_explosion_mask"] = set_explosion_mask;
    /// Sets the maximum length of a thrown rope (anchor segment not included). Unfortunately, setting this higher than default (6) creates visual glitches in the rope, even though it is fully functional.
    lua["set_max_rope_length"] = set_max_rope_length;
    /// Change ENT_TYPE's spawned by `FLOOR_SUNCHALLENGE_GENERATOR`, by default there are 4:<br/>
    /// {MONS_WITCHDOCTOR, MONS_VAMPIRE, MONS_SORCERESS, MONS_NECROMANCER}<br/>
    /// Use empty table as argument to reset to the game default
    lua["change_sunchallenge_spawns"] = change_sunchallenge_spawns;
    /// Change ENT_TYPE's spawned in dice shops (Madame Tusk as well), by default there are 25:<br/>
    /// {ITEM_PICKUP_BOMBBAG, ITEM_PICKUP_BOMBBOX, ITEM_PICKUP_ROPEPILE, ITEM_PICKUP_COMPASS, ITEM_PICKUP_PASTE, ITEM_PICKUP_PARACHUTE, ITEM_PURCHASABLE_CAPE, ITEM_PICKUP_SPECTACLES, ITEM_PICKUP_CLIMBINGGLOVES, ITEM_PICKUP_PITCHERSMITT,
    /// ENT_TYPE_ITEM_PICKUP_SPIKESHOES, ENT_TYPE_ITEM_PICKUP_SPRINGSHOES, ITEM_MACHETE, ITEM_BOOMERANG, ITEM_CROSSBOW, ITEM_SHOTGUN, ITEM_FREEZERAY, ITEM_WEBGUN, ITEM_CAMERA, ITEM_MATTOCK, ITEM_PURCHASABLE_JETPACK, ITEM_PURCHASABLE_HOVERPACK,
    /// ITEM_TELEPORTER, ITEM_PURCHASABLE_TELEPORTER_BACKPACK, ITEM_PURCHASABLE_POWERPACK}<br/>
    /// Min 6, Max 255, if you want less then 6 you need to write some of them more then once (they will have higher "spawn chance").
    /// If you use this function in the level with dice shop in it, you have to update `item_ids` in the [ITEM_DICE_PRIZE_DISPENSER](#PrizeDispenser).
    /// Use empty table as argument to reset to the game default
    lua["change_diceshop_prizes"] = change_diceshop_prizes;
    /// Change ENT_TYPE's spawned when you damage the altar, by default there are 6:<br/>
    /// {MONS_BAT, MONS_BEE, MONS_SPIDER, MONS_JIANGSHI, MONS_FEMALE_JIANGSHI, MONS_VAMPIRE}<br/>
    /// Max 255 types.
    /// Use empty table as argument to reset to the game default
    lua["change_altar_damage_spawns"] = change_altar_damage_spawns;
    /// Change ENT_TYPE's spawned when Waddler dies, by default there are 3:<br/>
    /// {ITEM_PICKUP_COMPASS, ITEM_CHEST, ITEM_KEY}<br/>
    /// Max 255 types.
    /// Use empty table as argument to reset to the game default
    lua["change_waddler_drop"] = change_waddler_drop;
    /// Change how much health the ankh gives you after death, with every beat (the heart beat effect) it will add `beat_add_health` to your health,
    /// `beat_add_health` has to be divisor of `health` and can't be 0, otherwise the function does nothing. Set `health` to 0 to return to the game defaults
    /// If you set `health` above the game max health it will be forced down to the game max
    lua["modify_ankh_health_gain"] = modify_ankh_health_gain;
    /// Change the amount of frames after the damage from poison is applied
    lua["change_poison_timer"] = change_poison_timer;
    /// Disable all crust item spawns, returns whether they were already disabled before the call
    lua["disable_floor_embeds"] = disable_floor_embeds;
    /// Force the character unlocked in either ending to ENT_TYPE. Set to 0 to reset to the default guys. Does not affect the texture of the actual savior. (See example)
    lua["set_ending_unlock"] = set_ending_unlock;
    /// Olmec cutscene moves Olmec and destroys the four floor tiles, so those things never happen if the cutscene is disabled, and Olmec will spawn on even ground. More useful for level gen mods, where the cutscene doesn't make sense. You can also set olmec_cutscene.timer to the last frame (809) to skip to the end, with Olmec in the hole.
    lua["set_olmec_cutscene_enabled"] = [](bool enable)
    { set_skip_olmec_cutscene(!enable); };
    /// Tiamat cutscene is also responsible for locking the exit door, so you may need to close it yourself if you still want Tiamat kill to be required
    lua["set_tiamat_cutscene_enabled"] = [](bool enable)
    { set_skip_tiamat_cutscene(!enable); };
    /// Activate custom variables for position used for detecting the player (normally hardcoded)
    /// note: because those variables are custom and game does not initiate them, you need to do it yourself for each Tiamat entity, recommending set_post_entity_spawn
    /// default game values are: attack_x = 17.5 attack_y = 62.5
    lua["activate_tiamat_position_hack"] = activate_tiamat_position_hack;
    /// Activate custom variables for speed and y coordinate limit for crushing elevator
    /// note: because those variables are custom and game does not initiate them, you need to do it yourself for each CrushElevator entity, recommending set_post_entity_spawn
    /// default game values are: speed = 0.0125, y_limit = 98.5
    lua["activate_crush_elevator_hack"] = activate_crush_elevator_hack;
    /// Activate custom variables for y coordinate limit for hundun and spawn of it's heads
    /// note: because those variables are custom and game does not initiate them, you need to do it yourself for each Hundun entity, recommending set_post_entity_spawn
    /// default game value are: y_limit = 98.5, rising_speed_x = 0, rising_speed_y = 0.0125, bird_head_spawn_y = 55, snake_head_spawn_y = 71
    lua["activate_hundun_hack"] = activate_hundun_hack;
    /// Allows you to disable the control over the door for Hundun and Tiamat
    /// This will also prevent game crashing when there is no exit door when they are in level
    lua["set_boss_door_control_enabled"] = set_boss_door_control_enabled;
    /// Setting to false disables all player logic in SCREEN.LEVEL, mainly the death screen from popping up if all players are dead or missing, but also shop camera zoom and some other small things.
    lua["set_level_logic_enabled"] = set_level_logic_enabled;
    /// Setting to true will stop the state update from unpausing after a screen load, leaving you with state.pause == PAUSE.FADE on the first frame to do what you want.
    lua["set_start_level_paused"] = set_start_level_paused;
    /// This disables the `state.camera_layer` to be forced to the `(leader player).layer` and setting of the `state.layer_transition_timer` & `state.transition_to_layer` when player enters layer door.
    /// Letting you control those manually.
    /// Look at the example on how to mimic game layer switching behavior
    lua["set_camera_layer_control_enabled"] = set_camera_layer_control_enabled;
    /// Change layer at which the liquid spawns in, THIS FUNCTION NEEDS TO BE CALLED BEFORE THE LEVEL IS BUILD, otherwise collisions and other stuff will be wrong for the newly spawned liquid
    /// This sadly also makes lavamanders extinct, since the logic for their spawn is hardcoded to front layer with bunch of other unrelated stuff (you can still spawn them with script or place them directly in level files)
    /// Everything should be working more or less correctly (report on community discord if you find something unusual)
    lua["set_liquid_layer"] = set_liquid_layer;
}
}; // namespace NGamePatches
