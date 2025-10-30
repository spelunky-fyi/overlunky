#pragma once

#include <cstdint>    // for uint32_t, uint8_t, int32_t, int64_t, int16_t
#include <functional> // for function
#include <optional>   // for optional, nullopt
#include <string>     // for string
#include <tuple>      // for tuple
#include <utility>    // for pair
#include <vector>     // for vector

#include "aliases.hpp" // for ENT_TYPE, LAYER
#include "color.hpp"   // for Color

class Player;
struct ParticleEmitterInfo;
struct Illumination;
class Entity;
struct AABB;
struct Layer;
struct StateMemory;

void attach_entity(Entity* overlay, Entity* attachee);
void attach_entity_by_uid(uint32_t overlay_uid, uint32_t attachee_uid);
int32_t attach_ball_and_chain(uint32_t uid, float off_x, float off_y);
void stack_entities(uint32_t bottom_uid, uint32_t top_uid, const float (&offset)[2]);
void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy);
void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy, LAYER layer);
void move_liquid_abs(uint32_t uid, float x, float y, float vx, float vy);
ENT_TYPE get_entity_type(uint32_t uid);
std::tuple<float, float, float, float> screen_aabb(float x1, float y1, float x2, float y2);
float screen_distance(float x);
std::vector<uint32_t> filter_entities(std::vector<uint32_t> entities, std::function<bool(Entity*)> predicate);
void set_contents(uint32_t uid, ENT_TYPE item_entity_type);
void entity_remove_item(uint32_t uid, uint32_t item_uid, std::optional<bool> check_autokill);
void kill_entity(uint32_t uid, std::optional<bool> destroy_corpse = std::nullopt);
void destroy_entity(uint32_t uid);
void unequip_backitem(uint32_t who_uid);
int32_t worn_backitem(uint32_t who_uid);
uint8_t get_max_rope_length();
bool is_inside_active_shop_room(float x, float y, LAYER layer);
bool is_inside_shop_zone(float x, float y, LAYER layer);
uint8_t waddler_count_entity(ENT_TYPE entity_type);
int8_t waddler_store_entity(ENT_TYPE entity_type);
void waddler_remove_entity(ENT_TYPE entity_type, uint8_t amount_to_remove = 99);
int16_t waddler_get_entity_meta(uint8_t slot);
void waddler_set_entity_meta(uint8_t slot, int16_t meta);
uint32_t waddler_entity_type_in_slot(uint8_t slot);
bool entity_type_check(const std::vector<ENT_TYPE>& types_array, const ENT_TYPE find);
std::vector<ENT_TYPE> get_proper_types(std::vector<ENT_TYPE> ent_types);
void poison_entity(int32_t entity_uid);
void move_grid_entity(int32_t uid, float x, float y, LAYER layer);
void destroy_grid(int32_t uid);
void destroy_grid(float x, float y, LAYER layer);
void add_item_to_shop(int32_t item_uid, int32_t shop_owner_uid);
void set_adventure_seed(int64_t first, int64_t second);
std::pair<int64_t, int64_t> get_adventure_seed(std::optional<bool> run_start);
void update_liquid_collision_at(float x, float y, bool add, std::optional<LAYER> layer = std::nullopt);
void add_entity_to_liquid_collision(uint32_t uid, bool add);
std::pair<uint8_t, uint8_t> get_liquids_at(float x, float y, LAYER layer);
void game_log(std::string message);
void load_death_screen();
void save_progress();
void set_level_string(std::u16string_view text);
void set_frametime(std::optional<double> frametime);
double get_frametime();
void set_frametime_inactive(std::optional<double> frametime);
double get_frametime_inactive();
ENT_TYPE add_custom_type(std::vector<ENT_TYPE> types);
ENT_TYPE add_custom_type();
int32_t get_current_money();
int32_t add_money(int32_t amount, std::optional<uint8_t> display_time);
int32_t add_money_slot(int32_t amount, uint8_t player_slot, std::optional<uint8_t> display_time);
void destroy_layer(uint8_t layer);
void destroy_level();
void create_layer(uint8_t layer);
void create_level();
bool get_start_level_paused();
void set_speedhack(std::optional<float> multiplier);
float get_speedhack();
void init_adventure();
void init_seeded(std::optional<uint32_t> seed);
uint8_t get_liquid_layer();
uint32_t lowbias32(uint32_t x);
uint32_t lowbias32_r(uint32_t x);
