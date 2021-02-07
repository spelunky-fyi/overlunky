#include <cstdarg>
#include "../injected/ui.hpp"
#include "entity.hpp"
#include "state.hpp"

uint32_t spawn_entity(uint32_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
    auto [_x, _y] = player->position();
    if (!s)
    {
        DEBUG("Spawning {} on {}, {}", id, x + _x, y + _y);
        return state.layer(player->layer())->spawn_entity(id, x + _x, y + _y, s, vx, vy, snap)->uid;
    }
    else
    {
        DEBUG("Spawning {} on screen {}, {}", id, x, y);
        return state.layer(player->layer())->spawn_entity(id, x, y, s, vx, vy, snap)->uid;
    }
}

uint32_t spawn_entity_abs(uint32_t id, float x, float y, int layer, float vx, float vy)
{
    auto state = State::get();
    if (layer == 0 || layer == 1)
    {
        return state.layer(layer)->spawn_entity(id, x, y, false, vx, vy, false)->uid;
    }
    else
    {
        auto player = state.items()->player(0);
        if (player == nullptr)
            return 0;
        auto [_x, _y] = player->position();
        DEBUG("Spawning {} on {}, {}", id, x + _x, y + _y);
        return state.layer(player->layer())->spawn_entity(id, x + _x, y + _y, false, vx, vy, false)->uid;
    }
}

uint32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
    auto [_x, _y] = player->position();
    DEBUG("Spawning door on {}, {}", x + _x, y + _y);
    state.layer(player->layer())->spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
    return state.layer(player->layer())->spawn_door(x + _x, y + _y, w, l, t)->uid;
}

uint32_t spawn_door_abs(float x, float y, int layer, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();
    if (layer == 0 || layer == 1)
    {
        return state.layer(layer)->spawn_door(x, y, w, l, t)->uid;
    }
    else
    {
        auto player = state.items()->player(0);
        if (player == nullptr)
            return 0;
        auto [_x, _y] = player->position();
        DEBUG("Spawning door on {}, {}", x + _x, y + _y);
        return state.layer(player->layer())->spawn_door(x + _x, y + _y, w, l, t)->uid;
    }
}

void spawn_backdoor(float x, float y)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto [_x, _y] = player->position();
    DEBUG("Spawning backdoor on {}, {}", x + _x, y + _y);
    state.layer(0)->spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(871, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(871, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
}

void spawn_backdoor_abs(float x, float y)
{
    auto state = State::get();
    DEBUG("Spawning backdoor on {}, {}", x, y);
    state.layer(0)->spawn_entity(26, x, y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(26, x, y, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(871, x, y - 1.0, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(871, x, y - 1.0, false, 0.0, 0.0, true);
}

void teleport(float x, float y, bool s, float vx, float vy, bool snap)
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

void list_items()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    for (auto &item : state.layer(player->layer())->items())
    {
        DEBUG("Item {} {:x}, {}", item->uid, item->type->search_flags, item->position_self());
    }
}

uint32_t get_entity_at(float x, float y, bool s, float r, uint32_t mask)
{
    auto state = State::get();
    if (s)
    {
        auto [rx, ry] = state.click_position(x, y);
        x = rx;
        y = ry;
    }
    DEBUG("Items at {}:", (x, y));
    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
    std::vector<std::tuple<int, float, Entity *>> found;
    for (auto &item : state.layer(player->layer())->items())
    {
        auto [ix, iy] = item->position();
        auto flags = item->type->search_flags;
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (((mask & flags) > 0 || mask == 0) && distance < r)
        {
            DEBUG(
                "Item {}, {:x} type, {} position, {} distance, {:x}",
                item->uid,
                item->type->search_flags,
                item->position_self(),
                distance,
                item->pointer());
            found.push_back({item->uid, distance, item});
        }
    }
    if (!found.empty())
    {
        std::sort(found.begin(), found.end(), [](auto a, auto b) -> bool { return std::get<1>(a) < std::get<1>(b); });
        auto picked = found[0];
        auto entity = std::get<2>(picked);
        DEBUG("{}", (void *)entity);
        return std::get<0>(picked);
    }
    return 0;
}

void move_entity(uint32_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->teleport(x, y, s, vx, vy, snap);
}

void move_entity_abs(uint32_t id, float x, float y, float vx, float vy)
{
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->teleport_abs(x, y, vx, vy);
}

uint32_t get_entity_flags(uint32_t id)
{
    if (id == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(id);
    if(ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t id, uint32_t flags)
{
    if (id == 0)
        return;
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t id)
{
    if (id == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t id, uint32_t flags)
{
    if (id == 0)
        return;
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t id)
{
    if (id == 0)
        return 0;
    auto state = State::get();
    auto ent = (Movable*)state.find(id);
    if (ent)
        return ent->move_state;
    return 0;
}

uint32_t get_hud_flags()
{
    auto state = State::get();
    return state.flags();
}

void set_hud_flags(uint32_t flags)
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
    DEBUG("Player {}", (void *)status, status->ropes, status->bombs);
    status->ropes = (99);
    status->bombs = (99);
}

Entity *get_entity_ptr(uint32_t id)
{
    if (id == 0)
    {
        return nullptr;
    }
    auto state = State::get();
    auto p = state.find(id);
    return p;
}

int32_t get_entity_type(uint32_t id)
{
    if (id == 0)
    {
        return 0;
    }
    auto state = State::get();
    auto p = state.find(id);
    if (!p)
        return 0;
    return p->type->id;
}

size_t get_state_ptr()
{
    auto state = State::get();
    return state.ptr();
}

void get_players()
{
    auto state = State::get();
    std::vector<Player *> found;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state.items()->player(i);
        if (player)
            found.push_back((Player *)player);
    }
    set_players(found);
}

std::pair<float, float> screen_position(float x, float y)
{
    return State::get().screen_position(x, y);
}

std::vector<uint32_t> get_entities()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        found.push_back(item->uid);
    }
    for (auto &item : state.layer(1)->items())
    {
        found.push_back(item->uid);
    }
    return found;
}

std::vector<uint32_t> get_entities_by_layer(int layer)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    if (layer == -1)
        layer = player->layer();
    std::vector<uint32_t> found;
    for (auto &item : state.layer(layer)->items())
    {
        found.push_back(item->uid);
    }
    return found;
}

std::vector<uint32_t> get_entities_by_type(std::vector<uint32_t> types)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

template<typename... Args>
std::vector<uint32_t> get_entities_by_type(Args... args)
{
    std::vector<uint32_t> types = {args...};
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

/*std::vector<uint32_t> get_entities_by_type(uint32_t type)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (item->type->id == type)
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (item->type->id == type)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}*/

std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (item->type->search_flags & mask)
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (item->type->search_flags & mask)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_by(uint32_t type, uint32_t mask, int layer)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    std::vector<uint32_t> found;
    if (!player)
        return {};
    if (layer == -1)
        layer = player->layer();
    if (layer >= 0 && state.layer(layer))
    {
        for (auto &item : state.layer(layer)->items())
        {
            if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == type || type == 0))
            {
                found.push_back(item->uid);
            }
        }
    }
    else
    {
        if (state.layer(0))
        {
            for (auto &item : state.layer(0)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == type || type == 0))
                {
                    found.push_back(item->uid);
                }
            }
        }
        if (state.layer(1))
        {
            for (auto &item : state.layer(1)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == type || type == 0))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_at(uint32_t type, uint32_t mask, int layer, float x, float y, float r)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    for (auto &item : state.layer(layer)->items())
    {
        auto [ix, iy] = item->position();
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (((item->type->search_flags & mask) > 0 || mask == 0) && (item->type->id == type || type == 0) && distance < r)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

void set_door_target(uint32_t id, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    Entity *door = get_entity_ptr(id);
    if (door == nullptr)
        return;
    static_cast<Door *>(door)->set_target(w, l, t);
}

void set_contents(uint32_t id, uint32_t item)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    Entity *container = get_entity_ptr(id);
    if (container == nullptr)
        return;
    int type = container->type->id;
    if (type != 435 && type != 402 && type != 422 && type != 423 && type != 475)
        return;
    static_cast<Movable *>(container)->inside = item;
    DEBUG("Set {} to {}", id, item);
}

void entity_remove_item(uint32_t id, uint32_t item)
{
    Entity *entity = get_entity_ptr(id);
    if (entity == nullptr)
        return;
    entity->remove_item(item);
}

uint32_t spawn_entity_over(uint32_t id, uint32_t over, float x, float y)
{
    return 0; // TODO broken
    auto state = State::get();
    Entity *overlay = get_entity_ptr(over);
    if (overlay == nullptr)
        return 0;
    int layer = overlay->layer();
    return state.layer(layer)->spawn_entity_over(id, overlay, x, y)->uid;
}

bool entity_has_item(uint32_t id, uint32_t item) // TODO
{
    return false;
};

bool entity_has_item_type(uint32_t id, uint32_t type) // TODO
{
    return false;
};

void lock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at(0, 0, 0, x, y, 1);
    for (auto id : items)
    {
        Entity *door = get_entity_ptr(id);
        if (door->type->id >= 22 && door->type->id <= 36)
        {
            door->flags &= ~(1U << 19);
            door->flags |= 1U << 21;
        }
        else if (door->type->id == 773 || door->type->id == 778 || door->type->id == 780)
        {
            door->animation &= ~1U;
        }
    }
}

void unlock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at(0, 0, 0, x, y, 1);
    for (auto id : items)
    {
        Entity *door = get_entity_ptr(id);
        if (door->type->id >= 22 && door->type->id <= 36)
        {
            door->flags |= 1U << 19;
            door->flags &= ~(1U << 21);
        }
        else if (door->type->id == 773 || door->type->id == 778 || door->type->id == 780)
        {
            door->animation |= 1U;
        }
    }
}
