#include "rpc.hpp"

#include "drops.hpp"
#include "entities_floors.hpp"
#include "entities_mounts.hpp"
#include "entity.hpp"
#include "game_manager.hpp"
#include "logger.h"
#include "state.hpp"
#include "virtual_table.hpp"
#include <cstdarg>
#include <detours.h>
#include <unordered_set>
#include <utility>

uint32_t setflag(uint32_t flags, int bit) //shouldn't we change those to #define ?
{
    return flags | (1U << (bit - 1));
}
uint32_t clrflag(uint32_t flags, int bit)
{
    return flags & ~(1U << (bit - 1));
}
bool testflag(uint32_t flags, int bit)
{
    return (flags & (1U << (bit - 1))) > 0;
}
uint32_t flipflag(uint32_t flags, int bit)
{
    return (flags ^ (1U << (bit - 1)));
}

void teleport(float x, float y, bool s, float vx, float vy, bool snap) //ui only
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    DEBUG("Teleporting to relative {}, {}, {}", x, y, s);
    player->teleport(x, y, s, vx, vy, snap);
}

void godmode(bool g)
{
    State::get().godmode(g);
}

void godmode_companions(bool g)
{
    State::get().godmode_companions(g);
}

void darkmode(bool g)
{
    State::get().darkmode(g);
}

void zoom(float level)
{
    State::get().zoom(level);
}

float get_zoom_level()
{
    return State::get().get_zoom_level();
}

void attach_entity(Entity* overlay, Entity* attachee)
{
    if (attachee->overlay)
    {
        overlay->remove_item_ptr(attachee);
    }

    auto [x, y] = overlay->position();
    attachee->x -= x;
    attachee->y -= y;
    attachee->special_offsetx = attachee->x;
    attachee->special_offsety = attachee->y;
    attachee->overlay = overlay;

    using AddItemPtr = void(Vector*, Entity*, bool);
    static AddItemPtr* add_item_ptr = (AddItemPtr*)get_address("add_item_ptr");
    add_item_ptr(&overlay->items, attachee, false);
}

void attach_entity_by_uid(uint32_t overlay_uid, uint32_t attachee_uid)
{
    if (Entity* overlay = get_entity_ptr(overlay_uid))
    {
        if (Entity* attachee = get_entity_ptr(attachee_uid))
        {
            attach_entity(overlay, attachee);
        }
    }
}

int32_t attach_ball_and_chain(uint32_t uid, float off_x, float off_y)
{
    if (Entity* entity = get_entity_ptr(uid))
    {
        static const auto ball_entity_type = to_id("ENT_TYPE_ITEM_PUNISHBALL");
        static const auto chain_entity_type = to_id("ENT_TYPE_ITEM_PUNISHCHAIN");

        auto [x, y, l] = get_position(uid);
        auto* layer_ptr = State::get().layer(l);

        PunishBall* ball = (PunishBall*)layer_ptr->spawn_entity(ball_entity_type, x + off_x, y + off_y, false, 0.0f, 0.0f, false);

        ball->attached_to_uid = uid;

        const uint8_t chain_length = 15;
        for (uint8_t i = 0; i < chain_length; i++)
        {
            StretchChain* chain = (StretchChain*)layer_ptr->spawn_entity(chain_entity_type, x, y, false, 0.0f, 0.0f, false);
            chain->animation_frame -= (i % 2);

            chain->at_end_of_chain_uid = ball->uid;
            chain->dot_offset = (float)i / chain_length;
            chain->position_in_chain = i;
            chain->inverse_doubled_position_in_chain = (chain_length - i) * 2;
        }
        return ball->uid;
    }
    return -1;
}

void stack_entities(uint32_t bottom_uid, uint32_t top_uid, const float (&offset)[2])
{
    if (Entity* bottom = get_entity_ptr(bottom_uid))
    {
        if (Entity* top = get_entity_ptr(top_uid))
        {
            if (top->overlay)
            {
                top->overlay->remove_item_ptr(top);
            }
            attach_entity(bottom, top);
            top->x = offset[0];
            top->y = offset[1];
            top->special_offsetx = offset[0];
            top->special_offsety = offset[1];
            if ((bottom->flags >> 0x10) & 0x1) // facing left
            {
                top->x *= -1.0f;
            }
        }
    }
}

int32_t get_entity_at(float x, float y, bool s, float radius, uint32_t mask) // ui only
{
    auto state = State::get();
    if (s)
    {
        auto [rx, ry] = state.click_position(x, y);
        x = rx;
        y = ry;
    }
    //DEBUG("Items at {}:", (x, y));
    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    std::vector<std::tuple<int32_t, float, Entity*>> found;
    for (auto& item : state.layer(player->layer)->items())
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
        //auto entity = std::get<2>(picked);
        //DEBUG("{}", (void*)entity);
        return std::get<0>(picked);
    }
    return -1;
}

int32_t get_grid_entity_at(float x, float y, LAYER layer)
{
    auto state = State::get();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* ent = state.layer(actual_layer)->get_grid_entity_at(x, y))
        return ent->uid;

    return -1;
}

void move_entity(uint32_t uid, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->teleport(x, y, s, vx, vy, snap);
}

void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->teleport_abs(x, y, vx, vy);
}

uint32_t get_entity_flags(uint32_t uid)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t uid, uint32_t flags)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t uid)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t uid, uint32_t flags)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t uid)
{
    auto state = State::get();
    auto ent = state.find(uid)->as<Movable>();
    if (ent)
        return ent->move_state;
    return 0;
}

uint32_t get_level_flags()
{
    auto state = State::get();
    return state.flags();
}

void set_level_flags(uint32_t flags)
{
    auto state = State::get();
    state.set_flags(flags);
}

void set_pause(uint8_t pause)
{
    auto state = State::get();
    state.set_pause(pause);
}

void player_status()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto status = player->inventory_ptr;
    DEBUG("Player {}", (void*)status, status->ropes, status->bombs);
    status->ropes = (99);
    status->bombs = (99);
}

Screen* get_screen_ptr(uint32_t screen_id)
{
    auto game_manager = get_game_manager();
    auto state = get_state_ptr();
    switch (screen_id)
    {
    case 0:
    {
        return game_manager->screen_logo;
    }
    case 1:
    {
        return game_manager->screen_intro;
    }
    case 2:
    {
        return game_manager->screen_prologue;
    }
    case 3:
    {
        return game_manager->screen_title;
    }
    case 4:
    {
        return game_manager->screen_menu;
    }
    case 5:
    {
        return game_manager->screen_options;
    }
    case 6:
    {
        return game_manager->screen_player_profile;
    }
    case 7:
    {
        return game_manager->screen_leaderboards;
    }
    case 8:
    {
        return game_manager->screen_seed_input;
    }
    case 9:
    {
        return state->screen_character_select;
    }
    case 10:
    {
        return state->screen_team_select;
    }
    case 11:
    {
        // a screen_camp also exists in State, but its buttons flags do not work, so we use GameManager's one
        return game_manager->screen_camp;
    }
    case 12:
    {
        // a screen_level also exists in State
        return game_manager->screen_level;
    }
    case 13:
    {
        return state->screen_transition;
    }
    case 14:
    {
        return state->screen_death;
    }
    // 15 = spaceship -> the spots in State and GameManager where this should be are both nullptr -> has no UI
    case 16:
    {
        return state->screen_win;
    }
    case 17:
    {
        return state->screen_credits;
    }
    case 18:
    {
        return state->screen_scores;
    }
    case 19:
    {
        return state->screen_constellation;
    }
    case 20:
    {
        return state->screen_recap;
    }
    case 21:
    {
        return state->screen_arena_menu;
    }
    case 22:
    case 24:
    {
        return state->screen_arena_stages_select1;
    }
    case 23:
    {
        return state->screen_arena_items;
    }
    case 25:
    {
        return state->screen_arena_intro;
    }
    case 26:
    {
        // the one in GameManager has no UI entries
        return state->screen_arena_level;
    }
    case 27:
    {
        return state->screen_arena_score;
    }
    case 28:
    {
        return game_manager->screen_online_loading;
    }
    case 29:
    {
        return game_manager->screen_online_lobby;
    }
    }
    DEBUG("Screen pointer requested for unknown screen ID: {}", screen_id);
    return nullptr;
}

Entity* get_entity_ptr(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find(uid);
    if (IsBadWritePtr(p, 0x178))
        return nullptr;
    return p;
}

ENT_TYPE get_entity_type(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find(uid);
    if (p == nullptr || IsBadWritePtr(p, 0x178))
        return UINT32_MAX;
    return p->type->id;
}

StateMemory* get_state_ptr()
{
    return State::get().ptr();
}

std::vector<Player*> get_players()
{
    auto state = State::get();
    std::vector<Player*> found;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state.items()->player(i);
        if (player)
            found.push_back((Player*)player);
    }
    return found;
}

std::pair<float, float> click_position(float x, float y)
{
    return State::get().click_position(x, y);
}

std::pair<float, float> screen_position(float x, float y)
{
    return State::get().screen_position(x, y);
}

std::tuple<float, float, float, float> screen_aabb(float left, float top, float right, float bottom)
{
    auto [sx1, sy1] = screen_position(left, top);
    auto [sx2, sy2] = screen_position(right, bottom);
    return std::tuple{sx1, sy1, sx2, sy2};
}

float screen_distance(float x)
{
    auto a = State::get().screen_position(0, 0);
    auto b = State::get().screen_position(x, 0);
    return b.first - a.first;
}

std::vector<uint32_t> filter_entities(std::vector<uint32_t> entities, std::function<bool(Entity*)> predicate)
{
    std::vector<uint32_t> filtered_entities{std::move(entities)};
    auto filter_fun = [&](uint32_t uid)
    {
        if (Entity* entity = get_entity_ptr(uid))
        {
            return !predicate(entity);
        }
        return false;
    };
    std::erase_if(filtered_entities, filter_fun);
    return filtered_entities;
}

std::vector<uint32_t> get_entities()
{
    return get_entities_by({}, 0, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by_layer(LAYER layer)
{
    return get_entities_by({}, 0, layer);
}

std::vector<uint32_t> get_entities_by_type(std::vector<ENT_TYPE> entity_types)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    uint8_t layer = 2;
    while (layer)
    {
        layer--;
        for (auto& item : state.layer(layer)->items())
        {
            if (std::find(entity_types.begin(), entity_types.end(), item->type->id) != entity_types.end())
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}
std::vector<uint32_t> get_entities_by_type(ENT_TYPE entity_type)
{
    return get_entities_by_type(std::vector<ENT_TYPE>{entity_type});
}

std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    return get_entities_by({}, mask, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    if (layer == LAYER::BOTH)
    {
        uint8_t layeridx = 2;
        while (layeridx)
        {
            layeridx--;
            for (auto& item : state.layer(layeridx)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && entity_type_check(entity_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);

        for (auto& item : state.layer(actual_layer)->items())
        {
            if (((item->type->search_flags & mask) || mask == 0) && entity_type_check(entity_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}
std::vector<uint32_t> get_entities_by(ENT_TYPE entity_type, uint32_t mask, LAYER layer)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, mask, layer);
}

std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    if (layer == LAYER::BOTH)
    {
        uint8_t layeridx = 2;
        while (layeridx)
        {
            layeridx--;
            for (auto& item : state.layer(layeridx)->items())
            {
                auto [ix, iy] = item->position();
                float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
                if (((item->type->search_flags & mask) > 0 || mask == 0) && distance < radius && entity_type_check(entity_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);

        for (auto& item : state.layer(actual_layer)->items())
        {
            auto [ix, iy] = item->position();
            float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
            if (((item->type->search_flags & mask) > 0 || mask == 0) && distance < radius && entity_type_check(entity_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}
std::vector<uint32_t> get_entities_at(ENT_TYPE entity_type, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    return get_entities_at(std::vector<ENT_TYPE>{entity_type}, mask, x, y, layer, radius);
}

std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, uint32_t mask, AABB hitbox, LAYER layer)
{
    auto state = State::get();
    std::vector<uint32_t> result;
    if (layer == LAYER::BOTH)
    {
        std::vector<uint32_t> result2;
        result = get_entities_overlapping_by_pointer(entity_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(0));
        result2 = get_entities_overlapping_by_pointer(entity_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(1));
        result.insert(result.end(), result2.begin(), result2.end());
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);
        result = get_entities_overlapping_by_pointer(entity_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(actual_layer));
    }
    return result;
}
std::vector<uint32_t> get_entities_overlapping_hitbox(ENT_TYPE entity_type, uint32_t mask, AABB hitbox, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, hitbox, layer);
}

std::vector<uint32_t> get_entities_overlapping(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(entity_types, mask, {sx, sy2, sx2, sy}, layer);
}
std::vector<uint32_t> get_entities_overlapping(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, {sx, sy2, sx2, sy}, layer);
}

std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    std::vector<uint32_t> found;
    for (auto& item : layer->items())
    {
        if (((item->type->search_flags & mask) > 0 || mask == 0) && entity_type_check(entity_types, item->type->id) && item->overlaps_with(sx, sy, sx2, sy2))
        {
            found.push_back(item->uid);
        }
    }
    return found;
}
std::vector<uint32_t> get_entities_overlapping_by_pointer(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    return get_entities_overlapping_by_pointer(std::vector<ENT_TYPE>{entity_type}, mask, sx, sy, sx2, sy2, layer);
}

void set_door_target(uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
{
    Entity* door = get_entity_ptr(uid);
    if (door == nullptr)
        return;
    door->as<ExitDoor>()->world = w;
    door->as<ExitDoor>()->level = l;
    door->as<ExitDoor>()->theme = t;
    door->as<ExitDoor>()->special_door = true;
}

std::tuple<uint8_t, uint8_t, uint8_t> get_door_target(uint32_t uid)
{
    Entity* door = get_entity_ptr(uid);
    if (door == nullptr || !door->as<ExitDoor>()->special_door)
        return std::make_tuple((uint8_t)0, (uint8_t)0, (uint8_t)0);
    return std::make_tuple(door->as<ExitDoor>()->world, door->as<ExitDoor>()->level, door->as<ExitDoor>()->theme);
}

void set_contents(uint32_t uid, ENT_TYPE item_entity_type)
{
    Entity* container = get_entity_ptr(uid);
    if (container == nullptr)
        return;
    uint32_t type = container->type->id;
    if (type != to_id("ENT_TYPE_ITEM_COFFIN") && type != to_id("ENT_TYPE_ITEM_CRATE") && type != to_id("ENT_TYPE_ITEM_PRESENT") &&
        type != to_id("ENT_TYPE_ITEM_GHIST_PRESENT") && type != to_id("ENT_TYPE_ITEM_POT"))
        return;
    container->as<Container>()->inside = item_entity_type;
}

void entity_remove_item(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return;
    entity->remove_item(item_uid);
}

bool entity_has_item_uid(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        auto pitems = entity->items.begin;
        for (unsigned int i = 0; i < entity->items.count; i++)
        {
            if (pitems[i] == item_uid)
                return true;
        }
    }
    return false;
};

bool entity_has_item_type(uint32_t uid, std::vector<ENT_TYPE> entity_types)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        int* pitems = (int*)entity->items.begin;
        for (unsigned int i = 0; i < entity->items.count; i++)
        {
            Entity* item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
                continue;
            if (std::find(entity_types.begin(), entity_types.end(), item->type->id) != entity_types.end())
                return true;
        }
    }
    return false;
}
bool entity_has_item_type(uint32_t uid, ENT_TYPE entity_type)
{
    return entity_has_item_type(uid, std::vector<ENT_TYPE>{entity_type});
}

std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, uint32_t mask)
{
    std::vector<uint32_t> found;
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return found;
    if (entity->items.count > 0)
    {
        uint32_t* pitems = entity->items.begin;
        for (unsigned int i = 0; i < entity->items.count; i++)
        {
            Entity* item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
            {
                continue;
            }
            if (((item->type->search_flags & mask) || mask == 0) && entity_type_check(entity_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}
std::vector<uint32_t> entity_get_items_by(uint32_t uid, ENT_TYPE entity_type, uint32_t mask)
{
    return entity_get_items_by(uid, std::vector<ENT_TYPE>{entity_type}, mask);
}

void lock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at({}, 0, x, y, LAYER::FRONT, 1);
    for (auto id : items)
    {
        Entity* door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags &= ~(1U << 19);
            door->flags |= 1U << 21;
        }
        else if (
            door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") ||
            door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
        {
            door->animation_frame &= ~1U;
        }
    }
}

void unlock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at({}, 0, x, y, LAYER::FRONT, 1);
    for (auto id : items)
    {
        Entity* door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags |= 1U << 19;
            door->flags &= ~(1U << 21);
        }
        else if (
            door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") ||
            door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
        {
            door->animation_frame |= 1U;
        }
    }
}

uint32_t get_frame_count()
{
    auto state = State::get();
    return state.get_frame_count();
}

void carry(uint32_t mount_uid, uint32_t rider_uid)
{
    auto mount = get_entity_ptr(mount_uid)->as<Mount>();
    auto rider = get_entity_ptr(rider_uid)->as<Player>();
    if (mount == nullptr || rider == nullptr)
        return;
    mount->carry(rider);
}

void kill_entity(uint32_t uid, std::optional<bool> destroy_corpse)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->kill(destroy_corpse.value_or(true), nullptr);
}

void destroy_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->destroy();
}

void apply_entity_db(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->apply_db();
}

void flip_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent == nullptr)
        return;
    ent->flags = flipflag(ent->flags, 17);
    if (ent->items.count > 0)
    {
        int* items = (int*)ent->items.begin;
        for (unsigned int i = 0; i < ent->items.count; i++)
        {
            Entity* item = get_entity_ptr(items[i]);
            item->flags = flipflag(item->flags, 17);
        }
    }
}

std::pair<float, float> get_camera_position()
{
    auto state = State::get();
    return state.get_camera_position();
}

void set_camera_position(float cx, float cy)
{
    auto state = State::get();
    state.set_camera_position(cx, cy);
}

void warp(uint8_t world, uint8_t level, uint8_t theme)
{
    auto state = State::get();
    state.warp(world, level, theme);
}

void set_seed(uint32_t seed)
{
    auto state = State::get();
    state.set_seed(seed);
}

void set_arrowtrap_projectile(ENT_TYPE regular_entity_type, ENT_TYPE poison_entity_type)
{
    write_mem_prot(get_address("arrowtrap_projectile"), regular_entity_type, true);
    write_mem_prot(get_address("poison_arrowtrap_projectile"), poison_entity_type, true);
}

void modify_sparktraps(float angle_increment, float distance)
{
    static size_t angle_increment_offset = 0;
    static size_t angle_increment_instruction = 0;
    if (angle_increment_offset == 0)
    {
        angle_increment_instruction = get_address("sparktrap_angle_increment");
        angle_increment_offset = angle_increment_instruction - 0x32;
        auto distance_offset_relative = static_cast<int32_t>(angle_increment_offset - (angle_increment_instruction + 8));
        write_mem_prot(angle_increment_instruction + 4, distance_offset_relative, true);
    }
    write_mem_prot(angle_increment_offset, angle_increment, true);

    static size_t distance_offset = 0;
    if (distance_offset == 0)
    {
        auto distance_instruction = angle_increment_instruction + 0x1F;
        distance_offset = angle_increment_instruction - 0x2E;
        auto distance_offset_relative = static_cast<int32_t>(distance_offset - (distance_instruction + 8));
        write_mem_prot(distance_instruction + 4, distance_offset_relative, true);
    }
    write_mem_prot(distance_offset, distance, true);
}

void set_kapala_blood_threshold(uint8_t threshold)
{
    write_mem_prot(get_address("kapala_blood_threshold"), threshold, true);
}

void set_kapala_hud_icon(int8_t icon_index)
{
    static size_t instruction_offset = 0;
    static size_t icon_index_offset = 0;
    static uint32_t distance = 0;

    if (instruction_offset == 0)
    {
        instruction_offset = get_address("kapala_hud_icon");
        icon_index_offset = instruction_offset + 0x12;
        distance = static_cast<uint32_t>(icon_index_offset - (instruction_offset + 7));
    }

    if (icon_index < 0) // reset to original
    {
        write_mem_prot(instruction_offset + 2, 0x00013089, true);
    }
    else
    {
        // Instead of loading the value from KapalaPowerup:amount_of_blood (the instruction pointed at by instruction_offset)
        // we overwrite this with an instruction that loads a byte located a bit after the current function.
        // So you need to assemble `movzx  <relevant register>,BYTE PTR [rip+<distance>]`
        write_mem_prot(instruction_offset + 2, {0x0d}, true);
        write_mem_prot(instruction_offset + 3, distance, true);
        if (icon_index > 6)
        {
            icon_index = 6;
        }
        write_mem_prot(icon_index_offset, icon_index, true);
    }
}

void set_blood_multiplication(uint32_t /*default_multiplier*/, uint32_t vladscape_multiplier)
{
    // Due to changes in 1.23.x, the default multiplier is automatically vlads - 1.
    write_mem_prot(get_address("blood_multiplication"), vladscape_multiplier, true);
}

SaveData* savedata()
{
    auto state = State::get();
    return state.savedata();
}

std::vector<int64_t> read_prng()
{
    auto state = State::get();
    return state.read_prng();
}

void pick_up(uint32_t who_uid, uint32_t what_uid)
{
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        ent->pick_up(item);
    }
}

void drop(uint32_t who_uid, uint32_t what_uid)
{
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        ent->drop(item);
    }
}

void unequip_backitem(uint32_t who_uid)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("unequip");
    }

    if (offset != 0)
    {
        auto backitem_uid = worn_backitem(who_uid);
        if (backitem_uid != -1)
        {
            Movable* ent = (Movable*)get_entity_ptr(who_uid);
            Entity* backitem_ent = get_entity_ptr(backitem_uid);
            if (ent != nullptr && backitem_ent != nullptr)
            {
                typedef size_t unequip_func(Entity*, uint32_t);
                static unequip_func* uf = (unequip_func*)(offset);
                uf(ent, backitem_ent->type->id);
            }
        }
    }
}

int32_t worn_backitem(uint32_t who_uid)
{
    static const std::unordered_set<uint32_t> backitem_types = {
        to_id("ENT_TYPE_ITEM_JETPACK"),
        to_id("ENT_TYPE_ITEM_HOVERPACK"),
        to_id("ENT_TYPE_ITEM_POWERPACK"),
        to_id("ENT_TYPE_ITEM_TELEPORTER_BACKPACK"),
        to_id("ENT_TYPE_ITEM_CAPE"),
        to_id("ENT_TYPE_ITEM_VLADS_CAPE"),
    };

    auto ent = get_entity_ptr(who_uid)->as<PowerupCapable>();
    if (ent != nullptr)
    {
        for (const auto& [powerup_type, powerup_entity] : ent->powerups)
        {
            if (backitem_types.count(powerup_type) > 0)
            {
                return powerup_entity->uid;
            }
        }
    }
    return -1;
}

void set_olmec_phase_y_level(uint8_t phase, float y)
{
    // Sets the Y-level Olmec changes phases. The defaults are :
    // - phase 1 (bombs) = 100
    // - phase 2 (ufos) = 83
    // Olmecs checks phases in order! The means if you want ufo's from the start
    // you have to put both phase 1 and 2 at e.g. level 199
    // If you want to make Olmec stay in phase 0 (stomping) all the time, you can just set
    // the phase 1 y level to 70. Don't set it too low, from 1.25.0 onwards, Olmec's stomp
    // activation distance seems to be related to the y-level trigger point.
    static size_t phase1_offset = 0;
    static size_t phase2_offset = 0;
    if (phase1_offset == 0)
    {
        // from 1.23.x onwards, there are now two instructions per phase that reference the y-level float
        size_t phase_1_instruction_a = get_address("olmec_transition_phase_1_y_level");
        size_t phase_1_instruction_b = phase_1_instruction_a + 0xd;
        phase1_offset = get_address("olmec_transition_phase_1_custom_floats");

        size_t phase_2_instruction_a = get_address("olmec_transition_phase_2_y_level");
        size_t phase_2_instruction_b = phase_2_instruction_a + 0x11;
        phase2_offset = phase1_offset + 0x4;

        // write the default values to our new floats
        write_mem_prot(phase1_offset, 100.0f, true);
        write_mem_prot(phase2_offset, 83.0f, true);

        // calculate the distances between our floats and the movss instructions
        auto distance_1_a = static_cast<int32_t>(phase1_offset - phase_1_instruction_a);
        auto distance_1_b = static_cast<int32_t>(phase1_offset - phase_1_instruction_b);
        auto distance_2_a = static_cast<int32_t>(phase2_offset - phase_2_instruction_a);
        auto distance_2_b = static_cast<int32_t>(phase2_offset - phase_2_instruction_b);

        // overwrite the movss instructions to load our floats
        write_mem_prot(phase_1_instruction_a - 4, distance_1_a, true);
        write_mem_prot(phase_1_instruction_b - 4, distance_1_b, true);
        write_mem_prot(phase_2_instruction_a - 4, distance_2_a, true);
        write_mem_prot(phase_2_instruction_b - 4, distance_2_b, true);
    }

    if (phase == 1)
    {
        write_mem_prot(phase1_offset, y, true);
    }
    else if (phase == 2)
    {
        write_mem_prot(phase2_offset, y, true);
    }
}

void set_ghost_spawn_times(uint32_t normal, uint32_t cursed)
{
    write_mem_prot(get_address("ghost_spawn_time"), normal, true);
    write_mem_prot(get_address("ghost_spawn_time_cursed_player1"), cursed, true);
    write_mem_prot(get_address("ghost_spawn_time_cursed_player2"), cursed, true);
    write_mem_prot(get_address("ghost_spawn_time_cursed_player3"), cursed, true);
    write_mem_prot(get_address("ghost_spawn_time_cursed_player4"), cursed, true);
}

void set_drop_chance(uint16_t dropchance_id, uint32_t new_drop_chance)
{
    if (dropchance_id < dropchance_entries.size())
    {
        auto& entry = dropchance_entries.at(dropchance_id);
        if (entry.offset == 0)
        {
            auto memory = Memory::get();
            size_t offset = memory.at_exe(find_inst(memory.exe(), entry.pattern, get_virtual_function_address(entry.vtable_offset, entry.vtable_rel_offset)));
            if (offset > memory.exe_ptr)
            {
                entry.offset = offset;
            }
        }

        if (entry.offset != 0)
        {
            if (entry.chance_sizeof == 4)
            {
                write_mem_prot(entry.offset, new_drop_chance, true);
            }
            else if (entry.chance_sizeof == 1)
            {
                uint8_t value = static_cast<uint8_t>(new_drop_chance);
                write_mem_prot(entry.offset, value, true);
            }
        }
    }
}

void replace_drop(uint16_t drop_id, ENT_TYPE new_drop_entity_type)
{
    if (new_drop_entity_type == 0)
    {
        return;
    }

    if (drop_id < drop_entries.size())
    {
        auto& entry = drop_entries.at(drop_id);
        if (entry.offsets[0] == 0)
        {
            auto memory = Memory::get();
            size_t offset = 0;
            size_t exe_offset = 0;
            if (entry.vtable_offset == VTABLE_OFFSET::NONE)
            {
                exe_offset = memory.at_exe(find_inst(memory.exe(), entry.pattern, memory.after_bundle) + entry.value_offset);
            }
            else
            {
                offset = find_inst(memory.exe(), entry.pattern, get_virtual_function_address(entry.vtable_offset, entry.vtable_rel_offset)) + entry.value_offset;
                exe_offset = memory.at_exe(offset);
            }

            for (auto x = 0; x < entry.vtable_occurrence; ++x)
            {
                if (exe_offset > memory.exe_ptr)
                {
                    entry.offsets[x] = exe_offset;
                }

                if (x + 1 < entry.vtable_occurrence)
                {
                    offset = find_inst(memory.exe(), entry.pattern, offset + 1) + entry.value_offset;
                    exe_offset = memory.at_exe(offset);
                }
            }
        }

        if (entry.offsets[0] != 0)
        {
            for (auto x = 0; x < entry.vtable_occurrence; ++x)
            {
                write_mem_prot(entry.offsets[x], new_drop_entity_type, true);
            }
        }
    }
}

void generate_particles(uint32_t particle_emitter_id, uint32_t uid)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("generate_particles");
    }

    if (offset != 0)
    {
        auto entity = get_entity_ptr(uid);
        if (entity != nullptr)
        {
            auto state = get_state_ptr();
            typedef size_t generate_particles_func(PointerList*, uint32_t, Entity*);
            static generate_particles_func* gpf = (generate_particles_func*)(offset);
            gpf(state->particle_emitters, particle_emitter_id, entity);
        }
    }
}

static bool g_journal_enabled = true;
using OnShowJournalFun = void(void*, uint8_t);
OnShowJournalFun* g_on_show_journal_trampoline{nullptr};
void on_show_journal(void* journal_ui, uint8_t page_shown)
{
    if (!g_journal_enabled && page_shown == 2) // page 2 = journal
    {
        return;
    }
    g_on_show_journal_trampoline(journal_ui, page_shown);
}

void set_journal_enabled(bool b)
{
    static bool function_hooked = false;
    if (!function_hooked)
    {
        g_on_show_journal_trampoline = (OnShowJournalFun*)get_address("show_journal");

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_on_show_journal_trampoline, &on_show_journal);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking on_show_journal: {}\n", error);
        }

        function_hooked = true;
    }
    g_journal_enabled = b;
}

void set_camp_camera_bounds_enabled(bool b)
{
    static size_t offset = 0;
    static char original_instruction[3] = {0};
    if (offset == 0)
    {
        offset = get_address("enforce_camp_camera_bounds");
        for (uint8_t x = 0; x < 3; ++x)
        {
            original_instruction[x] = read_u8(offset + x);
        }
    }
    if (b)
    {
        write_mem_prot(offset, std::string(original_instruction, 3), true);
    }
    else
    {
        write_mem_prot(offset, "\xC3\x90\x90"s, true);
    }
}

void set_explosion_mask(uint32_t mask)
{
    static size_t addr = 0;
    if (addr == 0)
    {
        addr = get_address("explosion_mask");
    }
    write_mem_prot(addr, mask, true);
}

void set_max_rope_length(uint8_t length)
{
    uint32_t length_32 = length;

    // there's four instances where the max (default=6) is used

    // 1) When throwing a rope and it attaches to the background, the initial entity is
    // given a start value in its segment_nr_inverse variable
    write_mem_prot(get_address("attach_thrown_rope_to_background"), length_32, true);

    // 2) and 3) at the top of the rope processing function are two comparisons to the max
    write_mem_prot(get_address("process_ropes_one"), length, true);
    write_mem_prot(get_address("process_ropes_two"), length, true);

    // 4) in the same function at the end of the little loop of process_ropes_two is a comparison to n-1
    uint8_t length_minus_one_8 = length - 1;
    write_mem_prot(get_address("process_ropes_three"), length_minus_one_8, true);
}

uint8_t waddler_count_entity(ENT_TYPE entity_type)
{
    auto state = get_state_ptr();
    uint8_t count = 0;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == entity_type)
        {
            count++;
        }
    }
    return count;
}

int8_t waddler_store_entity(ENT_TYPE entity_type)
{
    auto state = get_state_ptr();
    int8_t item_stored_in_slot = -1;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == 0)
        {
            state->waddler_storage[x] = entity_type;
            item_stored_in_slot = x;
            break;
        }
    }
    return item_stored_in_slot;
}

void waddler_remove_entity(ENT_TYPE entity_type, uint8_t amount_to_remove)
{
    auto state = get_state_ptr();

    uint8_t remove_count = 0;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (amount_to_remove == remove_count)
        {
            break;
        }

        if (state->waddler_storage[x] == entity_type)
        {
            state->waddler_storage[x] = 0;
            remove_count++;
        }
    }

    if (remove_count > 0)
    {
        uint32_t tmp[99] = {0};
        uint8_t tmp_x = 0;
        for (uint8_t x = 0; x < 99; ++x)
        {
            if (state->waddler_storage[x] != 0)
            {
                tmp[tmp_x++] = state->waddler_storage[x];
            }
        }
        memcpy(&(state->waddler_storage[0]), tmp, 99 * sizeof(uint32_t));
    }
}

int16_t waddler_get_entity_meta(uint8_t slot)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        return state->waddler_storage_meta[slot];
    }
    return 0;
}

void waddler_set_entity_meta(uint8_t slot, int16_t meta)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        state->waddler_storage_meta[slot] = meta;
    }
}

uint32_t waddler_entity_type_in_slot(uint8_t slot)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        return state->waddler_storage[slot];
    }
    return 0;
}

int32_t spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer)
{
    auto offset = get_address("spawn_companion");
    if (offset != 0)
    {
        auto state = get_state_ptr();
        typedef Player* spawn_companion_func(StateMemory*, float x, float y, size_t layer, uint32_t entity_type);
        static spawn_companion_func* sc = (spawn_companion_func*)(offset);
        Player* spawned = sc(state, x, y, enum_to_layer(layer), companion_type);
        return spawned->uid;
    }
    return -1;
}

uint8_t enum_to_layer(const LAYER layer)
{
    if (layer == LAYER::FRONT)
        return 0;
    else if (layer == LAYER::BACK)
        return 1;
    else if ((int)layer < -MAX_PLAYERS)
        return 0;
    else if (layer < LAYER::FRONT)
    {
        auto state = State::get();
        auto player = state.items()->player(static_cast<uint8_t>(abs((int)layer) - 1));
        if (player != nullptr)
        {
            return player->layer;
        }
    }
    return 0;
}

uint8_t enum_to_layer(const LAYER layer, std::pair<float, float>& player_position)
{
    if (layer == LAYER::FRONT)
    {
        player_position = {0.0f, 0.0f};
        return 0;
    }
    else if (layer == LAYER::BACK)
    {
        player_position = {0.0f, 0.0f};
        return 1;
    }
    else if ((int)layer < -MAX_PLAYERS)
        return 0;
    else if (layer < LAYER::FRONT)
    {
        auto state = State::get();
        auto player = state.items()->player(static_cast<uint8_t>(abs((int)layer) - 1));
        if (player != nullptr)
        {
            player_position = player->position();
            return player->layer;
        }
    }
    return 0;
}

bool entity_type_check(const std::vector<ENT_TYPE>& types_array, const ENT_TYPE find)
{
    if (types_array.empty() || types_array[0] == 0 || std::find(types_array.begin(), types_array.end(), find) != types_array.end())
        return true;

    return false;
}
