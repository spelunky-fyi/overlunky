class Movable : public Entity
{
  public:
    std::map<int64_t, int64_t> pa0;
    std::map<int, int> pb0;
    size_t anim_func;
    int32_t ic8;
    int32_t icc;
    float movex;
    float movey;
    uint32_t buttons;
    uint32_t stand_counter;
    float fe0;
    int32_t price;
    int32_t owner_uid;
    int32_t last_owner_uid;
    size_t animation_func;
    uint32_t idle_counter;
    int32_t standing_on_uid;
    float velocityx;
    float velocityy;
    int32_t holding_uid;
    uint8_t state;
    uint8_t last_state;
    uint8_t move_state;
    uint8_t health;
    uint16_t stun_timer;
    uint16_t stun_state;
    uint32_t some_state;
    int16_t poison_tick_timer;
    int16_t unknown_timer;
    int32_t i11c;
    int32_t i120;
    uint8_t b124;
    uint8_t airtime;
    uint8_t b126;
    uint8_t b127;

    virtual void v36() = 0;
    virtual void v37() = 0;
    virtual float sprint_factor() = 0;
    virtual void v39() = 0;
    virtual void v40() = 0;
    virtual void v41() = 0;
    virtual void v42() = 0;
    virtual void v43() = 0;
    virtual void v44() = 0;
    virtual void v45() = 0;
    virtual void v46() = 0;
    virtual void v47() = 0;
    virtual void v48() = 0;
    virtual void v49() = 0;
    virtual void v50() = 0;
    virtual void v51() = 0;
    virtual void v52() = 0;
    virtual void v53() = 0;
    virtual void v54() = 0;
    virtual void v55() = 0;
    virtual void v56() = 0;
    virtual void v57() = 0;
    virtual void v58() = 0;
    virtual void v59() = 0;
    virtual void v60() = 0;
    virtual void v61() = 0;
    virtual void v62() = 0;
    virtual void v63() = 0;
    virtual void v64() = 0;
    virtual void v65() = 0;
    virtual void v66() = 0;
    virtual void v67() = 0;
    virtual void v68() = 0;
    virtual void v69() = 0;
    virtual void v70() = 0;
    virtual void v71() = 0;
    virtual void v72() = 0;
    virtual void v73() = 0;
    virtual void v74() = 0;
    virtual void v75() = 0;
    virtual void v76() = 0;
    virtual void v77() = 0;
    virtual void v78() = 0;
    virtual void v79() = 0;
    virtual void v80() = 0;
    virtual void v81() = 0;
    virtual void v82() = 0;
    virtual void v83() = 0;
    virtual void v84() = 0;
    virtual void v85() = 0;
    virtual void v86() = 0;

    void poison(int16_t frames); // 1 - 32767 frames ; -1 = no poison
    bool is_poisoned();

    bool is_button_pressed(uint32_t button);
    bool is_button_held(uint32_t button);
    bool is_button_released(uint32_t button);
};

class Monster : public Movable
{
  public:
    std::map<int64_t, int64_t> inside;
};

class Player : public Monster
{
  public:
    Inventory *inventory_ptr;
    size_t p140;
    int32_t i148;
    int32_t i14c;
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
};

class Container : public Movable
{
  public:
    int32_t inside;
    int32_t timer;
};

class Gun : public Movable
{
  public:
    uint8_t cooldown;
    uint8_t shots;
    uint8_t shots2;
    uint8_t b12b;
    int32_t in_chamber;
};

class Crushtrap : public Movable
{
  public:
    float dirx;
    float diry;
};

class Mattock : public Movable
{
  public:
    int32_t remaining;
};

class Mount : public Monster
{
  public:
    void carry(Movable *rider);

    void tame(bool value);
};

class Jetpack : public Movable
{
  public:
    int32_t unknown1;
    int32_t unknown2;
    int32_t unknown3;
    int32_t unknown4;
    uint8_t fuel; // only set the fuel for an equipped jetpack (player->items)!
};

class Bomb : public Movable
{
  public:
    size_t unknown1;
    float scale_hor; // 1.25 = default regular bomb ; 1.875 = default giant bomb ; > 1.25 generates ENT_TYPE_FX_POWEREDEXPLOSION
    float scale_ver;
    bool unknown2;
};

class Olmec : public Movable
{
  public:
    size_t unknown_pointer; 
    uint32_t unknown1; // doesn't change during attack
    uint8_t attack_phase; // 0 = stomp ; 1 = bombs ; 2 = stomp+ufos ; 3 = in lava
    uint8_t attack_timer; // in phase 0/2: time spent looking for player ; in phase 1: time between bomb salvo
    uint8_t ai_timer; // general timer that counts down whenever olmec is active
    int8_t move_direction; // -1 = left ; 0 = down ; 1 = right (phase 0/2: depends on target, phase 1: travel direction)
    uint8_t jump_timer;
    uint8_t phase1_amount_of_bomb_salvos;
    uint8_t unknown_attack_state; // some attack state in phase 1/2 (changes to 2 when stomping and 3 during ufos)

    uint8_t broken_floaters();
};

class OlmecFloater : public Movable
{
  public:
    bool both_floaters_broken; // strangely, this indicates whether both are broken, not just this specific one
};

class Cape : public Movable
{
  public:
    size_t unknown;
    bool floating_down;
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    uint32_t floating_count; // increments whenever floating
};

class VladsCape : public Cape
{
  public:
    bool can_double_jump;
};

class KapalaPowerup : public Movable
{
  public:
    uint8_t amount_of_blood;
};

class ChasingMonster : public Monster
{
  public:
    int32_t chased_target_uid;
    uint32_t target_selection_timer; // when reaches zero, checks for new chase target and updates chased_target_uid
};

class Ghost : public ChasingMonster
{
  public:
    uint16_t split_timer;
    uint8_t unknown_counter;
    uint8_t padding;
    float velocity_multiplier;
};

class Jiangshi : public ChasingMonster
{
  public:
    uint8_t wait_timer; // wait time between jumps
};
