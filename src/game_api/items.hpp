#pragma once

#include <array>   // for array
#include <cstdint> // for uint8_t, int16_t, uint32_t, int8_t

#include "aliases.hpp" // for ENT_TYPE, MAX_PLAYERS

class Player;

struct Inventory
{
    /// Sum of the money collected in current level
    int32_t money;
    uint8_t bombs;
    uint8_t ropes;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    int16_t poison_tick_timer;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    bool cursed;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    bool elixir_buff;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    uint8_t health;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    uint8_t kapala_blood_amount;
    /// Is set to state.time_total when player dies in coop (to determinate who should be first to re-spawn from coffin)
    uint32_t time_of_death;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ENT_TYPE held_item;
    /// Metadata of the held item (health, is cursed etc.)
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    int16_t held_item_metadata;
    uint8_t saved_pets_count;

    int8_t player_slot;
    /// Used to transfer information to transition/next level (player riding a mount). Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ENT_TYPE mount_type;
    /// Metadata of the mount (health, is cursed etc.)
    /// Used to transfer information to transition/next level (player riding a mount). Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    int16_t mount_metadata;
    int16_t unknown_mount_related; // unsure, can be padding as well

    /// Types of gold/gems collected during this level, used later to display during the transition
    std::array<ENT_TYPE, 512> collected_money;
    /// Values of gold/gems collected during this level, used later to display during the transition
    std::array<uint32_t, 512> collected_money_values;
    /// Count/size for the `collected_money` arrays
    uint32_t collected_money_count;
    /// Types of enemies killed during this level, used later to display during the transition
    std::array<ENT_TYPE, 256> killed_enemies;
    uint32_t kills_level;
    uint32_t kills_total;

    /// Metadata of items held by companions (health, is cursed etc.)
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<int16_t, 8> companion_held_item_metadatas;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<int16_t, 8> companion_poison_tick_timers;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<ENT_TYPE, 8> companions;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<ENT_TYPE, 8> companion_held_items;
    /// (0..3) Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<uint8_t, 8> companion_trust;
    /// Number of companions, it will determinate how many companions will be transferred to next level
    /// Increments when player acquires new companion, decrements when one of them dies
    uint8_t companion_count;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<uint8_t, 8> companion_health;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<bool, 8> is_companion_cursed;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    /// Used to transfer information to transition/next level. Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    std::array<ENT_TYPE, 30> acquired_powerups;
    /// Total money collected during previous levels (so excluding the current one)
    int32_t collected_money_total;
};

struct SelectPlayerSlot
{
    bool activated;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    ENT_TYPE character; // Entity DB ID
    TEXTURE texture_id; // Texture DB ID
};

struct Items
{
    /// Index of leader player in coop
    uint8_t leader;
    uint8_t unknown1;
    uint8_t unknown2;
    uint8_t unknown3;
    uint32_t unknown; // padding probably
    /// Table of players, also keeps the dead body until they are destroyed (necromancer revive also destroys the old body)
    std::array<Player*, MAX_PLAYERS> players;
    std::array<Inventory, MAX_PLAYERS> player_inventories;
    std::array<SelectPlayerSlot, MAX_PLAYERS> player_select_slots;
    /// Pet information for level transition
    std::array<ENT_TYPE, 4> saved_pets;
    std::array<bool, 4> is_pet_cursed;
    std::array<bool, 4> is_pet_poisoned;
    /// Only for the level transition, the actual number is held in player inventory
    uint8_t saved_pets_count;

    uint8_t player_count;

    Player* player(uint8_t index) const
    {
        return index < players.size() ? players[index] : nullptr;
    }
};
