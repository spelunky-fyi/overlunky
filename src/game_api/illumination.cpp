#include "illumination.hpp"

#include <type_traits>

#include "color.hpp"  // for Color
#include "entity.hpp" // for Entity
#include "math.hpp"   // for Vec2
#include "search.hpp" // for get_address
#include "state.hpp"  // for get_state_ptr

Illumination* create_illumination(Vec2 pos, Color col, LIGHT_TYPE type, float size, uint8_t light_flags, int32_t uid, LAYER layer)
{
    static size_t offset = get_address("generate_illumination");

    if (offset != 0)
    {
        auto state = get_state_ptr();

        typedef Illumination* create_illumination_func(custom_vector<Illumination*>*, Vec2*, Color, LIGHT_TYPE, float, uint8_t light_flags, int32_t uid, uint8_t layer);
        static create_illumination_func* cif = (create_illumination_func*)(offset);
        // enum_to_layer here does not use offset which you could argue should be used, since this function is comparable with spawn type function
        auto emitted_light = cif(state->lightsources, &pos, std::move(col), type, size, light_flags, uid, enum_to_layer(layer));
        return emitted_light;
    }
    return nullptr;
}

Illumination* create_illumination(Color color, float size, float x, float y)
{
    return create_illumination(Vec2{x, y}, std::move(color), LIGHT_TYPE::NONE, size, 0x20, -1, LAYER::FRONT);
}

Illumination* create_illumination(Color color, float size, int32_t uid)
{
    auto entity = get_entity_ptr(uid);
    if (entity != nullptr)
    {
        return create_illumination(entity->abs_position(), std::move(color), LIGHT_TYPE::FOLLOW_ENTITY, size, 0x20, uid, (LAYER)entity->layer);
    }
    return nullptr;
}

void refresh_illumination(Illumination* illumination)
{
    illumination->timer = HeapBase::get().frame_count();
}
