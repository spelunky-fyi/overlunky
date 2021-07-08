#pragma once

#include "Entity.hpp"
#include "Movable.hpp"
#include <cstdint>

class Crushtrap : public Movable
{
  public:
    float dirx;
    float diry;
};

class Olmec : public Movable
{
  public:
    size_t unknown_pointer;
    uint32_t target_uid;
    uint8_t attack_phase;  // 0 = stomp ; 1 = bombs ; 2 = stomp+ufos ; 3 = in lava
    uint8_t attack_timer;  // in phase 0/2: time spent looking for player ; in phase 1: time between bomb salvo
    uint8_t ai_timer;      // general timer that counts down whenever olmec is active
    int8_t move_direction; // -1 = left ; 0 = down ; 1 = right (phase 0/2: depends on target, phase 1: travel direction)
    uint8_t jump_timer;
    uint8_t phase1_amount_of_bomb_salvos;
    uint8_t unknown_attack_state; // some attack state in phase 1/2 (changes to 2 when stomping and 3 during ufos)

    uint8_t broken_floaters();
};
