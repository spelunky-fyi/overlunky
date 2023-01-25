#pragma once

#include <cstddef>       // for size_t
#include <cstdint>       // for uint32_t, uint8_t, uint16_t, int32_t
#include <functional>    // for equal_to
#include <mutex>         // for mutex
#include <new>           // for operator new
#include <string>        // for allocator, string
#include <type_traits>   // for hash
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <utility>       // for pair

#include "aliases.hpp" // for TEXTURE
#include "color.hpp"   // for Color
#include "math.hpp"    // for Quad, AABB (ptr only)
#include "texture.hpp" // for Texture

struct JournalUI;

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

enum class WorldShader : std::uint8_t
{
    Colors = 0x0,
    Texture = 0x1,
    TextureColor = 0x2,
    TextureAlphaColor = 0x3,
    CircleOccluder = 0x4,
    TextureColorsWarp = 0x5,
    DeferredColorTransparent = 0x6,
    DeferredTextureColor = 0x7,
    DeferredTextureColor_Poisoned = 0x8,
    DeferredTextureColor_Cursed = 0x9,
    DeferredTextureColor_PoisonedCursed = 0xa,
    DeferredTextureColor_Transparent = 0xb,
    DeferredTextureColor_TransparentCorrected = 0xc,
    DeferredTextureColor_Emissive = 0x10,
    DeferredTextureColor_EmissiveGlow = 0x12,
    DeferredTextureColor_EmissiveGlowHeavy = 0x13,
    DeferredTextureColor_EmissiveGlowBrightness = 0x14,
    DeferredTextureColor_EmissiveColorizedGlow = 0x16,
    DeferredTextureColor_EmissiveColorizedGlow_DynamicGlow = 0x17,
    DeferredTextureColor_EmissiveColorizedGlow_Saturation = 0x18,
};

struct Letter
{
    /// Get's approximated center of a letter by finding the highest and lowest values, then finding the center of a rectangle build from those values
    Vec2 center() const
    {
        // there is probably better way of doing this?
        // could also just assume that the coordinates are correct and calculate the center using just the edges?
        auto right = std::max({bottom.A.x, bottom.B.x, bottom.C.x, top.A.x, top.B.x, top.C.x});
        auto left = std::min({bottom.A.x, bottom.B.x, bottom.C.x, top.A.x, top.B.x, top.C.x});
        auto _top = std::max({bottom.A.y, bottom.B.y, bottom.C.y, top.A.y, top.B.y, top.C.y});
        auto _bottom = std::min({bottom.A.y, bottom.B.y, bottom.C.y, top.A.y, top.B.y, top.C.y});
        return AABB{left, _bottom, right, _top}.center();
    }

    /// Get the Quad of a letter (easier to work with compared to the two triangles)
    /// This assumes that the triangles are in the correct 'touching each other' position
    /// if the positions were altered the results may not end up as expected
    Quad get_quad() const
    {
        return {bottom.A, bottom.B, bottom.C, top.C};
    }
    /// Inverse of the get_quad
    void set_quad(Quad quad)
    {
        // bottom_left, bottom_right, top_right, top_left
        std::tuple<Vec2, Vec2, Vec2, Vec2> vectors = quad;
        bottom.A = std::get<0>(vectors);
        bottom.B = top.B = std::get<1>(vectors);
        top.C = std::get<2>(vectors);
        bottom.C = top.A = std::get<3>(vectors);
    }

    // b - bottom, t - top
    //
    // bC    tA===tC
    // ||\    \   ||
    // || \    \  ||
    // ||  \    \ ||
    // ||   \    \||
    // bA===bB    tB

    Triangle bottom;
    Triangle top;
};

struct TextRenderingInfo
{
    TextRenderingInfo() = default;
    TextRenderingInfo(TextRenderingInfo&) = delete;
    ~TextRenderingInfo();

    /// Changes the text, only position stays the same, everything else (like rotation) is reset or set according to the parameters
    void set_textx(const std::u16string text, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle);

    void set_text(const std::u16string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle);
    void set_text(const std::string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle);

    /// Returns refrence to the letter coordinates relative to the x,y position
    std::span<Letter> get_dest()
    {
        return {dest, (dest + text_length)};
    }
    /// Returns refrence to the letter coordinates in the texture
    std::span<Letter> get_source()
    {
        return {source, (source + text_length)};
    }

    /// {width, height}, is only updated when you set/change the text. This is equivalent to draw_text_size
    std::pair<float, float> text_size() const
    {
        return {width, height};
    }
    uint32_t size() const
    {
        return text_length;
    }
    /// Rotates the text around the pivot point (default 0), pivot is relative to the text position (x, y), use px and py to offset it
    void rotate(float angle, std::optional<float> px, std::optional<float> py);

    float x;
    float y;
    /// You can also just use `#` operator on the whole object to get the text lenght
    uint32_t text_length;
    float width;
    float height;
    uint32_t unknown3; // padding probably

    Letter* dest{nullptr};
    Letter* source{nullptr};
    // 6 * wcslen(input_text), just numbers in order 0, 1, 2 ... have some strage effect if you change them
    uint16_t* unknown6{nullptr};

    uint16_t nof_special_character; // number of special characters, still not sure how the game knows which ones are the special ones?
                                    // setting higher value than the `text_length` will crash
    uint16_t unknown8;              // padding probably

    /// Used to draw buttons and stuff, default is -1 wich uses the buttons texture
    int32_t special_texture_id;

    uint8_t shader; // ? changing it can change the text color, or make the text all rectangles?
    uint8_t padding1[3];
    uint32_t padding2;
    Texture* font;
    size_t unknown13; // probably garbage
};

struct TextureRenderingInfo
{
    // where to draw on the screen:
    float x;
    float y;

    // destination is relative to the x,y centerpoint
    float destination_bottom_left_x;
    float destination_bottom_left_y;
    float destination_bottom_right_x;
    float destination_bottom_right_y;
    float destination_top_left_x;
    float destination_top_left_y;
    float destination_top_right_x;
    float destination_top_right_y;

    // source rectangle in the texture to render
    float source_bottom_left_x;
    float source_bottom_left_y;
    float source_bottom_right_x;
    float source_bottom_right_y;
    float source_top_left_x;
    float source_top_left_y;
    float source_top_right_x;
    float source_top_right_y;

    void set_destination(const AABB& bbox);
    Quad dest_get_quad() const;
    void dest_set_quad(const Quad& quad);
    Quad source_get_quad() const;
    void source_set_quad(const Quad& quad);
};

struct RenderAPI
{
    const size_t* api;
    size_t swap_chain_off;

    mutable std::mutex custom_textures_lock;
    std::unordered_map<TEXTURE, Texture> custom_textures;

    static RenderAPI& get();

    size_t renderer() const;
    size_t swap_chain() const;

    void set_lut(TEXTURE texture_id, uint8_t layer);
    void reset_lut(uint8_t layer);

    void draw_text(const TextRenderingInfo* tri, Color color);
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);
    void draw_screen_texture(Texture* texture, Quad source, Quad dest, Color color, uint8_t shader);
    void draw_world_texture(Texture* texture, Quad source, Quad dest, Color color, WorldShader shader);

    void set_post_render_game(void (*post_render_game)());
    void set_advanced_hud();

    void reload_shaders();
};

struct RenderInfo
{
    float x;
    float y;
    uint32_t unknown3;
    float unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    uint32_t unknown9;
    float x_dupe1;
    float y_dupe1;
    uint32_t unknown10;
    float x_dupe2;
    float y_dupe2;
    uint32_t unknown11;
    uint8_t unknown_timer1; // can someone test this at higher refresh rate if it's tided to the fps or Hz?
    uint8_t unknown_timer2; // for some entities this stops when the entity is not on screen but the above one don't
    bool unknown12c;
    bool unknown12d;
    bool render_inactive; // stops all the rendering stuff, the value is forced thou
    uint8_t unknown13b;
    uint8_t unknown13c;
    uint8_t unknown13d;
    uint32_t unknown14;
    uint8_t unknown15_counter;
    uint8_t unknown16_counter;
    bool unknown17;
    bool unknown18;
    uint32_t unknown19;
    WORLD_SHADER shader; // 0 - 36, game crash at around 55
    uint8_t unknown20a;
    uint8_t unknown20b;
    uint8_t unknown20c;

    // destination in world coords // this is also Quad, but some special one
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
    Quad source;

    float tilew;
    float tileh;
    float x_dupe3;
    float y_dupe3;
    float x_dupe4; // this will be writen into x next refresh
    float y_dupe4; // this will be writen into y next refresh
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
    uint32_t unknown58;
    float* unknown59;
    uint32_t unknown60;
    uint32_t unknown61; // end, next RenderInfo below

    virtual ~RenderInfo() = 0;
    virtual void unknown_v2() = 0;
    virtual void update() = 0;
    virtual void render(float*) = 0;
    virtual bool unknown_3() = 0; // init? sets darkness to 1.0 at the start, then does some other stuff

    // gets the entity owning this RenderInfo
    class Entity* get_entity() const;

    // for supporting HookableVTable
    uint32_t get_aux_id() const;
};

void init_render_api_hooks();
bool& get_journal_enabled();
void on_open_journal_chapter(JournalUI* journal_ui, uint8_t chapter, bool instant, bool play_sound);
