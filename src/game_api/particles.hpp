#pragma once

#include "texture.hpp"
#include <cstdint>
#include <string>
#include <vector>

struct ParticleDB
{
    uint32_t id;
    int32_t spawn_count_min;            // minimum amount of particles to spawn in an iteration, actual value is random between this value and spawn_count (-1 = no lower bound, uses spawn_count)
    uint32_t spawn_count;               // total amount of particles to spawn for 1 iteration (check with PETTING_PET or MOUNT_TAMED, amount of hearts shown)
    int32_t lifespan_min;               // minimum lifespan of a particle, actual value is random between this value and lifespan (-1 = no lower bound, uses lifespan)
    uint32_t lifespan;                  // amount of frames
    uint32_t sheet_id;                  // zero based index of the sprite in the texture sheet DDS file
    uint32_t animation_sequence_length; // amount of sequential sprites in the animation sequence
    uint32_t unknown7;
    uint32_t unknown8;
    float spawn_interval;       // how much time it takes to spawn the next particle
    float shrink_growth_factor; // negative makes particles shrink over time, positive grow
    float rotation_speed;
    float opacity;
    float max_velocity;    // default = FLT_MAX (no limit)
    float y_scale_related; // unknown
    float hor_scattering;
    float ver_scattering;
    float scale_x_min; // scale_x = ((scale_x - scale_x_min) * (rand(0-100)/100)) + scale_x_min
    float scale_y_min;
    float scale_x;
    float scale_y;
    float hor_deflection_1;
    float ver_deflection_1;
    float hor_deflection_2;
    float ver_deflection_2;
    float hor_velocity;
    float ver_velocity;
    uint8_t cyan;
    uint8_t magenta;
    uint8_t yellow;
    uint8_t unknown28;
    Texture* texture;
    size_t unknown29; // code pointer
    size_t unknown30; // code pointer
    uint8_t unknown31a;
    uint8_t unknown31b;
    uint8_t unknown31c;
    uint8_t unknown31d;
    float unknown32;
    float unknown33;
    bool permanent; // whether you have to constantly refresh, or whether it does so on its own (e.g. petting hearts vs dustwall)
    bool unknown35;
    bool unknown36;
    bool unknown37;
    bool invisible; // used for shockwaves and warping
    bool unknown39;
    uint8_t unknown40;
    uint8_t unknown41;
    uint32_t unknown42;

    std::uint32_t get_texture();
    bool set_texture(std::uint32_t texture_id);
};

struct ParticleEmitter
{
    std::string name;
    uint16_t id;

    ParticleEmitter(const std::string& name_, uint64_t id_)
        : name(name_), id(id_)
    {
    }
};

struct ParticleEmitterInfo
{
    ParticleDB* particle_type;
    uint8_t particle_count;
};

ParticleDB* get_particle_type(uint32_t id);
const std::vector<ParticleEmitter>& list_particles();
