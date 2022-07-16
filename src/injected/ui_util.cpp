#include "ui_util.hpp"

#include <array>            // for array
#include <cstdint>          // for uint32_t, int32_t, uint8_t, uin...
#include <initializer_list> // for initializer_list
#include <map>              // for map, _Tree_iterator, _Tree_cons...
#include <new>              // for operator new
#include <tuple>            // for tie, tuple

#include "entities_activefloors.hpp" // for Olmec
#include "entities_chars.hpp"        // for Player, PowerupCapable
#include "entities_floors.hpp"       // for Floor, Floor::(anonymous), FLOO...
#include "entities_items.hpp"        // for Torch
#include "entities_mounts.hpp"       // for Mount
#include "entity.hpp"                // for to_id, Entity, get_entity_ptr
#include "game_manager.hpp"          // for get_game_manager, GameManager
#include "items.hpp"                 // for Items
#include "layer.hpp"                 // for Layer, EntityList::Range, Entit...
#include "level_api.hpp"             // for LevelGenSystem
#include "math.hpp"                  // for AABB
#include "render_api.hpp"            // for RenderInfo
#include "rpc.hpp"                   // for get_entities_at, entity_get_ite...
#include "spawn_api.hpp"             // for spawn_liquid, spawn_companion
#include "state.hpp"                 // for State, StateMemory
#include "state_structs.hpp"         // for Camera, Illumination (ptr only)
#include "steam_api.hpp"             // for disable_steam_achievements, ena...

void UI::godmode(bool g)
{
    State::get().godmode(g);
}
void UI::godmode_companions(bool g)
{
    State::get().godmode_companions(g);
}
std::pair<float, float> UI::click_position(float x, float y)
{
    return State::click_position(x, y);
}
void UI::zoom(float level)
{
    State::get().zoom(level);
}
uint32_t UI::get_frame_count()
{
    return State::get().get_frame_count();
}
void UI::warp(uint8_t world, uint8_t level, uint8_t theme)
{
    State::get().warp(world, level, theme);
}
void UI::transition(uint8_t world, uint8_t level, uint8_t theme)
{
    auto state = State::get().ptr();
    if (state->screen != 12)
    {
        State::get().warp(world, level, theme);
        return;
    }
    state->world_next = world;
    state->level_next = level;
    state->theme_next = theme;
    state->screen_next = 13;
    state->fadeout = 5;
    state->fadein = 5;
    state->win_state = 0;
    state->loading = 1;
}
float UI::get_zoom_level()
{
    return State::get_zoom_level();
}
void UI::teleport(float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    player->teleport(x, y, s, vx, vy, snap);
}
std::pair<float, float> UI::screen_position(float x, float y)
{
    return State::screen_position(x, y);
}
Entity* UI::get_entity_at(float x, float y, bool s, float radius, uint32_t mask)
{
    auto state = State::get();

    static const auto masks_order = {
        0x1,    // Player
        0x2,    // Mount
        0x4,    // Monster
        0x8,    // Item
        0x80,   // Activefloor
        0x100,  // Floor
        0x200,  // Decoration
        0x400,  // BG
        0x800,  // Shadow
        0x2000, // Water
        0x4000, // Lava
        0x40,   // FX
        0x10,   // Explosion
        0x20,   // Rope
        0x1000, // Logical
    };
    if (s)
    {
        std::tie(x, y) = state.click_position(x, y);
    }
    Entity* current_entity = nullptr;
    float current_distance = radius;
    auto check_distance = [&current_entity, &current_distance, &x, &y](Entity* test_entity)
    {
        const auto [ix, iy] = test_entity->position();
        const float distance = (float)std::sqrt(std::pow(x - ix, 2) + std::pow(y - iy, 2));
        if (distance < current_distance)
        {
            current_entity = test_entity;
            current_distance = distance;
        }
    };

    if (mask == 0)
    {
        for (auto& item : state.layer(state.ptr()->camera_layer)->all_entities.entities())
        {
            check_distance(item);
        }
    }
    else
    {
        for (auto current_mask : masks_order)
        {
            if ((mask & current_mask) == 0)
                continue;

            const auto& entities = state.layer(state.ptr()->camera_layer)->entities_by_mask.find(current_mask);
            if (entities == state.layer(state.ptr()->camera_layer)->entities_by_mask.end())
                continue;

            for (auto& item : entities->second.entities())
            {
                check_distance(item);
            }
        }
    }

    return current_entity;
}
void UI::move_entity(uint32_t uid, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        ent->teleport(x, y, s, vx, vy, snap);
}
SaveData* UI::savedata()
{
    return State::get().savedata();
}
int32_t UI::spawn_entity(ENT_TYPE entity_type, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get().ptr_local();

    if (!s)
    {
        x += state->camera->focus_x;
        y += state->camera->focus_y;
    }

    return state->layers[state->camera_layer]->spawn_entity(entity_type, x, y, s, vx, vy, snap)->uid;
}
int32_t UI::spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get().ptr_local();
    x += state->camera->focus_x;
    y += state->camera->focus_y;

    Layer* layer = state->layers[state->camera_layer];
    layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x, y, false, 0.0, 0.0, true);
    return layer->spawn_door(x, y, w, l, t)->uid;
}
void UI::spawn_backdoor(float x, float y)
{
    auto state = State::get().ptr_local();
    x += state->camera->focus_x;
    y += state->camera->focus_y;

    Layer* front_layer = state->layers[0];
    Layer* back_layer = state->layers[1];
    front_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0f, false, 0.0, 0.0, true); // TODO: not needed if there is a floor
    back_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0f, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x, y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x, y, false, 0.0, 0.0, true);
}
std::pair<float, float> UI::get_position(Entity* ent, bool render)
{
    if (!ent)
        return {.0f, .0f};

    if (!render)
        return ent->position();

    if (ent->rendering_info && !ent->rendering_info->render_inactive)
        return {ent->rendering_info->x, ent->rendering_info->y};

    for (auto ent_item : ent->items.entities())
    {
        if (ent_item->rendering_info && ent_item->rendering_info->render_inactive == false)
        {
            return {ent_item->rendering_info->x - ent_item->x, ent_item->rendering_info->y - ent_item->y};
        }
    }
    return ent->position();
}
bool UI::has_active_render(Entity* ent)
{
    return (ent->rendering_info && !ent->rendering_info->render_inactive);
}

// Redirect to RPC / Spawn_API etc.:

void UI::set_time_ghost_enabled(bool enable)
{
    ::set_time_ghost_enabled(enable);
}
void UI::set_time_jelly_enabled(bool enable)
{
    ::set_time_jelly_enabled(enable);
}
void UI::set_cursepot_ghost_enabled(bool enable)
{
    ::set_cursepot_ghost_enabled(enable);
}
ENT_TYPE UI::get_entity_type(int32_t uid)
{
    return ::get_entity_type(uid);
}
std::vector<Player*> UI::get_players()
{
    return ::get_players();
}
int32_t UI::get_grid_entity_at(float x, float y, LAYER l)
{
    return ::get_grid_entity_at(x, y, l);
}
Illumination* UI::create_illumination(Color color, float size, float x, float y)
{
    return ::create_illumination(color, size, x, y);
}
void UI::set_camp_camera_bounds_enabled(bool b)
{
    ::set_camp_camera_bounds_enabled(b);
}
std::vector<uint32_t> UI::get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer)
{
    return ::get_entities_by(entity_types, mask, layer);
}
int32_t UI::spawn_companion(ENT_TYPE compatnion_type, float x, float y, LAYER l)
{
    return ::spawn_companion(compatnion_type, x, y, l);
}
void UI::spawn_liquid(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount, float blobs_separation)
{
    ::spawn_liquid(entity_type, x, y, velocityx, velocityy, liquid_flags, amount, blobs_separation);
}
void UI::spawn_liquid(ENT_TYPE entity_type, float x, float y)
{
    ::spawn_liquid(entity_type, x, y);
}
int32_t UI::spawn_entity_over(ENT_TYPE entity_type, uint32_t over_uid, float x, float y)
{
    return ::spawn_entity_over(entity_type, over_uid, x, y);
}
std::pair<float, float> UI::get_room_pos(uint32_t x, uint32_t y)
{
    return LevelGenSystem::get_room_pos(x, y);
}
std::string_view UI::get_room_template_name(uint16_t room_template)
{
    const auto state = State::get().ptr();
    return state->level_gen->get_room_template_name(room_template);
}
std::optional<uint16_t> UI::get_room_template(uint32_t x, uint32_t y, uint8_t l)
{
    const auto state = State::get().ptr();
    return state->level_gen->get_room_template(x, y, l);
}
void UI::steam_achievements(bool on)
{
    if (on)
        enable_steam_achievements();
    else
        disable_steam_achievements();
}
int32_t UI::destroy_entity_items(Entity* ent)
{
    auto items = entity_get_items_by(ent->uid, 0, 0);
    if (items.size() == 0)
        return -1;
    std::vector<uint32_t>::reverse_iterator it = items.rbegin();
    int32_t last_uid = *it;
    while (it != items.rend())
    {
        auto item = get_entity_ptr(*it);
        UI::destroy_entity_items(item);
        UI::safe_destroy(item, false, false);
        it++;
    }
    return last_uid;
}
void UI::destroy_entity_overlay(Entity* ent)
{
    while (ent->overlay)
        ent = ent->overlay;
    UI::safe_destroy(ent, false, true);
}
void UI::kill_entity_overlay(Entity* ent)
{
    while (ent->overlay)
        ent = ent->overlay;
    ent->kill(true, ent);
}
void UI::update_floor_at(float x, float y, LAYER l)
{
    static const auto thorn_vine = to_id("ENT_TYPE_FLOOR_THORN_VINE");
    static const auto pipe = to_id("ENT_TYPE_FLOOR_PIPE");
    static const auto quicksand = to_id("ENT_TYPE_FLOOR_QUICKSAND");
    static const auto destroy_deco = {
        to_id("ENT_TYPE_DECORATION_HANGING_HIDE"),
        to_id("ENT_TYPE_DECORATION_CROSS_BEAM"),
        to_id("ENT_TYPE_DECORATION_HANGING_WIRES"),
        to_id("ENT_TYPE_DECORATION_MINEWOOD_POLE"),
        to_id("ENT_TYPE_DECORATION_PAGODA_POLE"),
        to_id("ENT_TYPE_DECORATION_PIPE"),
    };
    auto uid = get_grid_entity_at(x, y, l);
    if (uid == -1)
        return;
    auto ent = get_entity_ptr(uid);
    if ((ent->type->search_flags & 0x100) == 0 || !test_flag(ent->flags, 3))
        return;
    auto floor = ent->as<Floor>();
    auto state = State::get().ptr();
    if (test_flag(state->special_visibility_flags, 1))
    {
        for (auto item : entity_get_items_by(floor->uid, 0, 0x8))
        {
            auto embed = get_entity_ptr(item);
            clr_flag(embed->flags, 1);
        }
    }
    if (test_flag(floor->type->properties_flags, 1) && floor->get_decoration_entity_type() != -1)
    {
        for (int i = 0; i < 7; ++i)
        {
            floor->remove_decoration((FLOOR_SIDE)i);
        }
    }
    else if (floor->type->id == pipe || floor->type->id == thorn_vine || floor->type->id == quicksand)
    {
        for (int i = 0; i < 4; ++i)
        {
            auto deco_ent = get_entity_ptr(floor->decos[i]);
            if (deco_ent)
                deco_ent->destroy();
            floor->decos[i] = -1;
        }
    }
    for (auto deco : entity_get_items_by(floor->uid, destroy_deco, 0x200))
    {
        auto deco_ent = get_entity_ptr(deco);
        if (deco_ent)
            deco_ent->destroy();
    }
    for (auto deco : get_entities_at(destroy_deco, 0, x, y, l, 0.5f))
    {
        auto deco_ent = get_entity_ptr(deco);
        if (deco_ent)
            deco_ent->destroy();
    }
    if (test_flag(floor->type->properties_flags, 1) || test_flag(floor->type->properties_flags, 2))
    {
        if (floor->type->id < 4)
        {
            floor->on_neighbor_destroyed();
            floor->fix_decorations(true, false);
        }
        else
        {
            floor->decorate_internal();
        }
    }
    else
    {
        floor->on_neighbor_destroyed();
    }
}
bool in_array(uint32_t needle, std::vector<uint32_t> haystack)
{
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}
void UI::cleanup_at(float x, float y, LAYER l, ENT_TYPE type)
{
    static const auto cleanup_ents = {
        to_id("ENT_TYPE_LOGICAL_REGENERATING_BLOCK"),
        to_id("ENT_TYPE_MIDBG"),
        to_id("ENT_TYPE_MIDBG_STYLEDDECORATION"),
    };

    static const auto platform_types = {
        to_id("ENT_TYPE_FLOOR_PLATFORM"),
        to_id("ENT_TYPE_FLOOR_PAGODA_PLATFORM"),
    };
    static const auto platform_bg = to_id("ENT_TYPE_MIDBG_PLATFORM_STRUCTURE");

    static const auto first_door = to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE");
    static const auto layer_door = to_id("ENT_TYPE_FLOOR_DOOR_LAYER");
    static const auto logical_door = to_id("ENT_TYPE_LOGICAL_DOOR");
    static const auto door_platform = to_id("ENT_TYPE_FLOOR_DOOR_PLATFORM");
    static const auto door_crap = {
        to_id("ENT_TYPE_FLOOR_DOOR_LAYER"),
        to_id("ENT_TYPE_LOGICAL_DOOR"),
        to_id("ENT_TYPE_BG_DOOR"),
        to_id("ENT_TYPE_BG_SHOP_BACKDOOR"),
        to_id("ENT_TYPE_BG_DOOR_BLACK_MARKET"),
        to_id("ENT_TYPE_BG_DOOR_COG"),
        to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"),
        to_id("ENT_TYPE_BG_DOOR_FRONT_LAYER"),
        to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"),
    };

    for (auto bg : get_entities_at(cleanup_ents, 0, x, y, l, 0.1f))
    {
        auto bg_ent = get_entity_ptr(bg);
        if (bg_ent)
            bg_ent->destroy();
    }

    if (in_array(type, platform_types))
    {
        while (true)
        {
            y -= 1.0f;
            auto bgs = get_entities_at(platform_bg, 0x400, x, y, l, 0.1f);
            if (bgs.size() == 0)
                return;
            for (auto bg : bgs)
            {
                auto ent = get_entity_ptr(bg);
                ent->destroy();
            }
        }
    }

    if (type >= first_door && type <= first_door + 14)
    {
        if (type == layer_door || type == logical_door)
            l = LAYER::BOTH;
        auto door_parts = get_entities_at(door_crap, 0, x, y, l, 0.5f);
        for (auto part : door_parts)
        {
            auto ent = get_entity_ptr(part);
            ent->destroy();
        }
        for (auto uid : get_entities_at(door_platform, 0x100, x, y - 1.0f, l, 0.5f))
        {
            auto ent = get_entity_ptr(uid);
            ent->destroy();
        }
    }
}
void UI::safe_destroy(Entity* ent, bool unsafe, bool recurse)
{
    if (!ent)
        return;

    static const auto backitems = {
        to_id("ENT_TYPE_ITEM_JETPACK"),
        to_id("ENT_TYPE_ITEM_HOVERPACK"),
        to_id("ENT_TYPE_ITEM_POWERPACK"),
        to_id("ENT_TYPE_ITEM_TELEPORTER_BACKPACK"),
        to_id("ENT_TYPE_ITEM_CAPE"),
        to_id("ENT_TYPE_ITEM_VLADS_CAPE"),
    };

    static const auto jellys = {
        to_id("ENT_TYPE_MONS_MEGAJELLYFISH"),
        to_id("ENT_TYPE_MONS_MEGAJELLYFISH_BACKGROUND"),
    };

    static const auto jelly_tail = {
        to_id("ENT_TYPE_FX_MEGAJELLYFISH_TAIL"),
        to_id("ENT_TYPE_FX_MEGAJELLYFISH_TAIL_BG"),
    };

    static const auto destroy_items = {
        to_id("ENT_TYPE_MONS_HUNDUN"),
        to_id("ENT_TYPE_MONS_KINGU"),
        to_id("ENT_TYPE_MONS_TIAMAT"),
        to_id("ENT_TYPE_MONS_OSIRIS_HEAD"),
        to_id("ENT_TYPE_MONS_APEP_HEAD"),
        to_id("ENT_TYPE_MONS_YAMA"),
    };

    static const auto kill_last_overlay = {
        to_id("ENT_TYPE_ITEM_SLIDINGWALL_CHAIN"),
        to_id("ENT_TYPE_ITEM_SLIDINGWALL_CHAIN_LASTPIECE"),
        to_id("ENT_TYPE_FLOOR_SLIDINGWALL_CEILING"),
        to_id("ENT_TYPE_ITEM_CHAIN"),
        to_id("ENT_TYPE_ITEM_CHAIN_LASTPIECE"),
        to_id("ENT_TYPE_FLOOR_SPIKEBALL_CEILING"),
        to_id("ENT_TYPE_ITEM_STICKYTRAP_PIECE"),
        to_id("ENT_TYPE_ITEM_STICKYTRAP_LASTPIECE"),
        to_id("ENT_TYPE_FLOOR_STICKYTRAP_CEILING"),
    };

    static const auto just_kill = {
        to_id("ENT_TYPE_ACTIVEFLOOR_SLIDINGWALL"),
        to_id("ENT_TYPE_ACTIVEFLOOR_CHAINED_SPIKEBALL"),
        to_id("ENT_TYPE_ITEM_STICKYTRAP_BALL"),
        to_id("ENT_TYPE_ACTIVEFLOOR_REGENERATINGBLOCK"),
    };

    static const auto flame = {
        to_id("ENT_TYPE_ITEM_WALLTORCHFLAME"),
        to_id("ENT_TYPE_ITEM_LAMPFLAME"),
        to_id("ENT_TYPE_ITEM_TORCHFLAME"),
        to_id("ENT_TYPE_ITEM_REDLANTERNFLAME"),
        to_id("ENT_TYPE_ITEM_PALACE_CANDLE_FLAME"),
    };

    static const auto olmecs = {
        to_id("ENT_TYPE_ACTIVEFLOOR_OLMEC"),
    };

    if (recurse)
    {
        auto check = ent;
        do
        {
            // TODO: weird hack, but destroying the olmec in 3-1 crashes. some logic is still using it
            // destroying olmec anywhere else would be fine I think
            if (check && in_array(check->type->id, olmecs))
            {
                auto olmec = check->as<Olmec>();
                olmec->attack_phase = 3;
                olmec->flags = 0x50001231;
                move_entity_abs(olmec->uid, 10.0f, 1000.0f, 0.0f, 0.0f);
                return;
            }
            else if (in_array(check->type->id, jellys))
            {
                const auto last_item = destroy_entity_items(check);
                check->destroy();
                // stupid tail is not attached to jelly, but it should always be here
                for (int tail_uid = last_item + 8; tail_uid > last_item; --tail_uid)
                {
                    auto tail = get_entity_ptr(tail_uid);
                    if (tail)
                        tail->destroy();
                }
                return;
            }
            // stupid tail is not attached to jelly. if trying to destroy tail, check entities backwards to find jelly
            else if (in_array(check->type->id, jelly_tail))
            {
                int32_t uid = check->uid;
                Entity* find_jelly;
                while (true)
                {
                    uid--;
                    find_jelly = get_entity_ptr(uid);
                    if (find_jelly && in_array(find_jelly->type->id, jellys))
                    {
                        safe_destroy(find_jelly);
                        return;
                    }
                    if (uid < check->uid - 32)
                        break;
                }
                return;
            }
            else if (in_array(check->type->id, destroy_items))
            {
                destroy_entity_items(check);
                check->destroy();
                return;
            }
            else if (in_array(check->type->id, kill_last_overlay) || (check->type->search_flags & 0x200 && check->draw_depth <= 11)) // normal floor decorations, missing those and killing floor is not good
            {
                kill_entity_overlay(check);
                return;
            }
            else if (in_array(check->type->id, just_kill))
            {
                check->kill(true, check);
                return;
            }
            else if (in_array(check->type->id, flame))
            {
                if (!check->overlay)
                    return;
                const auto torch = check->overlay->as<Torch>();
                torch->light_up(false);
                return;
            }
            else if (in_array(check->type->id, backitems))
            {
                if (check->overlay && (check->overlay->type->search_flags & 0x5) > 0)
                {
                    auto wearer = check->overlay->as<PowerupCapable>();
                    for (const auto& [powerup_type, powerup_entity] : wearer->powerups)
                    {
                        if (check == powerup_entity)
                        {
                            unequip_backitem(check->overlay->uid);
                            check->destroy();
                            return;
                        }
                    }
                }
            }
            check = check->overlay;
        } while (check);
    }
    if (!ent->is_player())
    {
        const auto [x, y] = UI::get_position(ent);
        const auto sf = ent->type->search_flags;
        destroy_entity_items(ent);
        if (sf & 0x100 && test_flag(ent->flags, 3)) // solid floor
        {
            ent->destroy();
            update_liquid_collision_at(x, y, false);
        }
        else if (ent->is_liquid())
        {
            ent->kill(true, ent);
        }
        else if (sf & 0x2)
        {
            auto mount = ent->as<Mount>();
            mount->remove_rider();
            mount->destroy();
        }
        else
        {
            ent->flags = set_flag(ent->flags, 29); // set dead before destroy == no mess
            ent->destroy();
        }
    }
    else if (unsafe)
    {
        ent->kill(true, ent);
    }
}
std::vector<uint32_t> UI::get_entities_overlapping(uint32_t mask, AABB hitbox, LAYER layer)
{
    return get_entities_overlapping_hitbox(0, mask, hitbox, layer);
}

bool UI::get_focus()
{
    return ::get_game_manager()->game_props->game_has_focus;
}
