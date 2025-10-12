#include "rpc.hpp"

#include <Windows.h>        // for VirtualFree, MEM_RELEASE, GetCurrent...
#include <array>            // for array
#include <cmath>            // for round, pow, sqrt
#include <cstring>          // for size_t, memcpy
#include <detours.h>        // for DetourAttach, DetourTransactionBegin
#include <fmt/format.h>     // for check_format_string, format, vformat
#include <initializer_list> // for initializer_list
#include <iosfwd>           // for ofstream
#include <list>             // for _List_const_iterator
#include <map>              // for map, _Tree_iterator, _Tree_const_ite...
#include <memory>           // for remove
#include <new>              // for operator new
#include <set>              // for set, set<>::iterator
#include <span>             // for span
#include <string>           // for operator""sv, string, operator""s
#include <string_view>      // for string_view
#include <type_traits>      // for move, hash
#include <unordered_set>    // for _Uset_traits<>::allocator_type, _Use...
#include <utility>          // for min, max, pair, find

#include "aliases.hpp"
#include "bucket.hpp"
#include "containers/custom_vector.hpp" //
#include "custom_types.hpp"             // for get_custom_entity_types, CUSTOM_TYPE
#include "entities_chars.hpp"           // for Player (ptr only), PowerupCapable
#include "entities_floors.hpp"          // for ExitDoor, Door
#include "entities_items.hpp"           // for StretchChain, PunishBall, Container
#include "entities_liquids.hpp"         // for Liquid
#include "entities_mounts.hpp"          // for Mount
#include "entity.hpp"                   // for get_entity_ptr, to_id, Entity, EntityDB
#include "entity_lookup.hpp"            //
#include "game_manager.hpp"             //
#include "game_patches.hpp"             //
#include "heap_base.hpp"                // for OnHeapPointer, HeapBase
#include "illumination.hpp"             //
#include "items.hpp"                    // for Items
#include "layer.hpp"                    // for EntityList, EntityList::Range, Layer
#include "liquid_engine.hpp"            // for LiquidPhysicsEngine
#include "logger.h"                     // for DEBUG
#include "math.hpp"                     // for AABB
#include "memory.hpp"                   // for write_mem_prot, write_mem_recoverable
#include "movable.hpp"                  // for Movable
#include "online.hpp"                   // for Online
#include "particles.hpp"                // for ParticleEmitterInfo
#include "prng.hpp"                     // for PRNG
#include "screen.hpp"                   //
#include "search.hpp"                   // for get_address, find_inst
#include "state.hpp"                    // for get_state_ptr, enum_to_layer
#include "state_structs.hpp"            // for ShopRestrictedItem, Illumination
#include "virtual_table.hpp"            // for get_virtual_function_address, VIRT_FUNC

uint32_t setflag(uint32_t flags, int bit)
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

void attach_entity(Entity* overlay, Entity* attachee)
{
    if (attachee->overlay)
    {
        if (attachee->overlay == overlay)
            return;

        attachee->overlay->remove_item(attachee, false);
    }

    auto [x, y] = overlay->abs_position();
    attachee->x -= x;
    attachee->y -= y;
    attachee->special_offsetx = attachee->x;
    attachee->special_offsety = attachee->y;
    attachee->overlay = overlay;

    overlay->items.insert(attachee, false);
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

        auto pos = entity->abs_position();
        auto* layer_ptr = HeapBase::get().state()->layer(entity->layer);

        PunishBall* ball = (PunishBall*)layer_ptr->spawn_entity(ball_entity_type, pos.x + off_x, pos.y + off_y, false, 0.0f, 0.0f, false);

        ball->attached_to_uid = uid;

        const uint8_t chain_length = 15;
        for (uint8_t i = 0; i < chain_length; i++)
        {
            StretchChain* chain = (StretchChain*)layer_ptr->spawn_entity(chain_entity_type, pos.x, pos.y, false, 0.0f, 0.0f, false);
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

void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
    {
        if (ent->is_liquid())
        {
            move_liquid_abs(uid, x, y, vx, vy);
        }
        else
        {
            ent->detach(false);
            ent->x = x;
            ent->y = y;
            if (ent->is_movable())
            {
                auto movable_ent = ent->as<Movable>();
                movable_ent->velocityx = vx;
                movable_ent->velocityy = vy;
            }
        }
    }
}

void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy, LAYER layer)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
    {
        Vec2 offset;
        enum_to_layer(layer, offset);
        if (ent->is_liquid())
        {
            move_liquid_abs(uid, offset.x + x, offset.y + y, vx, vy);
        }
        else
        {
            ent->detach(false);
            ent->x = offset.x + x;
            ent->y = offset.y + y;
            if (ent->is_movable())
            {
                auto movable_ent = ent->as<Movable>();
                movable_ent->velocityx = vx;
                movable_ent->velocityy = vy;
            }
            ent->set_layer(layer);
        }
    }
}

void move_liquid_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto entity = get_entity_ptr(uid)->as<Liquid>();
    if (entity)
    {
        auto liquid_engine = HeapBase::get().liquid_physics()->get_correct_liquid_engine(entity->type->id);
        if (liquid_engine)
        {
            liquid_engine->entity_coordinates[*entity->liquid_id] = {x, y};
            liquid_engine->entity_velocities[*entity->liquid_id] = {vx, vy};
        }
    }
}

ENT_TYPE get_entity_type(uint32_t uid)
{
    auto entity = get_entity_ptr(uid);
    if (entity)
        return entity->type->id;

    return UINT32_MAX; // TODO: shouldn't this be 0?
}

std::tuple<float, float, float, float> screen_aabb(float left, float top, float right, float bottom)
{
    auto [sx1, sy1] = API::screen_position(left, top);
    auto [sx2, sy2] = API::screen_position(right, bottom);
    return std::tuple{sx1, sy1, sx2, sy2};
}

float screen_distance(float x)
{
    auto a = API::screen_position(0, 0);
    auto b = API::screen_position(x, 0);
    return b.x - a.x;
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

void set_contents(uint32_t uid, ENT_TYPE item_entity_type)
{
    Entity* container = get_entity_ptr(uid);
    if (container == nullptr)
        return;
    uint32_t type = container->type->id;
    static auto compatible_entities = {
        to_id("ENT_TYPE_ITEM_COFFIN"),
        to_id("ENT_TYPE_ITEM_CRATE"),
        to_id("ENT_TYPE_ITEM_DMCRATE"),
        to_id("ENT_TYPE_ITEM_PRESENT"),
        to_id("ENT_TYPE_ITEM_GHIST_PRESENT"),
        to_id("ENT_TYPE_ITEM_POT"),
        to_id("ENT_TYPE_ALIVE_EMBEDDED_ON_ICE")};

    if (std::find(compatible_entities.begin(), compatible_entities.end(), type) == compatible_entities.end())
        return;

    container->as<Container>()->inside = item_entity_type;
}

void entity_remove_item(uint32_t uid, uint32_t item_uid, std::optional<bool> check_autokill)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return;

    auto entity_item = get_entity_ptr(item_uid);
    if (entity_item)
        entity->remove_item(entity_item, check_autokill.value_or(true));
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

void unequip_backitem(uint32_t who_uid)
{
    static const size_t offset = get_address("unequip");

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

int32_t worn_backitem(uint32_t who_uid)
{
    static const auto backitem_types = {
        to_id("ENT_TYPE_ITEM_JETPACK"),
        to_id("ENT_TYPE_ITEM_HOVERPACK"),
        to_id("ENT_TYPE_ITEM_POWERPACK"),
        to_id("ENT_TYPE_ITEM_TELEPORTER_BACKPACK"),
        to_id("ENT_TYPE_ITEM_CAPE"),
        to_id("ENT_TYPE_ITEM_VLADS_CAPE"),
    };

    auto ent = get_entity_ptr(who_uid)->as<PowerupCapable>();
    if (ent != nullptr && !ent->powerups.empty())
    {
        for (auto powerup_type : backitem_types)
        {
            auto it = ent->powerups.find(powerup_type);
            if (it != ent->powerups.end())
                return it->second->uid;
        }
    }
    return -1;
}

bool is_inside_active_shop_room(float x, float y, LAYER layer)
{
    // this functions just calculates the room index and then loops thru state->room_owners->owned_rooms and compares the room index
    // TODO: we could probably get rid of this pattern and write that ourselves
    static const size_t offset = get_address("coord_inside_active_shop_room");
    typedef bool coord_inside_shop_func(StateMemory*, uint32_t layer, float x, float y);
    static coord_inside_shop_func* cisf = (coord_inside_shop_func*)(offset);
    return cisf(get_state_ptr(), enum_to_layer(layer), x, y);
}

bool is_inside_shop_zone(float x, float y, LAYER layer)
{
    // this function is weird, the main check does this (where rax is the room_template):
    // ecx = rax - 0x41
    // cmp cx, 0x17
    // ja return 0
    //
    // if it doesn't jump there is a bunch of coordinate checks but also state.presence_flags, flipped rooms ...

    static const size_t offset = get_address("coord_inside_shop_zone");
    auto level_gen = HeapBase::get().level_gen();
    typedef bool coord_inside_shop_zone_func(LevelGenSystem*, uint32_t layer, float x, float y);
    coord_inside_shop_zone_func* ciszf = (coord_inside_shop_zone_func*)(offset);
    return ciszf(level_gen, enum_to_layer(layer), x, y);
}

uint8_t get_max_rope_length()
{
    static const auto address = get_address("attach_thrown_rope_to_background");
    return static_cast<uint8_t>(memory_read<uint32_t>(address));
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

void poison_entity(int32_t entity_uid)
{
    auto ent = get_entity_ptr(entity_uid);
    if (ent)
    {
        using PoisonEntity_fun = void(Entity*, bool);
        static auto poison_entity = (PoisonEntity_fun*)get_address("poison_entity");
        poison_entity(ent, true);
    }
}

void move_grid_entity(int32_t uid, float x, float y, LAYER layer)
{
    if (auto entity = get_entity_ptr(uid))
    {
        auto state = HeapBase::get().state();
        Vec2 offset;
        const auto actual_layer = enum_to_layer(layer, offset);
        state->layer(entity->layer)->move_grid_entity(entity, offset.x + x, offset.y + y, state->layers[actual_layer]);

        entity->detach(false);
        entity->x = offset.x + x;
        entity->y = offset.y + y;
        entity->set_layer(layer);
    }
}

void destroy_grid(int32_t uid)
{
    if (auto entity = get_entity_ptr(uid))
    {
        HeapBase::get().state()->layer(entity->layer)->destroy_grid_entity(entity);
    }
}

void destroy_grid(float x, float y, LAYER layer)
{
    auto state = HeapBase::get().state();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* entity = state->layers[actual_layer]->get_grid_entity_at(x, y))
    {
        state->layer(entity->layer)->destroy_grid_entity(entity);
    }
}

void add_item_to_shop(int32_t item_uid, int32_t shop_owner_uid)
{
    Movable* item = get_entity_ptr(item_uid)->as<Movable>();
    Entity* owner = get_entity_ptr(shop_owner_uid);
    if (item && owner && item->is_movable())
    {
        const static auto room_owners = {
            to_id("ENT_TYPE_MONS_SHOPKEEPER"),
            to_id("ENT_TYPE_MONS_MERCHANT"),
            to_id("ENT_TYPE_MONS_YANG"),
            to_id("ENT_TYPE_MONS_MADAMETUSK"),
            to_id("ENT_TYPE_MONS_STORAGEGUY"),
            to_id("ENT_TYPE_MONS_CAVEMAN_SHOPKEEPER"), // exception: not actually room owner
            to_id("ENT_TYPE_MONS_GHIST_SHOPKEEPE"),    // exception: not actually room owner
        };
        for (auto& it : room_owners)
        {
            if (owner->type->id == it) // TODO: check what happens if it's not room owner/shopkeeper
            {
                auto state = HeapBase::get().state();
                item->flags = setflag(item->flags, 23); // shop item
                item->flags = setflag(item->flags, 20); // Enable button prompt (flag is probably: show dialogs and other fx)
                state->layers[item->layer]->spawn_entity_over(to_id("ENT_TYPE_FX_SALEICON"), item, 0, 0);
                state->layers[item->layer]->spawn_entity_over(to_id("ENT_TYPE_FX_SALEDIALOG_CONTAINER"), item, 0, 0.5);

                auto& owner_details = state->room_owners.owned_items[item->uid];
                owner_details.owner_type = owner->type->id;
                owner_details.owner_uid = shop_owner_uid;
                return;
            }
        }
    }
}

void set_adventure_seed(int64_t first, int64_t second)
{
    static const size_t offset = get_address("adventure_seed");
    write_mem_prot(offset, first, true);
    write_mem_prot(offset + 8, second, true);
}

std::pair<int64_t, int64_t> get_adventure_seed(std::optional<bool> run_start)
{
    if (run_start.value_or(false))
    {
        auto bucket = Bucket::get();
        if (bucket->adventure_seed.first != 0)
            return bucket->adventure_seed;
        auto state = HeapBase::get().state();
        auto current = get_adventure_seed(false);
        for (uint8_t i = 0; i < state->level_count + (state->screen == 12 || state->screen == 14 ? 1 : 0); ++i)
            current.second -= current.first;
        bucket->adventure_seed.first = current.first;
        bucket->adventure_seed.second = current.second;
        return bucket->adventure_seed;
    }
    else
    {
        static const size_t offset = get_address("adventure_seed");
        return {memory_read<int64_t>(offset), memory_read<int64_t>(offset + 8)};
    }
}

void update_liquid_collision_at(float x, float y, bool add, std::optional<LAYER> layer)
{
    using UpdateLiquidCollision = void(LiquidPhysics*, int32_t, int32_t, uint8_t);
    static UpdateLiquidCollision* RemoveLiquidCollision_fun = (UpdateLiquidCollision*)get_address("remove_from_liquid_collision_map");
    static UpdateLiquidCollision* AddLiquidCollision_fun = (UpdateLiquidCollision*)get_address("add_to_liquid_collision_map");
    auto state = get_state_ptr();
    uint8_t actual_layer = enum_to_layer(layer.value_or(LAYER::FRONT));

    if (add)
        AddLiquidCollision_fun(state->liquid_physics, static_cast<int32_t>(std::round(x)), static_cast<int32_t>(std::round(y)), actual_layer);
    else
        RemoveLiquidCollision_fun(state->liquid_physics, static_cast<int32_t>(std::round(x)), static_cast<int32_t>(std::round(y)), actual_layer);
}

void add_entity_to_liquid_collision(uint32_t uid, bool add)
{
    using AddEntityLiquidCollision = void(LiquidPhysics*, Entity*, uint8_t);
    static AddEntityLiquidCollision* add_entity_liquid_collision = (AddEntityLiquidCollision*)get_address("add_movable_to_liquid_collision_map");
    auto state = get_state_ptr();
    auto entity = get_entity_ptr(uid);
    if (!entity)
        return;

    auto map = state->liquid_physics->push_blocks;
    if (!map)
        return;

    auto it = map->find(uid);

    // if it already exists we can't add it again, since it will create the collision struct anyway and just overwrite the pointer to it in the map
    // the actual collision struct is held somewhere else, unrelated to this map
    if (add && it == map->end())
        add_entity_liquid_collision(state->liquid_physics, entity, entity->layer);
    else if (!add && it != map->end())
    {
        // very illegal, don't do this, we can because we're professionals xd
        // game loops thru the map and checks if uid still exists, if not, it removes the collision
        // which is some bigger struct held in some weird container, and the function is doing other stuff, so this is the easiest way besides killing the entity
        auto key = const_cast<uint32_t*>(&it->first);
        *key = ~0u;
    }
}

std::pair<uint8_t, uint8_t> get_liquids_at(float x, float y, LAYER layer)
{
    uint8_t actual_layer = enum_to_layer(layer);
    LiquidPhysics* liquid_physics = HeapBase::get().liquid_physics();
    // if (y > 125.5f || y < .0f || x > 85.5f || x < .0f) // Original check by the game, can result is accesing the array out of bounds
    //     return 0;
    if (actual_layer != get_liquid_layer() || y < .0f || x < .0f)
        return {0, 0};

    uint32_t ix = static_cast<int>((x + 0.5f) / 0.3333333f);
    uint32_t iy = static_cast<int>((y + 0.5f) / 0.3333333f);
    if (iy >= (g_level_max_y * 3) || ix >= (g_level_max_x * 3))
        return {0, 0};

    auto& liquids_at = (*liquid_physics->liquids_by_third_of_tile)[iy][ix];
    return {liquids_at.water, liquids_at.lava};
}

void game_log(std::string message)
{
    using GameLogFun = void(std::ofstream*, const char*, void*, LogLevel);
    static const auto game_log_fun = (GameLogFun*)get_address("game_log_function");
    static const auto log_stream = (std::ofstream*)memory_read<int64_t>(get_address("game_log_stream"));
    game_log_fun(log_stream, message.c_str(), nullptr, LogLevel::Info);
}

void load_death_screen()
{
    HeapBase::get().state()->screen_death->init();
}

void save_progress()
{
    using SaveProgress = void(SaveRelated*);
    static auto save_game_to_file = (SaveProgress*)get_address("save_progress");
    static auto gm = get_game_manager();
    save_game_to_file(gm->save_related);
}

void set_level_string(std::u16string_view text)
{
    static const auto hud_text_address = get_address("hud_level_text");
    static const auto journal_text_address = get_address("journal_level_text");
    static const auto journal_map_text_address = get_address("journal_map_level_text");
    static char16_t* data;
    static size_t text_data_length = 0;

    if (text_data_length == 0 || text_data_length < text.length())
    {
        if (text_data_length != 0)
        {
            VirtualFree(data, 0, MEM_RELEASE);
        }
        text_data_length = text.length() == 0 ? 1 : text.length(); // just to make sure it's not set to 0

        auto new_array_offset = hud_text_address;
        if (journal_text_address > new_array_offset)
        {
            new_array_offset = journal_text_address;
        }
        if (journal_map_text_address > new_array_offset)
        {
            new_array_offset = journal_map_text_address;
        }

        data = (char16_t*)alloc_mem_rel32(new_array_offset + 4, (text_data_length + 5) * sizeof(char16_t));
        *data = 0x25; // for the theme name in the journal map
        *(data + 1) = 0x6C;
        *(data + 2) = 0x73;
        *(data + 3) = 0x0A;

        const int32_t hud_rel = static_cast<int32_t>((size_t)(data + 4) - (hud_text_address + 4));
        const int32_t journal_rel = static_cast<int32_t>((size_t)(data + 4) - (journal_text_address + 4));
        const int32_t journal_map_rel = static_cast<int32_t>((size_t)(data) - (journal_map_text_address + 4));

        write_mem_prot(hud_text_address, hud_rel, true);
        write_mem_prot(journal_text_address, journal_rel, true);
        write_mem_prot(journal_map_text_address, journal_map_rel, true);
    }
    memcpy(data + 4, text.data(), text.length() * sizeof(char16_t));
    *(data + 4 + text.length()) = NULL;
}

void set_frametime(std::optional<double> frametime)
{
    static const size_t offset = get_address("engine_frametime");
    if (frametime.has_value())
        write_mem_recoverable("engine_frametime", offset, frametime.value(), true);
    else
        recover_mem("engine_frametime");
}

double get_frametime()
{
    static const size_t offset = get_address("engine_frametime");
    return memory_read<double>(offset);
}

void set_frametime_inactive(std::optional<double> frametime)
{
    static const size_t offset = get_address("engine_frametime") + 0x10;
    if (frametime.has_value())
        write_mem_recoverable("engine_frametime_inactive", offset, frametime.value(), true);
    else
        recover_mem("engine_frametime_inactive");
}

double get_frametime_inactive()
{
    static const size_t offset = get_address("engine_frametime") + 0x10;
    return memory_read<double>(offset);
}

ENT_TYPE add_custom_type(std::vector<ENT_TYPE> types)
{
    return (ENT_TYPE)add_new_custom_type(std::move(types));
}

ENT_TYPE add_custom_type()
{
    return (ENT_TYPE)add_new_custom_type({});
}

int32_t get_current_money()
{
    auto state = HeapBase::get().state();
    int32_t money = state->money_shop_total;
    for (auto& inventory : state->items->player_inventories)
    {
        money += inventory.money;
        money += inventory.collected_money_total;
    }
    return money;
}

int32_t add_money(int32_t amount, std::optional<uint8_t> display_time)
{
    auto state = HeapBase::get().state();
    auto hud = get_hud();
    state->money_shop_total += amount;
    hud->money.counter += amount;
    hud->money.timer = display_time.value_or(0x3C);
    return get_current_money();
}

int32_t add_money_slot(int32_t amount, uint8_t player_slot, std::optional<uint8_t> display_time)
{
    auto state = HeapBase::get().state();
    auto hud = get_hud();
    uint8_t slot = player_slot - 1;
    if (slot > 3)
        return get_current_money();

    state->items->player_inventories[slot].money += amount;
    hud->money.counter += amount;
    hud->money.timer = display_time.value_or(0x3C);
    return get_current_money();
}

void destroy_layer(uint8_t layer)
{
    static const size_t offset = get_address("unload_layer");
    auto items = HeapBase::get().state()->items;
    for (auto i = 0; i < MAX_PLAYERS; ++i)
    {
        if (items->players[i] && items->players[i]->layer == layer)
            items->players[i] = nullptr;
    }
    auto* layer_ptr = HeapBase::get().state()->layer(layer);
    typedef void destroy_func(Layer*);
    static destroy_func* df = (destroy_func*)(offset);
    df(layer_ptr);
}

void destroy_level()
{
    destroy_layer(0);
    destroy_layer(1);
}

void create_layer(uint8_t layer)
{
    static const size_t offset = get_address("init_layer");
    auto* layer_ptr = HeapBase::get().state()->layer(layer);
    typedef void init_func(Layer*);
    static init_func* ilf = (init_func*)(offset);
    ilf(layer_ptr);
}

void create_level()
{
    create_layer(0);
    create_layer(1);
}

bool get_start_level_paused()
{
    return mem_written("start_level_paused");
}

bool g_speedhack_hooked = false;
float g_speedhack_multiplier = 1.0;
LARGE_INTEGER g_speedhack_prev;
LARGE_INTEGER g_speedhack_current;
LARGE_INTEGER g_speedhack_fake;
PVOID g_oldqpc;

#define PtrFromRva(base, rva) (((PBYTE)base) + rva)

// I didn't write this one, I just found it in the shady parts of the internet
// This could probably be done with detours
BOOL HookIAT(const char* szModuleName, const char* szFuncName, PVOID pNewFunc, PVOID* pOldFunc)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)PtrFromRva(pDosHeader, pDosHeader->e_lfanew);

    // Make sure we have valid data
    if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
        return FALSE;

    // Grab a pointer to the import data directory
    PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)PtrFromRva(pDosHeader, pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (UINT uIndex = 0; pImportDescriptor[uIndex].Characteristics != 0; uIndex++)
    {
        char* szDllName = (char*)PtrFromRva(pDosHeader, pImportDescriptor[uIndex].Name);

        // Is this our module?
        if (_strcmpi(szDllName, szModuleName) != 0)
            continue;

        if (!pImportDescriptor[uIndex].FirstThunk || !pImportDescriptor[uIndex].OriginalFirstThunk)
            return FALSE;

        PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)PtrFromRva(pDosHeader, pImportDescriptor[uIndex].FirstThunk);
        PIMAGE_THUNK_DATA pOrigThunk = (PIMAGE_THUNK_DATA)PtrFromRva(pDosHeader, pImportDescriptor[uIndex].OriginalFirstThunk);

        for (; pOrigThunk->u1.Function != NULL; pOrigThunk++, pThunk++)
        {
            // We can't process ordinal imports just named
            if (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                continue;

            PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)PtrFromRva(pDosHeader, pOrigThunk->u1.AddressOfData);

            // Is this our function?
            if (_strcmpi(szFuncName, (char*)import->Name) != 0)
                continue;

            DWORD dwJunk = 0;
            MEMORY_BASIC_INFORMATION mbi;

            // Make the memory section writable
            VirtualQuery(pThunk, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
            if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect))
                return FALSE;

            // Save the old pointer
            *pOldFunc = (PVOID*)(DWORD_PTR)pThunk->u1.Function;

// Write the new pointer based on CPU type
#ifdef _WIN64
            pThunk->u1.Function = (ULONGLONG)(DWORD_PTR)pNewFunc;
#else
            pThunk->u1.Function = (DWORD)(DWORD_PTR)pNewFunc;
#endif

            if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwJunk))
                return TRUE;
        }
    }
    return FALSE;
}

bool __stdcall QueryPerformanceCounterHook(LARGE_INTEGER* counter)
{
    QueryPerformanceCounter(&g_speedhack_current);
    g_speedhack_fake.QuadPart += (long long)((g_speedhack_current.QuadPart - g_speedhack_prev.QuadPart) * g_speedhack_multiplier);
    g_speedhack_prev = g_speedhack_current;
    *counter = g_speedhack_fake;
    return true;
}

void set_speedhack(std::optional<float> multiplier)
{
    g_speedhack_multiplier = multiplier.value_or(1.0f);
    if (!g_speedhack_hooked)
    {
        QueryPerformanceCounter(&g_speedhack_prev);
        g_speedhack_fake = g_speedhack_prev;
        HookIAT("kernel32.dll", "QueryPerformanceCounter", QueryPerformanceCounterHook, &g_oldqpc);
        g_speedhack_hooked = true;
    }
}

float get_speedhack()
{
    return g_speedhack_multiplier;
}

void init_adventure()
{
    // TODO: I didn't check exactly what this does, but it fixes issues with character select being broken after quick start
    static const size_t offset = get_address("init_adventure");
    typedef void init_func();
    static init_func* iaf = (init_func*)(offset);
    iaf();
}

void init_seeded(std::optional<uint32_t> seed)
{
    static const size_t offset = get_address("init_seeded");
    typedef void init_func(void*, uint32_t);
    static init_func* isf = (init_func*)(offset);
    auto* state = HeapBase::get().state();
    isf(state, seed.value_or(state->seed));
}

uint8_t get_liquid_layer()
{
    static auto addr = get_address("check_if_collides_with_liquid_layer");
    return memory_read<uint8_t>(addr);
}

uint32_t lowbias32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}
uint32_t lowbias32_r(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x43021123U;
    x ^= x >> 15 ^ x >> 30;
    x *= 0x1d69e2a5U;
    x ^= x >> 16;
    return x;
}
