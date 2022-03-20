#include "ui_util.hpp"

#include "entities_chars.hpp"
#include "level_api.hpp"
#include "rpc.hpp"
#include "spawn_api.hpp"
#include "state.hpp"
#include "steam_api.hpp"

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
    DEBUG("Teleporting to relative {}, {}, {}", x, y, s);
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

    DEBUG("Spawning {} on {}, {}", entity_type, x, y);
    return state->layers[state->camera_layer]->spawn_entity(entity_type, x, y, s, vx, vy, snap)->uid;
}
int32_t UI::spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get().ptr_local();
    x += state->camera->focus_x;
    y += state->camera->focus_y;

    DEBUG("Spawning door on {}, {}", x, y);
    Layer* layer = state->layers[state->camera_layer];
    layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x, y, false, 0.0, 0.0, true);
    return layer->spawn_door(x, y, w, l, t)->uid;
}
void UI::spawn_backdoor(float x, float y)
{
    auto state = State::get().ptr_local();
    x += state->camera->focus_x;
    y += state->camera->focus_y;

    DEBUG("Spawning backdoor on {}, {}", x, y);
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

    if (ent->rendering_info && !ent->rendering_info->stop_render)
        return {ent->rendering_info->x, ent->rendering_info->y};

    for (auto ent_item : ent->items.entities())
    {
        if (ent_item->rendering_info && ent_item->rendering_info->stop_render == false)
        {
            return {ent_item->rendering_info->x - ent_item->x, ent_item->rendering_info->y - ent_item->y};
        }
    }
    return ent->position();
}
bool UI::has_active_render(Entity* ent)
{
    return (ent->rendering_info && !ent->rendering_info->stop_render);
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
