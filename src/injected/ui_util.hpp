//#include "ui.hpp"

#include "aliases.hpp"
#include "color.hpp"

class Player;
struct SaveData;
struct Illumination;

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
    static int32_t get_entity_at(float x, float y, bool s, float radius, uint32_t mask);
    static void move_entity(uint32_t uid, float x, float y, bool s, float vx, float vy, bool snap);
    static SaveData* savedata();

    static void set_time_ghost_enabled(bool enable);
    static void set_time_jelly_enabled(bool enable);
    static ENT_TYPE get_entity_type(int32_t uid);
    static std::vector<Player*> get_players();
    static int32_t get_grid_entity_at(float, float, LAYER);
    static Illumination* create_illumination(Color color, float size, float x, float y);
    static void set_camp_camera_bounds_enabled(bool b);
    static std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer);
};