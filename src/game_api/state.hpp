#pragma once

#include "items.hpp"
#include "layer.hpp"
#include "memory.hpp"
#include "savedata.hpp"
#include "state_structs.hpp"
#include "thread_utils.hpp"

const float ZF = 0.737;

struct Layer;
struct ThemeInfo;
struct LevelGenSystem;

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
    uint8_t unknown1a;
    uint8_t unknown1b;
    bool world2_coffin_spawned;
    bool world4_coffin_spawned;
    bool world6_coffin_spawned;
    uint8_t unknown2b;
    uint8_t unknown2c;
    uint8_t unknown2d;
    uint32_t waddler_storage[99];
    int16_t waddler_storage_meta[99]; // to store mattock durability for example
    uint16_t journal_progression_count;
    JournalProgressionSlot journal_progression_slots[40];
    uint8_t skip2[844];
    ThemeProgression theme_progression;
    uint8_t unknown3;
    uint8_t unknown4;
    uint8_t unknown5a;
    uint8_t unknown5b;
    uint8_t unknown5c;
    uint8_t unknown5d;
    ArenaState arena;
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
    uint8_t cause_of_death;
    uint8_t padding10;
    uint8_t padding11;
    uint8_t padding12;
    uint32_t cause_of_death_entity_type;
    int32_t waddler_floor_storage; // entity uid of the first floor_storage entity
    size_t toast;
    size_t speechbubble;
    uint32_t speechbubble_timer;
    uint32_t toast_timer;
    int32_t speechbubble_owner;
    Dialogue basecamp_dialogue;
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
    Logic* logic;
    QuestsInfo* quests;
    std::unordered_map<uint32_t, int32_t>* ai_targets; // e.g. hired hand uid -> snake uid
    LiquidPhysics* liquid_physics;
    PointerList* particle_emitters; // list of ParticleEmitterInfo*
    PointerList* lightsources;      // list of Illumination*
    size_t unknown27;
    std::unordered_map<uint32_t, Entity*> instance_id_to_pointer;
    size_t unknown28;
    size_t unknown29;
    size_t unknown30;
    uint32_t layer_transition_effect_timer;
    uint32_t camera_layer;
    size_t unknown32;
    size_t unknown33;
    size_t unknown34;
    size_t unknown35;
    size_t unknown36;
    uint32_t time_startup;
    uint32_t special_visibility_flags;
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

    // Returns the main-thread version of StateMemory*
    StateMemory* ptr() const;
    // Returns the local-thread version of StateMemory*
    StateMemory* ptr_local() const;

    Layer* layer(uint8_t index)
    {
        return ptr()->layers[index];
    }
    Layer* layer_local(uint8_t index)
    {
        return ptr_local()->layers[index];
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
