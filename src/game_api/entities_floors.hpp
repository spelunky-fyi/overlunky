#pragma once

#include "entities_items.hpp" // for UnknownPointerGroup
#include "entity.hpp"
#include <cstdint>

class Floor : public Entity
{
  public:
    int32_t deco_top;
    int32_t deco_bottom;
    int32_t deco_left;
    int32_t deco_right;
};

class Door : public Entity
{
  public:
    uint8_t counter; // counts down as you go thru it
    int8_t unused1;
    int16_t unused2;
    int32_t unused3;
    Entity* fx_button;

    void set_target(uint8_t w, uint8_t l, uint8_t t);
    std::tuple<uint8_t, uint8_t, uint8_t> get_target();
};

class ExitDoor : public Door
{
  public:
    bool entered;      // if true entering it does not load the transition, probably for use in multiplayer
    bool special_door; // use provided world/level/theme
    uint8_t level;
    uint8_t timer; // counts from 30 to 0, dunno why
    uint8_t world;
    uint8_t theme;
    uint16_t padding;
};

class DecoratedDoor : public ExitDoor
{
  public:
    Entity* special_bg;
};

class LockedDoor : public Door
{
  public:
    bool unlocked;
};

class CityOfGoldDoor : public DecoratedDoor
{
  public:
    bool unlocked;
    uint8_t unknown1;
};

class MainExit : public ExitDoor
{
  public:
    SoundPosition* sound_pos;
    float top_value; // unsure
    float value;     // unsure
};

class EggShipDoor : public Door
{
  public:
    uint16_t unused1;
    uint8_t unused2;
    uint8_t timer; // counts from 30 to 0, dunno why
    uint16_t unknown3;
    uint16_t unknown4;
    bool entered; // works only for DOOR_EGGSHIP
};

class Arrowtrap : public Floor
{
  public:
    bool arrow_shot;

    void rearm();
};

class TotemTrap : public Floor
{
  public:
    uint32_t unknown1;
    float unknown2; // might be sound delay related, cannot reproduce anymore
    float unknown3;
    uint32_t spawn_entity_type;
    uint32_t first_sound_id;
    int32_t counter;  // adds some value depending on trap and with side is triggered, setting to low value pauses it?
    uint8_t unknown4; // forced to 1
    uint8_t unused1;
    uint16_t unused2;
    uint32_t unused3;
    uint8_t timer; // unsure
};

class LaserTrap : public Floor
{
  public:
    Illumination* emitted_light;
    uint8_t reset_timer; // after triggering counts from 0 to 255, changes the 'phase_2' then counts from 0 to 104
    bool phase_2;
};

class SparkTrap : public Floor
{
  public:
    Illumination* emitted_light;
    int32_t spark_uid;
};

class Altar : public Floor
{
  public:
    uint8_t timer; // counts from 0 to 20 then 0, then 1 then 0 and sacrifice happens
    bool unknown;  // goes true/false when body is landing on it, when it stops moving it increments timer on true
};

class SpikeballTrap : public Floor
{
  public:
    SoundPosition* sound_pos;
    Entity* chain;
    Entity* end_piece;
    int8_t state;            // going_up is only right when timer is 0, otherwise it just sits at the bottom
    uint8_t timer;           // for the start and retract
    uint8_t speed_direction; // i have no clue what this is, goes 255 when detecting something, but when it hits the unbreakable tile it 255 for ever, changing it can teleport it up/down/change speed etc.
};

class TransferFloor : public Floor
{
  public:
    std::unordered_map<size_t, size_t> unknown_map; // types are wrong, no idea what's inside
};

class ConveyorBelt : public TransferFloor
{
  public:
    UnknownPointerGroup unknown1;
    uint8_t timer; // 0 to 15
};

class Pipe : public TransferFloor
{
  public:
    int8_t type;
    bool end_pipe;
};

class Generator : public Floor
{
  public:
    int32_t spawned_uid;
    uint16_t set_timer;
    uint16_t timer;
    uint8_t start_counter; // works only for star challenge
    bool on_off;           // works only for star challenge
};

class SlidingWallCeiling : public Floor
{
  public:
    Entity* attached_piece;
    int32_t active_floor_part_uid;
    uint8_t state; // i labeled some as wrong_pause cause they are probably not used, they pause the movement but not the sound
    uint8_t unused1;
    uint16_t unused2;
    size_t unknown1;
    size_t unknown2;
};

class QuickSand : public Floor
{
  public:
    uint8_t unknown_timer;
};

class BigSpearTrap : public Floor
{
  public:
    int32_t spear_uid;
    uint8_t active; /*unsure*/ // forced to 1
    bool left_part;            // setting the left part to 0 or right part to 1 destroys the trap
};

class StickyTrap : public Floor
{
  public:
    SoundPosition* sound_pos;
    UnknownPointerGroup unknown1;
    int32_t attached_piece_uid;
    int32_t ball_uid;
    int8_t state;
    uint8_t timer; // for the start and for the retract
};

class MotherStatue : public Floor
{
  public:
    bool player1_standing;
    bool player2_standing;
    bool player3_standing;
    bool player4_standing;
    bool player1_health_received;
    bool player2_health_received;
    bool player3_health_received;
    bool player4_health_received;
    uint16_t player1_health_timer;
    uint16_t player2_health_timer;
    uint16_t player3_health_timer;
    uint16_t player4_health_timer;
    uint8_t eggplantchild_timer;
    uint8_t unknown;
    bool eggplantchild_detected;
};

class TeleportingBorder : public Floor
{
  public:
    int8_t direction;
};

class ForceField : public Floor
{
  public:
    Entity* first_item_beam; // if killed, the whole beam goes out
    Entity* fx;
    SoundPosition* sound_pos; // starts at timer 140
    Illumination* emitted_light;
    float bounce_force; /* unsure */ // max 1.0 ?
    bool is_on;                      // starts the sound + first_item_beam but not fx for some reason

    void activate_laserbeam(bool turn_on);
};

class TimedForceField : public ForceField
{
  public:
    uint8_t unused1;
    uint16_t unused2;
    uint32_t timer; // max 300? at 100 it's start the proces, changing the animation_frame
    bool pause;
};

class HorizontalForceField : public Floor
{
  public:
    Entity* first_item_beam; // if killed, the whole beam goes out
    Entity* fx;
    SoundPosition* sound_pos;
    float laser_bounce; /* unsure */
    uint16_t timer;     // As opposite to the vertical one, this one counts for the on and the for the off state
    bool is_on;
};

class TentacleBottom : public Floor
{
  public:
    UnknownPointerGroup unknown1;
    int32_t attached_piece_uid;
    int32_t tentacle_uid;
};

class PoleDeco : public Floor
{
  public:
    int32_t deco_up;
    int32_t deco_down;
};
