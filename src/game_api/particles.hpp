#pragma once

#include "texture.hpp"
#include <cstdint>
#include <string>
#include <vector>

struct ParticleDB
{
    uint32_t id;
    int32_t unknown1;
    uint32_t unknown2;
    int32_t unknown3;
    uint32_t unknown4;
    uint32_t sheet_id;                // zero based index of the sprite in the texture sheet DDS file
    uint32_t trailing_texture_rel_id; // not sure
    uint32_t main_trailing_count;     // not sure
    uint32_t unknown8;
    float amplitude;            // not sure
    float shrink_growth_factor; // negative makes particles shrink over time, positive grow
    float rotation_speed;       // not sure
    float opacity;
    uint32_t unknown_mask;
    float y_scale_related; // unknown
    float hor_scattering;
    float ver_scattering;
    float spin_and_scale_1; // unknown
    float spin_and_scale_2; // unknown
    float scale_x;
    float scale_y;
    float unknown21;
    float unknown22;
    float unknown23;
    float unknown24;
    float hor_velocity;
    float ver_velocity;
    float unknown27;
    Texture* texture;
    size_t unknown29; // code pointer
    size_t unknown30; // code pointer
    uint32_t flags;
    float unknown32;
    float unknown33;
    uint8_t unknown34;
    uint8_t unknown35;
    uint8_t unknown36;
    uint8_t unknown37;
    uint8_t unknown38;
    uint8_t unknown39;
    uint8_t unknown40;
    uint8_t unknown41;
    uint32_t unknown42;
};

struct ParticleEmitter
{
    std::string name;
    uint16_t id;

    ParticleEmitter(const std::string& name_, uint64_t id_) : name(name_), id(id_)
    {
    }
};

ParticleDB* get_particle_type(uint32_t id);
const std::vector<ParticleEmitter>& list_particles();