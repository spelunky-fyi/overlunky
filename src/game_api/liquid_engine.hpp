#pragma once

#include <cstddef> // fpr size_t
#include <cstdint>
#include <list>    // for std::list
#include <utility> // for pair

#include "containers/custom_map.hpp"
#include "containers/custom_vector.hpp"
#include "layer.hpp" // for g_level_max_x and g_level_max_y
#include "math.hpp"  // for Vec2

class Entity;

struct LiquidPhysicsEngine
{
    bool pause_physics;
    uint8_t padding[3];
    int32_t physics_tick_timer; /* unsure */
    int32_t unknown1;
    int32_t unknown2;
    int8_t unknown3;
    int8_t unknown4;
    int8_t unknown5;
    int8_t unknown6;
    int8_t unknown_7;
    int8_t unknown8;
    int8_t unknown9;
    int8_t unknown10;
    uint32_t unknown11;
    float unknown12;
    float blob_size;
    float weight;
    float unknown15;
    uint32_t entity_count;
    uint32_t allocated_size;
    uint32_t unk23; // padding probably
    // this is actually a pre C++11 version of std::list, which is different from current one!
    std::pair<size_t, size_t> unk1; // seams to be empty, or have one element 0?
    uint32_t resize_value;          // used to resize the arrays?
    uint32_t unk3b;                 // padding probably

    // this is actually a pre C++11 version of std::list, which is different from current one!
    std::pair<size_t, size_t> liquid_ids; // std::list<int32_t>
    // this is actually a pre C++11 version of std::list, which is different from current one!
    std::pair<size_t, size_t> unknown44; // std::list<int32_t> all of them are -1
    // this is actually a pre C++11 version of std::list, but the iterators work the same way
    std::list<int32_t>::const_iterator* list_liquid_ids; // list of all iterators of liquid_ids?
    int32_t unknown45a;                                  // size related for the array above
    int32_t unknown45b;                                  // padding
    uint32_t* liquid_flags;                              // array
    int32_t unknown47a;                                  // size related for the array above
    int32_t unknown47b;                                  // padding
    Vec2* entity_coordinates;                            // array
    int32_t unknown49a;                                  // size related for the array above
    int32_t unknown49b;                                  // padding
    Vec2* entity_velocities;                             // array
    int32_t unknown51a;                                  // size related for the array above
    int32_t unknown51b;                                  // padding
    std::pair<float, float>* unknown52;                  // not sure about the type
    std::pair<float, float>* unknown53;
    size_t unknown54;
    std::pair<float, float>* unknown55;
    int64_t unknown56;
    int64_t unknown57;
    int64_t unknown58;
    int64_t unknown59;
    size_t unknown60;
    Entity*** unknown61; // it's actually array of pointers to some struct, but the entity is first in that struct
    size_t unknown61a;   // stuff for array above
    char skip[256];
    float unknown95;     // LiquidParam->unknown3
    float cohesion;      // LiquidParam->cohesion?, surface tension? setting it to -1 makes the blobs repel each other
    float gravity;       // LiquidParam->gravity
    float unknown96;     // LiquidParam->unknown6
    float unknown97a;    // LiquidParam->unknown7
    float agitation;     // LiquidParam->agitation
    float unknown98a;    // LiquidParam->unknown9
    float unknown98b;    // LiquidParam->unknown10
    float unknown99a;    // LiquidParam->unknown11
    float unknown99b;    // LiquidParam->unknown12
    float unknown100a;   // LiquidParam->unknown13
    float unknown100b;   // LiquidParam->unknown14
    float unknown101a;   // LiquidParam->unknown15
    float unknown101b;   // LiquidParam->unknown16
    float unknown102a;   // LiquidParam->unknown17
    float unknown102b;   // LiquidParam->unknown18
    float unknown103a;   // LiquidParam->unknown19
    int32_t unknown103b; // LiquidParam->unknown20
    float unknown104a;   // LiquidParam->unknown21
    int32_t unknown104b; // LiquidParam->unknown22
    float unknown105a;   // LiquidParam->unknown23
    int32_t unknown105b; // LiquidParam->unknown24
    size_t unknown106;
    size_t unknown107;
    int64_t unknown108;
    int64_t unknown109;
};

struct LiquidPhysicsParams
{
    int32_t shader_type; // ? can also be flags, as for water, any value with bit one is fine
    uint8_t unknown2;    // shader related, shader id maybe?
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    float unknown3;
    float cohesion; // negative number makes the liquid balls come apart more easily?
    float gravity;  // negative number to invert gravity
    float unknown6;
    float unknown7;
    float agitation; // is agitation the right word? for me is just how bouncy the liquid is
    float unknown9;  // starts going nuts at around 2.70, pressure force? it seam to only matter at spawn, when there is a lot of liquid in one place
    float unknown10;
    float unknown11;
    float unknown12;
    float unknown13;
    float unknown14;
    float unknown15;
    float unknown16;
    float unknown17;
    float unknown18;
    float unknown19;
    uint32_t unknown20;
    float unknown21;
    uint32_t unknown22;
    float unknown23;
    uint32_t unknown24;
};

struct LiquidTileSpawnData
{
    uint32_t liquid_flags; // 2 - lava_interaction? crashes the game if no lava is present, 3 - pause_physics, 6 - low_agitation?, 7 - high_agitation?, 8 - high_surface_tension?, 9 - low_surface_tension?, 11 - high_bounce?, 12 - low_bounce?
    float last_spawn_x;
    float last_spawn_y;
    float spawn_velocity_x;
    float spawn_velocity_y;
    uint32_t unknown31;
    uint32_t unknown32;
    uint32_t unknown33;
    size_t unknown34;                  // MysteryLiquidPointer2 in plugin, contains last spawn entity
    size_t unknown35;                  // DataPointer? seam to get access validation if you change to something
    uint32_t liquidtile_liquid_amount; // how much liquid will be spawned from tilecode, 1=1x2, 2=2x3, 3=3x4 etc.
    float blobs_separation;
    int32_t unknown39; // is the last 4 garbage? seams not accessed
    float unknown40;
    float unknown41;
    uint32_t unknown42;
};

struct LiquidPool
{
    LiquidPhysicsParams physics_defaults;
    LiquidPhysicsEngine* physics_engine;
    LiquidTileSpawnData tile_spawn_data;
};

struct LiquidLake
{
    uint32_t position1;
    uint32_t position2;
    uint32_t position3;
    uint32_t lake_type;
    Entity* impostor_lake;
};

// Water blobs increase the number by 2 on the grid, while lava blobs increase it by 3. The maximum is 6
// Coarse water increase the number by 3, coarse and stagnant lava by 6. Combinations of both normal and coarse can make the number higher than 6
struct LiquidAmounts
{
    uint8_t lava;
    uint8_t water;
};

struct LiquidPhysics
{
    size_t unknown1; // MysteryLiquidPointer1 in plugin, collision with floors/activefloors related
    union
    {
        std::array<LiquidPool, 5> pools;
        struct
        {
            LiquidPhysicsParams water_physics_defaults;
            LiquidPhysicsEngine* water_physics_engine;
            LiquidTileSpawnData water_tile_spawn_data;
            LiquidPhysicsParams coarse_water_physics_defaults;
            LiquidPhysicsEngine* coarse_water_physics_engine;
            LiquidTileSpawnData coarse_water_tile_spawn_data;
            LiquidPhysicsParams lava_physics_defaults;
            LiquidPhysicsEngine* lava_physics_engine;
            LiquidTileSpawnData lava_tile_spawn_data;
            LiquidPhysicsParams coarse_lava_physics_defaults;
            LiquidPhysicsEngine* coarse_lava_physics_engine;
            LiquidTileSpawnData coarse_lava_tile_spawn_data;
            LiquidPhysicsParams stagnant_lava_physics_defaults;
            LiquidPhysicsEngine* stagnant_lava_physics_engine;
            LiquidTileSpawnData stagnant_lava_tile_spawn_data;
        };
    };
    custom_map<std::pair<uint8_t, uint8_t>, size_t*>* floors; // key is a grid position, the struct seams to be the same as in push_blocks
    custom_map<uint32_t, size_t*>* push_blocks;               // key is uid, not sure about the struct it points to (it's also possible that the value is 2 pointers)
    custom_vector<LiquidLake> impostor_lakes;                 //
    uint32_t total_liquid_spawned;                            // Total number of spawned liquid entities, all types.
    uint32_t unknown8;                                        // padding probably

    LiquidAmounts (*liquids_by_third_of_tile)[g_level_max_y * 3][g_level_max_x * 3]; // array byte* game allocates 0x2F9E8 bytes for it ((126 * 3) * (86 * 3) * 2 : y, x, liquid_type).
                                                                                     // always allocates after the LiquidPhysics

    uint32_t total_liquid_spawned2; // Same as total_liquid_spawned?
    bool unknown12;                 // if false, I think the game should check for liquids by looking for liquid entities rather than using the previous liquids array. Is set to true by the game actively
    uint8_t padding12a;
    uint8_t padding12b;
    uint8_t padding12c;
    uint32_t unknown13;

    LiquidPhysicsEngine* get_correct_liquid_engine(ENT_TYPE ent) const;
    void remove_liquid_oob();
};
