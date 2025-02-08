#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

#include "state_structs.hpp" // for Camera

struct ID3D11BlendState1;
struct ID3D11DepthStencilState;
struct ID3DUserDefinedAnnotation;
struct IDXGIAdapter4;
struct IDXGISwapChain4;
struct ID3D11Texture2D;
struct ID3D11Texture2D1;
struct ID3D11RasterizerState2;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11SamplerState;
struct ID3D11DepthStencilView;

struct Resource;
struct DxResource;

struct UnknownRenderStuff
{
    ID3D11BlendState1* unknown1;
    ID3D11DepthStencilState* unknown2;
    size_t unknown3;
};

struct DxBuffer
{
    ID3D11Buffer* buffer;
    // potentially another object here? or just some allocator weirdness

    virtual ~DxBuffer() = 0;
    virtual void func1() = 0; // create buffer?
    virtual void func2() = 0; // calls some QueryInterface stuff
    virtual void func3() = 0;
    virtual ID3D11Buffer* get_buffer() = 0;
};

struct DxResource
{
    ID3D11Texture2D* dx_id;
    ID3D11ShaderResourceView* dx_shader;
    ID3D11RenderTargetView* dx_target_view; // optional object, in TextureDB only used only for keyboard_buttons "texture"

    // all vtables for textures are identical
    virtual bool unknown1(uint32_t, uint32_t, uint8_t) = 0;
    virtual void release() = 0;  // calls Release on all objects
    virtual void unknown2() = 0; // just return
    virtual ID3D11RenderTargetView* get_render_target_view() = 0;
    virtual ID3D11Texture2D* get_dx_id() = 0;
    virtual ~DxResource() = delete;
};

struct DxShader
{
    size_t unknown; // this is either a vtable with one function, or object with pointer to function as the first element
    ID3D11VertexShader* vertex;
    ID3D11PixelShader* pixel;
    ID3D11InputLayout* layout;
};

struct Renderer
{
    // check x64dbg plugin for up to date structure

    uint32_t render_width; // same as window size unless resolution scale is set
    uint32_t render_height;

    uint32_t fps; // changing it doesn't seam to do anything
    uint32_t fps_denominator;

    uint32_t render_width2; // used by the liquids to know what part(?) of screen to draw on top of the liquid (for all the transformation effects)
    uint32_t render_height2;

    uint8_t flags1;
    uint8_t flags2;
    uint8_t padding[6];

    DxResource* unknown1[25];
    uint32_t unknown1a;
    uint32_t unknown1b; // padding?

    DxResource* unknown1c[15];
    bool unknown1d;
    // uint8_t padding[7];

    ID3D11BlendState1* unknown1f[5];
    ID3D11SamplerState* unknown1g[5];

    DxBuffer* unknown1h[15];

    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t unknown5;

    DxBuffer* unknown6[512];

    uint8_t skip2[0x7F048]; // a lot of nothing

    size_t unknown38; // bool?
    float unknown39;  // not sure if actually float
    float unknown40;  // some float counter
    float brightness; // whole game brightness, can be set above 1
    uint8_t unknown42[4];
    Resource* textures_to_load[7]; // textures to load for entity
    uint32_t texture_num;          // number of textures in use

    uint32_t unknown45; // padding?

    // feels like two standard containers or something
    void* unknown46;
    void* unknown47;
    size_t unknown48;
    void* unknown49;
    void* unknown50;
    size_t unknown51; //

    uint32_t unknown52;
    uint32_t unknown53; // padding probably
    size_t unknown54;   // sometimes -1 sometimes pointer
    uint32_t unknown55; // -1
    uint32_t unknown56; // null
    size_t unknown57;
    size_t unknown58;
    uint16_t unknown59;     // 2k
    uint16_t unknown60a;    // 512
    uint16_t unknown60b[2]; // padding?
    void* unknown61[4];
    size_t unknown62; // bool?
    std::unordered_map<std::string, Resource*> textures;

    // bounch of vectors that probably used to load textures or something
    // when i checked all seam to be already cleared and just have the data leftover
    size_t unknown64[6];              // possibly two more vectors?
    std::vector<Resource*> unknown65; // splash 0,1,2
    std::vector<Resource*> unknown66; // fonts, basecamp, pet
    std::vector<Resource*> unknown67; // fonts and menu textures + characters (character select screen textures?)
    std::vector<Resource*> unknown68; // main menu background textures?
    size_t unknown69[3];              // probably also vector, but it's null when i checked
    std::vector<Resource*> unknown70; // menu textures?
    std::vector<Resource*> unknown71; // only the ai.dds
    size_t unknown[8];                // null

    uint8_t unknown80;
    uint8_t unknown81;
    uint8_t unknown82;
    uint8_t unknown83; // padding probably
    float current_zoom;
    float target_zoom;
    float target_zoom_offset;
    float current_zoom_offset;
    float backlayer_light_level; // constantly overwritten by theme virtual get_backlayer_light_level
    uint8_t unknown84;
    uint8_t unknown85;
    // uint8_t unknown86[6]; // padding probably

    void* unknown87; // some vtables

    bool unknown87a[112]; // probably size 110

    UnknownRenderStuff unknown87b[110];

    OnHeapPointer<Camera> camera;

    DxShader* unknown87d; // shaders?
    IDXGIAdapter4* unknown88;
    IDXGISwapChain4* swap_chain;
    void* unknown89; // CID3D11Forwarder ?
    void* unknown90; // x64dbg claims ID3D11DeviceContext5** but that struct doesn't seam to exist?
    size_t unknown91;
    ID3D11Texture2D1* unknown92;
    ID3D11DepthStencilView* unknown93;
    ID3D11RasterizerState2* unknown94;
    ID3D11RasterizerState2* unknown95;
    void* unknown96; // resolutions list, with some extra stuff

    uint32_t unknown97;
    uint32_t unknown98;
    uint32_t unknown99;
    uint32_t unknown100;
    uint32_t unknown101;
    uint32_t unknown102; // same as below but full screen?
    uint32_t unknown103;
    uint32_t game_width; // windowed ?
    uint32_t game_height;
    uint8_t unknown106a;
    uint8_t unknown106b;
    uint8_t unknown106c;
    uint8_t unknown106d;

    size_t unknown107[19];

    float unknown126a;
    float unknown126b;

    ID3D11Buffer* unknown127;
    ID3D11Buffer* unknown128;

    float horizontal_stretch;
    float horizontal_skew;
    float unknown130a; // darkens the horizontal edges of the screen?
    float horizontal_tilt;

    float vertical_skew;
    float vertical_stretch;
    float unknown132a; // darkens the vertical edges of the screen?
    float vertical_tilt;

    Vec2 position;

    float unknown134a;
    float unknown134b; // render scale, but negative?

    Vec2 render_offset_x;

    float unknown136a;     // probably related to unknown134a
    float render_zoom_out; // probably related to unknown134b, zooms out the rendered part of the game inside the window

    ID3DUserDefinedAnnotation* unknown137;

    virtual ~Renderer() = 0;
    virtual void some_dx_stuff() = 0; // it actually has a ton of parameters
    virtual void vunknown2() = 0;
    virtual void vunknown3() = 0;
    virtual void vunknown4() = 0;
    virtual void vunknown5() = 0;
    virtual void vunknown6() = 0;
    virtual DxResource* create_dx_resource() = 0; // create object Texture->default_texture->unknown5 (just allocates the memory and sets the vtable)

    // virtual void func(Texture*) =0; // index 46 - populate resource ? calls read_encrypted_file, then setups the Texture->default_texture

    // 58 or 59 virtuals in total
};
static_assert(sizeof(Renderer) == 0x81138);

struct UnknownAPIStuff
{
    uint32_t unknown1;
    int32_t unknown2; // -1

    uint8_t unknown3; // controller input flags related maybe?
    uint8_t unknown4;
    uint8_t unknown5;
    uint8_t unknown6;
    uint32_t unknown7; // padding?
};

struct STEAM_CALLBACK // just guessing
{
    size_t _vtable;   // 4 functions, last one is destructor
    uint8_t unknown1; // probably bool?
    uint8_t padding1[3];
    uint32_t padding2; // probably base class padding

    // subclass OnGameOverlayActivated ?
    bool steam_overlay_open;
    // padding
    uint32_t unknown_timer;
    float unknown_timer_related;
    uint32_t unknown11; // padding?
};

struct GameAPI
{
    static GameAPI* get();

    float get_current_zoom() const;
    float get_target_zoom() const
    {
        return renderer->target_zoom + renderer->target_zoom_offset;
    }

    void set_zoom(std::optional<float> current, std::optional<float> target);

    bool unknown1;
    UnknownAPIStuff* unknown2;
    Renderer* renderer;
    uint32_t window_width;
    uint32_t window_height;

    // all this below can probably be steam related stuff

    size_t unknown5; // steam related?
    size_t exe_begin;
    size_t unknown7; // some offset, OnHeapPointer?
    size_t unknown8; // function pointer?
    STEAM_CALLBACK SteamAPI_Callback;
};
