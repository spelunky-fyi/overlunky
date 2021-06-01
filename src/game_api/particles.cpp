#include "particles.hpp"
#include "logger.h"
#include "memory.hpp"
#include "render_api.hpp"

size_t particle_db_ptr()
{
    static size_t cached_particledb_ptr;
    ONCE(size_t)
    {
        auto mem = Memory::get();
        std::string pattern = "\xB8\x01\x00\x00\x00\x66\x89\x05"s;
        res = mem.at_exe(decode_pc(mem.exe(), find_inst(mem.exe(), pattern, find_inst(mem.exe(), pattern, mem.after_bundle) + 1) + 5));
        return res;
    }
}

std::uint32_t ParticleDB::get_texture()
{
    return texture->id;
}
bool ParticleDB::set_texture(std::uint32_t texture_id)
{
    if (auto* new_texture = RenderAPI::get().get_texture(texture_id))
    {
        texture = new_texture;
        return true;
    }
    return false;
}

ParticleDB* get_particle_type(uint32_t id)
{
    static std::unordered_map<uint16_t, ParticleDB*> mapping = {};
    if (mapping.size() == 0)
    {
        uint32_t current_id = 0;
        uint32_t max_entries = 250;
        uint32_t counter = 0;
        ParticleDB* db = reinterpret_cast<ParticleDB*>(particle_db_ptr());
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
        auto mem = Memory::get();
        auto mapOffset = mem.at_exe(decode_pc(mem.exe(), find_inst(mem.exe(), "\xA8\x16\x00\x00\x48\x8D\x0D"s, mem.after_bundle) + 4));
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