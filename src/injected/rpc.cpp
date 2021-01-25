#include "entity.hpp"
#include "state.hpp"
#include "ui.hpp"

// TODO: expose this to RPC
uint32_t spawn_entity(size_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();
    // crate::example::spawn_caveman_with_turkey();
    // return 0;

    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
    auto [_x, _y] = player->position();
    if (!s)
    {
        DEBUG("Spawning {} on {}, {}", id, x + _x, y + _y);
        return state
            .layer(player->layer())
            ->spawn_entity(id, x + _x, y + _y, s, vx, vy, snap)
            ->uid;
    }
    else
    {
        DEBUG("Spawning {} on screen {}, {}", id, x, y);
        return state.layer(player->layer())->spawn_entity(id, x, y, s, vx, vy, snap)->uid;
    }
}

void spawn_door(float x, float y, uint8_t l, uint8_t w, uint8_t f, uint8_t t)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto [_x, _y] = player->position();
    DEBUG("Spawning door on {}, {}", x + _x, y + _y);
    state
        .layer(player->layer())
        ->spawn_door(x + _x, y + _y, l, w, f, t);
    state
        .layer(player->layer())
        ->spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state
        .layer(player->layer())
        ->spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
}

void spawn_backdoor(float x, float y)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto [_x, _y] = player->position();
    DEBUG("Spawning backdoor on {}, {}", x + _x, y + _y);
    state
        .layer(0)
        ->spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0, true);
    state
        .layer(1)
        ->spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0, true);
    state
        .layer(0)
        ->spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state
        .layer(1)
        ->spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state
        .layer(0)
        ->spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
    state
        .layer(1)
        ->spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
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

void zoom(float level)
{
    State::get().zoom(level);
}

void list_items()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    for (auto &item : state.layer(player->layer())->items())
    {
        DEBUG(
            "Item {} {:x}, {:?}",
            item->uid,
            item->type->search_flags,
            item->position_self());
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
    DEBUG("Items at {:?}:", (x, y));
    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
    std::vector<std::tuple<int, float, Entity *>> found;
    for (auto &item : state.layer(player->layer())->items())
    {
        auto [ix, iy] = item->position();
        auto flags = item->type->search_flags;
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (mask & flags > 0 && distance < r)
        {
            DEBUG(
                "Item {}, {:x} type, {:?} position, {} distance, {:x?}",
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
        std::sort(found.begin(), found.end(), [](auto a, auto b) -> bool { return std::get<1>(a)<std::get<1>(b); });
        auto picked = found[0];
        auto entity = std::get<2>(picked);
        DEBUG("{:#x?}", entity);
        return std::get<0>(picked);
    }
    return 0;
}

void move_entity(uint32_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    for (auto &item : state.layer(player->layer())
                          ->items())
    {
        if (item
                ->uid == id)
        {
            item->teleport(x, y, s, vx, vy, snap);
        }
    }
}

uint32_t get_entity_flags(uint32_t id)
{
    if (id == 0)
    {
        return 0;
    }
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
    for (auto &item :
         state.layer(player->layer())
             ->items())
    {
        if (item->uid == id)
        {
            return item->flags;
        }
    }
    return 0;
}

void set_entity_flags(uint32_t id, uint32_t flags)
{
    if (id == 0)
    {
        return;
    }
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    for (auto &item :
         state.layer(player->layer())
             ->items())
    {
        if (item
                ->uid == id)
        {
            item->flags = flags;
        }
    }
}

uint8_t get_hud_flags()
{
    auto state = State::get();
    return state.flags();
}

void set_hud_flags(uint8_t flags)
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
    auto status = player->status();
    DEBUG("Player {:?}", status, status->rope, status->bomb);
    status->rope = (99);
    status->bomb = (99);
}

Entity *get_entity_ptr(uint32_t id)
{
    if (id == 0)
    {
        return nullptr;
    }
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return nullptr;
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
    auto player = state.items()->player(0);
    if (player == nullptr)
        return 0;
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
    std::vector<uintptr_t> found;
    for (int i = 0; i < 4; i++)
    {
        auto player = state.items()->player(i);
        if(player) found.push_back((uintptr_t)player);
    }
    set_players(found);
}
