#include "particles.hpp"

#include <algorithm>     // for sort
#include <functional>    // for equal_to
#include <list>          // for _List_iterator, _List_const_iterator
#include <new>           // for operator new
#include <type_traits>   // for move, hash
#include <unordered_map> // for unordered_map, _Umap_traits<>::allocator_type
#include <utility>       // for min, max

#include "search.hpp"  // for get_address
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

ParticleDB* get_particle_type(uint32_t id)
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
