#include "state_lua.hpp"

#include <algorithm>              // for max
#include <cstdint>                // for uint8_t
#include <locale>                 // for num_put
#include <new>                    // for operator new
#include <sol/sol.hpp>            // for data_t, global_table, state, proxy...
#include <sol/usertype.hpp>       // for basic_usertype
#include <sol/usertype_proxy.hpp> // for usertype_proxy
#include <string>                 // for operator==, allocator
#include <tuple>                  // for get
#include <type_traits>            // for move, declval, decay_t, reference_...
#include <utility>                // for min, max

#include "entities_chars.hpp"     // IWYU pragma: keep
#include "entity.hpp"             // IWYU pragma: keep
#include "illumination.hpp"       // IWYU pragma: keep
#include "items.hpp"              // for Items, SelectPlayerSlot, Items::is...
#include "level_api.hpp"          // IWYU pragma: keep
#include "online.hpp"             // for OnlinePlayer, OnlineLobby, Online
#include "savestate.hpp"          // for SaveState
#include "screen.hpp"             // IWYU pragma: keep
#include "screen_arena.hpp"       // IWYU pragma: keep
#include "script/events.hpp"      // for pre_load_state
#include "script/lua_backend.hpp" // for LuaBackend
#include "state.hpp"              // for StateMemory, State, StateMemory::a...
#include "state_structs.hpp"      // for ArenaConfigArenas, ArenaConfigItems

namespace NState
{
void register_usertypes(sol::state& lua)
{
    /// Used in ArenaState
    auto arenaconfigarenas_type = lua.new_usertype<ArenaConfigArenas>("ArenaConfigArenas");
    arenaconfigarenas_type["list"] = &ArenaConfigArenas::list;
    arenaconfigarenas_type["dwelling_1"] = &ArenaConfigArenas::dwelling_1;
    arenaconfigarenas_type["dwelling_2"] = &ArenaConfigArenas::dwelling_2;
    arenaconfigarenas_type["dwelling_3"] = &ArenaConfigArenas::dwelling_3;
    arenaconfigarenas_type["dwelling_4"] = &ArenaConfigArenas::dwelling_4;
    arenaconfigarenas_type["dwelling_5"] = &ArenaConfigArenas::dwelling_5;
    arenaconfigarenas_type["jungle_1"] = &ArenaConfigArenas::jungle_1;
    arenaconfigarenas_type["jungle_2"] = &ArenaConfigArenas::jungle_2;
    arenaconfigarenas_type["jungle_3"] = &ArenaConfigArenas::jungle_3;
    arenaconfigarenas_type["jungle_4"] = &ArenaConfigArenas::jungle_4;
    arenaconfigarenas_type["jungle_5"] = &ArenaConfigArenas::jungle_5;
    arenaconfigarenas_type["volcana_1"] = &ArenaConfigArenas::volcana_1;
    arenaconfigarenas_type["volcana_2"] = &ArenaConfigArenas::volcana_2;
    arenaconfigarenas_type["volcana_3"] = &ArenaConfigArenas::volcana_3;
    arenaconfigarenas_type["volcana_4"] = &ArenaConfigArenas::volcana_4;
    arenaconfigarenas_type["volcana_5"] = &ArenaConfigArenas::volcana_5;
    arenaconfigarenas_type["tidepool_1"] = &ArenaConfigArenas::tidepool_1;
    arenaconfigarenas_type["tidepool_2"] = &ArenaConfigArenas::tidepool_2;
    arenaconfigarenas_type["tidepool_3"] = &ArenaConfigArenas::tidepool_3;
    arenaconfigarenas_type["tidepool_4"] = &ArenaConfigArenas::tidepool_4;
    arenaconfigarenas_type["tidepool_5"] = &ArenaConfigArenas::tidepool_5;
    arenaconfigarenas_type["temple_1"] = &ArenaConfigArenas::temple_1;
    arenaconfigarenas_type["temple_2"] = &ArenaConfigArenas::temple_2;
    arenaconfigarenas_type["temple_3"] = &ArenaConfigArenas::temple_3;
    arenaconfigarenas_type["temple_4"] = &ArenaConfigArenas::temple_4;
    arenaconfigarenas_type["temple_5"] = &ArenaConfigArenas::temple_5;
    arenaconfigarenas_type["icecaves_1"] = &ArenaConfigArenas::icecaves_1;
    arenaconfigarenas_type["icecaves_2"] = &ArenaConfigArenas::icecaves_2;
    arenaconfigarenas_type["icecaves_3"] = &ArenaConfigArenas::icecaves_3;
    arenaconfigarenas_type["icecaves_4"] = &ArenaConfigArenas::icecaves_4;
    arenaconfigarenas_type["icecaves_5"] = &ArenaConfigArenas::icecaves_5;
    arenaconfigarenas_type["neobabylon_1"] = &ArenaConfigArenas::neobabylon_1;
    arenaconfigarenas_type["neobabylon_2"] = &ArenaConfigArenas::neobabylon_2;
    arenaconfigarenas_type["neobabylon_3"] = &ArenaConfigArenas::neobabylon_3;
    arenaconfigarenas_type["neobabylon_4"] = &ArenaConfigArenas::neobabylon_4;
    arenaconfigarenas_type["neobabylon_5"] = &ArenaConfigArenas::neobabylon_5;
    arenaconfigarenas_type["sunkencity_1"] = &ArenaConfigArenas::sunkencity_1;
    arenaconfigarenas_type["sunkencity_2"] = &ArenaConfigArenas::sunkencity_2;
    arenaconfigarenas_type["sunkencity_3"] = &ArenaConfigArenas::sunkencity_3;
    arenaconfigarenas_type["sunkencity_4"] = &ArenaConfigArenas::sunkencity_4;
    arenaconfigarenas_type["sunkencity_5"] = &ArenaConfigArenas::sunkencity_5;

    /// Used in ArenaState
    auto arenaconfigitems_type = lua.new_usertype<ArenaConfigItems>("ArenaConfigItems");
    arenaconfigitems_type["rock"] = &ArenaConfigItems::rock;
    arenaconfigitems_type["pot"] = &ArenaConfigItems::pot;
    arenaconfigitems_type["bombbag"] = &ArenaConfigItems::bombbag;
    arenaconfigitems_type["bombbox"] = &ArenaConfigItems::bombbox;
    arenaconfigitems_type["ropepile"] = &ArenaConfigItems::ropepile;
    arenaconfigitems_type["pickup_12bag"] = &ArenaConfigItems::pickup_12bag;
    arenaconfigitems_type["pickup_24bag"] = &ArenaConfigItems::pickup_24bag;
    arenaconfigitems_type["cooked_turkey"] = &ArenaConfigItems::cooked_turkey;
    arenaconfigitems_type["royal_jelly"] = &ArenaConfigItems::royal_jelly;
    arenaconfigitems_type["torch"] = &ArenaConfigItems::torch;
    arenaconfigitems_type["boomerang"] = &ArenaConfigItems::boomerang;
    arenaconfigitems_type["machete"] = &ArenaConfigItems::machete;
    arenaconfigitems_type["mattock"] = &ArenaConfigItems::mattock;
    arenaconfigitems_type["crossbow"] = &ArenaConfigItems::crossbow;
    arenaconfigitems_type["webgun"] = &ArenaConfigItems::webgun;
    arenaconfigitems_type["freezeray"] = &ArenaConfigItems::freezeray;
    arenaconfigitems_type["shotgun"] = &ArenaConfigItems::shotgun;
    arenaconfigitems_type["camera"] = &ArenaConfigItems::camera;
    arenaconfigitems_type["plasma_cannon"] = &ArenaConfigItems::plasma_cannon;
    arenaconfigitems_type["wooden_shield"] = &ArenaConfigItems::wooden_shield;
    arenaconfigitems_type["metal_shield"] = &ArenaConfigItems::metal_shield;
    arenaconfigitems_type["teleporter"] = &ArenaConfigItems::teleporter;
    arenaconfigitems_type["mine"] = &ArenaConfigItems::mine;
    arenaconfigitems_type["snaptrap"] = &ArenaConfigItems::snaptrap;
    arenaconfigitems_type["paste"] = &ArenaConfigItems::paste;
    arenaconfigitems_type["climbing_gloves"] = &ArenaConfigItems::climbing_gloves;
    arenaconfigitems_type["pitchers_mitt"] = &ArenaConfigItems::pitchers_mitt;
    arenaconfigitems_type["spike_shoes"] = &ArenaConfigItems::spike_shoes;
    arenaconfigitems_type["spring_shoes"] = &ArenaConfigItems::spring_shoes;
    arenaconfigitems_type["parachute"] = &ArenaConfigItems::parachute;
    arenaconfigitems_type["cape"] = &ArenaConfigItems::cape;
    arenaconfigitems_type["vlads_cape"] = &ArenaConfigItems::vlads_cape;
    arenaconfigitems_type["jetpack"] = &ArenaConfigItems::jetpack;
    arenaconfigitems_type["hoverpack"] = &ArenaConfigItems::hoverpack;
    arenaconfigitems_type["telepack"] = &ArenaConfigItems::telepack;
    arenaconfigitems_type["powerpack"] = &ArenaConfigItems::powerpack;
    arenaconfigitems_type["excalibur"] = &ArenaConfigItems::excalibur;
    arenaconfigitems_type["scepter"] = &ArenaConfigItems::scepter;
    arenaconfigitems_type["kapala"] = &ArenaConfigItems::kapala;
    arenaconfigitems_type["true_crown"] = &ArenaConfigItems::true_crown;

    /// Used in ArenaState
    auto arenaconfigequippeditems_type = lua.new_usertype<ArenaConfigEquippedItems>("ArenaConfigEquippedItems");
    arenaconfigequippeditems_type["paste"] = &ArenaConfigEquippedItems::paste;
    arenaconfigequippeditems_type["climbing_gloves"] = &ArenaConfigEquippedItems::climbing_gloves;
    arenaconfigequippeditems_type["pitchers_mitt"] = &ArenaConfigEquippedItems::pitchers_mitt;
    arenaconfigequippeditems_type["spike_shoes"] = &ArenaConfigEquippedItems::spike_shoes;
    arenaconfigequippeditems_type["spring_shoes"] = &ArenaConfigEquippedItems::spring_shoes;
    arenaconfigequippeditems_type["parachute"] = &ArenaConfigEquippedItems::parachute;
    arenaconfigequippeditems_type["kapala"] = &ArenaConfigEquippedItems::kapala;
    arenaconfigequippeditems_type["scepter"] = &ArenaConfigEquippedItems::scepter;

    /// Used in StateMemory
    auto arenastate_type = lua.new_usertype<ArenaState>("ArenaState");
    arenastate_type["current_arena"] = &ArenaState::current_arena;
    arenastate_type["player_teams"] = &ArenaState::player_teams;
    arenastate_type["format"] = &ArenaState::format;
    arenastate_type["ruleset"] = &ArenaState::ruleset;
    arenastate_type["player_lives"] = &ArenaState::player_lives;
    arenastate_type["player_totalwins"] = &ArenaState::player_totalwins;
    arenastate_type["player_won"] = &ArenaState::player_won;
    arenastate_type["timer"] = &ArenaState::timer;
    arenastate_type["timer_ending"] = &ArenaState::timer_ending;
    arenastate_type["wins"] = &ArenaState::wins;
    arenastate_type["lives"] = &ArenaState::lives;
    arenastate_type["time_to_win"] = &ArenaState::time_to_win;
    arenastate_type["player_idolheld_countdown"] = &ArenaState::player_idolheld_countdown;
    arenastate_type["health"] = &ArenaState::health;
    arenastate_type["bombs"] = &ArenaState::bombs;
    arenastate_type["ropes"] = &ArenaState::ropes;
    arenastate_type["stun_time"] = &ArenaState::stun_time;
    arenastate_type["mount"] = &ArenaState::mount;
    arenastate_type["arena_select"] = &ArenaState::arena_select;
    arenastate_type["arenas"] = &ArenaState::arenas;
    arenastate_type["dark_level_chance"] = &ArenaState::dark_level_chance;
    arenastate_type["crate_frequency"] = &ArenaState::crate_frequency;
    arenastate_type["items_enabled"] = &ArenaState::items_enabled;
    arenastate_type["items_in_crate"] = &ArenaState::items_in_crate;
    arenastate_type["held_item"] = &ArenaState::held_item;
    arenastate_type["equipped_backitem"] = &ArenaState::equipped_backitem;
    arenastate_type["equipped_items"] = &ArenaState::equipped_items;
    arenastate_type["whip_damage"] = &ArenaState::whip_damage;
    arenastate_type["final_ghost"] = &ArenaState::final_ghost;
    arenastate_type["breath_cooldown"] = &ArenaState::breath_cooldown;
    arenastate_type["punish_ball"] = &ArenaState::punish_ball;

    /// Used in StateMemory
    lua.new_usertype<JournalProgressStickerSlot>(
        "JournalProgressStickerSlot",
        "theme",
        &JournalProgressStickerSlot::theme,
        "grid_position",
        &JournalProgressStickerSlot::grid_position,
        "entity_type",
        &JournalProgressStickerSlot::entity_type,
        "x",
        &JournalProgressStickerSlot::x,
        "y",
        &JournalProgressStickerSlot::y,
        "angle",
        &JournalProgressStickerSlot::angle);

    /// Used in StateMemory
    lua.new_usertype<JournalProgressStainSlot>(
        "JournalProgressStainSlot",
        "x",
        &JournalProgressStainSlot::x,
        "y",
        &JournalProgressStainSlot::y,
        "angle",
        &JournalProgressStainSlot::angle,
        "scale",
        &JournalProgressStainSlot::scale,
        "texture_column",
        &JournalProgressStainSlot::texture_column,
        "texture_row",
        &JournalProgressStainSlot::texture_row,
        "texture_range",
        &JournalProgressStainSlot::texture_range);

    /// Used in Items
    lua.new_usertype<SelectPlayerSlot>(
        "SelectPlayerSlot",
        "activated",
        &SelectPlayerSlot::activated,
        "character",
        &SelectPlayerSlot::character,
        "texture",
        &SelectPlayerSlot::texture_id);

    /// Used in StateMemory
    lua.new_usertype<Items>(
        "Items",
        "player_count",
        &Items::player_count,
        "saved_pets_count",
        &Items::saved_pets_count,
        "saved_pets",
        &Items::saved_pets,
        "is_pet_cursed",
        &Items::is_pet_cursed,
        "is_pet_poisoned",
        &Items::is_pet_poisoned,
        "leader",
        // &Items::leader,
        sol::property([](Items& s) -> uint8_t
                      { return s.leader + 1; },
                      [](Items& s, uint8_t leader)
                      { s.leader = leader - 1; }),
        "player_select",
        &Items::player_select_slots,
        "player_inventory",
        &Items::player_inventories,
        "players",
        &Items::players);

    /// Used in LiquidPool
    lua.new_usertype<LiquidPhysicsEngine>(
        "LiquidPhysicsEngine",
        "pause",
        &LiquidPhysicsEngine::pause_physics,
        "gravity",
        &LiquidPhysicsEngine::gravity,
        "cohesion",
        &LiquidPhysicsEngine::cohesion,
        "elasticity",
        &LiquidPhysicsEngine::agitation,
        "size",
        &LiquidPhysicsEngine::blob_size,
        "weight",
        &LiquidPhysicsEngine::weight,
        "count",
        &LiquidPhysicsEngine::entity_count);

    /// Used in LiquidPool
    lua.new_usertype<LiquidPhysicsParams>(
        "LiquidPhysicsParams",
        "gravity",
        &LiquidPhysicsParams::gravity,
        "cohesion",
        &LiquidPhysicsParams::cohesion,
        "elasticity",
        &LiquidPhysicsParams::agitation);

    /// Used in LiquidPhysics
    lua.new_usertype<LiquidPool>(
        "LiquidPool",
        "default",
        &LiquidPool::physics_defaults,
        "engine",
        &LiquidPool::physics_engine);

    /// Use LIQUID_POOL enum for the index<br/>
    /// Used in StateMemory
    lua.new_usertype<LiquidPhysics>(
        "LiquidPhysics",
        "pools",
        &LiquidPhysics::pools);

    lua.create_named_table(
        "LIQUID_POOL",
        "WATER",
        1,
        "COARSE_WATER",
        2,
        "LAVA",
        3,
        "COARSE_LAVA",
        4,
        "STAGNANT_LAVA",
        5);

    /// Can be accessed via global [state](#state)
    auto statememory_type = lua.new_usertype<StateMemory>("StateMemory");
    statememory_type["screen_last"] = &StateMemory::screen_last;
    statememory_type["screen"] = &StateMemory::screen;
    statememory_type["screen_next"] = &StateMemory::screen_next;
    statememory_type["pause"] = &StateMemory::pause;
    statememory_type["width"] = &StateMemory::w;
    statememory_type["height"] = &StateMemory::h;
    statememory_type["kali_favor"] = &StateMemory::kali_favor;
    statememory_type["kali_status"] = &StateMemory::kali_status;
    statememory_type["kali_altars_destroyed"] = &StateMemory::kali_altars_destroyed;
    statememory_type["kali_gifts"] = &StateMemory::kali_gifts;
    statememory_type["seed"] = &StateMemory::seed;
    statememory_type["time_total"] = &StateMemory::time_total;
    statememory_type["world"] = &StateMemory::world;
    statememory_type["world_next"] = &StateMemory::world_next;
    statememory_type["world_start"] = &StateMemory::world_start;
    statememory_type["level"] = &StateMemory::level;
    statememory_type["level_next"] = &StateMemory::level_next;
    statememory_type["level_start"] = &StateMemory::level_start;
    statememory_type["theme"] = &StateMemory::theme;
    statememory_type["theme_next"] = &StateMemory::theme_next;
    statememory_type["theme_start"] = &StateMemory::theme_start;
    statememory_type["current_theme"] = &StateMemory::current_theme;
    statememory_type["force_current_theme"] = &StateMemory::force_current_theme;
    statememory_type["shoppie_aggro"] = &StateMemory::shoppie_aggro;
    statememory_type["shoppie_aggro_next"] = &StateMemory::shoppie_aggro_levels;
    statememory_type["outposts_spawned"] = &StateMemory::outposts_spawned;
    statememory_type["merchant_aggro"] = &StateMemory::merchant_aggro;
    statememory_type["kills_npc"] = &StateMemory::kills_npc;
    statememory_type["level_count"] = &StateMemory::level_count;
    statememory_type["damage_taken"] = &StateMemory::damage_taken;
    statememory_type["journal_flags"] = &StateMemory::journal_flags;
    statememory_type["time_last_level"] = &StateMemory::time_last_level;
    statememory_type["time_level"] = &StateMemory::time_level;
    statememory_type["level_flags"] = &StateMemory::level_flags;
    statememory_type["quest_flags"] = &StateMemory::quest_flags;
    statememory_type["presence_flags"] = &StateMemory::presence_flags;
    statememory_type["loading"] = &StateMemory::loading;
    statememory_type["fade_value"] = &StateMemory::fade_value;
    /// NoDoc
    statememory_type["fadevalue"] = &StateMemory::fade_value;
    statememory_type["fade_timer"] = &StateMemory::fade_timer;
    /// NoDoc
    statememory_type["fadeout"] = &StateMemory::fade_timer;
    statememory_type["fade_length"] = &StateMemory::fade_length;
    /// NoDoc
    statememory_type["fadein"] = &StateMemory::fade_length;
    /// NoDoc
    statememory_type["loading_black_screen_timer"] = &StateMemory::fade_delay;
    statememory_type["fade_delay"] = &StateMemory::fade_delay;
    /// NoDoc
    statememory_type["ingame"] = &StateMemory::ingame;
    statememory_type["fade_enabled"] = &StateMemory::fade_enabled;
    /// NoDoc
    statememory_type["playing"] = &StateMemory::playing;
    statememory_type["fade_circle"] = &StateMemory::fade_circle;
    statememory_type["saved_dogs"] = &StateMemory::saved_dogs;
    statememory_type["saved_cats"] = &StateMemory::saved_cats;
    statememory_type["saved_hamsters"] = &StateMemory::saved_hamsters;
    statememory_type["win_state"] = &StateMemory::win_state;
    statememory_type["illumination"] = &StateMemory::illumination;
    statememory_type["money_last_levels"] = &StateMemory::money_last_levels;
    statememory_type["money_shop_total"] = &StateMemory::money_shop_total;
    statememory_type["player_inputs"] = &StateMemory::player_inputs;
    statememory_type["quests"] = &StateMemory::quests;
    statememory_type["camera"] = &StateMemory::camera;
    statememory_type["special_visibility_flags"] = &StateMemory::special_visibility_flags;
    statememory_type["cause_of_death"] = &StateMemory::cause_of_death;
    statememory_type["cause_of_death_entity_type"] = &StateMemory::cause_of_death_entity_type;
    statememory_type["toast_timer"] = &StateMemory::toast_timer;
    statememory_type["speechbubble_timer"] = &StateMemory::speechbubble_timer;
    statememory_type["speechbubble_owner"] = &StateMemory::speechbubble_owner;
    statememory_type["level_gen"] = &StateMemory::level_gen;
    statememory_type["correct_ushabti"] = &StateMemory::correct_ushabti;
    statememory_type["items"] = &StateMemory::items;
    statememory_type["camera_layer"] = &StateMemory::camera_layer;
    statememory_type["layer_transition_timer"] = &StateMemory::layer_transition_timer;
    statememory_type["transition_to_layer"] = &StateMemory::transition_to_layer;
    statememory_type["screen_character_select"] = &StateMemory::screen_character_select;
    statememory_type["screen_team_select"] = &StateMemory::screen_team_select;
    statememory_type["screen_camp"] = &StateMemory::screen_camp;
    statememory_type["screen_level"] = &StateMemory::screen_level;
    statememory_type["screen_transition"] = &StateMemory::screen_transition;
    statememory_type["screen_death"] = &StateMemory::screen_death;
    statememory_type["screen_win"] = &StateMemory::screen_win;
    statememory_type["screen_credits"] = &StateMemory::screen_credits;
    statememory_type["screen_scores"] = &StateMemory::screen_scores;
    statememory_type["screen_constellation"] = &StateMemory::screen_constellation;
    statememory_type["screen_recap"] = &StateMemory::screen_recap;
    statememory_type["screen_arena_stages_select"] = &StateMemory::screen_arena_stages_select1;
    statememory_type["screen_arena_intro"] = &StateMemory::screen_arena_intro;
    statememory_type["screen_arena_level"] = &StateMemory::screen_arena_level;
    statememory_type["screen_arena_score"] = &StateMemory::screen_arena_score;
    statememory_type["screen_arena_menu"] = &StateMemory::screen_arena_menu;
    statememory_type["screen_arena_items"] = &StateMemory::screen_arena_items;
    statememory_type["get_correct_ushabti"] = &StateMemory::get_correct_ushabti;
    statememory_type["set_correct_ushabti"] = &StateMemory::set_correct_ushabti;
    statememory_type["arena"] = &StateMemory::arena;
    statememory_type["speedrun_character"] = &StateMemory::speedrun_character;
    statememory_type["speedrun_activation_trigger"] = &StateMemory::speedrun_activation_trigger;
    statememory_type["end_spaceship_character"] = &StateMemory::end_spaceship_character;
    statememory_type["world2_coffin_spawned"] = &StateMemory::world2_coffin_spawned;
    statememory_type["world4_coffin_spawned"] = &StateMemory::world4_coffin_spawned;
    statememory_type["world6_coffin_spawned"] = &StateMemory::world6_coffin_spawned;
    statememory_type["first_damage_cause"] = &StateMemory::first_damage_cause;
    statememory_type["first_damage_world"] = &StateMemory::first_damage_world;
    statememory_type["first_damage_level"] = &StateMemory::first_damage_level;
    statememory_type["time_speedrun"] = &StateMemory::time_speedrun;
    statememory_type["coffin_contents"] = &StateMemory::coffin_contents;
    statememory_type["screen_change_counter"] = &StateMemory::screen_change_counter;
    statememory_type["time_startup"] = &StateMemory::time_startup;
    statememory_type["storage_uid"] = &StateMemory::waddler_floor_storage;
    statememory_type["waddler_storage"] = &StateMemory::waddler_storage;
    statememory_type["waddler_metadata"] = &StateMemory::waddler_storage_meta;
    statememory_type["journal_progress_sticker_count"] = &StateMemory::journal_progress_sticker_count;
    statememory_type["journal_progress_sticker_slots"] = &StateMemory::journal_progress_sticker_slots;
    statememory_type["journal_progress_stain_count"] = &StateMemory::journal_progress_stain_count;
    statememory_type["journal_progress_stain_slots"] = &StateMemory::journal_progress_stain_slots;
    statememory_type["journal_progress_theme_count"] = &StateMemory::journal_progress_theme_count;
    statememory_type["journal_progress_theme_slots"] = &StateMemory::journal_progress_theme_slots;
    statememory_type["theme_info"] = &StateMemory::current_theme;
    statememory_type["logic"] = &StateMemory::logic;
    statememory_type["liquid"] = &StateMemory::liquid_physics;
    statememory_type["next_entity_uid"] = &StateMemory::next_entity_uid;
    statememory_type["room_owners"] = &StateMemory::room_owners;

    auto state_get_user_data = [](StateMemory& state) -> sol::object
    {
        auto backend = LuaBackend::get_calling_backend();
        auto local_datas = backend->local_state_datas;
        if (local_datas.contains(&state))
        {
            return local_datas[&state].user_data;
        }
        return sol::nil;
    };

    auto state_set_user_data = [](StateMemory& state, sol::object user_data) -> void
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->local_state_datas[&state].user_data = user_data;
    };
    auto user_data = sol::property(state_get_user_data, state_set_user_data);

    statememory_type["user_data"] = std::move(user_data);
    /* StateMemory
    // user_data
    // You can store a table (or lua primitive) here and it will store data correctly in online multiplayer, by having a different copy on each state and being copied over when the game does.
    // Doesn't support recursive tables / cyclic references. Metatables will be transferred by reference instead of being copied
    */

    lua.create_named_table("FADE", "NONE", 0, "OUT", 1, "LOAD", 2, "IN", 3);

    lua.create_named_table("QUEST_FLAG", "RESET", 1, "DARK_LEVEL_SPAWNED", 2, "VAULT_SPAWNED", 3, "SPAWN_OUTPOST", 4, "SHOP_SPAWNED", 5, "SHORTCUT_USED", 6, "SEEDED", 7, "DAILY", 8, "CAVEMAN_SHOPPIE_AGGROED", 9, "WADDLER_AGGROED", 10, "SHOP_BOUGHT_OUT", 11, "EGGPLANT_CROWN_PICKED_UP", 12, "UDJAT_EYE_SPAWNED", 17, "BLACK_MARKET_SPAWNED", 18, "DRILL_SPAWNED", 19, "MOON_CHALLENGE_SPAWNED", 25, "STAR_CHALLENGE_SPAWNED", 26, "SUN_CHALLENGE_SPAWNED", 27);

    lua.create_named_table("PRESENCE_FLAG", "UDJAT_EYE", 1, "BLACK_MARKET", 2, "VLADS_CASTLE", 3, "DRILL", 3, "MOON_CHALLENGE", 9, "STAR_CHALLENGE", 10, "SUN_CHALLENGE", 11);

    lua.create_named_table("JOURNAL_FLAG", "PACIFIST", 1, "VEGAN", 2, "VEGETARIAN", 3, "PETTY_CRIMINAL", 4, "WANTED_CRIMINAL", 5, "CRIME_LORD", 6, "KING", 7, "QUEEN", 8, "FOOL", 9, "EGGPLANT", 10, "NO_GOLD", 11, "LIKED_PETS", 12, "LOVED_PETS", 13, "TOOK_DAMAGE", 14, "ANKH", 15, "KINGU", 16, "OSIRIS", 17, "TIAMAT", 18, "HUNDUN", 19, "COSMOS", 20, "DIED", 21);

    /// Used in Illumination
    lua.new_usertype<LightParams>(
        "LightParams",
        "red",
        &LightParams::red,
        "green",
        &LightParams::green,
        "blue",
        &LightParams::blue,
        "size",
        &LightParams::size,
        "as_color",
        &LightParams::as_color);

    /// Generic object for lights in the game, you can make your own with [create_illumination](#create_illumination)<br/>
    /// Used in StateMemory, Player, PlayerGhost, BurningRopeEffect ...
    auto illumination_type = lua.new_usertype<Illumination>("Illumination");
    illumination_type["lights"] = &Illumination::lights;
    illumination_type["light1"] = &Illumination::light1;
    illumination_type["light2"] = &Illumination::light2;
    illumination_type["light3"] = &Illumination::light3;
    illumination_type["light4"] = &Illumination::light4;
    illumination_type["brightness"] = &Illumination::brightness;
    illumination_type["brightness_multiplier"] = &Illumination::brightness_multiplier;
    illumination_type["light_pos_x"] = &Illumination::light_pos_x;
    illumination_type["light_pos_y"] = &Illumination::light_pos_y;
    illumination_type["offset_x"] = &Illumination::offset_x;
    illumination_type["offset_y"] = &Illumination::offset_y;
    illumination_type["distortion"] = &Illumination::distortion;
    illumination_type["entity_uid"] = &Illumination::entity_uid;
    illumination_type["timer"] = &Illumination::timer;
    illumination_type["flags"] = &Illumination::flags;
    illumination_type["type_flags"] = &Illumination::type_flags;
    illumination_type["enabled"] = &Illumination::enabled;
    illumination_type["layer"] = &Illumination::layer;

    lua.create_named_table("LIGHT_TYPE", "NONE", LIGHT_TYPE::NONE, "FOLLOW_CAMERA", LIGHT_TYPE::FOLLOW_CAMERA, "FOLLOW_ENTITY", LIGHT_TYPE::FOLLOW_ENTITY, "ROOM_LIGHT", LIGHT_TYPE::ROOM_LIGHT);

    /* LIGHT_TYPE
    // NONE
    // Normal static light, position can be edited to move it around
    // FOLLOW_CAMERA
    // Position is updated to the camera position, can be moved around via offset
    // FOLLOW_ENTITY
    // Position is updated to the entity position (from the uid field), if the uid is not found it will behave as LIGHT_TYPE.NONE, can be moved around via offset
    // ROOM_LIGHT
    // Rectangle, full brightness always uses light1, disabling light1 does nothing
    */

    auto camera_type = lua.new_usertype<Camera>("Camera");
    camera_type["bounds_left"] = &Camera::bounds_left;
    camera_type["bounds_right"] = &Camera::bounds_right;
    camera_type["bounds_bottom"] = &Camera::bounds_bottom;
    camera_type["bounds_top"] = &Camera::bounds_top;
    camera_type["calculated_focus_x"] = &Camera::calculated_focus_x;
    camera_type["calculated_focus_y"] = &Camera::calculated_focus_y;
    camera_type["adjusted_focus_x"] = &Camera::adjusted_focus_x;
    camera_type["adjusted_focus_y"] = &Camera::adjusted_focus_y;
    camera_type["focus_offset_x"] = &Camera::focus_offset_x;
    camera_type["focus_offset_y"] = &Camera::focus_offset_y;
    camera_type["focus_x"] = &Camera::focus_x;
    camera_type["focus_y"] = &Camera::focus_y;
    camera_type["vertical_pan"] = &Camera::vertical_pan;
    camera_type["shake_countdown_start"] = &Camera::shake_countdown_start;
    camera_type["shake_countdown"] = &Camera::shake_countdown;
    camera_type["shake_amplitude"] = &Camera::shake_amplitude;
    camera_type["shake_multiplier_x"] = &Camera::shake_multiplier_x;
    camera_type["shake_multiplier_y"] = &Camera::shake_multiplier_y;
    camera_type["uniform_shake"] = &Camera::uniform_shake;
    camera_type["focused_entity_uid"] = &Camera::focused_entity_uid;
    camera_type["inertia"] = &Camera::inertia;
    camera_type["peek_timer"] = &Camera::peek_timer;
    camera_type["peek_layer"] = &Camera::peek_layer;
    camera_type["get_bounds"] = &Camera::get_bounds;
    camera_type["set_bounds"] = &Camera::set_bounds;

    /// Can be accessed via global [online](#online)
    lua.new_usertype<Online>(
        "Online",
        "online_players",
        &Online::online_players,
        "local_player",
        &Online::local_player,
        "lobby",
        &Online::lobby);
    /// Used in Online
    lua.new_usertype<OnlinePlayer>(
        "OnlinePlayer",
        "ready_state",
        sol::readonly(&OnlinePlayer::ready_state),
        "character",
        &OnlinePlayer::character,
        "player_name",
        sol::readonly(&OnlinePlayer::player_name));
    /// Used in Online
    lua.new_usertype<OnlineLobby>(
        "OnlineLobby",
        "code",
        &OnlineLobby::code,
        "local_player_slot",
        sol::property([](OnlineLobby& ol) // -> uint8_t
                      { return ol.local_player_slot + 1; },
                      [](OnlineLobby& ol, int8_t val)
                      {
                          ol.local_player_slot = val - 1;
                      }),
        "get_code",
        &OnlineLobby::get_code);

    /// Used in StateMemory
    lua.new_usertype<RoomOwnersInfo>("RoomOwnersInfo", "owned_items", &RoomOwnersInfo::owned_items, "owned_rooms", &RoomOwnersInfo::owned_rooms);

    /// Used in RoomOwnersInfo
    lua.new_usertype<ItemOwnerDetails>("ItemOwnerDetails", "owner_type", &ItemOwnerDetails::owner_type, "owner_uid", &ItemOwnerDetails::owner_uid);

    /// Used in RoomOwnersInfo
    lua.new_usertype<RoomOwnerDetails>("RoomOwnerDetails", "layer", &RoomOwnerDetails::layer, "room_index", &RoomOwnerDetails::room_index, "owner_uid", &RoomOwnerDetails::owner_uid);

    lua.create_named_table("CAUSE_OF_DEATH", "DEATH", 0, "ENTITY", 1, "LONG_FALL", 2, "STILL_FALLING", 3, "MISSED", 4, "POISONED", 5);

    lua["toast_visible"] = []() -> bool
    {
        return State::get().ptr()->toast != 0;
    };

    lua["speechbubble_visible"] = []() -> bool
    {
        return State::get().ptr()->speechbubble != 0;
    };

    lua["cancel_toast"] = []()
    {
        State::get().ptr()->toast_timer = 1000;
    };

    lua["cancel_speechbubble"] = []()
    {
        State::get().ptr()->speechbubble_timer = 1000;
    };

    /// Save current level state to slot 1..4. These save states are invalid and cleared after you exit the current level, but can be used to rollback to an earlier state in the same level. You probably definitely shouldn't use save state functions during an update, and sync them to the same event outside an update (i.e. GUIFRAME, POST_UPDATE). These slots are already allocated by the game, actually used for online rollback, and use no additional memory. Also see SaveState if you need more.
    lua["save_state"] = [](int slot)
    {
        if (slot >= 1 && slot <= 4)
        {
            copy_save_slot(5, slot);
        }
    };

    /// Load level state from slot 1..4, if a save_state was made in this level.
    lua["load_state"] = [](int slot)
    {
        if (slot >= 1 && slot <= 4 && get_save_state(slot))
            copy_save_slot(slot, 5);
    };

    /// Clear save state from slot 1..4.
    lua["clear_state"] = [](int slot)
    {
        if (slot >= 1 && slot <= 4 && get_save_state(slot))
            get_save_state(slot)->screen = 0;
    };

    /// Get StateMemory from a save_state slot.
    lua["get_save_state"] = [](int slot) -> StateMemory*
    {
        if (slot >= 1 && slot <= 5)
            return get_save_state(slot);
        return nullptr;
    };

    lua.new_usertype<SaveState>("SaveState", sol::constructors<SaveState()>(), "load", &SaveState::load, "save", &SaveState::save, "clear", &SaveState::clear, "get_state", &SaveState::get_state);
}
}; // namespace NState
