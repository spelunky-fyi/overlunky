#pragma once

#include <array>         // for array
#include <cstdint>       // for int32_t, uint8_t, uint32_t, uint16_t
#include <functional>    // for equal_to
#include <memory>        // for allocator
#include <new>           // for operator new
#include <type_traits>   // for hash
#include <unordered_map> // for _Umap_traits<>::allocator_type, unorde...

#include "aliases.hpp"        // for ENT_TYPE
#include "entities_items.hpp" // for UnknownPointerGroup
#include "entity.hpp"         // Entity

struct Illumination;
struct SoundMeta;

enum FLOOR_SIDE
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
};

void trigger_trap(Entity* trap, int32_t who_uid, uint8_t direction = 0);

class Floor : public Entity
{
  public:
    union
    {
        std::array<int32_t, 4> decos;
        struct
        {
            int32_t deco_top;
            int32_t deco_bottom;
            int32_t deco_left;
            int32_t deco_right;
        };
    };

    /// Sets `animation_frame` of the floor for types `FLOOR_BORDERTILE`, `FLOOR_BORDERTILE_METAL` and `FLOOR_BORDERTILE_OCTOPUS`.
    void fix_border_tile_animation();

    /// Used to add decoration to a floor entity after it was spawned outside of level gen, is not necessary when spawning during level gen.
    /// Set `fix_also_neighbors` to `true` to fix the neighboring floor tile decorations on the border of the two tiles.
    /// Set `fix_styled_floor` to `true` to fix decorations on `FLOORSTYLED_*` entities, those usually only have decorations when broken.
    void fix_decorations(bool fix_also_neighbors, bool fix_styled_floor);
    /// Explicitly add a decoration on the given side. Corner decorations only exist for `FLOOR_BORDERTILE` and `FLOOR_BORDERTILE_OCTOPUS`.
    void add_decoration(FLOOR_SIDE side);
    /// Explicitly remove a decoration on the given side. Corner decorations only exist for `FLOOR_BORDERTILE` and `FLOOR_BORDERTILE_OCTOPUS`.
    void remove_decoration(FLOOR_SIDE side);

    Entity* find_corner_decoration(FLOOR_SIDE side);
    void add_decoration_opt(FLOOR_SIDE side, ENT_TYPE decoration_entity_type, struct Layer* layer_ptr);

    bool has_corners() const;
    bool is_styled() const;
    int32_t get_decoration_entity_type() const;
    uint16_t get_decoration_animation_frame(FLOOR_SIDE side) const;

    static FLOOR_SIDE get_opposite_side(FLOOR_SIDE side);
    static bool get_perpendicular_sides(FLOOR_SIDE side, FLOOR_SIDE (&perp_sides)[2]);
    static bool get_corner_sides(FLOOR_SIDE side, FLOOR_SIDE (&corner_sides)[2]);

    virtual void decorate_internal() = 0;     // decorates undecorated floor and floorstyled, doesn't remove old decorations, runs only on level gen
    virtual void on_neighbor_destroyed() = 0; // called on every neighbor of destroyed floor to decorate it
    /// Returns it's ENT_TYPE except for FLOOR_PEN (returns FLOORSTYLED_MINEWOOD) and FLOOR_QUICKSAND, FLOOR_TOMB, FLOOR_EMPRESS_GRAVE which return FLOOR_GENERIC
    virtual ENT_TYPE get_floor_type() = 0; // Used for spawning decorations
};

class Door : public Floor
{
  public:
    uint8_t counter; // counts down as you go thru it
    int8_t unused1[7];
    Entity* fx_button;

    /// Lock/Unlock doors
    void unlock(bool unlock);

    // this function doesn't do much, checks if it's CHAR_*, checks if hes holding anything (if yes calls some function), then checks if Player.can_use is equal to 4 calls some other function
    // can't be bother to look into the functions
    virtual void on_enter_attempt(Entity* who) = 0;

    // check if it's CHAR_*, then sets state.level_flags -> 21 (Hide hud, transition)
    virtual void hide_ui(Entity* who) = 0;

    /// Returns the entity state / behavior id to set the entity to after the entering animation.
    virtual uint8_t enter(Entity* who) = 0;

    // checks layer of the Entity entering, except for FLOOR_DOOR_EGGSHIP_ROOM which gets the overlay (BG_EGGSHIP_ROOM) and returns BGEggshipRoom.player_in
    virtual bool entered_from_front_layer(Entity* who) = 0;

    // returns 0.0 except for eggship doors
    // for example: FLOOR_DOOR_EGGSHIP_ROOM returns 0.75 when entering the room, and 1.0 when exiting, runs every frame while entering/exiting
    /// Returns the darkest light level used to fade the entity when entering or exiting. 0 = black, 1 = no change
    virtual float light_level() = 0;
    /// Should we display the button prompt when collided by player. Will always return `true` for exits, layers and others that the game never locks, even if you lock it with `unlock` function
    virtual bool is_unlocked() = 0;
    /// Can the door actually be entered by player. Overrides the button prompt too if false.
    virtual bool can_enter(Entity* player) = 0;
};

class ExitDoor : public Door
{
  public:
    /// if true entering it does not load the transition
    bool entered;
    /// use provided world/level/theme
    bool special_door;
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
    /// Normally `FX_MAIN_EXIT_DOOR` but setting any entity here will block the door
    Entity* door_blocker;
    SoundMeta* sound;
    float top_value; // unsure
    float value;     // unsure
};

class EggShipDoor : public Door // probably the same as ExitDoor, but only timer is in use
{
  public:
    uint16_t unused11;
    uint8_t unused21;
    uint8_t timer; // counts from 30 to 0, dunno why
    uint16_t unknown3;
    uint16_t unknown4;
};

class EggShipDoorS : public EggShipDoor
{
  public:
    bool entered;
};

class Arrowtrap : public Floor
{
  public:
    bool arrow_shot;

    void rearm();
    /// The uid must be movable entity for ownership transfers
    void trigger(int32_t who_uid)
    {
        return trigger_trap(this, who_uid);
    }

    /// Just that, nothing fancy. The rest is done by the logical trigger i believe
    virtual Entity* spawn_arrow(float x, float y) = 0;
};

class TotemTrap : public Floor
{
  public:
    uint32_t unknown1;
    float unknown2; // might be sound delay related, cannot reproduce anymore
    float unknown3;
    ENT_TYPE spawn_entity_type;
    SOUNDID first_sound_id;
    int32_t counter;  // adds some value depending on trap and with side is triggered, setting to low value pauses it?
    uint8_t unknown4; // forced to 1
    uint8_t unused1;
    uint16_t unused2;
    uint32_t unused3;
    uint8_t timer; // unsure
    /// The uid must be movable entity for ownership transfers
    void trigger(int32_t who_uid, bool left)
    {
        return trigger_trap(this, who_uid, left ? 1 : 2);
    }
};

class LaserTrap : public Floor
{
  public:
    Illumination* emitted_light;
    union
    {
        /// counts up from 0 after triggering, cannot shoot again until 360
        uint16_t timer;
        // Deprecated
        struct
        {
            /// NoDoc
            uint8_t reset_timer;
            /// NoDoc
            bool phase_2;
        };
    };
    /// The uid must be movable entity for ownership transfers
    void trigger(int32_t who_uid)
    {
        return trigger_trap(this, who_uid);
    }
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
    /// for normal altar: counts from 0 to 20 then 0, then 1 then 0 and sacrifice happens
    uint8_t timer;
    bool unknown; // goes true/false when body is landing on it, when it stops moving it increments timer on true
};

class SpikeballTrap : public Floor
{
  public:
    SoundMeta* sound;
    Entity* chain;
    Entity* end_piece;
    /// 0 - none, 1 - start, 2 - going_down, 3 - going_up, 4 - pause; going_up is only right when timer is 0, otherwise it just sits at the bottom
    int8_t state;
    /// for the start and retract
    uint8_t timer;
    uint8_t speed_direction; // i have no clue what this is, goes 255 when detecting something, but when it hits the unbreakable tile it 255 for ever, changing it can teleport it up/down/change speed etc.
};

class TransferFloor : public Floor
{
  public:
    /// Index is the uid, value is frame the entity entered the floor (time_level), use `pairs` to loop thru
    std::unordered_map<int32_t, uint32_t> transferred_entities;
};

class ConveyorBelt : public TransferFloor
{
  public:
    UnknownPointerGroup unknown1;
    uint8_t timer;  // 0 to 15
    uint8_t timer2; // not exposed to lua
};

class Pipe : public Floor
{
  public:
    UnknownPointerGroup unknown1;
    /// 3 - straight_horizontal, 4 - blocked, 5 - down_left_turn, 6 - down_right_turn, 8 - blocked, 9 - up_left_turn, 10 - up_right_turn, 12 - straight_vertical
    int8_t direction_type;
    bool end_pipe;
};

class Generator : public Floor
{
  public:
    int32_t spawned_uid;
    uint16_t set_timer;
    uint16_t timer;
    /// Applicable only for ENT_TYPE`.FLOOR_SUNCHALLENGE_GENERATOR`
    uint8_t start_counter;
    /// Applicable only for ENT_TYPE`.FLOOR_SUNCHALLENGE_GENERATOR`
    bool on_off;

    virtual void randomize_timer() = 0; // called after it spawns entity and it's "ready" (have proper flags set etc.)
};

class SlidingWallCeiling : public Floor
{
  public:
    Entity* attached_piece;
    int32_t active_floor_part_uid;
    /// 1 - going up / is at the top, 2 - pause
    uint8_t state; // i labeled some as wrong_pause cause they are probably not used, they pause the movement but not the sound
    uint8_t unused1;
    uint16_t unused2;
    SoundMeta* ball_rise;
    SoundMeta* ball_drop;
};

class QuickSand : public Floor
{
  public:
    uint8_t unknown_timer1;
    uint8_t unknown_timer2;
    uint8_t unknown_timer3;
    uint8_t unknown_timer4;
};

class BigSpearTrap : public Floor
{
  public:
    int32_t spear_uid;
    uint8_t active; /*unsure*/ // forced to 1
    /// setting the left part to 0 or right part to 1 destroys the trap
    bool left_part;
    /// The uid must be movable entity for ownership transfers, has to be called on the left part of the trap,
    void trigger(int32_t who_uid, bool left)
    {
        return trigger_trap(this, who_uid, left ? 1 : 2);
    }
};

class StickyTrap : public Floor
{
  public:
    SoundMeta* sound;
    UnknownPointerGroup unknown1;
    int32_t attached_piece_uid;
    int32_t ball_uid;
    /// 0 - none, 1 - start, 2 - going down, 3 - is at the bottom, 4 - going up, 5 - pause
    int8_t state;
    uint8_t timer; // for the start and for the retract
};

class MotherStatue : public Floor
{
  public:
    union
    {
        /// Table of player1_standing, player2_standing, ... etc.
        std::array<bool, 4> players_standing;
        struct
        {
            bool player1_standing;
            bool player2_standing;
            bool player3_standing;
            bool player4_standing;
        };
    };
    union
    {
        /// Table of player1_health_received, player2_health_received, ... etc.
        std::array<bool, 4> players_health_received;
        struct
        {
            bool player1_health_received;
            bool player2_health_received;
            bool player3_health_received;
            bool player4_health_received;
        };
    };
    union
    {
        /// Table of player1_health_timer, player2_health_timer, ... etc.
        std::array<uint16_t, 4> players_health_timer;
        struct
        {
            uint16_t player1_health_timer;
            uint16_t player2_health_timer;
            uint16_t player3_health_timer;
            uint16_t player4_health_timer;
        };
    };
    uint8_t eggplantchild_timer;
    uint8_t unknown;
    bool eggplantchild_detected;
};

class TeleportingBorder : public Floor
{
  public:
    /// 0 - right, 1 - left, 2 - bottom, 3 - top, 4 - disable
    int8_t direction;
};

class ForceField : public Floor
{
  public:
    Entity* first_item_beam; // if killed, the whole beam goes out
    Entity* fx;
    SoundMeta* sound; // starts at timer 140
    Illumination* emitted_light;
    float bounce_force; /* unsure */ // max 1.0 ?
    bool is_on;                      // starts the sound + first_item_beam but not fx for some reason
    uint8_t unused1;
    uint16_t unused2;

    void activate_laserbeam(bool turn_on);
};

class TimedForceField : public ForceField
{
  public:
    uint32_t timer; // max 300? at 100 it's start the proces, changing the animation_frame
    bool pause;
};

class HorizontalForceField : public Floor
{
  public:
    Entity* first_item_beam; // if killed, the whole beam goes out
    Entity* fx;
    SoundMeta* sound;
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
    /// 0 - none, 1 - start, 2 - moving up, 3 - at the top, 4 - moving down 5 - pause
    int8_t state;
};

class PoleDeco : public Floor
{
  public:
    int32_t deco_up;
    int32_t deco_down;
};

class JungleSpearTrap : public Floor
{
  public:
    /// The uid must be movable entity for ownership transfers, direction: 1 = left, 2 = right, 3 = up, 4 = down
    void trigger(int32_t who_uid, uint8_t direction)
    {
        return trigger_trap(this, who_uid, direction);
    }
};
