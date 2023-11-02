#include "particles.hpp"

#include <algorithm>     // for sort
#include <functional>    // for equal_to
#include <list>          // for _List_iterator, _List_const_iterator
#include <new>           // for operator new
#include <type_traits>   // for move, hash
#include <unordered_map> // for unordered_map, _Umap_traits<>::allocator_type
#include <utility>       // for min, max

#include "entity.hpp"  //
#include "search.hpp"  // for get_address
#include "state.hpp"   //
#include "texture.hpp" // for get_texture, Texture

ParticleDB* particle_db_ptr()
{
    static ParticleDB* addr = (ParticleDB*)get_address("particle_emitter_db");
    return addr;
}

std::uint64_t ParticleDB::get_texture()
{
    return texture->id;
}
bool ParticleDB::set_texture(std::uint32_t texture_id)
{
    if (auto* new_texture = ::get_texture(texture_id))
    {
        texture = new_texture;
        return true;
    }
    return false;
}

EmittedParticlesInfo::Iterator EmittedParticlesInfo::begin()
{
    return Iterator{this, 0};
}
EmittedParticlesInfo::Iterator EmittedParticlesInfo::end()
{
    return Iterator{this, particle_count};
}
EmittedParticlesInfo::ConstIterator EmittedParticlesInfo::begin() const
{
    return cbegin();
}
EmittedParticlesInfo::ConstIterator EmittedParticlesInfo::end() const
{
    return cend();
}
EmittedParticlesInfo::ConstIterator EmittedParticlesInfo::cbegin() const
{
    return ConstIterator{this, 0};
}
EmittedParticlesInfo::ConstIterator EmittedParticlesInfo::cend() const
{
    return ConstIterator{this, particle_count};
}

Particle EmittedParticlesInfo::front()
{
    return (*this)[0];
}
Particle EmittedParticlesInfo::back()
{
    return (*this)[particle_count - 1];
}
const Particle EmittedParticlesInfo::front() const
{
    return (*this)[0];
}
const Particle EmittedParticlesInfo::back() const
{
    return (*this)[particle_count - 1];
}

bool EmittedParticlesInfo::empty()
{
    return particle_count == 0;
}
EmittedParticlesInfo::size_type EmittedParticlesInfo::size()
{
    return particle_count;
}

Particle EmittedParticlesInfo::operator[](const size_type idx)
{
    return static_cast<const EmittedParticlesInfo&>(*this)[idx];
}
const Particle EmittedParticlesInfo::operator[](const size_type idx) const
{
    return {
        max_lifetimes + idx,
        lifetimes + idx,
        x_positions + idx,
        y_positions + idx,
        unknown_x_positions + idx,
        unknown_y_positions + idx,
        colors + idx,
        widths + idx,
        heights + idx,
        x_velocities + idx,
        y_velocities + idx,
    };
}

ParticleDB* get_particle_type(PARTICLEEMITTER id)
{
    static std::unordered_map<uint32_t, ParticleDB*> mapping = {};
    if (mapping.size() == 0)
    {
        uint32_t current_id = 0;
        uint32_t max_entries = 250;
        uint32_t counter = 0;
        ParticleDB* db = particle_db_ptr();
        while (counter < max_entries)
        {
            if (db->id > current_id && db->id < (current_id + 10)) // allow for gaps in the id's
            {
                mapping[db->id] = db;
                current_id = db->id;
            }
            else
            {
                break;
            }
            db++;
            counter++;
        }
    }
    if (mapping.count(id) != 0)
    {
        return mapping.at(id);
    }
    return nullptr;
}

const std::vector<ParticleEmitter>& list_particles()
{
    static std::vector<ParticleEmitter> particles = {};
    if (particles.size() == 0)
    {
        auto mapOffset = get_address("particle_emitter_list");
        std::unordered_map<std::string, uint16_t>* map = reinterpret_cast<std::unordered_map<std::string, uint16_t>*>(mapOffset);
        for (const auto& [particle_name, particle_id] : *map)
        {
            particles.emplace_back(particle_name, particle_id);
        }
        std::sort(particles.begin(), particles.end(), [](ParticleEmitter& a, ParticleEmitter& b) -> bool
                  { return a.id < b.id; });
    }
    return particles;
}

ParticleDB::ParticleDB(PARTICLEEMITTER particle_id)
    : ParticleDB(*get_particle_type(particle_id)){};

ParticleEmitterInfo* generate_world_particles(PARTICLEEMITTER particle_emitter_id, uint32_t uid)
{
    static size_t offset = get_address("generate_world_particles");

    if (offset != 0)
    {
        auto entity = get_entity_ptr(uid);
        if (entity != nullptr)
        {
            auto state = get_state_ptr();
            typedef ParticleEmitterInfo* generate_particles_func(custom_vector<ParticleEmitterInfo*>*, uint32_t, Entity*);
            static generate_particles_func* gpf = (generate_particles_func*)(offset);
            return gpf(state->particle_emitters, particle_emitter_id, entity);
        }
    }
    return nullptr;
}

ParticleEmitterInfo* generate_screen_particles(PARTICLEEMITTER particle_emitter_id, float x, float y)
{
    static size_t offset = get_address("generate_screen_particles");

    if (offset != 0)
    {
        typedef ParticleEmitterInfo* generate_particles_func(uint32_t, float, float, size_t);
        static generate_particles_func* gpf = (generate_particles_func*)(offset);
        return gpf(particle_emitter_id, x, y, 0);
    }
    return nullptr;
}

void advance_screen_particles(ParticleEmitterInfo* particle_emitter)
{
    static size_t offset = get_address("advance_screen_particles");

    if (offset != 0)
    {
        typedef void advance_particles_func(ParticleEmitterInfo*);
        static advance_particles_func* apf = (advance_particles_func*)(offset);
        apf(particle_emitter);
    }
}

void render_screen_particles(ParticleEmitterInfo* particle_emitter)
{
    static size_t offset = get_address("render_screen_particles");

    if (offset != 0)
    {
        typedef void render_particles_func(ParticleEmitterInfo*, size_t, size_t, size_t);
        static render_particles_func* rpf = (render_particles_func*)(offset);
        rpf(particle_emitter, 0, 0, 0);
    }
}

void extinguish_particles(ParticleEmitterInfo* particle_emitter)
{
    // removing from state only applies to world emitters, but it just won't find the screen one in the vector, so no big deal
    auto state = get_state_ptr();
    std::erase(*state->particle_emitters, particle_emitter);

    using generic_free_func = void(void*);
    static generic_free_func* generic_free = (generic_free_func*)get_address("generic_free");

    if (particle_emitter != nullptr)
    {
        generic_free(particle_emitter->emitted_particles.memory);
        generic_free(particle_emitter->emitted_particles_back_layer.memory);
        generic_free(particle_emitter);
    }
}
