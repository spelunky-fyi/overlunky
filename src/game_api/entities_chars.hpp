#pragma once

#include "movable.hpp"
#include <cstdint>

struct Illumination;
struct PlayerInputs;

class Ai
{
  public:
    uint32_t unknown1a; // flags?
    uint32_t unknown1b; // flags?
    int32_t unknown2;
    int32_t unknown3; // garbage?
    size_t unknown4;
    size_t button_sequence; /*unsure*/
    Entity* self_pointer;
    size_t unknown7;
    Entity* target;
    size_t unknown9;
    size_t unknown10;
    int16_t unknown11;
    int16_t unknown11b;
    int32_t timer;
    /// AI state (patrol, sleep, attack, aggro...)
    int8_t state; // 8 = HH agro
    int8_t unknown12;
    /// Levels completed with, 0..3
    uint8_t trust;
    /// How many times master has violated us
    uint8_t whipped;
    int8_t unknown15;
    int8_t unknown16;
    int16_t unknown17;
    int16_t unknown19;
    int16_t unknown20; // distance to target?
    int32_t target_uid;
    int8_t unknown22;
    int8_t unknown23;
    int8_t unknown24;
    int8_t unknown25;
    int32_t unknown26;
    // Map unknown27;
};

class PowerupCapable : public Movable
{
  public:
    std::map<ENT_TYPE, Entity*> powerups; // type id -> entity

    /// Removes a currently applied powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Removing the Eggplant crown does not seem to undo the throwing of eggplants, the other powerups seem to work.
    void remove_powerup(ENT_TYPE powerup_type);

    /// Gives the player/monster the specified powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Giving true crown to a monster crashes the game.
    void give_powerup(ENT_TYPE powerup_type);

    /// Checks whether the player/monster has a certain powerup
    bool has_powerup(ENT_TYPE powerup_type);

    /// Return all powerups that the entity has
    std::vector<ENT_TYPE> get_powerups();

    /// Unequips the currently worn backitem
    void unequip_backitem();

    /// Returns the uid of the currently worn backitem, or -1 if wearing nothing
    int32_t worn_backitem();

    // thoes could be wrong becous of the update
    virtual void on_blood_collision() = 0; // only triggers when player has kapala

    // called for stunned entities, check bounch of stuff like state, hold entity, standing on entity etc. runs until returned 1
    // this is used to clear the last_owner of stunned entity when it is no longed stunned
    virtual bool can_clear_last_owner() = 0;
};

struct Inventory
{
    /// Sum of the money collected in current level
    uint32_t money;
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
    uint8_t unknown5c; // padding?

    int8_t player_slot;
    /// Used to transfer information to transition/next level (player rading a mout). Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    ENT_TYPE mount_type;
    /// Metadata of the mount (health, is cursed etc.)
    /// Used to transfer information to transition/next level (player rading a mout). Is not updated during a level
    /// You can use `ON.PRE_LEVEL_GENERATION` to access/edit this
    int16_t mount_metadata;
    int16_t unknown_mount_ralated; // unsure, can be padding as well

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
    /// Number of companions, it will determinate how many companions will be transfered to next level
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
    uint32_t collected_money_total;
};

class Player : public PowerupCapable
{
  public:
    Inventory* inventory_ptr;
    Illumination* emitted_light;
    int32_t linked_companion_child;  // entity uid
    int32_t linked_companion_parent; // entity uid
    Ai* ai;
    PlayerInputs* input_ptr;
    Entity* basecamp_button_entity_pointer;
    int32_t i168;
    int32_t i16c;
    float y_pos; // not sure why, seams to be the same as abs_y
    uint8_t jump_flags_a;
    uint8_t jump_flags_b;
    uint8_t jump_flags_c;
    uint8_t jump_flags_d;
    uint8_t timer_since_last_jump; // something different happens before or after 6
    uint8_t can_use;               // is compared with bit numbers, so probably flags

    void set_jetpack_fuel(uint8_t fuel);
    uint8_t kapala_blood_amount();

    void set_name(std::u16string name);
    void set_short_name(std::u16string name);

    /// Get the full name of the character, this will be the modded name not only the vanilla name.
    std::u16string get_name();
    /// Get the short name of the character, this will be the modded name not only the vanilla name.
    std::u16string get_short_name();
    /// Get the heart color of the character, this will be the modded heart color not only the vanilla heart color.
    Color get_heart_color();
    /// Check whether the character is female, will be `true` if the character was modded to be female as well.
    bool is_female();

    /// Set the heart color the character.
    void set_heart_color(Color hcolor);
    /// Drops from ladders, ropes and ledge grabs
    void let_go();
};

std::u16string get_character_name(ENT_TYPE type_id);
std::u16string get_character_short_name(ENT_TYPE type_id);
Color get_character_heart_color(ENT_TYPE type_id);
bool is_character_female(ENT_TYPE type_id);

void set_character_heart_color(ENT_TYPE type_id, Color color);
