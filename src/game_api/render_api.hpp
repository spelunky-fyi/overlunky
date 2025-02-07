#pragma once

#include <array>         // for array
#include <cstddef>       // for size_t
#include <cstdint>       // for uint32_t, uint8_t, uint16_t, int32_t
#include <functional>    // for equal_to
#include <mutex>         // for mutex
#include <new>           // for operator new
#include <string>        // for allocator, string
#include <type_traits>   // for hash
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <utility>       // for pair

#include "aliases.hpp"                       // for TEXTURE
#include "color.hpp"                         // for Color
#include "containers/game_unordered_map.hpp" // for game_unordered_map
#include "containers/game_vector.hpp"        // for game_vector
#include "math.hpp"                          // for Quad, AABB (ptr only)
#include "texture.hpp"                       // for Texture
#include "thread_utils.hpp"                  // for OnHeapPointer

struct JournalUI;
struct Layer;
class Entity;
struct Renderer;
struct ParticleEmitterInfo;

using VANILLA_TEXT_ALIGNMENT = uint32_t;
using VANILLA_FONT_STYLE = uint32_t;

enum JOURNAL_VFTABLE
{
    // to get those offsets, find "vftable_JournalPages" then go to each page in journal, then to it's first vtable function address
    // and calculate offset difference
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
        return {bottom.A, bottom.B, top.C, top.A};
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

    // example letter: N
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
    void set_textx(const std::u16string text, float scale_x, float scale_y, VANILLA_TEXT_ALIGNMENT alignment, VANILLA_FONT_STYLE fontstyle);

    void set_text(const std::u16string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle);
    void set_text(const std::string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle);

    /// Returns reference to the letter coordinates relative to the x,y position
    std::span<Letter> get_dest()
    {
        return {dest, (dest + text_length)};
    }
    /// Returns reference to the letter coordinates in the texture
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
    TEXTURE get_font() const
    {
        if (font)
            return font->id;
        else
            return 0;
    }
    bool set_font(TEXTURE id)
    {
        if (auto* texture = get_texture(id))
        {
            font = get_texture(id);
            return true;
        }
        return false;
    }
    /// Rotates the text around the pivot point (default 0), pivot is relative to the text position (x, y), use px and py to offset it
    void rotate(float angle, std::optional<float> px, std::optional<float> py);

    float x;
    float y;
    /// You can also just use `#` operator on the whole TextRenderingInfo to get the text length
    uint32_t text_length;
    float width;
    float height;
    uint32_t unknown3; // padding probably

    Letter* dest{nullptr};
    Letter* source{nullptr};
    // 6 * text_length, just numbers in order 0, 1, 2 ... have some strange effect if you change them
    uint16_t* unknown6{nullptr};

    uint16_t nof_special_character; // number of special characters, still not sure how the game knows which ones are the special ones?
                                    // setting higher value than the `text_length` will crash
    uint16_t unknown8;              // padding probably

    /// Used to draw buttons and stuff, default is -1 which uses the buttons texture
    TEXTURE special_texture_id;

    uint8_t shader; // ? changing it can change the text color, or make the text all rectangles?
    uint8_t padding1[3];
    uint32_t padding2;
    Texture* font;
    size_t unknown13; // probably garbage
};

struct TextureRenderingInfo
{
    // where to draw on the screen:
    float x{0};
    float y{0};

    /// destination is relative to the x,y center point
    float destination_bottom_left_x{0};
    float destination_bottom_left_y{0};
    float destination_bottom_right_x{0};
    float destination_bottom_right_y{0};
    float destination_top_left_x{0};
    float destination_top_left_y{0};
    float destination_top_right_x{0};
    float destination_top_right_y{0};

    // source rectangle in the texture to render
    float source_bottom_left_x{0};
    float source_bottom_left_y{0};
    float source_bottom_right_x{0};
    float source_bottom_right_y{0};
    float source_top_left_x{0};
    float source_top_left_y{0};
    float source_top_right_x{0};
    float source_top_right_y{0};

    void set_destination(const AABB& bbox);
    Quad dest_get_quad() const;
    void dest_set_quad(const Quad& quad);
    Quad source_get_quad() const;
    void source_set_quad(const Quad& quad);
};

struct RenderAPI
{
    mutable std::mutex custom_textures_lock;
    std::unordered_map<TEXTURE, Texture> custom_textures;
    std::unordered_map<TEXTURE, Texture> original_textures;
    std::unordered_map<std::string, Color> texture_colors;
    std::unordered_map<TEXTURE, Color> original_colors;

    static RenderAPI& get();

    void set_lut(TEXTURE texture_id, uint8_t layer);
    void reset_lut(uint8_t layer);

    void draw_text(const TextRenderingInfo* tri, Color color);
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);
    void draw_screen_texture(Texture* texture, Quad source, Quad dest, Color color, uint8_t shader);
    void draw_screen_texture(Texture* texture, TextureRenderingInfo tri, Color color, uint8_t shader);
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
    float offset_x;
    float offset_y;
    uint32_t unknown6;
    uint32_t unknown7;
    float unknown8; // used for parallax?
    float unknown9; // automatically goes to 0, while it's non 0 game does (unknown9/unknown8) or something like that and influences position

    float x_dupe1; // position last refresh
    float y_dupe1; // position last refresh
    uint32_t unknown10;
    float x_dupe2;
    float y_dupe2;
    uint32_t unknown11;
    uint8_t unknown_timer1; // can someone test this at higher refresh rate if it's tided to the fps or Hz?
    uint8_t unknown_timer2; // for some entities this stops when the entity is not on screen but the above one doesn't
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
    WORLD_SHADER shader;
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
    float x_dupe4; // this will be written into x next refresh
    float y_dupe4; // this will be written into y next refresh
    float angle1;
    float angle2;
    float angle_related;
    uint32_t animation_frame;
    uint32_t unknown38; // padding
    Texture* texture;   // probably just used for definition
    Resource* texture_names[7];
    // second_texture_name Normal map texture on COG entities (shader 30), shine texture on ice entities. May not have a correct value on entities that don't use it
    // third_texture_name Shine texture on COG entities (shader 30). May not have a correct value on entities that don't use it

    uint32_t texture_num; // liquids use 0, most sprite entities use 1, ice uses 2, COG entities use 3
    uint32_t padding1;
    OnHeapPointer<Entity> entity_offset;
    bool flip_horizontal; // facing left
    uint8_t padding2[3];
    uint32_t unknown55;
    float brightness; // 0.0 = completely black ; 1.0 = normal (used for dark effect like when on fire)

    virtual ~RenderInfo() = 0;
    /// Called when the entity enters the camera view, using its hitbox with an extra threshold. Handles low-level graphics tasks related to the GPU
    virtual void draw() = 0;   // initializes positions
    virtual void update() = 0; // math, basically always runs before render
    /// Offset used in CO to draw the fake image of the entity on the other side of a level
    virtual void render(Vec2* offset) = 0;
    virtual bool set_entity(Texture* texture, Entity* entity) = 0;

    // gets the entity owning this RenderInfo
    Entity* get_entity() const;

    // for supporting HookableVTable
    uint32_t get_aux_id() const;

    bool set_second_texture(TEXTURE texture_id);
    bool set_third_texture(TEXTURE texture_id);
    /// Set the number of textures that may be used, need to have them set before for it to work
    bool set_texture_num(uint32_t num);
    /// Sets second_texture to the texture specified, then sets third_texture to SHINE_0 and texture_num to 3. You still have to change shader to 30 to render with normal map (same as COG normal maps)
    bool set_normal_map_texture(TEXTURE texture_id);
};

void init_render_api_hooks();
bool& get_journal_enabled();
void on_open_journal_chapter(JournalUI* journal_ui, uint8_t chapter, bool instant, bool play_sound);
void render_draw_depth(Layer* layer, uint8_t draw_depth, float bbox_left, float bbox_bottom, float bbox_right, float bbox_top);
float get_layer_transition_zoom_offset(uint8_t layer);

struct SpritePosition
{
    uint32_t column;
    uint32_t row;
};

struct HudInventory
{
    bool enabled;
    uint8_t health;
    uint8_t bombs;
    uint8_t ropes;

    uint8_t b00;
    bool ankh;
    bool kapala;
    uint8_t b03;
    float kapala_scale;

    union
    {
        /// NoDoc
        uint32_t kapala_blood;
        SpritePosition kapala_sprite;
    };
    bool poison;
    bool curse;
    bool elixir;
    uint8_t b13;
    /// Powerup type or 0
    ENT_TYPE crown;
    std::array<SpritePosition, 18> powerup_sprites;

    /// Amount of generic pickup items at the bottom. Set to 0 to not draw them.
    uint32_t powerup_count;
};
static_assert(sizeof(HudInventory) == 176);

struct HudElement
{
    /// Hide background and dim if using the auto adjust setting.
    bool dim; // it's actually  3 states: 0 - bright, 1 - dim, anything above - hide completely
    /// Background will be drawn if this is not 0.5
    float opacity;
    /// Level time when element should dim again after highlighted, INT_MAX if dimmed on auto adjust. 0 on opaque.
    int32_t time_dim; // it's set to INT_MAX after it was dimmed
};

struct HudPlayer : HudElement
{
    int16_t health;
    int16_t bombs;
    int16_t ropes;
    // int16_t padding;
    int32_t idunno;
};
static_assert(sizeof(HudPlayer) == 24);

struct HudMoney : HudElement
{
    int32_t total;
    int32_t counter;
    uint8_t timer;
    // uint8_t padding[3];
};

struct HudData
{
    std::array<HudInventory, MAX_PLAYERS> inventory;
    bool udjat;
    // uint8_t padding[3];
    int32_t money_total;
    int32_t money_counter;
    /// in ms
    uint32_t time_level;
    /// in ms
    uint32_t time_total;
    uint8_t world_num;
    uint8_t level_num;
    bool angry_shopkeeper;
    bool seed_shown;
    uint32_t seed;
    float opacity;

    /// For player related icons, they use the same TextureRendering, just offset while drawing
    TextureRenderingInfo player_highlight;
    TextureRenderingInfo player_heart;
    TextureRenderingInfo player_ankh;
    TextureRenderingInfo kapala_icon;
    TextureRenderingInfo player_crown;
    TextureRenderingInfo unknown_texture5;
    TextureRenderingInfo player_bomb;
    TextureRenderingInfo player_rope;
    TextureRenderingInfo unknown_texture8;
    TextureRenderingInfo unknown_texture9;
    TextureRenderingInfo unknown_texture10;
    TextureRenderingInfo unknown_texture11;
    TextureRenderingInfo unknown_texture12;
    TextureRenderingInfo udjat_icon;
    TextureRenderingInfo unknown_texture14;
    /// Money and time use the same TextureRendering, just offset while drawing
    TextureRenderingInfo money_and_time_highlight;
    TextureRenderingInfo dollar_icon;
    TextureRenderingInfo hourglass_icon;
    TextureRenderingInfo clover_icon;
    TextureRenderingInfo level_highlight;
    TextureRenderingInfo level_icon;
    TextureRenderingInfo seed_background;
    float roll_in;
    float unknown6;
    float unknown7;
    float unknown8;
    float unknown9;
    float unknown10;
    std::array<float, MAX_PLAYERS> player_zoom; // ?
    float unknown15;
    float unknown16;
    float unknown17;
    float unknown18;

    std::array<HudPlayer, MAX_PLAYERS> players;
    HudMoney money;
    ParticleEmitterInfo* money_increase_sparkles;
    HudElement timer;
    HudElement level;
    game_vector<HudElement*> elements; // not sure what's for, just lists all the elements above, even for the unactive players

    uint32_t unknown20;
    uint8_t unknown21;
    uint8_t unknown22;
    uint16_t unknown23;
    float clover_falling_apart_timer;
    float unknown25;
    ParticleEmitterInfo* unknown26;
    TextureRenderingInfo unknown27;
    TextureRenderingInfo unknown28;
    TextureRenderingInfo unknown29;
    float unknown30;
    // uint32_t unknown31; // probably padding
    game_unordered_map<uint32_t, uint32_t> unknown32;
    float unknown33;
    float unknown34;
    float unknown35;
    float unknown36;
    float unknown37;
    float unknown38;
    float unknown39;
    TextureRenderingInfo loading_dragon;
    float loading_dragon_visibility;
    float unknown42;
    float unknown43;
    TextureRenderingInfo loading_cog;
    float unknown45;
    uint32_t loading_cog_timer;
    uint32_t unknown47;
    bool unknown48;
    // uint8_t unknown49[3]; //probably padding
    float unknown51;
    std::array<ParticleEmitterInfo*, MAX_PLAYERS> player_cursed_particles;
    std::array<ParticleEmitterInfo*, MAX_PLAYERS> player_poisoned_particles;
};
// static_assert(sizeof(HudData) <= 0xa00);

struct Hud
{
    float y;
    float opacity;
    HudData* data;
};

HudData* get_hud();
