#pragma once

#include "movable.hpp"
#include "particles.hpp"
#include "sound_manager.hpp"

struct Illumination;

class OlmecFloater : public Movable
{
  public:
    bool both_floaters_intact; // strangely, this indicates whether both are intact, not just this specific one
    bool on_breaking;
};

class LiquidSurface : public Movable
{
  public:
    float glow_radius;
    float sine_pos;           // x position to calculate sine wave, it moves the glow back and forth
    float sine_pos_increment; // just an on frame increment for the value above
};

class EggshipCenterJetFlame : public Movable
{
  public:
    SoundMeta* sound;
    int64_t unknown2;
    Illumination* emitted_light;
    ParticleEmitterInfo* particle;
    bool smoke_on;
};

class MiniGameShipOffset : public Movable
{
  public:
    float offset_x;
    float offset_y; // = y but also special_offsety
    float unknown2;
    /// Is added to offset_y
    float normal_y_offset;
};

class Button : public Movable
{
  public:
    /// Only one can be set:
    /// 1 - pad: A, key: Z
    /// 2 - pad: X, key: X
    /// 4 - pad: B, key: C
    /// 8 - pad: Y, key: D
    /// 16 - pad: LB, key: L Shift
    /// 32 - pad: RB, key: A
    /// 64 - pad: menu?, key: (none)
    /// 128 - pad: copy?, key: Tab
    uint8_t button_sprite; // changes the button shown, setting more then one flag defaults to the first
    int8_t unknown2;
    int16_t padding1;
    float visibility;
    /// It's false for selldialog used in shops
    bool is_visible;
    /// It's set true even if player does not see the button, like the drill or COG door
    bool player_trigger;
    bool unknown4;
    int8_t padding2;
    int32_t player1_on_screen_timer; // a weird one, does not start at 0, sometimes accessed as byte, similar to the jetpack fly_counter, that is not per button, also some buttons use it, some not
    int32_t player2_on_screen_timer; // they all are not even per player, as they run in sync
    int32_t player3_on_screen_timer; // timers sometimes stay, sometimes are set back to -1
    int32_t player4_on_screen_timer;
    ///
    /// -1 - hasn't been seen
    /// 0 - last seen by player 1
    /// 1 - last seen by player 2
    /// 2 - last seen by player 3
    /// 3 - last seen by player 4
    int8_t seen;
    int8_t unknown11;
    int16_t padding3;
    bool (*check_leader_autorun_status)(); // used to create correct graphics in the basecamp tutorial
};

class FxTornJournalPage : public Movable
{
  public:
    /// Only in tutorial
    int32_t page_number;
};

class FxMainExitDoor : public Movable
{
  public:
    Illumination* emitted_light;
    /// When breaking open in tutorial
    int32_t timer;
    float unknown; // increments by 0.15
};

class Birdies : public Movable
{
  public:
    uint8_t unknown; // animation_frame offset? , 0 for monsters, 160 for character
};

class Explosion : public Movable
{
  public:
    Illumination* emitted_light;
};

class FxOuroboroOccluder : public Movable
{
  public:
    float unknown; // counts from 0.0 to 10.0
};

class FxOuroboroDragonPart : public Movable
{
  public:
    float unknown1; // angle?
    float unknown2; // fade in/fade out?
    float unknown3;
    float speed;   // increment value for unknown1
    int16_t timer; // affects the speed and fade away
    int16_t padding1;
    int32_t padding2;
    ParticleEmitterInfo* particle;
};

class Rubble : public Movable
{
  public:
    uint8_t unknown1; // is different depending of the block source
    int8_t unknown2;
    int16_t unknown3;
    int32_t unknown4;
    float unknown5;
    bool unknown6; // shrinks it down to nothing?
};

class FxCompass : public Movable
{
  public:
    float unknown1; // moving distance
    /// Counts form 0 to 2*pi, responsible for moving back and forth
    float sine_angle;
    float visibility;
    /// Player has compass
    bool is_active;
};

class SleepBubble : public Movable
{
  public:
    uint8_t show_hide_timer;
};

class MovingIcon : public Movable
{
  public:
    /// Used to move it up and down in sync with others
    uint8_t movement_timer;
};

class FxSaleContainer : public Movable
{
  public:
    Entity* fx_value;
    Entity* fx_icon;
    Entity* fx_button;
    /// For effect when you don't have enough money
    float shake_amplitude;
    /// Also sound_played, keeps re-triggering from time to time
    bool sound_trigger;
    uint8_t pop_in_out_procentage;
};

class FxPickupEffect : public Movable
{
  public:
    float spawn_y;
    float visibility; // or the disappearing animation progression
};

class FxShotgunBlast : public Movable
{
  public:
    Illumination* illumination;
};

class FxJetpackFlame : public Movable
{
  public:
    ParticleEmitterInfo* particle_smoke;
    ParticleEmitterInfo* particle_flame;
    SoundMeta* sound;
    Illumination* illumination;
};

class FxPlayerIndicator : public Movable
{
  public:
    int32_t attached_to;
    float pos_x;
    float pos_y;
    int32_t unknown;
};

class FxSpringtrapRing : public Movable
{
  public:
    int32_t timer;
    int32_t padding;
    Illumination* illumination;
};

class FxWitchdoctorHint : public Movable
{
  public:
    /// There can be only one Hint above the player, so it has list of witchdoctors
    /// in case there are more then one attacking you the same time
    std::set<int32_t> witchdoctor;
};

class FxNecromancerANKH : public Movable
{
  public:
    float unknown;
};

class FxWebbedEffect : public Movable
{
  public:
    bool visible;
};

class FxUnderwaterBubble : public Movable
{
  public:
    float unknown1;
    int32_t bubble_source_uid;
    /// 1 / -1
    int8_t direction;
    /// Setting it true makes it disappear/fade away
    bool pop;
    bool inverted; // goes down instead of up
};

class FxWaterDrop : public Movable
{
  public:
    bool inverted; // floats up instead of drooping down
    bool unknown1;
    int8_t unknown2;
    int8_t unknown3;
    int32_t unknown4;
    int32_t droplet_source_uid;
};

class FxKinguSliding : public Movable
{
  public:
    ParticleEmitterInfo* particle;
};

class FxAlienBlast : public Movable
{
  public:
    int8_t unknown;
};

class FxSparkSmall : public Movable
{
  public:
    float unknown1;
    float step; // per frame increment for variable above
    float unknown3;
    int8_t timer; // all values gets randomized when timer = 0
};

class FxTiamatHead : public Movable
{
  public:
    int16_t timer;
};

class FxTiamatTorso : public Movable
{
  public:
    int16_t timer;
    int16_t padding;
    /// Slowly increases/decreases to the given value
    float torso_target_size;
};

class FxTiamatTail : public Movable
{
  public:
    /// Added _two just to not shadow angle in entity, it's angle but the pivot point is at the edge
    float angle_two;
    float x_pos;
    float y_pos;
};

class FxVatBubble : public Movable
{
  public:
    float max_y;
};

class FxHundunNeckPiece : public Movable
{
  public:
    /// Short timer after the head is dead
    int16_t kill_timer;
};

class FxJellyfishStar : public Movable
{
  public:
    float rotation_angle;
    float radius;
    float speed;
};

class FxQuickSand : public Movable
{
  public:
    float unknown;
};

class FxSorceressAttack : public Movable
{
  public:
    float size;
    float unknown2; // pulsing related
    bool unknown3;  // pulsing related
};

class FxLamassuAttack : public Movable
{
  public:
    float attack_angle;
    float attack_angle2; // seams to be exactly the same as above
    bool unknown;
};

class FxFireflyLight : public Movable
{
  public:
    Illumination* illumination;
    uint8_t light_timer;
    int8_t padding;
    /// Timer between light flashes
    uint16_t cooldown_timer;
};

class FxEmpress : public Movable
{
  public:
    float sine_angle;
};

class FxAnkhRotatingSpark : public Movable
{
  public:
    float radius;
    float inclination;
    /// 0 - 1.0
    float speed; // a weird one, it gets values lower then 1.0, if you set 1.0 or bigger it will stop
    float sine_angle;
    float size;
};

class FxAnkhBrokenPiece : public Movable
{
  public:
    float unknown;
};

class MegaJellyfishEye : public Movable
{
  public:
    uint8_t timer;
};
