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
    int32_t i118;
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
    size_t i150;
    size_t p158;
    size_t p160;
    int32_t i168;
    int32_t i16c;
    uint32_t jump_flags;
    uint8_t some_timer;
    uint8_t can_use;
    uint8_t b176;
    uint8_t b177;
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

class Block : public Movable
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
