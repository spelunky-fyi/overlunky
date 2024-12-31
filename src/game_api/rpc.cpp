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
#include "illumination.hpp"             //
#include "items.hpp"                    // for Items
#include "layer.hpp"                    // for EntityList, EntityList::Range, Layer
#include "logger.h"                     // for DEBUG
#include "math.hpp"                     // for AABB
#include "memory.hpp"                   // for write_mem_prot, write_mem_recoverable
#include "movable.hpp"                  // for Movable
#include "online.hpp"                   // for Online
#include "particles.hpp"                // for ParticleEmitterInfo
#include "prng.hpp"                     // for PRNG
#include "screen.hpp"                   //
#include "search.hpp"                   // for get_address, find_inst
#include "state.hpp"                    // for State, get_state_ptr, enum_to_layer
#include "state_structs.hpp"            // for ShopRestrictedItem, Illumination
#include "thread_utils.hpp"             // for OnHeapPointer
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
        auto* layer_ptr = State::get().layer(entity->layer);

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

uint32_t get_entity_flags(uint32_t uid)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t uid, uint32_t flags)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t uid)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t uid, uint32_t flags)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t uid)
{
    auto ent = get_entity_ptr(uid)->as<Movable>();
    if (ent && ent->is_movable())
        return ent->move_state;
    return 0;
}

uint32_t get_level_flags()
{
    auto& state = State::get();
    return state.flags();
}

void set_level_flags(uint32_t flags)
{
    auto& state = State::get();
    state.set_flags(flags);
}

ENT_TYPE get_entity_type(uint32_t uid)
{
    auto entity = get_entity_ptr(uid);
    if (entity)
        return entity->type->id;

    return UINT32_MAX; // TODO: shouldn't this be 0?
}

std::vector<Player*> get_players(StateMemory* state)
{
    state = state != nullptr
                ? state
                : State::get().ptr();

    std::vector<Player*> found;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state->items->player(i);
        if (player)
            found.push_back((Player*)player);
    }
    return found;
}

std::tuple<float, float, float, float> screen_aabb(float left, float top, float right, float bottom)
{
    auto [sx1, sy1] = State::screen_position(left, top);
    auto [sx2, sy2] = State::screen_position(right, bottom);
    return std::tuple{sx1, sy1, sx2, sy2};
}

float screen_distance(float x)
{
    auto a = State::screen_position(0, 0);
    auto b = State::screen_position(x, 0);
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

void set_door_target(uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
{
    if (auto door = get_entity_ptr(uid)->as<ExitDoor>())
    {
        door->world = w;
        door->level = l;
        door->theme = t;
        door->special_door = true;
    }
}

std::tuple<uint8_t, uint8_t, uint8_t> get_door_target(uint32_t uid)
{
    auto door = get_entity_ptr(uid)->as<ExitDoor>();
    if (door == nullptr || !door->special_door)
        return std::make_tuple((uint8_t)0, (uint8_t)0, (uint8_t)0);

    return std::make_tuple(door->world, door->level, door->theme);
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

void carry(uint32_t mount_uid, uint32_t rider_uid)
{
    auto mount = get_entity_ptr(mount_uid)->as<Mount>();
    auto rider = get_entity_ptr(rider_uid)->as<Movable>();
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
    if (ent->items.size > 0)
    {
        for (auto item : ent->items.entities())
        {
            item->flags = flipflag(item->flags, 17);
        }
    }
}

void set_camera_position(float cx, float cy)
{
    auto& state = State::get();
    state.set_camera_position(cx, cy);
}

void warp(uint8_t world, uint8_t level, uint8_t theme)
{
    auto& state = State::get();
    state.warp(world, level, theme);
}

void set_seed(uint32_t seed)
{
    auto& state = State::get();
    state.set_seed(seed);
}

void set_arrowtrap_projectile(ENT_TYPE regular_entity_type, ENT_TYPE poison_entity_type)
{
    static const auto arrowtrap = get_address("arrowtrap_projectile");
    static const auto poison_arrowtrap = get_address("poison_arrowtrap_projectile");
    write_mem_prot(arrowtrap, regular_entity_type, true);
    write_mem_prot(poison_arrowtrap, poison_entity_type, true);
}

float* g_sparktrap_parameters{nullptr};
void modify_sparktraps(float angle_increment, float distance)
{
    if (g_sparktrap_parameters == nullptr)
    {
        static const auto offset = get_address("sparktrap_angle_increment") + 4;

        if (memory_read<uint8_t>(offset - 1) == 0x89) // check if sparktraps_hack is active
            return;

        const int32_t distance_offset = 0xF1;
        g_sparktrap_parameters = (float*)alloc_mem_rel32(offset + 4, sizeof(float) * 2);
        if (!g_sparktrap_parameters)
            return;

        int32_t rel = static_cast<int32_t>((size_t)g_sparktrap_parameters - (offset + 4));
        write_mem_prot(offset, rel, true);
        write_mem_prot(offset + distance_offset, (int32_t)(rel - distance_offset + sizeof(float)), true);
    }
    *g_sparktrap_parameters = angle_increment;
    *(g_sparktrap_parameters + 1) = distance;
}

float* get_sparktraps_parameters_ptr() // only for the UI
{
    return g_sparktrap_parameters;
}

void activate_sparktraps_hack(bool activate)
{
    if (activate)
    {
        static const auto offset = get_address("sparktrap_angle_increment");
        const int32_t distance_offset = 0xF1;

        write_mem_recoverable("sparktraps_hack", offset, "\xF3\x0F\x58\x89\x6C\x01\x00\x00"sv, true);
        write_mem_recoverable("sparktraps_hack", offset + distance_offset, "\xF3\x0F\x10\xB9\x70\x01\x00\x00"sv, true);
    }
    else
    {
        recover_mem("sparktraps_hack");
    }
}

void set_storage_layer(LAYER layer)
{
    static const auto storage_layer = get_address("storage_layer");
    if (layer == LAYER::FRONT || layer == LAYER::BACK)
        write_mem_prot(storage_layer, 0x1300 + 8 * (uint8_t)layer, true);
}

void set_kapala_blood_threshold(uint8_t threshold)
{
    static const auto kapala_blood_threshold = get_address("kapala_blood_threshold");
    write_mem_prot(kapala_blood_threshold, threshold, true);
}

void set_kapala_hud_icon(int8_t icon_index)
{
    static const size_t instruction_offset = get_address("kapala_hud_icon");
    static const size_t icon_index_offset = instruction_offset + 0x12;
    static const uint32_t distance = static_cast<uint32_t>(icon_index_offset - (instruction_offset + 7));

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
    static const auto blood_multiplication = get_address("blood_multiplication");
    write_mem_prot(blood_multiplication, vladscape_multiplier, true);
}

std::vector<int64_t> read_prng()
{
    std::vector<int64_t> prng_raw;
    prng_raw.resize(20);
    auto prng = reinterpret_cast<int64_t*>(HeapBase::get().prng());
    std::memcpy(prng_raw.data(), prng, sizeof(int64_t) * 20);
    return prng_raw;
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

void drop(uint32_t who_uid, std::optional<uint32_t> what_uid)
{
    auto ent = get_entity_ptr(who_uid);
    if (ent == nullptr)
        return;

    if (!ent->is_movable()) // game would probably use the is_player_or_monster function here, since they are the only ones who should be able to hold something
        return;

    auto mov = ent->as<Movable>();
    if (what_uid.has_value()) // should we handle what_uid = -1 the same way?
    {
        auto item = get_entity_ptr(what_uid.value());
        if (item == nullptr)
            return;
        if (item->overlay != mov && mov->holding_uid == what_uid)
            return;
    }
    mov->drop();
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
    static size_t phase1_offset;
    if (phase1_offset == 0)
    {
        // from 1.23.x onwards, there are now two instructions per phase that reference the y-level float
        const size_t phase_1_instruction_a = get_address("olmec_transition_phase_1_y_level");
        const size_t phase_1_instruction_b = phase_1_instruction_a + 0xd;

        const size_t phase_2_instruction_a = get_address("olmec_transition_phase_2_y_level");
        const size_t phase_2_instruction_b = phase_2_instruction_a + 0x11;
        phase1_offset = (size_t)alloc_mem_rel32(phase_2_instruction_b + 4, sizeof(float) * 2);
        if (!phase1_offset)
            return;

        auto phase2_offset = phase1_offset + 0x4;

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
        *(float*)phase1_offset = y;
    }
    else if (phase == 2)
    {
        *(float*)(phase1_offset + sizeof(float)) = y;
    }
}

void force_olmec_phase_0(bool b)
{
    static const size_t offset = get_address("olmec_transition_phase_1");

    if (b)
        write_mem_recoverable("force_olmec_phase_0", offset, "\xEB\x2E"s, true); // jbe -> jmp
    else
        recover_mem("force_olmec_phase_0");
}

void set_ghost_spawn_times(uint32_t normal, uint32_t cursed)
{
    static const auto ghost_spawn_time = get_address("ghost_spawn_time");
    static const auto ghost_spawn_time_cursed_p1 = get_address("ghost_spawn_time_cursed_player1");
    static const auto ghost_spawn_time_cursed_p2 = get_address("ghost_spawn_time_cursed_player2");
    static const auto ghost_spawn_time_cursed_p3 = get_address("ghost_spawn_time_cursed_player3");
    static const auto ghost_spawn_time_cursed_p4 = get_address("ghost_spawn_time_cursed_player4");

    write_mem_prot(ghost_spawn_time, normal, true);
    write_mem_prot(ghost_spawn_time_cursed_p1, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p2, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p3, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p4, cursed, true);
}

void set_time_ghost_enabled(bool b)
{
    static size_t offset_trigger = 0;
    static size_t offset_toast_trigger = 0;
    if (offset_trigger == 0)
    {
        auto& memory = Memory::get();
        offset_trigger = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
        offset_toast_trigger = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TOAST_TRIGGER, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
    }
    if (b)
    {
        recover_mem("set_time_ghost_enabled");
    }
    else
    {
        write_mem_recoverable("set_time_ghost_enabled", offset_trigger, "\xC3\x90\x90\x90"s, true);
        write_mem_recoverable("set_time_ghost_enabled", offset_toast_trigger, "\xC3\x90\x90\x90"s, true);
    }
}

void set_time_jelly_enabled(bool b)
{
    auto& memory = Memory::get();
    static const size_t offset = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_COSMIC_OCEAN, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
    if (b)
        recover_mem("set_time_jelly_enabled");
    else
        write_mem_recoverable("set_time_jelly_enabled", offset, "\xC3\x90\x90\x90"s, true);
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
    auto state = State::get().ptr(); // the game gets level gen from heap pointer and we always get it from state, not sure if it matters
    typedef bool coord_inside_shop_zone_func(LevelGenSystem*, uint32_t layer, float x, float y);
    coord_inside_shop_zone_func* ciszf = (coord_inside_shop_zone_func*)(offset);
    return ciszf(state->level_gen, enum_to_layer(layer), x, y);
}

void set_journal_enabled(bool b)
{
    get_journal_enabled() = b;
}

void set_camp_camera_bounds_enabled(bool b)
{
    static const size_t offset = get_address("enforce_camp_camera_bounds");
    if (b)
        recover_mem("camp_camera_bounds");
    else
        write_mem_recoverable("camp_camera_bounds", offset, "\xC3\x90\x90"s, true);
}

void set_explosion_mask(int32_t mask)
{
    static const size_t addr = get_address("explosion_mask");
    if (mask == -1)
        recover_mem("explosion_mask");
    else
        write_mem_recoverable("explosion_mask", addr, mask, true);
}

void set_max_rope_length(uint8_t length)
{
    uint32_t length_32 = length;
    static const auto attach_thrown_rope = get_address("attach_thrown_rope_to_background");
    static const auto process_ropes_one = get_address("process_ropes_one");
    static const auto process_ropes_two = get_address("process_ropes_two");
    static const auto process_ropes_three = get_address("process_ropes_three");

    // there's four instances where the max (default=6) is used

    // 1) When throwing a rope and it attaches to the background, the initial entity is
    // given a start value in its segment_nr_inverse variable
    write_mem_prot(attach_thrown_rope, length_32, true);

    // 2) and 3) at the top of the rope processing function are two comparisons to the max
    write_mem_prot(process_ropes_one, length, true);
    write_mem_prot(process_ropes_two, length, true);

    // 4) in the same function at the end of the little loop of process_ropes_two is a comparison to n-1
    uint8_t length_minus_one_8 = length - 1;
    write_mem_prot(process_ropes_three, length_minus_one_8, true);
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

void enter_door(int32_t player_uid, int32_t door_uid)
{
    auto player = get_entity_ptr(player_uid);
    auto door = get_entity_ptr(door_uid)->as<Door>();
    if (player == nullptr || door == nullptr)
        return;

    door->enter(player);
}

void change_sunchallenge_spawns(std::vector<ENT_TYPE> ent_types)
{
    // [Known_Issue]: as all the functions that base some functionality on static, this can break if used in PL and OV simultaneously
    static uintptr_t offset;
    static uintptr_t new_code_address;
    if (offset == 0)
    {
        offset = get_address("sun_challenge_generator_ent_types");

        // just so we can recover the oryginal later
        save_mem_recoverable("sunchallenge_spawn", offset, 14, true);
    }
    const size_t table_offset = offset + 10; // offset to the offset of ent_type table
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(table_offset) + table_offset + 4);
    bool was_edited_before = mem_written("sunchallenge_spawn");
    if (ent_types.size() == 0)
    {
        recover_mem("sunchallenge_spawn");
        if (was_edited_before)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        // just free it since it's just easier to put the code again
        if (new_code_address != 0)
        {
            VirtualFree(reinterpret_cast<LPVOID>(new_code_address), 0, MEM_RELEASE);
            new_code_address = 0;
        }
        return;
    }
    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(table_offset + 4, data_size);
    if (new_array)
    {
        std::memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (table_offset + 4));
        write_mem_prot(table_offset, rel, true);

        if (new_code_address == 0)
        {
            std::string new_code = fmt::format("\x31\xD2\xB9{}\xF7\xF1\x67\x8D\x04\x95\x00\x00\x00\x00"sv, to_le_bytes(static_cast<uint32_t>(ent_types.size())));
            // xor edx, edx                 ; dividend high half = 0.
            // mov ecx, ent_types.size()    ; dividend low half
            // div ecx                      ; division, (divisor already in rax)
            //                              ; edx - remainder
            // lea eax,[edx * 4 + 0]        ; multiply by 4 (sizeof ENT_TYPE) and put result in rax

            new_code_address = patch_and_redirect(offset, 7, new_code, true);
        }
        else // update the size since the code is in place
            write_mem_prot(new_code_address + 3, to_le_bytes(static_cast<uint32_t>(ent_types.size())), true);

        if (was_edited_before)
            VirtualFree(old_types_array, 0, MEM_RELEASE);
    }
}

void change_diceshop_prizes(std::vector<ENT_TYPE> ent_types)
{
    static const auto offset = get_address("dice_shop_prizes_id_roll");
    static const auto array_offset = get_address("dice_shop_prizes");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);
    bool original_instr = (memory_read<uint8_t>(offset) == 0x89);

    if (ent_types.size() > 255 || ent_types.size() < 6) // has to be min 6 as the game needs 6 uniqe item ids for prize_dispenser
    {
        if (!ent_types.size())
        {
            if (!original_instr)
                VirtualFree(old_types_array, 0, MEM_RELEASE);

            recover_mem("diceshop_prizes");
        }
        return;
    }

    if ((original_instr && ent_types.size() == 25) ||                              // if it's the unchanged instruction and we set the same number of ent_type's
        (!original_instr && memory_read<uint8_t>(offset + 5) == ent_types.size())) // or new instruction but the same size
    {
        for (unsigned int i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("diceshop_prizes", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);

    if (new_array)
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("diceshop_prizes", array_offset, rel, true);

        if (original_instr)
        {
            std::string new_code = fmt::format("\x50\x31\xC0\x41\xB3{}\x88\xD0\x41\xF6\xF3\x88\xE2\x58"sv, to_le_bytes((uint8_t)ent_types.size()));
            // push rax
            // xor eax, eax
            // mov r11b, (size)
            // mov al, dl
            // divb r11b
            // mov dl, ah
            // pop rax
            write_mem_recoverable("diceshop_prizes", offset, new_code, true);
        }
        else
        {
            write_mem_recoverable("diceshop_prizes", offset + 5, (uint8_t)ent_types.size(), true);
        }
    }
}

void change_altar_damage_spawns(std::vector<ENT_TYPE> ent_types)
{
    if (ent_types.size() > 255)
        return;

    static const auto array_offset = get_address("altar_break_ent_types");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);
    const auto code_offset = array_offset + 0xDD;
    const auto instruction_shr = array_offset + 0x13D;
    const auto instruction_to_modifiy = array_offset + 0x204;
    const auto original_instr = (memory_read<uint8_t>(instruction_shr) == 0x41);
    if (ent_types.empty())
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        recover_mem("altar_damage_spawn");
        return;
    }
    if (!original_instr && memory_read<uint8_t>(code_offset + 2) == ent_types.size())
    {
        // original array is used for something else as well, so i never edit that content
        for (uint32_t i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("altar_damage_spawn", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }
    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);
    if (new_array)
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("altar_damage_spawn", array_offset, rel, true);

        if (original_instr)
        {
            std::string new_code = fmt::format("\x41\xB1{}\x48\xC1\xE8\x38\x41\xF6\xF1\x49\x89\xC1"sv, to_le_bytes((uint8_t)ent_types.size()));
            // mov R9b, (size)
            // shr RAX, 0x38
            // divb R9b
            // mov R9, RAX
            write_mem_recoverable("altar_damage_spawn", code_offset, new_code, true);
            write_mem_recoverable("altar_damage_spawn", instruction_shr, "\x49\xC1\xE9\x08"sv, true); // shr r9,0x8
            write_mem_recoverable("altar_damage_spawn", instruction_to_modifiy, (uint8_t)0x8C, true); // r9+r12 => r12+r9*4
        }
        else
        {
            write_mem_recoverable("altar_damage_spawn", code_offset + 2, (uint8_t)ent_types.size(), true);
        }
    }
}

void change_waddler_drop(std::vector<ENT_TYPE> ent_types)
{
    static bool modified = false;

    static const auto offset = get_address("waddler_drop_size");
    static const auto array_offset = get_address("waddler_drop_array");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);

    if (ent_types.size() > 255 || ent_types.size() < 1)
    {
        if (!ent_types.size())
        {
            if (modified)
                VirtualFree(old_types_array, 0, MEM_RELEASE);

            recover_mem("waddler_drop");
            modified = false;
        }
        return;
    }

    if ((!modified && ent_types.size() == 3) ||                         // if it's the unchanged instruction and we set the same number of ent_type's
        (modified && memory_read<uint8_t>(offset) == ent_types.size())) // or new instruction but the same size
    {
        for (unsigned int i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("waddler_drop", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);

    if (new_array)
    {
        if (modified)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("waddler_drop", array_offset, rel, true);
        write_mem_recoverable("waddler_drop", offset, (uint8_t)ent_types.size(), true);
        modified = true;
    }
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

void modify_ankh_health_gain(uint8_t health, uint8_t beat_add_health)
{
    static size_t offsets[4];
    static const auto size_minus_one = get_address("ankh_health");
    if (!health)
    {
        recover_mem("ankh_health");
        return;
    }
    if (size_minus_one && beat_add_health)
    {
        if (!offsets[0])
        {
            auto& memory = Memory::get();
            size_t offset = size_minus_one - memory.exe_address();
            const auto limit_size = offset + 0x200;

            offsets[0] = find_inst(memory.exe(), "\x41\x80\xBF\x17\x01\x00\x00"sv, offset, limit_size, "ankh_health_gain_1");
            offsets[1] = find_inst(memory.exe(), "\x41\x80\xBF\x17\x01\x00\x00"sv, offsets[0] + 7, limit_size, "ankh_health_gain_2");
            offsets[2] = find_inst(memory.exe(), "\x0F\x42\xCA\x83\xC0"sv, offset, limit_size, "ankh_health_gain_3");
            offsets[3] = find_inst(memory.exe(), "\x8A\x83\x17\x01\x00\x00\x3C"sv, offset, std::nullopt, "ankh_health_gain_4"); // this is some bs
            if (!offsets[0] || !offsets[1] || !offsets[2] || !offsets[3])
            {
                offsets[0] = 0;
                return;
            }
            offsets[0] = memory.at_exe(offsets[0] + 7); // add pattern size
            offsets[1] = memory.at_exe(offsets[1] + 7);
            offsets[2] = memory.at_exe(offsets[2] + 5);
            offsets[3] = memory.at_exe(offsets[3] + 7);
        }
        const uint8_t game_maxhp = memory_read<uint8_t>(offsets[2] - 14);
        if (health > game_maxhp)
            health = game_maxhp;

        if (health % beat_add_health == 0)
        {
            write_mem_recoverable("ankh_health", size_minus_one, (uint8_t)(health - 1), true);
            write_mem_recoverable("ankh_health", offsets[0], health, true);
            write_mem_recoverable("ankh_health", offsets[1], health, true);
            write_mem_recoverable("ankh_health", offsets[2], beat_add_health, true);
            if (health < 4)
            {
                write_mem_recoverable("ankh_health", offsets[3], (uint8_t)0, true);
            }
            else
            {
                if (memory_read<uint8_t>(offsets[3]) != 3)
                    recover_mem("ankh_health", offsets[3]);
            }
        }
    }
}

void move_grid_entity(int32_t uid, float x, float y, LAYER layer)
{
    if (auto entity = get_entity_ptr(uid))
    {
        auto& state = State::get();
        Vec2 offset;
        const auto actual_layer = enum_to_layer(layer, offset);
        state.layer(entity->layer)->move_grid_entity(entity, offset.x + x, offset.y + y, state.layer(actual_layer));

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
        auto& state = State::get();
        state.layer(entity->layer)->destroy_grid_entity(entity);
    }
}

void destroy_grid(float x, float y, LAYER layer)
{
    auto& state = State::get();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* entity = state.layer(actual_layer)->get_grid_entity_at(x, y))
    {
        state.layer(entity->layer)->destroy_grid_entity(entity);
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
                auto state = State::get().ptr();
                item->flags = setflag(item->flags, 23); // shop item
                item->flags = setflag(item->flags, 20); // Enable button prompt (flag is probably: show dialogs and other fx)
                state->layers[item->layer]->spawn_entity_over(to_id("ENT_TYPE_FX_SALEICON"), item, 0, 0);
                state->layers[item->layer]->spawn_entity_over(to_id("ENT_TYPE_FX_SALEDIALOG_CONTAINER"), item, 0, 0.5);

                ItemOwnerDetails iod{shop_owner_uid, owner->type->id};
                state->room_owners.owned_items.insert({item->uid, iod});
                return;
            }
        }
    }
}

void change_poison_timer(int16_t frames)
{
    static const size_t offset_first = get_address("first_poison_tick_timer_default");
    static const size_t offset_subsequent = get_address("subsequent_poison_tick_timer_default");

    if (frames == -1)
    {
        recover_mem("change_poison_timer");
    }
    else
    {
        write_mem_recoverable("change_poison_timer", offset_first, frames, true);
        write_mem_recoverable("change_poison_timer", offset_subsequent, frames, true);
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
        auto state = State::get().ptr();
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
    LiquidPhysics* liquid_physics = State::get().ptr()->liquid_physics;
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

bool disable_floor_embeds(bool disable)
{
    static const auto address = get_address("spawn_floor_embeds");
    const bool current_value = memory_read<uint8_t>(address) == 0xc3;
    if (disable)
        write_mem_recoverable("disable_floor_embeds", address, "\xC3"sv, true);
    else
        recover_mem("disable_floor_embeds");
    return current_value;
}

void set_cursepot_ghost_enabled(bool enable)
{
    static const auto address = get_address("ghost_jar_ghost_spawn");
    if (!enable)
        write_mem_recoverable("ghost_jar_ghost_spawn", address, "\x90\x90\x90\x90\x90"sv, true);
    else
        recover_mem("ghost_jar_ghost_spawn");
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
    auto state = State::get().ptr();
    state->screen_death->init();
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

void set_ending_unlock(ENT_TYPE type)
{
    static const ENT_TYPE first = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
    static const ENT_TYPE last = to_id("ENT_TYPE_CHAR_CLASSIC_GUY");
    if (type >= first && type <= last)
    {
        static const auto offset = get_address("ending_unlock");
        const int32_t char_offset = 10;

        write_mem_recoverable("ending_unlock", offset, "\x90\x90\x90\x90\x90\x90\x90\x90"sv, true);
        write_mem_recoverable("ending_unlock", offset + char_offset, type, true);
    }
    else
    {
        recover_mem("ending_unlock");
    }
}

void set_olmec_cutscene_enabled(bool enable)
{
    set_skip_olmec_cutscene(!enable);
}

void set_tiamat_cutscene_enabled(bool enable)
{
    set_skip_tiamat_cutscene(!enable);
}

void activate_tiamat_position_hack(bool activate)
{
    static const auto code_addr = get_address("tiamat_attack_position");

    static const std::string_view code{"\xF3\x0F\x5C\xBE\x78\x01\x00\x00"sv   // subss  xmm7,DWORD PTR [rsi+0x178]
                                       "\xF3\x0F\x5C\xB6\x7C\x01\x00\x00"sv}; // subss  xmm6,DWORD PTR [rsi+0x17C]

    if (activate)
        write_mem_recoverable("activate_tiamat_position_hack", code_addr, code, true);
    else
        recover_mem("activate_tiamat_position_hack");
}

void activate_crush_elevator_hack(bool activate)
{
    auto& memory = Memory::get();
    static size_t offsets[3];
    if (offsets[0] == 0)
    {
        auto func_offset = get_virtual_function_address(VTABLE_OFFSET::ACTIVEFLOOR_CRUSHING_ELEVATOR, 78);

        offsets[0] = find_inst(memory.exe(), "\xF3\x0F\x58\xD0"sv, func_offset, func_offset + 0x80, "activate_crush_elevator_hack");
        if (offsets[0] == 0)
            return;

        offsets[0] += 4; // pattern size
        offsets[1] = find_inst(memory.exe(), "\xEB*\x0F\x57\xD2"sv, offsets[0], offsets[0] + 0xF0, "activate_crush_elevator_hack");
        if (offsets[1] == 0)
            return;

        offsets[1] += 5; // pattern size
        offsets[2] = find_inst(memory.exe(), "\xF3\x0F\x58\xC1"sv, offsets[1], offsets[1] + 0x40, "activate_crush_elevator_hack");
        if (offsets[2] == 0)
            return;

        offsets[2] += 4; // pattern size
    }

    if (activate)
    {
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[0]), "\x0f\x2e\x90\x30\x01\x00\x00"sv, true); // ucomiss xmm2,DWORD PTR [rax+0x130] // limit
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[1]), "\xf3\x0f\x10\x9b\x30\x01\x00"sv, true); // movss  xmm3,DWORD PTR [rbx+0x130]  // limit
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[2]), "\xf3\x0f\x58\x83\x34\x01\x00"sv, true); // addss  xmm0,DWORD PTR [rbx+0x134]  // speed
    }
    else
        recover_mem("activate_crush_elevator_hack");
}

void activate_hundun_hack(bool activate)
{
    /*
     * Pointer to Hundun entity is stored in r13 register. which means we need 8 bytes for ucomiss instruction
     * but we have 7 available, that's why we jump out to new code with the instruction and back
     */
    static size_t offsets[6]; // y_limit, y_limit, bird_head, sneak_head, speed, speed
    static char new_code[3][8];

    if (offsets[0] == 0)
    {
        auto& memory = Memory::get();
        auto func_offset = get_virtual_function_address(VTABLE_OFFSET::MONS_HUNDUN, 78);
        offsets[0] = find_inst(memory.exe(), "\x41\xF6\x85\x61\x01\x00\x00\x08"sv, func_offset, func_offset + 0x1420, "activate_hundun_hack");
        if (offsets[0] == 0)
            return;

        offsets[0] -= 13; // offset, no good pattern above
        offsets[1] = find_inst(memory.exe(), "\x41\x80\x8D\x61\x01\x00\x00\x04"sv, offsets[0], offsets[0] + 0xF40, "activate_hundun_hack");
        if (offsets[1] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[1] += 8; // pattern size

        offsets[2] = find_inst(memory.exe(), "\xF3\x41\x0F\x58\x45\x7C"sv, offsets[0], offsets[1], "activate_hundun_hack");
        if (offsets[2] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[2] += 6; // pattern size

        offsets[3] = find_inst(memory.exe(), "\xF3\x41\x0F\x58\x45\x7C"sv, offsets[2], offsets[1], "activate_hundun_hack");
        if (offsets[3] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[3] += 6; // pattern size

        offsets[4] = find_inst(memory.exe(), "\x83\x7A\x0C\x0E"sv, offsets[1], offsets[1] + 0xC0, "activate_hundun_hack");
        if (offsets[4] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[4] += 6; // pattern size plus jump

        offsets[5] = find_inst(memory.exe(), "\xF3\x41\x0F"sv, offsets[4], offsets[4] + 0x58, "activate_hundun_hack");
        if (offsets[5] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[5] += 9; // instruction size (didn't include the whole thing in pattern, very short distance from previous pattern)

        offsets[0] = memory.at_exe(offsets[0]);
        offsets[1] = memory.at_exe(offsets[1]);
        offsets[2] = memory.at_exe(offsets[2]);
        offsets[3] = memory.at_exe(offsets[3]);
        offsets[4] = memory.at_exe(offsets[4]);
        offsets[5] = memory.at_exe(offsets[5]);

        char old_code[3][8];

        std::memcpy(old_code[0], (void*)offsets[0], 7);
        std::memcpy(old_code[1], (void*)offsets[1], 7);
        std::memcpy(old_code[2], (void*)offsets[5], 8);

        const std::string_view patch_code{"\x41\x0F\x2E\xBD\x64\x01\x00\x00"sv};      // ucomiss xmm7,DWORD PTR [r13+0x164]
        const std::string_view speed_patch{"\xF3\x41\x0F\x58\x85\x6C\x01\x00\x00"sv}; // addss  xmm0,DWORD PTR [r13+0x16C]

        patch_and_redirect(offsets[0], 7, patch_code, true);
        patch_and_redirect(offsets[1], 7, patch_code, true);
        patch_and_redirect(offsets[5], 8, speed_patch, true);

        std::memcpy(new_code[0], (void*)offsets[0], 7);
        std::memcpy(new_code[1], (void*)offsets[1], 7);
        std::memcpy(new_code[2], (void*)offsets[5], 8);

        // writing back the old code so we can just use write_mem_recoverable for going from vanilla to the patch
        write_mem_prot(offsets[0], std::string_view{&old_code[0][0], &old_code[0][7]}, true);
        write_mem_prot(offsets[1], std::string_view{&old_code[1][0], &old_code[1][7]}, true);
        write_mem_prot(offsets[5], std::string_view{&old_code[2][0], &old_code[2][8]}, true);
    }

    if (activate)
    {
        static const std::string_view speed_code{"\x49\x8D\x95\x68\x01\x00\x00"sv                           // lea    rdx,[r13+0x168]
                                                 "\x66\x2E\x0F\x1F\x84\x00\x00\x00\x00\x00\x90\x90\x90"sv}; //  spoiled with space, all nop

        write_mem_recoverable("activate_hundun_hack", offsets[0], std::string_view{&new_code[0][0], &new_code[0][7]}, true); // limit
        write_mem_recoverable("activate_hundun_hack", offsets[1], std::string_view{&new_code[1][0], &new_code[1][7]}, true); // limit
        write_mem_recoverable("activate_hundun_hack", offsets[5], std::string_view{&new_code[2][0], &new_code[2][8]}, true); // speed for adding to the y_limit

        write_mem_recoverable("activate_hundun_hack", offsets[4], speed_code, true); // speed (for adding to the x position)

        write_mem_recoverable("activate_hundun_hack", offsets[2], "\x0F\x2E\xB8\x70\x01\x00\x00"sv, true); // ucomiss xmm7,DWORD PTR [rax+0x170] // bird_head
        write_mem_recoverable("activate_hundun_hack", offsets[3], "\x0F\x2E\xB8\x74\x01\x00\x00"sv, true); // ucomiss xmm7,DWORD PTR [rax+0x174] // snake head
    }
    else
        recover_mem("activate_hundun_hack");
}

void set_boss_door_control_enabled(bool enable)
{
    static size_t offsets[2];
    if (offsets[0] == 0)
    {
        auto& memory = Memory::get();
        offsets[0] = get_address("hundun_door_control");
        if (offsets[0] == 0)
            return;
        // find tiamat door control (the same pattern)
        offsets[1] = find_inst(memory.exe(), "\x4A\x8B\xB4\xC8\x80\xF4\x00\x00"sv, offsets[0] - memory.exe_address() + 0x777, std::nullopt, "set_boss_door_control_enabled");
        if (offsets[1] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[1] = function_start(memory.at_exe(offsets[1]));
    }
    if (!enable)
    {
        write_mem_recoverable("set_boss_door_control_enabled", offsets[0], "\xC3\x90"sv, true);
        write_mem_recoverable("set_boss_door_control_enabled", offsets[1], "\xC3\x90"sv, true);
    }
    else
        recover_mem("set_boss_door_control_enabled");
}

void update_state()
{
    static const size_t offset = get_address("state_refresh");
    auto state = State::get().ptr();
    typedef void refresh_func(StateMemory*);
    static refresh_func* rf = (refresh_func*)(offset);
    rf(state);
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
    auto state = State::get().ptr();
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
    auto state = State::get().ptr();
    auto hud = get_hud();
    state->money_shop_total += amount;
    hud->money.counter += amount;
    hud->money.timer = display_time.value_or(0x3C);
    return get_current_money();
}

int32_t add_money_slot(int32_t amount, uint8_t player_slot, std::optional<uint8_t> display_time)
{
    auto state = State::get().ptr();
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
    auto state = State::get().ptr();
    for (auto i = 0; i < MAX_PLAYERS; ++i)
    {
        if (state->items->players[i] && state->items->players[i]->layer == layer)
            state->items->players[i] = nullptr;
    }
    auto* layer_ptr = State::get().layer(layer);
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
    auto* layer_ptr = State::get().layer(layer);
    typedef void init_func(Layer*);
    static init_func* ilf = (init_func*)(offset);
    ilf(layer_ptr);
}

void create_level()
{
    create_layer(0);
    create_layer(1);
}

void set_level_logic_enabled(bool enable)
{
    auto state = State::get().ptr();
    static const size_t offset = get_virtual_function_address(state->screen_level, 1);

    if (!enable)
        write_mem_recoverable("set_level_logic_enabled", offset, "\xC3\x90"sv, true);
    else
        recover_mem("set_level_logic_enabled");
}

void set_camera_layer_control_enabled(bool enable)
{
    static const size_t offset = get_address("camera_layer_control");
    static const size_t offset2 = get_address("player_behavior_layer_switch");

    if (enable)
    {
        recover_mem("set_camera_layer_control");
    }
    else
    {
        write_mem_recoverable("set_camera_layer_control", offset, get_nop(7), true);
        write_mem_recoverable("set_camera_layer_control", offset2, get_nop(18), true);
    }
}

void set_start_level_paused(bool enable)
{
    static const size_t offset = get_address("unpause_level");
    if (enable)
        write_mem_recoverable("start_level_paused", offset, get_nop(3), true);
    else
        recover_mem("start_level_paused");
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
    auto* state = State::get().ptr();
    isf(state, seed.value_or(state->seed));
}

void set_liquid_layer(LAYER l)
{
    static std::array<uintptr_t, 7> jumps;          // jne (0F85) -> je (0F84)
    static std::array<uintptr_t, 20> layer_offsets; // 0x1300 -> 0x1308
    static std::array<uintptr_t, 6> layer_byte;
    static uintptr_t jump2;
    static uintptr_t jump3;
    if (jumps[0] == 0)
    {
        layer_byte[0] = get_address("check_if_collides_with_liquid_layer");
        layer_byte[1] = get_address("check_if_collides_with_liquid_layer2");
        layer_byte[2] = get_address("lavamander_spewing_lava");
        layer_byte[3] = get_address("movement_calculations_layer_check");
        layer_byte[4] = get_address("jump_calculations_layer_check");
        // i don't actually know what this bit does, probably bool param, it's not just liquid relates as it's for all the entities with collision mask
        // and it's not layer as well since other collision occur in back layer even with this set to 0 and vice versa
        layer_byte[5] = get_address("collision_mask_check_param");

        for (auto addr : layer_byte)
            if (addr == 0)
                return;

        auto& mem = Memory::get();
        layer_offsets[0] = get_address("spawn_liquid_layer");

        {
            auto sound_stuff = get_address("liquid_stream_spawner");
            if (sound_stuff == 0)
                return;

            auto last_offset = sound_stuff - mem.exe_address();
            bool skip = true;
            for (uint8_t idx = 0; idx < 6; ++idx)
            {
                last_offset = find_inst(mem.exe(), "\x48\x8B\x8A"sv, last_offset, last_offset + 0x170, "set_liquid_layer-sound stuff");
                if (idx == 5 && skip) // skip one, same instruction but not layer related
                {
                    idx = 4;
                    skip = false;
                    last_offset += 7;
                    continue;
                }
                layer_offsets[idx + 1] = mem.at_exe(last_offset);
                last_offset += 7;
            }
        }
        layer_offsets[7] = get_address("tidepool_impostor_spawn");
        layer_offsets[8] = get_address("tiamat_impostor_spawn");
        layer_offsets[9] = get_address("olmec_impostor_spawn");
        layer_offsets[10] = get_address("abzu_impostor_spawn");

        {
            auto logic_magman_spawn = get_virtual_function_address(VTABLE_OFFSET::LOGIC_VOLCANA_RELATED, 1);
            if (logic_magman_spawn == 0)
                return;

            auto lookup_patterns = {
                // in order
                "\x48\x8B\x8D*\x13\x00\x00"sv,
                "\x48\x03\xB7*\x13\x00\x00"sv,
                "\x48\x8B\x89*\x13\x00\x00"sv,
                "\x48\x03\x95*\x13\x00\x00"sv,
                "\x48\x03\x95*\x13\x00\x00"sv,
                "\x48\x03\x8D*\x13\x00\x00"sv,
                "\x48\x8B\x8A*\x13\x00\x00"sv,
            };
            auto current_offset = logic_magman_spawn;
            uint8_t idx = 11; // next free index
            for (auto& pattern : lookup_patterns)
            {
                current_offset = find_inst(mem.exe(), pattern, current_offset + 7, logic_magman_spawn + 0x764, "set_liquid_layer-volcana");
                if (current_offset == 0)
                    return;

                layer_offsets[idx++] = mem.at_exe(current_offset);
            }
        }
        layer_offsets[18] = get_address("logic_volcana_gather_magman_spawn_locations");
        layer_offsets[19] = get_address("logic_volcana_gather_magman_spawn_locations2");

        for (auto addr : layer_offsets)
            if (addr == 0)
                return;

        jump2 = get_address("robot_layer_check");
        jump3 = get_address("logic_underwater_bubbles_loop_check");
        if (jump2 == 0 || jump3 == 0)
            return;

        jumps[0] = get_address("layer_check_in_add_liquid_collision");
        jumps[1] = get_address("layer_check_in_remove_liquid_collision");
        jumps[2] = get_address("is_entity_in_liquid_check"); // TODO there is also layer offset nearby, test if it's related
        jumps[3] = get_address("liquid_render_layer");
        jumps[4] = get_address("entity_in_liquid_detection1");
        jumps[5] = get_address("entity_in_liquid_detection2");
        jumps[6] = get_address("layer_check_in_add_movable_liquid_collision");

        for (auto addr : jumps)
            if (addr == 0)
            {
                jumps[0] = 0;
                return;
            }
    }
    auto actual_layer = enum_to_layer(l);
    uint8_t offset_ending = actual_layer == 0 ? 0 : 8;
    uint8_t jump_oppcode = actual_layer == 0 ? 0x85 : 0x84;
    uint8_t jump_oppcode2 = actual_layer == 0 ? 0x75 : 0x74;
    uint8_t jump_oppcode2_inverse = actual_layer == 0 ? 0x74 : 0x75;

    for (auto addr : jumps)
        write_mem_prot(addr + 1, jump_oppcode, true);

    for (auto addr : layer_offsets)
        write_mem_prot(addr + 3, offset_ending, true);

    for (auto addr : layer_byte)
        write_mem_prot(addr, actual_layer, true);

    write_mem_prot(jump2, jump_oppcode2, true);
    write_mem_prot(jump3, jump_oppcode2_inverse, true);
}

uint8_t get_liquid_layer()
{
    static auto addr = get_address("check_if_collides_with_liquid_layer");
    return memory_read<uint8_t>(addr);
}
