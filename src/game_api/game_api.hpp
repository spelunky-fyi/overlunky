#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

struct Renderer
{
    uint32_t render_width; // same as window size unless resolution scale is set
    uint32_t render_height;

    uint32_t fps; // changing it doesn't seam to do anything
    uint32_t fps_denominator;

    uint32_t render_width2; // repeat?
    uint32_t render_height2;

    uint8_t flags1;
    uint8_t flags2;
    uint8_t padding[6];

    uint8_t skip[0x1228]; // tons of pointers, some structs with vtables

    uint8_t skip2[0x7F048]; // a lot of nothing

    size_t unknown38; // bool?
    float unknown39;  // not sure if actually float
    float unknown40;
    float unknown41;
    uint8_t unknown42[4];
    const char** unknown43a; // font/floor it's changing
    const char** unknown43b; // noise0.dds
    const char** unknown43c; // noise1.dds
    size_t unknown44[4];     // null?
    size_t unknown45;        // bool?

    // feels like two standard containers or something
    size_t* unknown46;
    size_t* unknown47;
    size_t unknown48;
    size_t* unknown49;
    size_t* unknown50;
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
    size_t* unknown61[4];
    size_t unknown62;                           // bool?
    std::unordered_map<int, size_t*> unknown63; // not sure about the key/value

    // bounch of vectors that probably used to load textures or something, they all seam to contain names of the .dds files
    // when i checked all seam to be already cleared and just have the data leftover, the "const char**" pointers identical as in texturedb

    size_t unknown64[6];                 // possibly two more vectors?
    std::vector<const char**> unknown65; // splash 0,1,2
    std::vector<const char**> unknown66; // fonts, basecamp, pet
    std::vector<const char**> unknown67; // fonts and menu textures + characters (character select screen textures?)
    std::vector<const char**> unknown68; // main menu background textures?
    size_t unknown69[3];                 // probably also vector, but it's null when i checked
    std::vector<const char**> unknown70; // menu textures?
    std::vector<const char**> unknown71; // only the ai.dds
    size_t unknown[8];                   // null

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
    uint8_t unknown86[6]; // padding probably

    size_t* unknown87; // some vtables

    uint8_t skip3[0xAD8]; // probably some static arrays of ... stuff

    size_t swap_chain;
    // 3 more pointers, some bit fields, then 5 more pointers

    // somewhere there should be shaders stored

    // added just to have the vtable
    virtual ~Renderer() = 0;
    virtual void some_dx_stuff() = 0; // it actually has a ton of parameters
};

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
