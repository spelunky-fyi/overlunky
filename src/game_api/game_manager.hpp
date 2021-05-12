#include "savedata.hpp"
#include "thread_utils.hpp"

struct TmpStruct
{
    OnHeapPointer<SaveData> savedata;
};

struct GameManager
{
    void* another_state;
    TmpStruct* tmp;
};

GameManager* get_game_manager();