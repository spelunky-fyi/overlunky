#include "particles_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for global_table, proxy_key_t, data_t, state
#include <string>      // for allocator, operator==, string
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max
#include <vector>      // for _Vector_const_iterator, vector

#include "particles.hpp" // for ParticleDB, ParticleEmitterInfo, ParticleEm...
#include "rpc.hpp"       // for generate_world_particles, advance_screen_pa...

template <auto MemberT>
auto MakeParticleMemberAccess()
{
    return sol::property(
        [](const Particle& p)
        { return *(p.*MemberT); },
        [](Particle& p, std::remove_pointer_t<std::decay_t<decltype(std::declval<Particle>().*MemberT)>> v)
        { *(p.*MemberT) = v; });
};

namespace NParticles
{
void register_usertypes(sol::state& lua)
{
    /// Get the [ParticleDB](#ParticleDB) details of the specified ID
    lua["get_particle_type"] = get_particle_type;
    /// Generate particles of the specified type around the specified entity uid (use e.g. `local emitter = generate_world_particles(PARTICLEEMITTER.PETTING_PET, players[1].uid)`). You can then decouple the emitter from the entity with `emitter.entity_uid = -1` and freely move it around. See the `particles.lua` example script for more details.
    lua["generate_world_particles"] = generate_world_particles;
    /// Generate particles of the specified type at a certain screen coordinate (use e.g. `local emitter = generate_screen_particles(PARTICLEEMITTER.CHARSELECTOR_TORCHFLAME_FLAMES, 0.0, 0.0)`). See the `particles.lua` example script for more details.
    lua["generate_screen_particles"] = generate_screen_particles;
    /// Advances the state of the screen particle emitter (simulates the next positions, ... of all the particles in the emitter). Only used with screen particle emitters. See the `particles.lua` example script for more details.
    lua["advance_screen_particles"] = advance_screen_particles;
    /// Renders the particles to the screen. Only used with screen particle emitters. See the `particles.lua` example script for more details.
    lua["render_screen_particles"] = render_screen_particles;
    /// Extinguish a particle emitter (use the return value of `generate_world_particles` or `generate_screen_particles` as the parameter in this function)
    lua["extinguish_particles"] = extinguish_particles;

    /// Deprecated
    /// Use `generate_world_particles`
    lua["generate_particles"] = generate_world_particles;

    /// Used in ParticleDB, [get_particle_type](#get_particle_type)
    auto particledb_type = lua.new_usertype<ParticleDB>("ParticleDB", sol::constructors<ParticleDB(ParticleDB&), ParticleDB(PARTICLEEMITTER)>());
    particledb_type["id"] = &ParticleDB::id;
    particledb_type["spawn_count_min"] = &ParticleDB::spawn_count_min;
    particledb_type["spawn_count"] = &ParticleDB::spawn_count;
    particledb_type["lifespan_min"] = &ParticleDB::lifespan_min;
    particledb_type["lifespan"] = &ParticleDB::lifespan;
    particledb_type["sheet_id"] = &ParticleDB::sheet_id;
    particledb_type["animation_sequence_length"] = &ParticleDB::animation_sequence_length;
    particledb_type["spawn_interval"] = &ParticleDB::spawn_interval;
    particledb_type["shrink_growth_factor"] = &ParticleDB::shrink_growth_factor;
    particledb_type["rotation_speed"] = &ParticleDB::rotation_speed;
    particledb_type["opacity"] = &ParticleDB::opacity;
    particledb_type["hor_scattering"] = &ParticleDB::hor_scattering;
    particledb_type["ver_scattering"] = &ParticleDB::ver_scattering;
    particledb_type["scale_x_min"] = &ParticleDB::scale_x_min;
    particledb_type["scale_x"] = &ParticleDB::scale_x;
    particledb_type["scale_y_min"] = &ParticleDB::scale_y_min;
    particledb_type["scale_y"] = &ParticleDB::scale_y;
    particledb_type["hor_deflection_1"] = &ParticleDB::hor_deflection_1;
    particledb_type["ver_deflection_1"] = &ParticleDB::ver_deflection_1;
    particledb_type["hor_deflection_2"] = &ParticleDB::hor_deflection_2;
    particledb_type["ver_deflection_2"] = &ParticleDB::ver_deflection_2;
    particledb_type["hor_velocity"] = &ParticleDB::hor_velocity;
    particledb_type["ver_velocity"] = &ParticleDB::ver_velocity;
    particledb_type["red"] = &ParticleDB::red;
    particledb_type["green"] = &ParticleDB::green;
    particledb_type["blue"] = &ParticleDB::blue;
    particledb_type["permanent"] = &ParticleDB::permanent;
    particledb_type["invisible"] = &ParticleDB::invisible;
    particledb_type["get_texture"] = &ParticleDB::get_texture;
    particledb_type["set_texture"] = &ParticleDB::set_texture;

    /// Generic type for creating particles in the game, you can make your own with [generate_world_particles](#generate_world_particles) or [generate_screen_particles](#generate_screen_particles)<br/>
    /// Used in ScreenCharacterSelect, ScreenTitle, CursedEffect, OnFireEffect, PoisonedEffect ...
    lua.new_usertype<ParticleEmitterInfo>(
        "ParticleEmitterInfo",
        "particle_type",
        &ParticleEmitterInfo::particle_type,
        "particle_type2",
        &ParticleEmitterInfo::particle_type2,
        "particle_count",
        sol::property([](ParticleEmitterInfo& e) -> uint32_t
                      { return e.emitted_particles.particle_count; }),
        "particle_count_back_layer",
        sol::property([](ParticleEmitterInfo& e) -> uint32_t
                      { return e.emitted_particles_back_layer.particle_count; }),
        "entity_uid",
        &ParticleEmitterInfo::entity_uid,
        "x",
        &ParticleEmitterInfo::x,
        "y",
        &ParticleEmitterInfo::y,
        "offset_x",
        &ParticleEmitterInfo::offset_x,
        "offset_y",
        &ParticleEmitterInfo::offset_y,
        "layer",
        &ParticleEmitterInfo::layer,
        "draw_depth",
        &ParticleEmitterInfo::draw_depth,
        "emitted_particles",
        &ParticleEmitterInfo::emitted_particles,
        "emitted_particles_back_layer",
        &ParticleEmitterInfo::emitted_particles_back_layer);

    /// Used in ParticleEmitterInfo
    lua.new_usertype<Particle>(
        "Particle",
        "x",
        MakeParticleMemberAccess<&Particle::x>(),
        "y",
        MakeParticleMemberAccess<&Particle::y>(),
        "velocityx",
        MakeParticleMemberAccess<&Particle::velocityx>(),
        "velocityy",
        MakeParticleMemberAccess<&Particle::velocityy>(),
        "color",
        MakeParticleMemberAccess<&Particle::color>(),
        "width",
        MakeParticleMemberAccess<&Particle::width>(),
        "height",
        MakeParticleMemberAccess<&Particle::height>(),
        "lifetime",
        MakeParticleMemberAccess<&Particle::lifetime>(),
        "max_lifetime",
        MakeParticleMemberAccess<&Particle::max_lifetime>());

    lua.create_named_table("PARTICLEEMITTER"
                           //, "TITLE_TORCHFLAME_SMOKE", 1
                           //, "", ...check__[particle_emitters.txt]\[game_data/particle_emitters.txt\]...
                           //, "MINIGAME_BROKENASTEROID_SMOKE", 219
    );
    for (const auto& particle : list_particles())
    {
        auto name = particle.name.substr(16, particle.name.size());
        lua["PARTICLEEMITTER"][name] = particle.id;
    }
}
}; // namespace NParticles
