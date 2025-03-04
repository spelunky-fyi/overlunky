#include "game_manager.hpp"

#include "search.hpp" // for get_address

GameManager* get_game_manager()
{
    static GameManager** gm = (GameManager**)get_address("game_manager"sv);
    return *gm;
}

RawInput* get_raw_input()
{
    static auto offset = get_address("input_table");
    return reinterpret_cast<RawInput*>(offset);
}
