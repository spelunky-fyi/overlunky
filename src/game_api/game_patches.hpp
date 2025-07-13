#pragma once

#include <cstdint>
#include <vector>

#include "aliases.hpp"

void patch_orbs_limit();
void patch_olmec_kill_crash();
void patch_liquid_OOB();
void set_skip_olmec_cutscene(bool skip);
void patch_tiamat_kill_crash();
void set_skip_tiamat_cutscene(bool skip);
void patch_ushabti_error();
void patch_entering_closed_door_crash();

void modify_sparktraps(float angle_increment = 0.015, float distance = 3.0);
float* get_sparktraps_parameters_ptr(); // for UI
void activate_sparktraps_hack(bool activate);
void set_storage_layer(LAYER layer);
void set_kapala_blood_threshold(uint8_t threshold);
void set_kapala_hud_icon(int8_t icon_index);
void set_olmec_phase_y_level(uint8_t phase, float y);
void force_olmec_phase_0(bool b);
void set_ghost_spawn_times(uint32_t normal = 10800, uint32_t cursed = 9000);
void set_time_ghost_enabled(bool b);
void set_time_jelly_enabled(bool b);
void set_camp_camera_bounds_enabled(bool b);
void set_explosion_mask(int32_t mask);
void set_max_rope_length(uint8_t length);
void change_sunchallenge_spawns(std::vector<ENT_TYPE> ent_types);
void change_diceshop_prizes(std::vector<ENT_TYPE> ent_types);
void change_altar_damage_spawns(std::vector<ENT_TYPE> ent_types);
void change_waddler_drop(std::vector<ENT_TYPE> ent_types);
void modify_ankh_health_gain(uint8_t max_health, uint8_t beat_add_health);
void change_poison_timer(int16_t frames);
bool disable_floor_embeds(bool disable);
void set_cursepot_ghost_enabled(bool enable);
void set_ending_unlock(ENT_TYPE type);
void activate_tiamat_position_hack(bool activate);
void activate_crush_elevator_hack(bool activate);
void activate_hundun_hack(bool activate);
void set_boss_door_control_enabled(bool enable);
void set_level_logic_enabled(bool enable);
void set_camera_layer_control_enabled(bool enable);
void set_start_level_paused(bool enable);
void set_liquid_layer(LAYER l);
