#pragma once

#include "movable.hpp"
#include <cstdint>

class PowerupCapable : public Movable
{
  public:
    std::map<uint32_t, Entity*> powerups; // type id -> entity

    /// Removes a currently applied powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Removing the Eggplant crown does not seem to undo the throwing of eggplants, the other powerups seem to work.
    void remove_powerup(ENT_TYPE powerup_type);

    /// Gives the player/monster the specified powerup. Specify `ENT_TYPE.ITEM_POWERUP_xxx`, not `ENT_TYPE.ITEM_PICKUP_xxx`! Giving true crown to a monster crashes the game.
    void give_powerup(ENT_TYPE powerup_type);

    /// Checks whether the player/monster has a certain powerup
    bool has_powerup(ENT_TYPE powerup_type);

    virtual void on_instakill_death() = 0; // seems to only trigger for enemies that die in one hit

    virtual void on_blood_collision() = 0; // only triggers when player has kapala

    virtual void unknown_v93() = 0;
};

class Player : public PowerupCapable
{
  public:
    Inventory* inventory_ptr;
    Illumination* emitted_light;
    int32_t linked_companion_child;  // entity uid
    int32_t linked_companion_parent; // entity uid
    size_t ai_func;
    size_t input_ptr;
    size_t p160;
    int32_t i168;
    int32_t i16c;
    uint32_t jump_flags;
    uint8_t some_timer;
    uint8_t can_use;
    uint8_t b176;
    uint8_t b177;

    void set_jetpack_fuel(uint8_t fuel);
    uint8_t kapala_blood_amount();

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
