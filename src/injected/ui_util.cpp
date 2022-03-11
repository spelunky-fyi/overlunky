#include "ui_util.hpp"

#include "entities_chars.hpp"
#include "rpc.hpp"
#include "spawn_api.hpp"
#include "state.hpp"

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
    return State::get().click_position(x, y);
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
    return State::get().get_zoom_level();
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
    return State::get().screen_position(x, y);
}
int32_t UI::get_entity_at(float x, float y, bool s, float radius, uint32_t mask)
{
    auto state = State::get();
    if (s)
    {
        auto [rx, ry] = state.click_position(x, y);
        x = rx;
        y = ry;
    }
    // DEBUG("Items at {}:", (x, y));
    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    std::vector<std::tuple<int32_t, float, Entity*>> found;
    for (auto& item : state.layer(player->layer)->all_entities.entities())
    {
        auto [ix, iy] = item->position();
        auto flags = item->type->search_flags;
        float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
        if (((mask & flags) > 0 || mask == 0) && distance < radius)
        {
            /*DEBUG(
                "Item {}, {:x} type, {} position, {} distance, {:x}",
                item->uid,
                item->type->search_flags,
                item->position_self(),
                distance,
                item->pointer());*/
            found.push_back({item->uid, distance, item});
        }
    }
    if (!found.empty())
    {
        std::sort(found.begin(), found.end(), [](auto a, auto b) -> bool
                  { return std::get<1>(a) < std::get<1>(b); });
        auto picked = found[0];
        // auto entity = std::get<2>(picked);
        // DEBUG("{}", (void*)entity);
        return std::get<0>(picked);
    }
    return -1;
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
    auto state = State::get();
    Player* player = nullptr;

    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        if (state.items()->player(i) != nullptr)
        {
            player = state.items()->player(i); // maybe spawn offset to camera focus uid then the player itself?
            break;
        }
    }
    if (player == nullptr)
        return -1;

    std::pair<float, float> offset_position;
    if (!s)
        offset_position = player->position();

    DEBUG("Spawning {} on {}, {}", entity_type, x + offset_position.first, y + offset_position.second);
    return state.layer_local(player->layer)->spawn_entity(entity_type, x + offset_position.first, y + offset_position.second, s, vx, vy, snap)->uid;
}
int32_t UI::spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();

    auto player = state.items()->player(0); // do the same stuff as in spawn_entity?
    if (player == nullptr)
        return -1;
    auto [_x, _y] = player->position();
    DEBUG("Spawning door on {}, {}", x + _x, y + _y);
    Layer* layer = state.layer_local(player->layer);
    layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    return layer->spawn_door(x + _x, y + _y, w, l, t)->uid;
}
void UI::spawn_backdoor(float x, float y)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto [_x, _y] = player->position();
    DEBUG("Spawning backdoor on {}, {}", x + _x, y + _y);
    Layer* front_layer = state.layer_local(0);
    Layer* back_layer = state.layer_local(1);
    front_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0f, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0f, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
}

// Redirect to RPC / Spawn_API:

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
