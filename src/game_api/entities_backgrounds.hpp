#pragma once

#include "entity.hpp"
#include "sound_manager.hpp"

struct Illumination;

class BGBackLayerDoor : public Entity
{
  public:
    Illumination* illumination1;
    Illumination* illumination2;
};

class BGSurfaceStar : public Entity
{
  public:
    int32_t blink_timer; // why negative?
    float relative_x;
    float relative_y;
    int32_t unknown_padding;
};

class BGRelativeElement : public Entity
{
  public:
    float relative_x;
    float relative_y;
};

class BGSurfaceLayer : public BGRelativeElement
{
  public:
    float relative_offset_x;
    float relative_offset_y;
};

class BGEggshipRoom : public Entity
{
  public:
    SoundMeta* sound;
    Entity* fx_shell;
    Entity* fx_door;
    Entity* platform_left;
    Entity* platform_middle;
    Entity* platform_right;
    bool player_in;
};

class BGMovingStar : public BGSurfaceStar
{
  public:
    /// Can make it rise if set to negative
    float falling_speed;
};

class BGTutorialSign : public Entity
{
  public:
    bool is_shown;
};

class BGShootingStar : public BGRelativeElement
{
  public:
    float x_increment;
    float y_increment;
    int16_t timer;
    int16_t max_timer;
    /// Gets smaller as the timer gets close to the max_timer
    float size;
    float light_size; // UNSURE if you make it the same as size it starts to flicker, making this bigger increases the size as well
};

class BGShopEntrance : public Entity
{
  public:
    bool on_entering;
};

class BGFloatingDebris : public BGSurfaceLayer
{
  public:
    /// Distance it travels up and down from spawn position
    float distance;
    float speed;
    float sine_angle;
};

class BGShopKeeperPrime : public Entity
{
  public:
    float normal_y;
    float sine_pos;
    int16_t bubbles_timer;
    bool bubble_spawn_trigger;
    int8_t unknown_padding;
    int16_t bubble_spawn_delay; // normally it's just 0, 1 or 2, but you can set it to some value and then when using bubble_spawn_trigger it will count down and then spawn bubbles
};
