#include "entities_floors.hpp"

#include <assert.h> // for assert
#include <cmath>    // for floor, abs
#include <cstdlib>  // for rand, abs, size_t

#include "layer.hpp"     // for EntityList, EntityList::Range, Layer, Entit...
#include "movable.hpp"   // for Movable
#include "spawn_api.hpp" // for spawn_entity_over
#include "state.hpp"     // for State
#include "texture.hpp"   // for Texture

void Floor::fix_border_tile_animation()
{
    if (type->id == 3)
    {
        const uint32_t random_idx = rand() % 5;
        switch (random_idx)
        {
        default:
        case 0:
            animation_frame = 50;
            break;
        case 1:
            animation_frame = 51;
            break;
        case 2:
            animation_frame = 58;
            break;
        case 3:
            animation_frame = 59;
            break;
        case 4:
            animation_frame = 55;
            break;
        }
    }
    else
    {
        auto [x_pos, y_pos] = abs_position();
        auto base_x = static_cast<uint32_t>(std::floor(x_pos / 2.0f));
        auto base_y = static_cast<uint32_t>(std::floor(y_pos / 2.0f));
        auto sub_index_x = static_cast<uint32_t>(std::floor(x_pos - base_x * 2));
        auto sub_index_y = static_cast<uint32_t>(std::floor(y_pos - base_y * 2));
        if (type->id == 1)
        {
            auto fake_random = [](uint32_t i)
            {
                i = ((i >> 16) ^ i) * 0x45d9f3b;
                i = ((i >> 16) ^ i) * 0x45d9f3b;
                i = (i >> 16) ^ i;
                return i;
            };

            const uint32_t base = fake_random(base_x + base_y) % 3;
            animation_frame = (uint16_t)(base * 16 + sub_index_x + sub_index_y * 8);
        }
        else if (type->id == 2)
        {
            animation_frame = (uint16_t)(48 + sub_index_x + sub_index_y * 8);
        }
    }
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

    const bool has_corners = this->has_corners();

    const auto [x_pos, y_pos] = position_self();

    static float offsets[4][2]{
        {0.0f, +1.0f},
        {0.0f, -1.0f},
        {-1.0f, 0.0f},
        {+1.0f, 0.0f}};

    Floor* neighbours[4]{};
    bool neighbours_same[4]{};

    auto& state = State::get();
    auto layer_ptr = state.layer(layer);

    for (size_t i = 0; i < 4; i++)
    {
        auto [x_off, y_off] = offsets[i];
        auto* floor = layer_ptr->get_grid_entity_at(x_pos + x_off, y_pos + y_off)->as<Floor>();
        neighbours[i] = floor;
        neighbours_same[i] = floor != nullptr && floor->type->id == type->id && floor->texture->id == texture->id;
    }

    for (size_t i = 0; i < 4; i++)
    {
        auto* floor = neighbours[i];
        bool same = neighbours_same[i];
        if (floor != nullptr && same)
        {
            remove_decoration(FLOOR_SIDE(i));
        }
        else if (decos[i] == -1)
        {
            add_decoration_opt(FLOOR_SIDE(i), decoration_entity_type, layer_ptr);
        }

        if (floor != nullptr && fix_also_neighbors)
        {
            if (same)
            {
                FLOOR_SIDE opposite = get_opposite_side(FLOOR_SIDE(i));
                floor->remove_decoration(opposite);

                if (has_corners)
                {
                    FLOOR_SIDE perp_sides[2]{};
                    FLOOR_SIDE corner_sides[2]{};
                    get_perpendicular_sides(opposite, perp_sides);
                    get_corner_sides(opposite, corner_sides);

                    for (size_t j = 0; j < 2; j++)
                    {
                        if (floor->decos[perp_sides[j]] >= 0)
                        {
                            floor->remove_decoration(corner_sides[j]);
                        }
                        else if (!neighbours_same[perp_sides[j]])
                        {
                            floor->add_decoration_opt(corner_sides[j], decoration_entity_type, layer_ptr);
                        }
                    }
                }
            }
        }
    }

    if (has_corners)
    {
        static float corner_offsets[4][2]{
            {-1.0f, +1.0f},
            {+1.0f, +1.0f},
            {-1.0f, -1.0f},
            {+1.0f, -1.0f}};
        for (size_t i = 4; i < 8; i++)
        {
            FLOOR_SIDE perp_sides[2]{};
            get_perpendicular_sides(FLOOR_SIDE(i), perp_sides);

            if (!neighbours_same[perp_sides[0]] && !neighbours_same[perp_sides[1]])
            {
                add_decoration_opt(FLOOR_SIDE(i), decoration_entity_type, layer_ptr);
            }
            else
            {
                auto [x_off, y_off] = corner_offsets[i - 4];
                auto* floor = layer_ptr->get_grid_entity_at(x_pos + x_off, y_pos + y_off)->as<Floor>();
                bool same = floor != nullptr && floor->type->id == type->id;

                if ((same && !neighbours_same[perp_sides[0]] && !neighbours_same[perp_sides[1]]) || (!same && neighbours_same[perp_sides[0]] && neighbours_same[perp_sides[1]]))
                {
                    add_decoration_opt(FLOOR_SIDE(i), decoration_entity_type, layer_ptr);
                }
                else
                {
                    remove_decoration(FLOOR_SIDE(i));
                    if (same)
                    {
                        floor->remove_decoration(get_opposite_side(FLOOR_SIDE(i)));
                    }
                }
            }
        }
    }
}
void Floor::add_decoration(FLOOR_SIDE side)
{
    int32_t decoration_entity_type = get_decoration_entity_type();
    if (decoration_entity_type == -1)
    {
        return;
    }

    auto& state = State::get();
    auto layer_ptr = state.layer(layer);
    add_decoration_opt(side, decoration_entity_type, layer_ptr);
}
void Floor::remove_decoration(FLOOR_SIDE side)
{
    if (side > RIGHT && has_corners())
    {
        if (Entity* deco = find_corner_decoration(side))
        {
            if (items.contains(deco))
            {
                deco->color.a = 0;
                deco->kill(false, nullptr);
            }
        }
    }
    else
    {
        if (decos[side] >= 0)
        {
            if (Entity* deco = get_entity_ptr(decos[side]))
            {
                if (items.contains(deco))
                {
                    deco->color.a = 0;
                    deco->kill(false, nullptr);
                }
            }
            decos[side] = -1;

            if (side == FLOOR_SIDE::TOP)
            {
                if (deco_left >= 0)
                {
                    if (Entity* deco = get_entity_ptr(deco_left))
                    {
                        deco->animation_frame = get_decoration_animation_frame(FLOOR_SIDE::LEFT);
                    }
                }
                if (deco_right >= 0)
                {
                    if (Entity* deco = get_entity_ptr(deco_right))
                    {
                        deco->animation_frame = get_decoration_animation_frame(FLOOR_SIDE::RIGHT);
                    }
                }
            }
        }
    }
}

Entity* Floor::find_corner_decoration(FLOOR_SIDE side)
{
    static float offsets[4][2]{
        {-0.42f, +0.39f},
        {+0.42f, +0.39f},
        {-0.42f, -0.42f},
        {+0.42f, -0.42f}};

    for (auto item : items.entities())
    {
        auto [x_pos, y_pos] = item->position_self();
        if (std::abs(x_pos - offsets[side - 4][0]) < 0.0001f && std::abs(y_pos - offsets[side - 4][1]) < 0.0001f)
        {
            return item;
        }
    }
    return nullptr;
}
void Floor::add_decoration_opt(FLOOR_SIDE side, ENT_TYPE decoration_entity_type, Layer* layer_ptr)
{
    if (side > RIGHT)
    {
        if (type->id == 1 || type->id == 3)
        {
            EntityDB* decoration_type = get_type(decoration_entity_type);
            uint8_t draw_depth_before = decoration_type->draw_depth;
            decoration_type->draw_depth = 5;
            if (find_corner_decoration(side) == nullptr)
            {
                static float offsets[4][2]{
                    {-0.42f, +0.39f},
                    {+0.42f, +0.39f},
                    {-0.42f, -0.42f},
                    {+0.42f, -0.42f}};

                static uint8_t anims[4]{
                    14, 13, 6, 5};

                auto [x_off, y_off] = offsets[side - 4];
                auto anim = anims[side - 4];

                if (type->id == 3)
                {
                    anim += 24;
                }

                Entity* deco = layer_ptr->spawn_entity_over(decoration_entity_type, this, x_off, y_off);
                deco->animation_frame = anim;
                deco->set_texture(get_texture());
            }
            decoration_type->draw_depth = draw_depth_before;
        }
    }
    else
    {
        static float offsets[4][3]{
            {0.0f, +0.5f, +1.0f},
            {0.0f, -0.5f, +1.0f},
            {-0.5f, 0.0f, -1.0f},
            {+0.5f, 0.0f, +1.0f}};

        if (decos[side] == -1)
        {
            auto [x_off, y_off, scale] = offsets[side];

            if (type->id <= 3)
            {
                if (side == TOP)
                {
                    y_off += 0.05f;
                }
                else if (side == BOTTOM)
                {
                    y_off -= 0.05f;
                }
                else if (side == LEFT)
                {
                    x_off += 0.1f;
                }
                else if (side == RIGHT)
                {
                    x_off -= 0.1f;
                }
            }

            Entity* deco = layer_ptr->spawn_entity_over(decoration_entity_type, this, x_off, y_off);
            deco->w *= scale;
            deco->animation_frame = get_decoration_animation_frame(side);
            deco->set_texture(get_texture());
            decos[side] = deco->uid;
        }
    }
}

bool Floor::has_corners() const
{
    return type->id == 1 || type->id == 3;
}
bool Floor::is_styled() const
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
int32_t Floor::get_decoration_entity_type() const
{
    [[maybe_unused]] static auto do_once = []()
    {
        assert(0x1 == to_id("ENT_TYPE_FLOOR_BORDERTILE"));
        assert(0x2 == to_id("ENT_TYPE_FLOOR_BORDERTILE_METAL"));
        assert(0x3 == to_id("ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS"));
        assert(0x4 == to_id("ENT_TYPE_FLOOR_GENERIC"));
        assert(0xb == to_id("ENT_TYPE_FLOOR_TUNNEL_CURRENT"));
        assert(0xc == to_id("ENT_TYPE_FLOOR_TUNNEL_NEXT"));
        assert(0x5 == to_id("ENT_TYPE_FLOOR_SURFACE"));
        assert(0x6 == to_id("ENT_TYPE_FLOOR_SURFACE_HIDDEN"));
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
    case 0xb: // ENT_TYPE_FLOOR_TUNNEL_CURRENT
    case 0xc: // ENT_TYPE_FLOOR_TUNNEL_NEXT
        return 0x75;
    case 0x5: // FLOOR_SURFACE
        return 0x76;
    // case 0x6: // FLOOR_SURFACE_HIDDEN
    //     return 0x77;
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
uint16_t Floor::get_decoration_animation_frame(FLOOR_SIDE side) const
{
    uint8_t num_variants = 0;
    bool styled = false;

    switch (type->id)
    {
    default:
        return 0;
    case 0x1: // FLOOR_BORDERTILE
    {
        uint16_t anim_frame = 2;
        if (side == TOP)
        {
            anim_frame += 8;
        }
        if (side == BOTTOM)
        {
            anim_frame += 16;
        }
        return anim_frame + rand() % 3;
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
        uint8_t anim_frame = 26;
        if (side == TOP)
        {
            anim_frame += 8;
        }
        if (side == BOTTOM)
        {
            anim_frame += 16;
        }
        return anim_frame + rand() % 3;
    }
    case 0x6: // FLOOR_SURFACE_COVER
    {
        if (side != TOP)
        {
            return 0;
        }

        return 16 + rand() % 3;
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
        num_variants = 3;
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
    case 0xb: // ENT_TYPE_FLOOR_TUNNEL_CURRENT
    case 0xc: // ENT_TYPE_FLOOR_TUNNEL_NEXT
        num_variants = 2;
        break;
    }

    if (styled)
    {
        uint8_t anim_frame = 55;
        if (side == TOP)
            anim_frame += 10;
        if (side == BOTTOM)
            anim_frame += 20;
        if (num_variants > 0)
            anim_frame += rand() % num_variants;
        return anim_frame;
    }

    uint8_t anim_frame = 65;
    if (side == TOP)
        anim_frame += 12;
    if (side == BOTTOM)
        anim_frame += 24;
    if (num_variants > 0)
    {
        if (side == FLOOR_SIDE::LEFT || side == FLOOR_SIDE::RIGHT)
        {
            if (deco_top >= 0)
            {
                anim_frame += 2;
            }
            else
            {
                num_variants--;
                anim_frame += rand() % num_variants;
            }
        }
        else
        {
            anim_frame += rand() % num_variants;
        }
    }
    return anim_frame;
}

FLOOR_SIDE Floor::get_opposite_side(FLOOR_SIDE side)
{
    switch (side)
    {
    default:
    case FLOOR_SIDE::TOP:
    case FLOOR_SIDE::LEFT:
        return FLOOR_SIDE(side + 1);
    case FLOOR_SIDE::BOTTOM:
    case FLOOR_SIDE::RIGHT:
        return FLOOR_SIDE(side - 1);
    case FLOOR_SIDE::TOP_LEFT:
        return FLOOR_SIDE::BOTTOM_RIGHT;
    case FLOOR_SIDE::BOTTOM_RIGHT:
        return FLOOR_SIDE::TOP_LEFT;
    case FLOOR_SIDE::TOP_RIGHT:
        return FLOOR_SIDE::BOTTOM_LEFT;
    case FLOOR_SIDE::BOTTOM_LEFT:
        return FLOOR_SIDE::TOP_RIGHT;
    }
}
bool Floor::get_perpendicular_sides(FLOOR_SIDE side, FLOOR_SIDE (&perp_sides)[2])
{
    switch (side)
    {
    default:
        return false;
    case FLOOR_SIDE::TOP:
    case FLOOR_SIDE::BOTTOM:
        perp_sides[0] = FLOOR_SIDE::LEFT;
        perp_sides[1] = FLOOR_SIDE::RIGHT;
        break;
    case FLOOR_SIDE::LEFT:
    case FLOOR_SIDE::RIGHT:
        perp_sides[0] = FLOOR_SIDE::TOP;
        perp_sides[1] = FLOOR_SIDE::BOTTOM;
        break;
    case FLOOR_SIDE::TOP_LEFT:
        perp_sides[0] = FLOOR_SIDE::TOP;
        perp_sides[1] = FLOOR_SIDE::LEFT;
        break;
    case FLOOR_SIDE::TOP_RIGHT:
        perp_sides[0] = FLOOR_SIDE::TOP;
        perp_sides[1] = FLOOR_SIDE::RIGHT;
        break;
    case FLOOR_SIDE::BOTTOM_LEFT:
        perp_sides[0] = FLOOR_SIDE::BOTTOM;
        perp_sides[1] = FLOOR_SIDE::LEFT;
        break;
    case FLOOR_SIDE::BOTTOM_RIGHT:
        perp_sides[0] = FLOOR_SIDE::BOTTOM;
        perp_sides[1] = FLOOR_SIDE::RIGHT;
        break;
    }
    return true;
}
bool Floor::get_corner_sides(FLOOR_SIDE side, FLOOR_SIDE (&corner_sides)[2])
{
    switch (side)
    {
    default:
        return false;
    case FLOOR_SIDE::TOP:
        corner_sides[0] = FLOOR_SIDE::TOP_LEFT;
        corner_sides[1] = FLOOR_SIDE::TOP_RIGHT;
        break;
    case FLOOR_SIDE::BOTTOM:
        corner_sides[0] = FLOOR_SIDE::BOTTOM_LEFT;
        corner_sides[1] = FLOOR_SIDE::BOTTOM_RIGHT;
        break;
    case FLOOR_SIDE::LEFT:
        corner_sides[0] = FLOOR_SIDE::TOP_LEFT;
        corner_sides[1] = FLOOR_SIDE::BOTTOM_LEFT;
        break;
    case FLOOR_SIDE::RIGHT:
        corner_sides[0] = FLOOR_SIDE::TOP_RIGHT;
        corner_sides[1] = FLOOR_SIDE::BOTTOM_RIGHT;
        break;
    }
    return true;
}

void Arrowtrap::rearm()
{
    if (arrow_shot)
    {
        static auto arrow_trap_trigger_id = to_id("ENT_TYPE_LOGICAL_ARROW_TRAP_TRIGGER");
        arrow_shot = false;
        auto trigger = get_entity_ptr(spawn_entity_over(arrow_trap_trigger_id, uid, 0., 0.));
        if ((flags & (1 << 16)) > 0)
        {
            trigger->flags |= (1 << 16);
        }
    }
}

void trigger_trap(Entity* trap, int32_t who_uid, uint8_t direction)
{
    auto who = get_entity_ptr(who_uid)->as<Movable>();
    static const ENT_TYPE ar_logical_trigger = to_id("ENT_TYPE_LOGICAL_ARROW_TRAP_TRIGGER"); // + 5
    static const ENT_TYPE bs_logical_trigger = to_id("ENT_TYPE_LOGICAL_BIGSPEAR_TRAP_TRIGGER");
    static const ENT_TYPE laser_trap = to_id("ENT_TYPE_FLOOR_LASER_TRAP");
    if (who)
    {
        for (auto item : trap->items.entities())
        {
            if ((item->type->id >= ar_logical_trigger && item->type->id < ar_logical_trigger + 6) || item->type->id == bs_logical_trigger)
            {
                if (trap->type->id == laser_trap)
                {
                    auto vel_tmp = who->velocityx;
                    who->velocityx = 1.0;
                    trap->v23(item, who);
                    who->velocityx = vel_tmp;
                    return;
                }
                if (direction == 1) // left
                {
                    if (item->x >= 0)
                        continue;
                }
                else if (direction == 2) // right
                {
                    if (item->x <= 0)
                        continue;
                }
                else if (direction == 3) // up
                {
                    if (item->y <= 0)
                        continue;
                }
                else if (direction == 4) // down
                {
                    if (item->y >= 0)
                        continue;
                }
                item->on_collision2(who);
                return;
            }
        }
    }
}

void ForceField::activate_laserbeam(bool turn_on)
{
    if (turn_on && !is_on)
    {
        is_on = true;
        static auto laserbeam_id = to_id("ENT_TYPE_FX_LASERBEAM");
        auto laserbeam_uid = spawn_entity_over(laserbeam_id, uid, 0., 0.);
        fx = get_entity_ptr(laserbeam_uid);
        animation_frame = 53;
    }
    else if (!turn_on && is_on)
    {
        is_on = false;
        if (first_item_beam)
            first_item_beam->kill(false, nullptr);

        if (fx)
            fx->kill(false, nullptr);

        first_item_beam = nullptr;
        fx = nullptr;
        animation_frame = 50;
    }
}

void Door::unlock(bool unlock)
{
    // TODO: DOOR_EGGSHIP, DOOR_EGGSHIP_ATREZZO, DOOR_EGGSHIP_ROOM, HUNDUN ?
    static const ENT_TYPE entrance_door = to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE");
    static const ENT_TYPE locked_door = to_id("ENT_TYPE_FLOOR_DOOR_LOCKED");
    static const ENT_TYPE COG_door = to_id("ENT_TYPE_FLOOR_DOOR_COG");
    static const ENT_TYPE eggchild_room_door = to_id("ENT_TYPE_FLOOR_DOOR_MOAI_STATUE");
    static const ENT_TYPE EW_door = to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD");
    const auto ent_type = this->type->id;
    auto& state = State::get();

    if (ent_type == locked_door || ent_type == locked_door + 1) // plus one for DOOR_LOCKED_PEN
    {
        const auto door_ent = this->as<LockedDoor>();
        if (unlock)
        {
            door_ent->unlocked = true;
            door_ent->set_invisible(true);
        }
        else
        {
            door_ent->unlocked = false;
            door_ent->set_invisible(false);
        }
    }
    else if (ent_type == EW_door || (ent_type >= entrance_door && ent_type < locked_door))
    {
        if (unlock)
        {
            this->flags |= 0x80000; // set flag 20 (Enable button prompt)
        }
        else
        {
            this->flags &= ~0x80000; // clr flag 20 (Enable button prompt)
        }

        // entrance, exit, starting exit
        if (ent_type == entrance_door || ent_type == entrance_door + 1 || ent_type == entrance_door + 3)
        {
            static const ENT_TYPE door_bg = to_id("ENT_TYPE_BG_DOOR");
            const auto entities = state.layer(this->layer)->get_entities_overlapping_grid_at(x, y);
            if (entities == nullptr)
                return;
            for (const auto& item : entities->entities())
            {
                // technically for exit door the bg is uid + 3, but it feels wrong to do it that way
                if (item->type->id == door_bg)
                {
                    item->animation_frame = unlock ? 1 : 0;
                    // for Hundun door
                    // there is locked door sprite in both textures, so we don't mess with it and just use animation_frame when locking back up
                    // added example in the API doc on how to do the texture correctly for the other variant
                    if (unlock && item->get_texture() == 202) // TEXTURE.DATA_TEXTURES_DECO_EGGPLANT_0
                    {
                        item->set_texture(200); // TEXTURE.DATA_TEXTURES_FLOOR_SUNKEN_3
                    }
                    break;
                }
            }
        }
        else if (ent_type == entrance_door + 2) // main exit
        {
            static const ENT_TYPE fx_maindoor = to_id("ENT_TYPE_FX_MAIN_EXIT_DOOR");
            static const ENT_TYPE door_bg_large = to_id("ENT_TYPE_BG_DOOR_LARGE");
            const auto main_door = this->as<MainExit>();
            if (unlock)
            {
                if (main_door->door_blocker)
                {
                    if (main_door->door_blocker->type->id == fx_maindoor)
                    {
                        main_door->door_blocker->as<Movable>()->move_state = 5; // this will kill it
                        main_door->door_blocker = nullptr;                      // unlock the door, but the background still stays
                    }
                    else
                    {
                        main_door->door_blocker->destroy(); // destroy entity if it's something else
                        main_door->door_blocker = nullptr;
                    }
                }
            }
            else
            {
                if (!main_door->door_blocker)
                {
                    main_door->door_blocker = state.layer(layer)->spawn_entity_over(door_bg_large, this, 0, 2.0);
                    main_door->door_blocker->animation_frame = 1;
                }
            }
        }
    }
    else if (ent_type == COG_door)
    {
        const auto door_ent = this->as<CityOfGoldDoor>();
        door_ent->unlocked = unlock;
        if (door_ent->special_bg)
            door_ent->special_bg->animation_frame = unlock ? 1 : 0;
    }
    else if (ent_type == eggchild_room_door)
    {
        if (unlock)
        {
            this->set_invisible(false);
        }
        else
        {
            this->set_invisible(true);
        }
    }
}
