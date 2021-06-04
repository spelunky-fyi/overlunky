#pragma once

#include "items.hpp"
#include "layer.hpp"
#include "memory.hpp"
#include "savedata.hpp"
#include "thread_utils.hpp"

const float ZF = 0.737;

struct Layer;
struct LevelGenSystem;
struct Illumination;
struct PlayerInputs;
struct QuestsInfo;
struct Camera;

struct StateMemory
{
    size_t p00;
    uint32_t screen_last;
    uint32_t screen;
    uint32_t screen_next;
    uint32_t loading;
    Illumination* illumination;
    float fadevalue; // 0.0 = all visible; 1.0 = all black
    uint32_t fadeout;
    uint32_t fadein;
    uint32_t loading_black_screen_timer; // if state.loading is 1, this timer counts down to 0 while the screen is black (used after Ouroboros, in
                                         // credits, ...)
    uint8_t ingame;
    uint8_t playing;
    uint8_t pause;
    uint8_t b33;
    int32_t i34;
    uint32_t quest_flags;
    uint8_t correct_ushabti; // correct_ushabti = anim_frame - (2 * floor(anim_frame/12))
    uint8_t i3cb;
    uint8_t i3cc;
    uint8_t i3cd;
    uint32_t speedrun_character;         // who administers the speedrun in base camp
    uint8_t speedrun_activation_trigger; // must transition from true to false to activate it
    uint8_t padding4;
    uint8_t padding5;
    uint8_t padding6;
    uint32_t w;
    uint32_t h;
    int8_t kali_favor;
    int8_t kali_status;
    int8_t kali_altars_destroyed;
    uint8_t b4f;
    int32_t i50;
    int32_t money_shop_total; // total $ spent at shops, persists between levels, number will be negative
    uint8_t world_start;
    uint8_t level_start;
    uint8_t theme_start;
    uint8_t b5f;
    uint32_t seed;
    uint32_t time_total;
    uint8_t world;
    uint8_t world_next;
    uint8_t level;
    uint8_t level_next;
    int32_t i6c; // i6c and i70 are a pointer to ThemeInfo (todo)
    int32_t i70;
    uint8_t theme;
    uint8_t theme_next;
    uint8_t win_state; // 0 = no win 1 = tiamat win 2 = hundun win 3 = CO win; set this and next doorway leads to victory scene
    uint8_t b73;
    uint32_t end_spaceship_character; // who pops out the spaceship for a tiamat/hundun win
    uint8_t shoppie_aggro;
    uint8_t shoppie_aggro_levels;
    uint8_t merchant_aggro;
    uint8_t saved_dogs;
    uint8_t saved_cats;
    uint8_t saved_hamsters;
    uint8_t kills_npc;
    uint8_t level_count;
    uint8_t pad84[0x970];
    uint32_t journal_flags;
    int32_t first_damage_cause; // entity type that caused first damage, for the journal
    int8_t first_damage_world;
    int8_t first_damage_level;
    uint8_t i9f4c;
    uint8_t i9f4d;
    uint32_t time_last_level;
    uint32_t time_level;
    uint32_t time_speedrun;
    uint32_t money_last_levels;
    int32_t hud_flags;
    uint32_t presence_flags;
    uint32_t coffin_contents; // entity type - the contents of the coffin that will be spawned (during levelgen)
    bool player_died;
    uint8_t padding10;
    uint8_t padding11;
    uint8_t padding12;
    uint32_t cause_of_death;       // entity type
    int32_t waddler_floor_storage; // entity uid of the first floor_storage entity
    size_t toast;
    size_t speechbubble;
    uint32_t speechbubble_timer;
    uint32_t toast_timer;
    int32_t speechbubble_owner;

    char pada14[1980];

    size_t arena_choose_teams_screen;
    size_t unknown8_during_basecamp;
    size_t unknown9_during_level;
    size_t level_transition_screen;
    size_t unknown11;
    size_t unknown12;
    size_t victory_walk_screen;
    size_t credits_screen;
    size_t final_score_screen;
    size_t cosmic_ocean_win_screen;
    size_t dear_journal_screen;
    size_t unknown13;
    size_t unknown14_screen;
    size_t unknown15;
    size_t unknown16_screen;
    size_t arena_lineup_screen;
    size_t arena_gameplay_screen;
    size_t arena_scorepillars_screen;
    size_t unknown17;
    size_t unknown18;
    uint32_t next_entity_uid;
    uint16_t unknown20;
    uint16_t screen_change_counter; // increments every time screen changes; used in online sync together with next_entity_uid and unknown20 as a 64bit number
    PlayerInputs* player_inputs;

    Items* items;
    LevelGenSystem* level_gen;
    Layer* layers[2];
    size_t logic;
    QuestsInfo* quests;
    size_t unknown24;
    size_t liquid_physics;
    size_t particle_emitters_info;
    size_t unknown26;
    size_t unknown27;
    std::unordered_map<uint32_t, Entity*> instance_id_to_pointer;
    size_t unknown28;
    size_t unknown29;
    size_t unknown30;
    size_t unknown31;
    size_t unknown32;
    size_t unknown33;
    size_t unknown34;
    size_t unknown35;
    size_t unknown36;
    uint32_t time_startup;
    uint32_t unknown38;
    Camera* camera;
};
struct State
{
    size_t location;
    size_t addr_damage;
    size_t addr_insta;
    size_t addr_zoom;
    size_t addr_zoom_shop;
    size_t addr_dark;

    static void set_write_load_opt(bool allow);

    static State& get();

    StateMemory* ptr() const;

    Layer* layer(uint8_t index)
    {
        return ptr()->layers[index];
    }

    Items* items()
    {
        auto pointer = ptr()->items;
        return (Items*)(pointer);
    }

    void godmode(bool g)
    {
        // log::debug!("God {:?}" mode; g);
        if (g)
        {
            write_mem_prot(addr_damage, ("\xC3"s), true);
            write_mem_prot(addr_insta, ("\xC3"s), true);
        }
        else
        {
            write_mem_prot(addr_damage, ("\x48"s), true);
            write_mem_prot(addr_insta, ("\x40"s), true);
        }
    }

    void darkmode(bool g)
    {
        // log::debug!("God {:?}" mode; g);
        if (g)
        {
            write_mem_prot(addr_dark, ("\x90\x90"s), true);
        }
        else
        {
            write_mem_prot(addr_dark, ("\xEB\x2E"s), true);
        }
    }

    void zoom(float level)
    {
        // This technically sets camp zoom but not interactively :(
        // auto addr_zoom = find_inst(memory.exe(), &hex!("C7 80 E8 04 08 00"),
        // memory.after_bundle); write_mem_prot(memory.at_exe(addr_zoom + 6),
        // to_le_bytes(level), true); addr_zoom = memory.after_bundle;

        uint8_t roomx = ptr()->w;
        if (level == 0.0)
        {
            switch (roomx)
            {
            case 1:
                level = 9.50;
                break;
            case 2:
                level = 16.29;
                break;
            case 3:
                level = 23.08;
                break;
            case 4:
                level = 29.87;
                break;
            case 5:
                level = 36.66;
                break;
            case 6:
                level = 43.45;
                break;
            case 7:
                level = 50.24;
                break;
            case 8:
                level = 57.03;
                break;
            default:
                level = 13.5;
            }
        }
        write_mem_prot(addr_zoom, to_le_bytes(level), true);
        write_mem_prot(addr_zoom_shop, to_le_bytes(level), true);
    }

    std::pair<float, float> click_position(float x, float y);
    std::pair<float, float> screen_position(float x, float y);
    float get_zoom_level();

    uint32_t flags()
    {
        return ptr()->hud_flags;
    }

    void set_flags(uint32_t f)
    {
        ptr()->hud_flags = f;
    }

    void set_pause(uint8_t p)
    {
        ptr()->pause = p;
    }

    uint32_t get_frame_count()
    {
        return read_u32((size_t)ptr() - 0xd0);
    }

    std::vector<int64_t> read_prng()
    {
        std::vector<int64_t> prng;
        for (int i = 0; i < 20; ++i)
        {
            prng.push_back(read_i64((size_t)ptr() - 0xb0 + 8 * i));
        }
        return prng;
    }

    Entity* find(uint32_t unique_id)
    {
        auto& map = ptr()->instance_id_to_pointer;
        auto it = map.find(unique_id);
        if (it == map.end())
            return nullptr;
        return it->second;
    }

    std::pair<float, float> get_camera_position();
    void set_camera_position(float cx, float cy);
    void warp(uint8_t w, uint8_t l, uint8_t t);
    void set_seed(uint32_t seed);
    SaveData* savedata();
};

struct SaturationVignette
{
    float red; // default = 1.0 (can go over 1.0 for oversaturation)
    float green;
    float blue;
    float vignette_aperture; // default = 20000.0 ; visible aperture change in range 1.0 - 100.0
};

struct Illumination
{
    SaturationVignette saturation_vignette;
    SaturationVignette saturation_vignette_other[3]; // there's three more, no idea why (multiplayer doesn't change these)
    float brightness1;
    float brightness2;
    float something_min;
    float something_max;
    size_t unknown_empty;
    float unknown_float;
    float unknown_nan;
    uint32_t unknown_timer;
    uint8_t frontlayer_global_illumination; // 0 = on; 1 = off; 4 = white; ... higher starts to flicker
    uint8_t unknown_illumination1;
    uint8_t backlayer_global_illumination; // 0 = off ; 1 = on but turns front layer off
    uint8_t unknown_illumination2;         // influences backlayer_global_illumination
    uint32_t unknown_int1;                 // crash when changed
    uint32_t unknown_int2;
};

struct InputMapping
{
    uint8_t jump;
    uint8_t attack;
    uint8_t bomb;
    uint8_t rope;
    uint8_t walk_run;
    uint8_t use_door_buy;
    uint8_t pause_menu;
    uint8_t journal;
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
};

struct PlayerSlot
{
    uint16_t buttons_gameplay;
    uint16_t buttons;
    uint32_t unknown1;
    InputMapping* input_mapping_keyboard;
    InputMapping* input_mapping_controller;
    uint8_t player_id;
    bool is_participating;
    uint8_t unknown4; // padding most likely
    uint8_t unknown5; // padding most likely
    uint32_t unknown6;
};

struct PlayerSlotSettings
{
    bool controller_vibration;
    bool auto_run_enabled;
    bool controller_right_stick;
};

struct PlayerInputs
{
    PlayerSlot player_slot_1;
    PlayerSlot player_slot_2;
    PlayerSlot player_slot_3;
    PlayerSlot player_slot_4;
    PlayerSlotSettings player_slot_1_settings;
    PlayerSlotSettings player_slot_2_settings;
    PlayerSlotSettings player_slot_3_settings;
    PlayerSlotSettings player_slot_4_settings;
};

struct QuestsInfo
{
    size_t unknown1; // the first six are pointers to small similar objects
    size_t unknown2; // that don't appear to change at all
    size_t unknown3;
    size_t unknown4;
    size_t unknown5;
    size_t unknown6;
    int8_t yang_state;
    int8_t jungle_sisters_flags; // flags! not state ; -1 = sisters angry
    int8_t van_horsing_state;
    int8_t sparrow_state;
    int8_t madame_tusk_state;
    int8_t beg_state;
    uint8_t unknown13;
    uint8_t unknown14;
    uint32_t unknown15;
    uint32_t unknown16;
};

struct Camera
{
    float bounds_left;
    float bounds_right;
    float bounds_bottom;
    float bounds_top;
    float adjusted_focus_x; // focus adjusted so camera doesn't show beyond borders
    float adjusted_focus_y;
    float calculated_focus_x; // forced values
    float calculated_focus_y;
    float focus_offset_x; // added to position of focused entity, if any
    float focus_offset_y;
    float unknown1; // does not get reset on level reload
    float focus_x;  // the unadjusted center point to focus the camera on; set this for full camera control when the focused entity = -1
    float focus_y;
    float unknown2;
    float vertical_pan; // set to a high number, like 5000 and the camera pans from top to bottom

    // to shake the camera, set shake_countdown_start and shake_countdown to the number of frames you want to shake for
    // set the shake amplitude, and control the direction with the multipliers, optionally add randomness by toggling uniform_shake

    uint32_t shake_countdown_start; // probably used to calculate the percentage of the amplitude when progressing through the shake
    uint32_t shake_countdown;
    float shake_amplitude;    // the amount of camera shake
    float shake_multiplier_x; // set to 0 to eliminate horizontal shake; negative inverts direction
    float shake_multiplier_y; // set to 0 to eliminate vertical shake; negative inverts direction
    bool uniform_shake;       // if false, the shake gets randomized a bit
    uint8_t padding1;
    uint8_t padding2;
    uint8_t padding3;
    int32_t focused_entity_uid; // if set to -1, you have free control over camera focus through focus_x, focus_y
    uint32_t unknown3;
    uint32_t unknown4;
    float unknown_adjustment;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
};
