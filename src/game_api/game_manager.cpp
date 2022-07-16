#include "game_manager.hpp"

#include <string> // for operator""sv

#include "search.hpp" // for get_address

GameManager* get_game_manager()
{
    static GameManager** gm = (GameManager**)get_address("game_manager"sv);
    return *gm;
}
