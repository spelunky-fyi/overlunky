#include "savedata.hpp"
struct ModAPI
{
    static void godmode(bool g);
    static void godmode_companions(bool g);
    static void darkmode(bool g);

    static void set_do_hooks(bool do_hooks);

    static void set_write_load_opt(bool allow);

    static void init(class SoundManager* sound_manager = nullptr);
    static void post_init();

    static SaveData* savedata();
};
void init_state_update_hook();
void init_process_input_hook();
void init_game_loop_hook();

int64_t get_global_frame_count();
int64_t get_global_update_count();

bool get_forward_events();
