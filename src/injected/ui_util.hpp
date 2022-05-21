//#include "ui.hpp"

#include "aliases.hpp"
#include "color.hpp"

class Player;
class Entity;
struct SaveData;
struct Illumination;

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
    static std::pair<float, float> click_position(float x, float y);
    static void zoom(float level);
    static uint32_t get_frame_count();
    static void warp(uint8_t world, uint8_t level, uint8_t theme);
    static float get_zoom_level();
    static void teleport(float x, float y, bool s, float vx, float vy, bool snap);
    static std::pair<float, float> screen_position(float x, float y);
    static Entity* get_entity_at(float x, float y, bool s, float radius, uint32_t mask);
    static void move_entity(uint32_t uid, float x, float y, bool s, float vx, float vy, bool snap);
    static SaveData* savedata();
    static int32_t spawn_entity(ENT_TYPE entity_type, float x, float y, bool s, float vx, float vy, bool snap);
    static int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t);
    static void spawn_backdoor(float x, float y);
    static std::pair<float, float> get_position(Entity* ent, bool render = false);
    static bool has_active_render(Entity* ent);

    static void set_time_ghost_enabled(bool enable);
    static void set_time_jelly_enabled(bool enable);
    static ENT_TYPE get_entity_type(int32_t uid);
    static std::vector<Player*> get_players();
    static int32_t get_grid_entity_at(float, float, LAYER);
    static Illumination* create_illumination(Color color, float size, float x, float y);
    static void set_camp_camera_bounds_enabled(bool b);
    static std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer);
    static int32_t spawn_companion(ENT_TYPE compatnion_type, float x, float y, LAYER l);
    static void spawn_liquid(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount, float blobs_separation = INFINITY);
    static void spawn_liquid(ENT_TYPE entity_type, float x, float y);
    static int32_t spawn_entity_over(ENT_TYPE entity_type, uint32_t over_uid, float x, float y);
    static std::pair<float, float> get_room_pos(uint32_t x, uint32_t y);
    static std::string_view get_room_template_name(uint16_t room_template);
    static std::optional<uint16_t> get_room_template(uint32_t x, uint32_t y, uint8_t l);
    static void steam_achievements(bool on);
};
