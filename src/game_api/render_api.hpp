#pragma once

#include "entity.hpp"
#include "memory.hpp"
#include "texture.hpp"

#include <mutex>

enum JOURNAL_VFTABLE
{
    // to get those offsets, find "vftable_JournalPages" then go to each page in journal, then to it's first vtable function address
    // and calculate offset diference
    PROGRESS = 496,         // open journal: page 0 or die, journal: page 2
    MENU = 448,             // open journal: page 1
    PLACES = 672,           // open journal > Places: any page > 1
    PEOPLE = 624,           // open journal > People: any page > 1
    BESTIARY = 576,         // open journal > Bestiary: any page > 1
    ITEMS = 400,            // open journal > Items: any page > 1
    TRAPS = 912,            // open journal > Traps: any page > 1
    STORY = 864,            // open journal > Story: any page > 1
    FEATS = 96,             // open journal > Feats: any page > 1 or open player profile from main menu, any page > 1
    DEATH_CAUSE = 160,      // die, journal: page 0
    DEATH_MENU = 256,       // die, journal: page 1
    RECAP = 208,            // die, journal: page 3 Dear Journal
    PLAYER_PROFILE = 720,   // open player profile from main menu, journal: page 0
    LAST_GAME_PLAYED = 816, // open player profile from main menu, journal: page 1
};

struct TextureDefinition
{
    std::string texture_path;
    uint32_t width;
    uint32_t height;
    uint32_t tile_width;
    uint32_t tile_height;
    uint32_t sub_image_offset_x{0};
    uint32_t sub_image_offset_y{0};
    uint32_t sub_image_width{0};
    uint32_t sub_image_height{0};
};

struct RenderAPI
{
    const size_t* api;
    size_t swap_chain_off;

    mutable std::mutex custom_textures_lock;
    std::unordered_map<TEXTURE, Texture> custom_textures;

    static RenderAPI& get();

    size_t renderer() const
    {
        return read_u64(*api + 0x10);
    }

    size_t swap_chain() const
    {
        return read_u64(renderer() + swap_chain_off);
    }

    TextureDefinition get_texture_definition(TEXTURE texture_id);
    Texture* get_texture(TEXTURE texture_id);
    TEXTURE define_texture(TextureDefinition data);
    void reload_texture(const char* texture_name);  // Does a lookup for the right texture to reload
    void reload_texture(const char** texture_name); // Reloads the texture directly

    void draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle);
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color);
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, QuadTree dest, Color color);
};

// straight out of the x64dbg plugin
struct RenderInfo
{
    size_t __vftable;
    float x;
    float y;
    uint32_t unknown3;
    float unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    uint32_t unknown9;
    float x_dupe_1;
    float y_dupe_1;
    uint32_t unknown10;
    float x_dupe2;
    float y_dupe2;
    uint32_t unknown11;
    uint8_t unknown_timer1; // can someone test this at higher refresh rate if it's tided to the fps or Hz?
    uint8_t unknown_timer2; // for some entities this stops when the entity is not on screen but the above one don't
    bool unknown12c;
    bool unknown12d;
    bool stop_render; // stops all the rendering stuff, the value is forced thou
    uint8_t unknown13b;
    uint8_t unknown13c;
    uint8_t unknown13d;
    uint32_t unknown14;
    uint8_t unknown15_counter;
    uint8_t unknown16_counter;
    bool unknown17;
    bool unknown18;
    uint32_t unknown19;
    uint8_t shader; //0 - 36, game crash at around 55
    uint8_t unknown20a;
    uint8_t unknown20b;
    uint8_t unknown20c;

    // destination in world coords // this is also QuadTree, but some special one
    float destination_bottom_left_x; // entity.x - (entity.w/2)
    float destination_bottom_left_y; // entity.y - (entity.h/2)
    float destination_bottom_left_unknown;
    float destination_bottom_right_x;
    float destination_bottom_right_y;
    float destination_bottom_right_unknown;
    float destination_top_right_x;
    float destination_top_right_y;
    float destination_top_right_unknown;
    float destination_top_left_x;
    float destination_top_left_y;
    float destination_top_left_unknown;

    // source in sprite sheet coords (multiply the floats by TextureDB.width and height)
    QuadTree source;

    float tilew;
    float tileh;
    float x_dupe3;
    float y_dupe3;
    float x_dupe4;
    float y_dupe4;
    float angle1;
    float angle2;
    float angle_related;
    uint32_t animation_frame;
    uint32_t unknown38;
    Texture* texture;
    const char** texture_name;

    size_t unknown39;
    size_t unknown40;
    size_t unknown41;
    size_t unknown42;
    size_t unknown43;
    size_t unknown44;
    bool render_as_non_liquid; // for liquids, forced to false, for non-liquids: sprite goes crazy when moving about
    uint8_t unknown47;
    uint8_t unknown48;
    uint8_t unknown49;
    uint32_t unknown50;
    size_t entity_offset; // the offset of the associated entity in memory, starting from the memory segment that State resides in
    bool flip_horizontal; // facing left
    uint8_t unknown52;
    uint8_t unknown53;
    uint8_t unknown54;
    uint32_t unknown55;
    float darkness; // 0.0 = completely black ; 1.0 = normal (dark effect like when on fire)
    uint32_t unknown56;
    uint32_t unknown57;
    uint32_t unknown58; //end, next RenderInfo below
};

struct TextRenderingInfo
{
    float x;
    float y;
    uint32_t text_length;
    float width;
    float height;
    uint32_t unknown3;
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
    uint16_t unknown7;
    uint16_t unknown8;
    int32_t unknown9;
    size_t unknown10;
    Texture* font;
};

struct TextureRenderingInfo
{
    // where to draw on the screen:
    float x;
    float y;
    union
    {
        struct
        {
            // destination is relative to the x,y centerpoint
            QuadTree destination;
            // source rectangle in the texture to render
            QuadTree source;
        };
        struct
        {
            float destination_top_left_x;
            float destination_top_left_y;
            float destination_top_right_x;
            float destination_top_right_y;
            float destination_bottom_left_x;
            float destination_bottom_left_y;
            float destination_bottom_right_x;
            float destination_bottom_right_y;
            float source_top_left_x;
            float source_top_left_y;
            float source_top_right_x;
            float source_top_right_y;
            float source_bottom_left_x;
            float source_bottom_left_y;
            float source_bottom_right_x;
            float source_bottom_right_y;
        };
    };
    void set_destination(const AABB& bbox);
};

void init_render_api_hooks();
