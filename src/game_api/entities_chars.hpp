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
    uint32_t jump_flags;
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
};

std::u16string get_character_name(ENT_TYPE type_id);
std::u16string get_character_short_name(ENT_TYPE type_id);
Color get_character_heart_color(ENT_TYPE type_id);
bool is_character_female(ENT_TYPE type_id);

void set_character_heart_color(ENT_TYPE type_id, Color color);
