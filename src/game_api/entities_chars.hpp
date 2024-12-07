#pragma once

#include <cstddef>    // for size_t
#include <cstdint>    // for int32_t, uint8_t, int8_t, int16_t, uint32_t
#include <functional> // for less
#include <map>        // for map
#include <string>     // for u16string, allocator
#include <vector>     // for vector

#include "aliases.hpp"               // for ENT_TYPE
#include "containers/custom_map.hpp" //
#include "movable.hpp"               // for Movable

struct Illumination;
struct PlayerInputs;
struct Inventory;
struct PlayerSlot;
struct Color;
class Entity;

class Ai
{
  public:
    uint32_t unknown1a;     // flags?
    uint32_t unknown1b;     // flags?
    int16_t unknown2a;      // unknown
    int16_t unknown2b;      // garbage/padding?
    int32_t unknown3;       // garbage/padding?
    size_t unknown4;        // array of targets? (uids)
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
    int8_t state; // 8 = HH aggro
    int8_t last_state;
    /// Levels completed with, 0..3
    uint8_t trust;
    /// Number of times whipped by player
    uint8_t whipped;
    int8_t unknown15;
    int8_t unknown16;
    /// positive: walking, negative: waiting/idle
    int16_t walk_pause_timer;
    int16_t unknown19;
    int16_t unknown20; // distance to target?
    int32_t target_uid;
    int8_t unknown22;
    int8_t unknown23;
    int8_t unknown24;
    int8_t unknown25;
    int32_t unknown26;
    std::map<size_t, size_t> unknown27;

    size_t unknown28; // probably vector?
    size_t unknown29;
    size_t unknown30;

    std::vector<uintptr_t> unknown31;      // pointers to the Ai targets in state
    std::map<uint32_t, uint8_t> unknown32; // dunno what any of this is, keeps changing rapidly

    size_t unknown33;
    uintptr_t unknown34; // pointer to map/set ?
    std::map<size_t, size_t> unknown35;
};

class PowerupCapable : public Movable
{
  public:
    custom_map<ENT_TYPE, Entity*> powerups; // type id -> entity

    /// Removes a currently applied powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Removing the Eggplant crown does not seem to undo the throwing of eggplants, the other powerups seem to work.
    void remove_powerup(ENT_TYPE powerup_type);

    /// Gives the player/monster the specified powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Giving true crown to a monster crashes the game.
    void give_powerup(ENT_TYPE powerup_type);

    /// Checks whether the player/monster has a certain powerup
    bool has_powerup(ENT_TYPE powerup_type)
    {
        return powerups.find(powerup_type) != powerups.end();
    }

    /// Return all powerups that the entity has
    std::vector<ENT_TYPE> get_powerups();
    /// Unequips the currently worn backitem
    void unequip_backitem();
    /// Returns the uid of the currently worn backitem, or -1 if wearing nothing
    int32_t worn_backitem();

    /// only triggers when it has kapala
    virtual bool on_blood_collision() = 0;
    // called for stunned entities, check bunch of stuff like state, hold entity, standing on entity etc. runs until returned 1
    // this is used to clear the last_owner of stunned entity when it is no longed stunned
    virtual bool can_clear_last_owner() = 0;
};

class Player : public PowerupCapable
{
  public:
    Inventory* inventory_ptr;
    Illumination* emitted_light;
    /// entity uid
    int32_t linked_companion_child;
    /// entity uid
    int32_t linked_companion_parent;
    Ai* ai;
    PlayerSlot* input_ptr;
    /// Used in base camp to talk with the NPC's
    Entity* basecamp_button_entity;
    /// For Lise System walking and looking up sounds
    SoundMeta* special_sound;
    float y_pos; // not sure why, seams to be the same as abs_y
    /// Increases when holding jump button in the air, set to max while jumping. If this isn't 0, a jump will only be
    /// registered if the jump button was not held on the previous frame.
    uint8_t jump_lock_timer;
    /// can jump while airborne if greater than 0
    int8_t coyote_timer;
    /// 0-25 alphabetical index of hired hand names.
    uint8_t hired_hand_name;
    /// Timer between strokes when holding jump button in water.
    uint8_t swim_timer;
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

    /// Set the heart color for the character.
    void set_heart_color(Color hcolor);
    /// Drops from ladders, ropes and ledge grabs
    void let_go();
};

std::u16string get_character_name(ENT_TYPE type_id);
std::u16string get_character_short_name(ENT_TYPE type_id);
Color get_character_heart_color(ENT_TYPE type_id);
bool is_character_female(ENT_TYPE type_id);

void set_character_heart_color(ENT_TYPE type_id, Color color);
