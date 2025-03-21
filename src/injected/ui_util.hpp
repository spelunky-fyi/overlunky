#pragma once

#include <cmath>       // for INFINITY
#include <cstdint>     // for uint32_t, uint8_t, int32_t, uint16_t
#include <optional>    // for optional
#include <string_view> // for string_view
#include <utility>     // for pair
#include <vector>      // for vector

#include "aliases.hpp" // for ENT_TYPE, LAYER
#include "color.hpp"   // for Color

class Player;
class Entity;
class SparkTrap;
struct SaveData;
struct Illumination;
struct AABB;
struct StateMemory;

constexpr uint32_t set_flag(uint32_t& flags, int bit)
{
    return flags |= (1U << (bit - 1));
}

constexpr uint32_t clr_flag(uint32_t& flags, int bit)
{
    return flags &= ~(1U << (bit - 1));
}

constexpr bool test_flag(uint32_t flags, int bit)
{
    return (flags & (1U << (bit - 1))) > 0;
}

class UI
{
  public:
    static void godmode(bool g);
    static void godmode_companions(bool g);
    static void death_enabled(bool g);
    static std::pair<float, float> click_position(float x, float y);
    static void zoom(float level);
    static void zoom_reset();
    static uint32_t get_frame_count();
    static void warp(uint8_t world, uint8_t level, uint8_t theme);
    static void transition(uint8_t world, uint8_t level, uint8_t theme);
    static float get_zoom_level();
    static void teleport(float x, float y, bool s, float vx, float vy, bool snap);
    static std::pair<float, float> screen_position(float x, float y);
    static float screen_distance(float x);
    static Entity* get_entity_at(float x, float y, bool s, float radius, uint32_t mask);
    static void move_entity(uint32_t uid, float x, float y, bool s, float vx, float vy, bool snap);
    static SaveData* savedata();
    static int32_t spawn_entity(ENT_TYPE entity_type, float x, float y, bool s, float vx, float vy, bool snap);
    static int32_t spawn_grid(ENT_TYPE entity_type, float x, float y, uint8_t layer);
    static int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);
    static void spawn_backdoor(float x, float y);
    static std::pair<float, float> get_position(Entity* ent, bool render = false);
    static bool has_active_render(Entity* ent);
    static void set_time_ghost_enabled(bool enable);
    static void set_time_jelly_enabled(bool enable);
    static void set_cursepot_ghost_enabled(bool enable);
    static ENT_TYPE get_entity_type(int32_t uid);
    static std::vector<Player*> get_players();
    static int32_t get_grid_entity_at(float, float, LAYER);
    static Illumination* create_illumination(Color color, float size, float x, float y);
    static void set_camp_camera_bounds_enabled(bool b);
    static std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer);
    static int32_t spawn_companion(ENT_TYPE compatnion_type, float x, float y, LAYER l, float velocityx, float velocityy);
    static void spawn_liquid(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount, float blobs_separation = INFINITY);
    static void spawn_liquid(ENT_TYPE entity_type, float x, float y);
    static int32_t spawn_entity_over(ENT_TYPE entity_type, uint32_t over_uid, float x, float y);
    static std::pair<float, float> get_room_pos(uint32_t x, uint32_t y);
    static std::string_view get_room_template_name(uint16_t room_template);
    static std::optional<uint16_t> get_room_template(uint32_t x, uint32_t y, uint8_t l);
    static void steam_achievements(bool on);
    static int32_t destroy_entity_items(Entity* ent);
    static bool destroy_entity_item_type(Entity* ent, ENT_TYPE type);
    static void destroy_entity_overlay(Entity* ent);
    static void kill_entity_overlay(Entity* ent);
    static void update_floor_at(float x, float y, LAYER l);
    static void cleanup_at(float x, float y, LAYER l, ENT_TYPE type = 0);
    static void safe_destroy(Entity* ent, bool unsafe = false, bool recurse = true);
    static std::vector<uint32_t> get_entities_overlapping(uint32_t mask, AABB hitbox, LAYER layer);
    static bool get_focus();
    static float get_spark_distance(SparkTrap* ent);
    static void save_progress();
    static int32_t spawn_playerghost(ENT_TYPE char_type, float x, float y, LAYER layer, float vx, float vy);
    static void spawn_player(uint8_t player_slot, std::optional<float> x = std::nullopt, std::optional<float> y = std::nullopt, std::optional<LAYER> layer = std::nullopt);
    static std::pair<float, float> spawn_position();
    static void load_death_screen();
    static void set_start_level_paused(bool enable);
    static bool get_start_level_paused();
    static void speedhack(float multiplier);
    static float get_speedhack();
    static void init_adventure();
    static void init_seeded(uint32_t seed);
    static std::pair<int64_t, int64_t> get_adventure_seed(std::optional<bool> run_start);
    static void set_adventure_seed(int64_t first, int64_t second);
    static void load_state_as_main(int from);
    static void save_main_state(int to);
    static StateMemory* get_save_state(int slot);
    static void set_camera_layer_control_enabled(bool enable);
    static void teleport_entity_abs(Entity* ent, float dx, float dy, float vx, float vy);
};
