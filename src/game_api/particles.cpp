#include "particles.hpp"
#include "logger.h"
#include "memory.hpp"
#include "render_api.hpp"

size_t particle_db_ptr()
{
    // Rev.Eng.: locate the particles init function (see `list_particles()` below). At the start you will see the value 1
    // being written to a memory address. This is the start of the particle DB. Note that this function builds up the entire
    // DB so you have to step into it quite a bit to see the particle DB start forming in memory.
    // The size of a particle emitter is 0xA0
    ONCE(size_t)
    {
        auto mem = Memory::get();
        std::string pattern = "\xFE\xFF\xFF\xFF\x66\xC7\x05"s;
        res = mem.at_exe(decode_pc(mem.exe(), find_inst(mem.exe(), pattern, mem.after_bundle) + 4, 3, 2));
        return res;
    }
}

std::uint64_t ParticleDB::get_texture()
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
    static std::unordered_map<uint32_t, ParticleDB*> mapping = {};
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
    // Rev.Eng.: find string reference PARTICLEEMITTER_TITLE_TORCHFLAME_SMOKE, used in particles init function, it processes all particle emitters
    // and inserts all into the unordered_map. Look at function call(s) shortly after string reference (insertion in map), it will contain a
    // reference to the start of the map, or the `size` which it increments. The unordered map insertion function can be recognized by a little
    // stub with some jumps to `Xlength_error` at the end.
    static std::vector<ParticleEmitter> particles = {};
    if (particles.size() == 0)
    {
        auto mem = Memory::get();
        auto mapOffset = mem.at_exe(decode_pc(mem.exe(), find_inst(mem.exe(), "\xF3\x48\x0F\x2A\xC0\xF3\x0F\x58\xC0\xF3\x0F\x10\x0D"s, mem.after_bundle) + 9, 4));
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
