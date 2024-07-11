#include "logic_lua.hpp"

#include <cstdint>
#include <sol/sol.hpp>      // for data_t, global_table, state, proxy...
#include <sol/usertype.hpp> // for basic_usertype

#include "entity.hpp"
#include "sound_manager.hpp"
#include "state.hpp"
#include "state_structs.hpp"

namespace NLogic
{
void register_usertypes(sol::state& lua)
{

    auto stop_logic = sol::overload(
        static_cast<void (LogicList::*)(LOGIC)>(&LogicList::stop_logic),
        static_cast<void (LogicList::*)(Logic*)>(&LogicList::stop_logic));

    lua.create_named_table("LOGIC", //
                           "TUTORIAL",
                           LOGIC::TUTORIAL,
                           "OUROBOROS",
                           LOGIC::OUROBOROS,
                           "SPEEDRUN",
                           LOGIC::SPEEDRUN,
                           "GHOST",
                           LOGIC::GHOST,
                           "GHOST_TOAST",
                           LOGIC::GHOST_TOAST,
                           "TUN_AGGRO",
                           LOGIC::TUN_AGGRO,
                           "DICESHOP",
                           LOGIC::DICESHOP,
                           "PRE_CHALLENGE",
                           LOGIC::PRE_CHALLENGE,
                           "MOON_CHALLENGE",
                           LOGIC::MOON_CHALLENGE,
                           "STAR_CHALLENGE",
                           LOGIC::STAR_CHALLENGE,
                           "SUN_CHALLENGE",
                           LOGIC::SUN_CHALLENGE,
                           "MAGMAMAN_SPAWN",
                           LOGIC::MAGMAMAN_SPAWN,
                           "WATER_BUBBLES",
                           LOGIC::WATER_BUBBLES,
                           "OLMEC_CUTSCENE",
                           LOGIC::OLMEC_CUTSCENE,
                           "TIAMAT_CUTSCENE",
                           LOGIC::TIAMAT_CUTSCENE,
                           "APEP",
                           LOGIC::APEP,
                           "COG_SACRIFICE",
                           LOGIC::COG_SACRIFICE,
                           "DUAT_BOSSES",
                           LOGIC::DUAT_BOSSES,
                           "BUBBLER",
                           LOGIC::BUBBLER,
                           "PLEASURE_PALACE",
                           LOGIC::PLEASURE_PALACE,
                           "DISCOVERY_INFO",
                           LOGIC::DISCOVERY_INFO,
                           "BLACK_MARKET",
                           LOGIC::BLACK_MARKET,
                           "JELLYFISH",
                           LOGIC::JELLYFISH,
                           "ARENA_1",
                           LOGIC::ARENA_1,
                           "ARENA_2",
                           LOGIC::ARENA_2,
                           "ARENA_3",
                           LOGIC::ARENA_3,
                           "ARENA_ALIEN_BLAST",
                           LOGIC::ARENA_ALIEN_BLAST,
                           "ARENA_LOOSE_BOMBS",
                           LOGIC::ARENA_LOOSE_BOMBS);

    auto start_logic = [&lua](LogicList& l, LOGIC idx) -> sol::object
    {
        auto return_logic = l.start_logic(idx);
        switch (idx)
        {
        case LOGIC::TUTORIAL:
            return sol::make_object(lua, (LogicTutorial*)return_logic);
        case LOGIC::OUROBOROS:
            return sol::make_object(lua, (LogicOuroboros*)return_logic);
        case LOGIC::SPEEDRUN:
            return sol::make_object(lua, (LogicBasecampSpeedrun*)return_logic);
        case LOGIC::GHOST_TOAST:
            return sol::make_object(lua, (LogicGhostToast*)return_logic);
        case LOGIC::DICESHOP:
            return sol::make_object(lua, (LogicDiceShop*)return_logic);
        case LOGIC::PRE_CHALLENGE:
            return sol::make_object(lua, (LogicTunPreChallenge*)return_logic);
        case LOGIC::MOON_CHALLENGE:
            return sol::make_object(lua, (LogicMoonChallenge*)return_logic);
        case LOGIC::STAR_CHALLENGE:
            return sol::make_object(lua, (LogicStarChallenge*)return_logic);
        case LOGIC::SUN_CHALLENGE:
            return sol::make_object(lua, (LogicSunChallenge*)return_logic);
        case LOGIC::MAGMAMAN_SPAWN:
            return sol::make_object(lua, (LogicMagmamanSpawn*)return_logic);
        case LOGIC::WATER_BUBBLES:
            return sol::make_object(lua, (LogicUnderwaterBubbles*)return_logic);
        case LOGIC::OLMEC_CUTSCENE:
            return sol::make_object(lua, (LogicOlmecCutscene*)return_logic);
        case LOGIC::TIAMAT_CUTSCENE:
            return sol::make_object(lua, (LogicTiamatCutscene*)return_logic);
        case LOGIC::APEP:
            return sol::make_object(lua, (LogicApepTrigger*)return_logic);
        case LOGIC::COG_SACRIFICE:
            return sol::make_object(lua, (LogicCOGAnkhSacrifice*)return_logic);
        case LOGIC::BUBBLER:
            return sol::make_object(lua, (LogicTiamatBubbles*)return_logic);
        case LOGIC::PLEASURE_PALACE:
            return sol::make_object(lua, (LogicTuskPleasurePalace*)return_logic);
        case LOGIC::ARENA_1:
            return sol::make_object(lua, (LogicArena1*)return_logic);
        // case LOGIC::ARENA_2:
        //     return sol::make_object(lua, () return_logic);
        case LOGIC::ARENA_ALIEN_BLAST:
            return sol::make_object(lua, (LogicArenaAlienBlast*)return_logic);
        case LOGIC::ARENA_LOOSE_BOMBS:
            return sol::make_object(lua, (LogicArenaLooseBombs*)return_logic);
        default:
            return sol::make_object(lua, return_logic);
        }
    };

    /// Used in StateMemory
    lua.new_usertype<LogicList>(
        "LogicList",
        "tutorial",
        &LogicList::tutorial,
        "ouroboros",
        &LogicList::ouroboros,
        "basecamp_speedrun",
        &LogicList::basecamp_speedrun,
        "ghost_trigger",
        &LogicList::ghost_trigger,
        "ghost_toast_trigger",
        &LogicList::ghost_toast_trigger,
        "tun_aggro",
        &LogicList::tun_aggro,
        "diceshop",
        &LogicList::diceshop,
        "tun_pre_challenge",
        &LogicList::tun_pre_challenge,
        "tun_moon_challenge",
        &LogicList::tun_moon_challenge,
        "tun_star_challenge",
        &LogicList::tun_star_challenge,
        "tun_sun_challenge",
        &LogicList::tun_sun_challenge,
        "magmaman_spawn",
        &LogicList::magmaman_spawn,
        "water_bubbles",
        &LogicList::water_bubbles,
        "olmec_cutscene",
        &LogicList::olmec_cutscene,
        "tiamat_cutscene",
        &LogicList::tiamat_cutscene,
        "apep_spawner",
        &LogicList::apep_spawner,
        "city_of_gold_ankh_sacrifice",
        &LogicList::city_of_gold_ankh_sacrifice,
        "duat_bosses_spawner",
        &LogicList::duat_bosses_spawner,
        "bubbler",
        &LogicList::bubbler,
        "tusk_pleasure_palace",
        &LogicList::tusk_pleasure_palace,
        "discovery_info",
        &LogicList::discovery_info,
        "black_market",
        &LogicList::black_market,
        "jellyfish_trigger",
        &LogicList::jellyfish_trigger,
        "arena_1",
        &LogicList::arena_1,
        "arena_2",
        &LogicList::arena_2,
        "arena_3",
        &LogicList::arena_3,
        "arena_alien_blast",
        &LogicList::arena_alien_blast,
        "arena_loose_bombs",
        &LogicList::arena_loose_bombs,
        "start_logic",
        start_logic,
        "stop_logic",
        stop_logic);
    /// Used in LogicList
    lua.new_usertype<Logic>(
        "Logic",
        "logic_index",
        sol::readonly(&Logic::logic_index));
    /// Used in LogicList
    lua.new_usertype<LogicTutorial>(
        "LogicTutorial",
        "pet_tutorial",
        &LogicTutorial::pet_tutorial,
        "timer",
        &LogicTutorial::timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicOuroboros>(
        "LogicOuroboros",
        "sound",
        &LogicOuroboros::sound,
        "timer",
        &LogicOuroboros::timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicBasecampSpeedrun>(
        "LogicBasecampSpeedrun",
        "administrator",
        &LogicBasecampSpeedrun::administrator,
        "crate",
        &LogicBasecampSpeedrun::crate,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicGhostToast>(
        "LogicGhostToast",
        "toast_timer",
        &LogicGhostToast::toast_timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    auto logicdiceshop_type = lua.new_usertype<LogicDiceShop>("LogicDiceShop", sol::base_classes, sol::bases<Logic>());
    logicdiceshop_type["boss_uid"] = &LogicDiceShop::boss_uid;
    logicdiceshop_type["boss_type"] = &LogicDiceShop::boss_type;
    logicdiceshop_type["bet_machine"] = &LogicDiceShop::bet_machine;
    logicdiceshop_type["die1"] = &LogicDiceShop::die1;
    logicdiceshop_type["die2"] = &LogicDiceShop::die2;
    logicdiceshop_type["die_1_value"] = &LogicDiceShop::die_1_value;
    logicdiceshop_type["die_2_value"] = &LogicDiceShop::die_2_value;
    logicdiceshop_type["prize_dispenser"] = &LogicDiceShop::prize_dispenser;
    logicdiceshop_type["prize"] = &LogicDiceShop::prize;
    logicdiceshop_type["forcefield"] = &LogicDiceShop::forcefield;
    logicdiceshop_type["bet_active"] = &LogicDiceShop::bet_active;
    logicdiceshop_type["forcefield_deactivated"] = &LogicDiceShop::forcefield_deactivated;
    /// NoDoc
    logicdiceshop_type["boss_angry"] = &LogicDiceShop::unknown;
    logicdiceshop_type["result_announcement_timer"] = &LogicDiceShop::result_announcement_timer;
    logicdiceshop_type["won_prizes_count"] = &LogicDiceShop::won_prizes_count;
    logicdiceshop_type["balance"] = &LogicDiceShop::balance;
    /// Used in LogicList
    lua.new_usertype<LogicTunPreChallenge>(
        "LogicTunPreChallenge",
        "tun_uid",
        &LogicTunPreChallenge::tun_uid,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicMoonChallenge, LogicStarChallenge, LogicSunChallenge
    lua.new_usertype<LogicChallenge>(
        "LogicChallenge",
        "floor_challenge_entrance_uid",
        &LogicChallenge::floor_challenge_entrance_uid,
        "floor_challenge_waitroom_uid",
        &LogicChallenge::floor_challenge_waitroom_uid,
        "challenge_active",
        &LogicChallenge::challenge_active,
        "forcefield_countdown",
        &LogicChallenge::forcefield_countdown,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicMoonChallenge>(
        "LogicMoonChallenge",
        "mattock_uid",
        &LogicMoonChallenge::mattock_uid,
        sol::base_classes,
        sol::bases<Logic, LogicChallenge>());
    /// Used in LogicList
    lua.new_usertype<LogicStarChallenge>(
        "LogicStarChallenge",
        "torches",
        &LogicStarChallenge::torches,
        "start_countdown",
        &LogicStarChallenge::start_countdown,
        sol::base_classes,
        sol::bases<Logic, LogicChallenge>());
    /// Used in LogicList
    lua.new_usertype<LogicSunChallenge>(
        "LogicSunChallenge",
        "start_countdown",
        &LogicSunChallenge::start_countdown,
        sol::base_classes,
        sol::bases<Logic, LogicChallenge>());
    auto add_spawn = sol::overload(
        static_cast<void (LogicMagmamanSpawn::*)(uint32_t, uint32_t)>(&LogicMagmamanSpawn::add_spawn),
        static_cast<void (LogicMagmamanSpawn::*)(MagmamanSpawnPosition)>(&LogicMagmamanSpawn::add_spawn));
    auto remove_spawn = sol::overload(
        static_cast<void (LogicMagmamanSpawn::*)(uint32_t, uint32_t)>(&LogicMagmamanSpawn::remove_spawn),
        static_cast<void (LogicMagmamanSpawn::*)(MagmamanSpawnPosition)>(&LogicMagmamanSpawn::remove_spawn));
    /// Used in LogicList
    lua.new_usertype<LogicMagmamanSpawn>(
        "LogicMagmamanSpawn",
        "magmaman_positions",
        &LogicMagmamanSpawn::magmaman_positions,
        "add_spawn",
        add_spawn,
        "remove_spawn",
        remove_spawn,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicUnderwaterBubbles>(
        "LogicUnderwaterBubbles",
        "gravity_direction",
        &LogicUnderwaterBubbles::gravity_direction,
        "droplets_spawn_chance",
        &LogicUnderwaterBubbles::droplets_spawn_chance,
        "droplets_enabled",
        &LogicUnderwaterBubbles::droplets_enabled,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicOlmecCutscene>(
        "LogicOlmecCutscene",
        "fx_olmecpart_large",
        &LogicOlmecCutscene::fx_olmecpart_large,
        "olmec",
        &LogicOlmecCutscene::olmec,
        "player",
        &LogicOlmecCutscene::player,
        "cinematic_anchor",
        &LogicOlmecCutscene::cinematic_anchor,
        "timer",
        &LogicOlmecCutscene::timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicTiamatCutscene>(
        "LogicTiamatCutscene",
        "tiamat",
        &LogicTiamatCutscene::tiamat,
        "player",
        &LogicTiamatCutscene::player,
        "cinematic_anchor",
        &LogicTiamatCutscene::cinematic_anchor,
        "timer",
        &LogicTiamatCutscene::timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicApepTrigger>(
        "LogicApepTrigger",
        "spawn_cooldown",
        &LogicApepTrigger::spawn_cooldown,
        "cooling_down",
        &LogicApepTrigger::cooling_down,
        "apep_journal_entry_logged",
        &LogicApepTrigger::apep_journal_entry_logged,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicCOGAnkhSacrifice>(
        "LogicCOGAnkhSacrifice",
        "timer",
        &LogicCOGAnkhSacrifice::timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicTiamatBubbles>(
        "LogicTiamatBubbles",
        "bubble_spawn_timer",
        &LogicTiamatBubbles::bubble_spawn_timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicTuskPleasurePalace>(
        "LogicTuskPleasurePalace",
        "locked_door",
        &LogicTuskPleasurePalace::locked_door,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicArena1>(
        "LogicArena1",
        "crate_spawn_timer",
        &LogicArena1::crate_spawn_timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicArenaAlienBlast>(
        "LogicArenaAlienBlast",
        "timer",
        &LogicArenaAlienBlast::timer,
        sol::base_classes,
        sol::bases<Logic>());
    /// Used in LogicList
    lua.new_usertype<LogicArenaLooseBombs>(
        "LogicArenaLooseBombs",
        "timer",
        &LogicArenaLooseBombs::timer,
        sol::base_classes,
        sol::bases<Logic>());

    /// Used in LogicMagmamanSpawn
    lua.new_usertype<MagmamanSpawnPosition>(
        "MagmamanSpawnPosition",
        sol::constructors<MagmamanSpawnPosition(uint32_t, uint32_t)>{},
        "x",
        &MagmamanSpawnPosition::x,
        "y",
        &MagmamanSpawnPosition::y,
        "timer",
        &MagmamanSpawnPosition::timer);
}
} // namespace NLogic
