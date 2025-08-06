#include "entities_liquids.hpp"

#include "heap_base.hpp"     // for HeapBase
#include "liquid_engine.hpp" // for LiquidPhysicsEngine

uint32_t Liquid::get_liquid_flags()
{
    auto liquid_engine = HeapBase::get().liquid_physics()->get_correct_liquid_engine(type->id);
    return liquid_engine->liquid_flags[*liquid_id];
}

void Liquid::set_liquid_flags(uint32_t liquid_flags)
{
    auto liquid_engine = HeapBase::get().liquid_physics()->get_correct_liquid_engine(type->id);
    liquid_engine->liquid_flags[*liquid_id] = liquid_flags;
}
