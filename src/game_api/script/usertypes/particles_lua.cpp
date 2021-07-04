#include "particles_lua.hpp"

#include "particles.hpp"
#include "rpc.hpp"

#include <sol/sol.hpp>

namespace NParticles
{
void register_usertypes(sol::state& lua)
{
    /// Get the [ParticleDB](#particledb) details of the specified ID
    lua["get_particle_type"] = get_particle_type;
    /// Generate particles of the specified type around the specified entity uid (use e.g. generate_particles(PARTICLEEMITTER.PETTING_PET, player.uid))
    lua["generate_particles"] = generate_particles;

    lua.new_usertype<ParticleDB>(
        "ParticleDB",
        "id",
        &ParticleDB::id,
        "spawn_count_min",
        &ParticleDB::spawn_count_min,
        "spawn_count",
        &ParticleDB::spawn_count,
        "lifespan_min",
        &ParticleDB::lifespan_min,
        "lifespan",
        &ParticleDB::lifespan,
        "sheet_id",
        &ParticleDB::sheet_id,
        "animation_sequence_length",
        &ParticleDB::animation_sequence_length,
        "spawn_interval",
        &ParticleDB::spawn_interval,
        "shrink_growth_factor",
        &ParticleDB::shrink_growth_factor,
        "rotation_speed",
        &ParticleDB::rotation_speed,
        "opacity",
        &ParticleDB::opacity,
        "hor_scattering",
        &ParticleDB::hor_scattering,
        "ver_scattering",
        &ParticleDB::ver_scattering,
        "scale_x_min",
        &ParticleDB::scale_x_min,
        "scale_x",
        &ParticleDB::scale_x,
        "scale_y_min",
        &ParticleDB::scale_y_min,
        "scale_y",
        &ParticleDB::scale_y,
        "hor_deflection_1",
        &ParticleDB::hor_deflection_1,
        "ver_deflection_1",
        &ParticleDB::ver_deflection_1,
        "hor_deflection_2",
        &ParticleDB::hor_deflection_2,
        "ver_deflection_2",
        &ParticleDB::ver_deflection_2,
        "hor_velocity",
        &ParticleDB::hor_velocity,
        "ver_velocity",
        &ParticleDB::ver_velocity,
        "red",
        &ParticleDB::red,
        "green",
        &ParticleDB::green,
        "blue",
        &ParticleDB::blue,
        "permanent",
        &ParticleDB::permanent,
        "invisible",
        &ParticleDB::invisible,
        "get_texture",
        &ParticleDB::get_texture,
        "set_texture",
        &ParticleDB::set_texture);

    lua.create_named_table("PARTICLEEMITTER"
                           //, "TITLE_TORCHFLAME_SMOKE", 1
                           //, "", ...check__particle_emitters.txt__output__by__Overlunky...
                           //, "MINIGAME_BROKENASTEROID_SMOKE", 219
    );
    for (const auto& particle : list_particles())
    {
        auto name = particle.name.substr(16, particle.name.size());
        lua["PARTICLEEMITTER"][name] = particle.id;
    }
}
}; // namespace NParticles
