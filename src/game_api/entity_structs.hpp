#pragma once

#include <cstddef>       // for size_t
#include <cstdint>       // for uint8_t, uint32_t, int32_t, uint16_t, int64_t
#include <functional>    // for function, equal_to
#include <span>          // for span
#include <string>        // for allocator, string
#include <string_view>   // for string_view
#include <tuple>         // for tuple
#include <type_traits>   // for move
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map
#include <utility>       // for pair
#include <vector>        // for vector

enum class REPEAT_TYPE : uint8_t
{
    NoRepeat,
    Linear,
    BackAndForth,
};

enum class SHAPE : uint8_t
{
    RECTANGLE = 1,
    CIRCLE = 2,
};

struct Animation
{
    int32_t texture;
    int32_t count;
    int32_t interval;
    uint8_t key;
    REPEAT_TYPE repeat;
};

struct Rect
{
    float offsetx;
    float offsety;
    float hitboxx;
    float hitboxy;
};

struct CollisionInfo
{
    Rect rect;
    SHAPE shape;
    bool hitbox_enabled;
    uint8_t field_3A;
    uint8_t field_3B;
};

struct SoundInfo
{
    int64_t unknown1;
    uint32_t sound_id;
    int32_t unknown2;
    const char* sound_name;
    int64_t unknown3;
    int64_t unknown4;
    int64_t unknown5;
};

struct SoundPosition
{
    size_t __vftable;
    float x;
    float y;
    SoundInfo* sound_effect_info; // param to FMOD::Studio::EventInstance::SetParameterByID (this ptr + 0x30)
    uint64_t fmod_param_id;       // param to FMOD::Studio::EventInstance::SetParameterByID
    float POS_SCREEN_X;           // VANILLA_SOUND_PARAM names, for now
    float DIST_CENTER_X;
    float DIST_CENTER_Y;
    float DIST_Z;
    float DIST_PLAYER; // seams to be always here, even you you get nil in lua
    float SUBMERGED;
    float LIQUID_STREAM;
    float unknown10; // LIQUID_STREAM related? , maybe LIQUID_INTENSITY?
    float VALUE;
    float unknown12;
    float unknown13;
    float unknown14;
    float unknown15;
    float unknown16;
    float unknown17;
    float unknown18;
    float unknown19;
    float unknown20;
    float unknown21;
    float unknown22;
    float unknown23;
    float unknown24;
    float unknown25;
    float unknown26;
    float unknown27;
    float unknown28;
    float unknown29;
    float POISONED;
    float CURSED;
    float unknown32;
    float unknown33;
    float unknown34;
    float unknown35;
    float unknown36;
    float unknown37;
    float unknown38;
    float unknown39;
    float unknown40;
    float unknown41; // all the values repeat from this point, maybe all those floats are just an array?
};
