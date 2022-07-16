#include "entities_liquids.hpp"

#include "state.hpp"         // for State
#include "state_structs.hpp" // for LiquidPhysicsEngine

uint32_t Liquid::get_liquid_flags()
{
    auto liquid_engine = State::get().get_correct_liquid_engine(type->id);
    return liquid_engine->liquid_flags[*liquid_id];
}

void Liquid::set_liquid_flags(uint32_t liquid_flags)
{
    auto liquid_engine = State::get().get_correct_liquid_engine(type->id);
    liquid_engine->liquid_flags[*liquid_id] = liquid_flags;
}
