#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

struct Renderer
{
    uint32_t render_width; // sam as window size unless resolution scale is set
    uint32_t render_height;

    uint32_t fps; // changing it doesn't seam to do anything
    uint32_t fps_denominator;

    uint32_t render_width2; // repeat
    uint32_t render_height2;

    uint8_t flags1;
    uint8_t flags2;
    uint8_t padding[6];

    uint8_t skip[0x1228]; // tons of pointers

    uint8_t skip2[0x7F284]; // a lot of nothing

    float current_zoom;
    float target_zoom;
    float target_zoom_offset;
    float current_zoom_offset;
    float backlayer_light_level; // constantly overwritten by theme virtual get_backlayer_light_level
    uint8_t unknown2;
    uint8_t unknown3;
    uint16_t unknown4;

    uint8_t skip3[0xAE4];

    size_t swap_chain;

    // somewhere there should be shareds stored

    // added just to have the vtable
    virtual ~Renderer() = 0;
    virtual void some_dx_stuff() = 0; // it actually has a ton of parameters
};

struct GameAPI // size 0x60
{
    static GameAPI* get();

    float get_current_zoom();
    float get_target_zoom();

    void set_zoom(std::optional<float> current, std::optional<float> target);

    bool unknown1;
    size_t unknown2; // pointer
    Renderer* renderer;
    uint32_t window_width;
    uint32_t window_height;

    size_t unknown5;          // garbage?
    size_t unknown6;          // exe start
    size_t unknown7;          // some offset
    size_t unknown8;          // garbage?
    size_t SteamAPI_Callback; // just vtable? don't know much about steam stuff

    uint8_t unknown10a; // bool ?
    uint32_t unknown10b;

    size_t unknown11; // garbage?
    size_t unknown12; // garbage?
};
