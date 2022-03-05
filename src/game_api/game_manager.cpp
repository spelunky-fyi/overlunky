#include "game_manager.hpp"

#include "memory.hpp"

GameManager* get_game_manager()
{
    static GameManager** gm = (GameManager**)get_address("game_manager"sv);
    return *gm;
}
