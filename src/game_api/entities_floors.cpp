#include "entities_floors.hpp"

#include "rpc.hpp"
#include "state.hpp"

#include <numbers>

void Floor::add_decoration(FLOOR_SIDE side)
{
    int32_t decoration_entity_type = get_decoration_entity_type();
    if (decoration_entity_type == -1)
    {
        return;
    }

    auto state = State::get();
    auto layer_ptr = state.layer(layer);
    add_decoration_opt(side, decoration_entity_type, layer_ptr);
}
void Floor::fix_decorations(bool fix_also_neighbors, bool fix_styled_floor)
{
    if (!fix_styled_floor && is_styled())
    {
        return;
    }

    int32_t decoration_entity_type = get_decoration_entity_type();
    if (decoration_entity_type == -1)
    {
        return;
    }

    const auto [x, y] = position_self();

    static float offsets[4][3]{
        {0.0f, +1.0f, std::numbers::pi_v<float> * 0.0f},
        {0.0f, -1.0f, std::numbers::pi_v<float> * 0.0f},
        {-1.0f, 0.0f, std::numbers::pi_v<float> * 1.0f},
        {+1.0f, 0.0f, std::numbers::pi_v<float> * 0.0f}};

    auto state = State::get();
    auto layer_ptr = state.layer(layer);
    for (size_t i = 0; i < 4; i++)
    {
        auto [x_off, y_off, angle] = offsets[i];

        auto* floor = layer_ptr->get_grid_entity_at(x + x_off, y + y_off)->as<Floor>();
        if ((floor != nullptr) && floor->type->id == type->id)
        {
            remove_decoration(FLOOR_SIDE(i));
        }
        else if (decos[i] == -1)
        {
            add_decoration_opt(FLOOR_SIDE(i), decoration_entity_type, layer_ptr);
        }

        if (floor != nullptr && fix_also_neighbors)
        {
            floor->remove_decoration(FLOOR_SIDE(i % 2 == 0 ? i + 1 : i - 1));
        }
    }
}
void Floor::remove_decoration(FLOOR_SIDE side)
{
    if (decos[side] >= 0)
    {
        if (Entity* deco = get_entity_ptr(decos[side]))
        {
            deco->kill(false, nullptr);
        }
        decos[side] = -1;
    }
}

void Floor::add_decoration_opt(FLOOR_SIDE side, int32_t decoration_entity_type, Layer* layer_ptr)
{
    static float offsets[4][3]{
        {0.0f, +0.5f, std::numbers::pi_v<float> *0.0f},
        {0.0f, -0.5f, std::numbers::pi_v<float> *0.0f},
        {-0.5f, 0.0f, std::numbers::pi_v<float> *1.0f},
        {+0.5f, 0.0f, std::numbers::pi_v<float> *0.0f} };

    if (decos[side] == -1)
    {
        auto [x_off, y_off, angle] = offsets[side];

        // TODO: Fix BORDERTILE offsets and corners?
        if (type->id <= 3)
        {
            if (side == TOP)
            {
                y_off += 0.1f;
            }
            else if (side == BOTTOM)
            {
                y_off -= 0.1f;
            }
        }

        Entity* deco = layer_ptr->spawn_entity_over(decoration_entity_type, this, x_off, y_off);
        deco->angle = angle;
        deco->animation_frame = get_decoration_animation_frame(side);
        deco->set_texture(get_texture());
        decos[side] = deco->uid;
    }
}

bool Floor::is_styled()
{
    switch (type->id)
    {
    case 0x47: // FLOOR_DUAT_ALTAR
    case 0x70: // FLOORSTYLED_DUAT
    case 0x67: // FLOORSTYLED_STONE
    case 0x68: // FLOORSTYLED_TEMPLE
    case 0x69: // FLOORSTYLED_PAGODA
    case 0x6a: // FLOORSTYLED_BABYLON
    case 0x6b: // FLOORSTYLED_SUNKEN
    case 0x6c: // FLOORSTYLED_BEEHIVE
    case 0x6d: // FLOORSTYLED_VLAD
    case 0x6e: // FLOORSTYLED_COG
    case 0x6f: // FLOORSTYLED_MOTHERSHIP
    case 0x72: // FLOORSTYLED_GUTS
    case 0x71: // FLOORSTYLED_PALACE
    case 0x31: // IDOL_TRAP_CEILING
    case 0x5d: // FLOOR_PEN
    case 0x66: // FLOORSTYLED_MINEWOOD
        return true;
    default:
        return false;
    }
}
int32_t Floor::get_decoration_entity_type()
{
    [[maybe_unused]] static auto do_once = []()
    {
        assert(0x1 == to_id("ENT_TYPE_FLOOR_BORDERTILE"));
        assert(0x2 == to_id("ENT_TYPE_FLOOR_BORDERTILE_METAL"));
        assert(0x3 == to_id("ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS"));
        assert(0x4 == to_id("ENT_TYPE_FLOOR_GENERIC"));
        assert(0x5 == to_id("ENT_TYPE_FLOOR_SURFACE"));
        assert(0x6 == to_id("ENT_TYPE_FLOOR_SURFACE_COVER"));
        assert(0xa == to_id("ENT_TYPE_FLOOR_JUNGLE"));
        assert(0x31 == to_id("ENT_TYPE_IDOL_TRAP_CEILING"));
        assert(0x47 == to_id("ENT_TYPE_FLOOR_DUAT_ALTAR"));
        assert(0x67 == to_id("ENT_TYPE_FLOORSTYLED_STONE"));
        assert(0x68 == to_id("ENT_TYPE_FLOORSTYLED_TEMPLE"));
        assert(0x69 == to_id("ENT_TYPE_FLOORSTYLED_PAGODA"));
        assert(0x6a == to_id("ENT_TYPE_FLOORSTYLED_BABYLON"));
        assert(0x6b == to_id("ENT_TYPE_FLOORSTYLED_SUNKEN"));
        assert(0x6c == to_id("ENT_TYPE_FLOORSTYLED_BEEHIVE"));
        assert(0x6d == to_id("ENT_TYPE_FLOORSTYLED_VLAD"));
        assert(0x6e == to_id("ENT_TYPE_FLOORSTYLED_COG"));
        assert(0x6f == to_id("ENT_TYPE_FLOORSTYLED_MOTHERSHIP"));
        assert(0x71 == to_id("ENT_TYPE_FLOORSTYLED_PALACE"));
        assert(0x72 == to_id("ENT_TYPE_FLOORSTYLED_GUTS"));
    };

    switch (type->id)
    {
    case 0x1: // FLOOR_BORDERTILE
    case 0x2: // FLOOR_BORDERTILE_METAL
    case 0x3: // FLOOR_BORDERTILE_OCTOPUS
        return 0x73;
    case 0x4: // FLOOR_GENERIC
        return 0x75;
    case 0x5: // FLOOR_SURFACE
        return 0x76;
    case 0x6: // FLOOR_SURFACE_COVER
        return 0x77;
    case 0xa: // FLOOR_JUNGLE
        return 0x77;
    case 0x31: // IDOL_TRAP_CEILING
    case 0x5d: // FLOOR_PEN
    case 0x66: // FLOORSTYLED_MINEWOOD
        return 0x78;
    case 0x47: // FLOOR_DUAT_ALTAR
    case 0x70: // FLOORSTYLED_DUAT
        return 0xab;
    case 0x67: // FLOORSTYLED_STONE
        return 0x7b;
    case 0x68: // FLOORSTYLED_TEMPLE
        return 0x7c;
    case 0x69: // FLOORSTYLED_PAGODA
        return 0x7d;
    case 0x6a: // FLOORSTYLED_BABYLON
        return 0x7f;
    case 0x6b: // FLOORSTYLED_SUNKEN
        return 0x7e;
    case 0x6c: // FLOORSTYLED_BEEHIVE
        return 0xa2;
    case 0x6d: // FLOORSTYLED_VLAD
        return 0xa5;
    case 0x6e: // FLOORSTYLED_COG
        return 0xa9;
    case 0x6f: // FLOORSTYLED_MOTHERSHIP
        return 0xaa;
    case 0x71: // FLOORSTYLED_PALACE
        return 0xaf;
    case 0x72: // FLOORSTYLED_GUTS
        return 0xb3;
    }
    return -1;
}
uint8_t Floor::get_decoration_animation_frame(FLOOR_SIDE side)
{
    bool has_variants = false;
    bool styled = false;

    switch (type->id)
    {
    default:
        return 0;
    case 0x1: // FLOOR_BORDERTILE
    {
        uint8_t animation_frame = 2;
        if (side == TOP)
        {
            animation_frame += 8;
        }
        if (side == BOTTOM)
        {
            animation_frame += 16;
        }
        return animation_frame + rand() % 3;
    }
    case 0x2: // FLOOR_BORDERTILE_METAL
    {
        if (side == TOP)
        {
            return 52 + rand() % 3;
        }
        if (side == BOTTOM)
        {
            return 60 + rand() % 3;
        }
        return 31 + (rand() % 2 ? 0 : 8);
    }
    case 0x3: // FLOOR_BORDERTILE_OCTOPUS
    {
        uint8_t animation_frame = 26;
        if (side == TOP)
        {
            animation_frame += 8;
        }
        if (side == BOTTOM)
        {
            animation_frame += 16;
        }
        return animation_frame + rand() % 3;
    }
    case 0x6: // FLOOR_SURFACE_COVER
    {
        if (side != TOP)
        {
            return 0;
        }

        return 14 + rand() % 3;
    }

    case 0x47: // FLOOR_DUAT_ALTAR
    case 0x70: // FLOORSTYLED_DUAT
    case 0x67: // FLOORSTYLED_STONE
    case 0x68: // FLOORSTYLED_TEMPLE
    case 0x69: // FLOORSTYLED_PAGODA
    case 0x6a: // FLOORSTYLED_BABYLON
    case 0x6b: // FLOORSTYLED_SUNKEN
    case 0x6c: // FLOORSTYLED_BEEHIVE
    case 0x6d: // FLOORSTYLED_VLAD
    case 0x6e: // FLOORSTYLED_COG
    case 0x6f: // FLOORSTYLED_MOTHERSHIP
    case 0x72: // FLOORSTYLED_GUTS
        has_variants = true;
        [[fallthrough]];
    case 0x71: // FLOORSTYLED_PALACE
    case 0x31: // IDOL_TRAP_CEILING
    case 0x5d: // FLOOR_PEN
    case 0x66: // FLOORSTYLED_MINEWOOD
        styled = true;
        break;
    case 0x4: // FLOOR_GENERIC
    case 0x5: // FLOOR_SURFACE
    case 0xa: // FLOOR_JUNGLE
        has_variants = true;
        break;
    }

    if (styled)
    {
        uint8_t animation_frame = 55;
        if (side == TOP)
            animation_frame += 10;
        if (side == BOTTOM)
            animation_frame += 20;
        if (has_variants)
            animation_frame += rand() % 3;
        return animation_frame;
    }

    uint8_t animation_frame = 65;
    if (side == TOP)
        animation_frame += 12;
    if (side == BOTTOM)
        animation_frame += 24;
    if (has_variants)
        animation_frame += rand() % 3;
    return animation_frame;
}
