#include "particles.hpp"
#include "memory.hpp"
#include "logger.h"

size_t particle_db_ptr()
{
    static size_t cached_particledb_ptr;
    ONCE(size_t)
    {
        auto mem = Memory::get();
        std::string pattern = "\xB8\x01\x00\x00\x00\x66\x89\x05"s;
        res = mem.at_exe(
            decode_pc(mem.exe(),
                find_inst(mem.exe(), pattern,
                    find_inst(mem.exe(), pattern, mem.after_bundle) + 1) + 5));
        return res;
    }
}

ParticleDB* get_particle_type(uint32_t id)
{
    static std::unordered_map<uint16_t, ParticleDB*> mapping = {};
    if ( mapping.size() == 0 )
    {
        uint32_t current_id = 0;
        uint32_t max_entries = 250;
        uint32_t counter = 0;
        ParticleDB* db = reinterpret_cast<ParticleDB*>(particle_db_ptr());
        while ( counter < max_entries )
        {
            if ( db->id > current_id && db->id < (current_id + 10)) // allow for gaps in the id's
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
    if ( mapping.count(id) != 0 )
    {
        return mapping.at(id);
    }
    return nullptr;
}
