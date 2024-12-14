#include "lua_console.hpp"

#include <array>        // for array, _Array_const_iterator
#include <compare>      // for operator<
#include <cstdlib>      // for exit, free
#include <cstring>      // for memchr, memset
#include <ctype.h>      // for isalnum
#include <deque>        // for _Deque_iterator, deque, deque<>::i...
#include <exception>    // for exception
#include <fmt/format.h> // for check_format_string, format, vformat
#include <iterator>     // for back_insert_iterator, back_inserter
#include <limits>       // for numeric_limits
#include <map>          // for map, _Tree_iterator, _Tree_const_i...
#include <new>          // for operator new
#include <regex>        // for regex, regex_replace, regex_search
#include <set>          // for set
#include <sol/sol.hpp>  // for basic_userdata, basic_table_core
#include <tuple>        // for get
#include <utility>      // for min, max, tuple_element<>::type

#include "lua_libs/lua_libs.hpp"  // for require_serpent_lua
#include "lua_vm.hpp"             // for execute_lua, expose_unsafe_libraries
#include "script/lua_backend.hpp" // for LuaBackend
#include "util.hpp"               // for ON_SCOPE_EXIT

class SoundManager;

LuaConsole::LuaConsole(SoundManager* soundmanager)
    : LockableLuaBackend<LuaConsole>(soundmanager, this)
{
    lua["__script_id"] = "console_proxy.lua";

    expose_unsafe_libraries(lua);

    // THIS LIST IS AUTO GENERATED
    // To recreate it, run the entity_casting.py script
    // It will dump the list to stdout, and overwrite docs/entities-hierarchy.md
    entity_down_cast_map =
        {
            {"BoneBlock", "as_movable"},
            {"Boulder", "as_movable"},
            {"Movable", "as_entity"},
            {"ChainedPushBlock", "as_pushblock"},
            {"TrapPart", "as_movable"},
            {"CrushTrap", "as_movable"},
            {"Drill", "as_movable"},
            {"Elevator", "as_movable"},
            {"FallingPlatform", "as_movable"},
            {"ClamBase", "as_movable"},
            {"LightArrowPlatform", "as_movable"},
            {"Olmec", "as_movable"},
            {"PushBlock", "as_movable"},
            {"RegenBlock", "as_movable"},
            {"ThinIce", "as_movable"},
            {"TimedPowderkeg", "as_pushblock"},
            {"CrushElevator", "as_movable"},
            {"UnchainedSpikeBall", "as_movable"},
            {"WoodenlogTrap", "as_movable"},
            {"BGSurfaceStar", "as_entity"},
            {"BGFloatingDebris", "as_bgsurfacelayer"},
            {"BGBackLayerDoor", "as_entity"},
            {"BGSurfaceLayer", "as_bgrelativeelement"},
            {"BGEggshipRoom", "as_entity"},
            {"BGShopEntrence", "as_entity"},
            {"BGMovingStar", "as_bgsurfacestar"},
            {"BGRelativeElement", "as_entity"},
            {"BGShootingStar", "as_bgrelativeelement"},
            {"BGTutorialSign", "as_entity"},
            {"BGShopKeeperPrime", "as_entity"},
            {"Player", "as_powerupcapable"},
            {"CrossBeam", "as_entity"},
            {"DestructibleBG", "as_entity"},
            {"PalaceSign", "as_entity"},
            {"DecoRegeneratingBlock", "as_entity"},
            {"Floor", "as_entity"},
            {"PoleDeco", "as_floor"},
            {"Altar", "as_floor"},
            {"Arrowtrap", "as_floor"},
            {"BigSpearTrap", "as_floor"},
            {"ForceField", "as_floor"},
            {"ConveyorBelt", "as_transferfloor"},
            {"CityOfGoldDoor", "as_decorateddoor"},
            {"DecoratedDoor", "as_exitdoor"},
            {"EggShipDoorS", "as_eggshipdoor"},
            {"EggShipDoor", "as_door"},
            {"Door", "as_floor"},
            {"ExitDoor", "as_door"},
            {"LockedDoor", "as_door"},
            {"MainExit", "as_exitdoor"},
            {"Generator", "as_floor"},
            {"HorizontalForceField", "as_floor"},
            {"JungleSpearTrap", "as_floor"},
            {"LaserTrap", "as_floor"},
            {"TotemTrap", "as_floor"},
            {"MotherStatue", "as_floor"},
            {"Pipe", "as_floor"},
            {"QuickSand", "as_floor"},
            {"SlidingWallCeiling", "as_floor"},
            {"SparkTrap", "as_floor"},
            {"SpikeballTrap", "as_floor"},
            {"StickyTrap", "as_floor"},
            {"TeleportingBorder", "as_floor"},
            {"TentacleBottom", "as_floor"},
            {"TimedForceField", "as_forcefield"},
            {"FxAlienBlast", "as_movable"},
            {"FxAnkhBrokenPiece", "as_movable"},
            {"FxAnkhRotatingSpark", "as_movable"},
            {"Birdies", "as_movable"},
            {"Button", "as_movable"},
            {"FxCompass", "as_movable"},
            {"FxFireflyLight", "as_movable"},
            {"MovingIcon", "as_movable"},
            {"EggshipCenterJetFlame", "as_movable"},
            {"FxEmpress", "as_movable"},
            {"Explosion", "as_movable"},
            {"FxHundunNeckPiece", "as_movable"},
            {"FxJetpackFlame", "as_movable"},
            {"FxKinguSliding", "as_movable"},
            {"FxLamassuAttack", "as_movable"},
            {"LiquidSurface", "as_movable"},
            {"FxMainExitDoor", "as_movable"},
            {"MegaJellyfishEye", "as_movable"},
            {"FxJellyfishStar", "as_movable"},
            {"MiniGameShipOffset", "as_movable"},
            {"FxNecromancerANKH", "as_movable"},
            {"OlmecFloater", "as_movable"},
            {"FxOuroboroDragonPart", "as_movable"},
            {"FxOuroboroOccluder", "as_movable"},
            {"FxPickupEffect", "as_movable"},
            {"FxPlayerIndicator", "as_movable"},
            {"FxQuickSand", "as_movable"},
            {"FxSaleContainer", "as_movable"},
            {"FxShotgunBlast", "as_movable"},
            {"SleepBubble", "as_movable"},
            {"Flame", "as_movable"},
            {"FxSorceressAttack", "as_movable"},
            {"FxSparkSmall", "as_movable"},
            {"FxSpringtrapRing", "as_movable"},
            {"LightEmitter", "as_movable"},
            {"FxTiamatHead", "as_movable"},
            {"FxTiamatTail", "as_movable"},
            {"FxTiamatTorso", "as_movable"},
            {"FxTornJournalPage", "as_movable"},
            {"FxUnderwaterBubble", "as_movable"},
            {"FxVatBubble", "as_movable"},
            {"FxWaterDrop", "as_movable"},
            {"FxWebbedEffect", "as_movable"},
            {"FxWitchdoctorHint", "as_movable"},
            {"Container", "as_movable"},
            {"Coffin", "as_movable"},
            {"WallTorch", "as_torch"},
            {"AxolotlShot", "as_projectile"},
            {"Spear", "as_movable"},
            {"Bomb", "as_movable"},
            {"Boombox", "as_movable"},
            {"Boomerang", "as_purchasable"},
            {"Bullet", "as_projectile"},
            {"Gun", "as_purchasable"},
            {"YellowCape", "as_cape"},
            {"Chain", "as_movable"},
            {"Chest", "as_movable"},
            {"ClimbableRope", "as_movable"},
            {"CloneGunShot", "as_lightshot"},
            {"CookFire", "as_torch"},
            {"AcidBubble", "as_movable"},
            {"Claw", "as_movable"},
            {"StretchChain", "as_movable"},
            {"Bow", "as_movable"},
            {"CursedPot", "as_movable"},
            {"PrizeDispenser", "as_movable"},
            {"EggSac", "as_movable"},
            {"OlmecShip", "as_movable"},
            {"TreasureHook", "as_movable"},
            {"EmpressGrave", "as_movable"},
            {"HundunChest", "as_treasure"},
            {"Treasure", "as_movable"},
            {"Excalibur", "as_movable"},
            {"Fireball", "as_soundshot"},
            {"Orb", "as_movable"},
            {"Fly", "as_movable"},
            {"TimedShot", "as_lightshot"},
            {"FrozenLiquid", "as_movable"},
            {"Present", "as_purchasable"},
            {"GiantClamTop", "as_movable"},
            {"FlyHead", "as_movable"},
            {"Projectile", "as_movable"},
            {"Goldbar", "as_movable"},
            {"Coin", "as_movable"},
            {"HangAnchor", "as_movable"},
            {"HangStrand", "as_movable"},
            {"Honey", "as_movable"},
            {"LaserBeam", "as_movable"},
            {"Hoverpack", "as_backpack"},
            {"Idol", "as_movable"},
            {"Jetpack", "as_backpack"},
            {"JungleSpearCosmetic", "as_movable"},
            {"SoundShot", "as_lightshot"},
            {"Torch", "as_movable"},
            {"LampFlame", "as_flame"},
            {"Landmine", "as_lightemitter"},
            {"LightShot", "as_projectile"},
            {"Leaf", "as_movable"},
            {"LightArrow", "as_arrow"},
            {"Purchasable", "as_movable"},
            {"Mattock", "as_purchasable"},
            {"Arrow", "as_purchasable"},
            {"Shield", "as_purchasable"},
            {"MiniGameAsteroid", "as_movable"},
            {"MiniGameShip", "as_movable"},
            {"OlmecCannon", "as_movable"},
            {"PlayerBag", "as_movable"},
            {"RollingItem", "as_purchasable"},
            {"PlayerGhost", "as_lightemitter"},
            {"GhostBreath", "as_projectile"},
            {"Pot", "as_purchasable"},
            {"Backpack", "as_movable"},
            {"AnkhPowerup", "as_powerup"},
            {"Powerup", "as_movable"},
            {"KapalaPowerup", "as_powerup"},
            {"ParachutePowerup", "as_powerup"},
            {"TrueCrownPowerup", "as_powerup"},
            {"PunishBall", "as_movable"},
            {"DummyPurchasableEntity", "as_purchasable"},
            {"Rubble", "as_movable"},
            {"ScepterShot", "as_lightemitter"},
            {"SpecialShot", "as_lightemitter"},
            {"SkullDropTrap", "as_movable"},
            {"Switch", "as_movable"},
            {"SnapTrap", "as_movable"},
            {"Spark", "as_flame"},
            {"Teleporter", "as_purchasable"},
            {"TeleporterBackpack", "as_backpack"},
            {"Telescope", "as_movable"},
            {"Tentacle", "as_chain"},
            {"TiamatShot", "as_lightemitter"},
            {"TorchFlame", "as_flame"},
            {"TV", "as_movable"},
            {"UdjatSocket", "as_movable"},
            {"Ushabti", "as_movable"},
            {"VladsCape", "as_cape"},
            {"FlameSize", "as_flame"},
            {"Web", "as_movable"},
            {"WebShot", "as_projectile"},
            {"Lava", "as_liquid"},
            {"Liquid", "as_entity"},
            {"LogicalAnchovyFlock", "as_entity"},
            {"LogicalTrapTrigger", "as_entity"},
            {"LogicalDoor", "as_entity"},
            {"BoulderSpawner", "as_entity"},
            {"BurningRopeEffect", "as_entity"},
            {"CameraFlash", "as_entity"},
            {"CinematicAnchor", "as_entity"},
            {"LogicalConveyorbeltSound", "as_logicalsound"},
            {"CursedEffect", "as_entity"},
            {"DMAlienBlast", "as_entity"},
            {"DMSpawning", "as_entity"},
            {"LogicalSound", "as_entity"},
            {"DustWallApep", "as_entity"},
            {"EggplantThrower", "as_entity"},
            {"FrostBreathEffect", "as_entity"},
            {"IceSlidingSound", "as_logicalsound"},
            {"JungleTrapTrigger", "as_logicaltraptrigger"},
            {"LogicalDrain", "as_entity"},
            {"LimbAnchor", "as_entity"},
            {"LogicalMiniGame", "as_entity"},
            {"MummyFliesSound", "as_logicalsound"},
            {"OnFireEffect", "as_entity"},
            {"OuroboroCameraAnchor", "as_entity"},
            {"OuroboroCameraZoomin", "as_entity"},
            {"PipeTravelerSound", "as_logicalsound"},
            {"PoisonedEffect", "as_entity"},
            {"Portal", "as_entity"},
            {"LogicalStaticSound", "as_logicalsound"},
            {"QuickSandSound", "as_logicalsound"},
            {"LogicalRegeneratingBlock", "as_entity"},
            {"RoomLight", "as_entity"},
            {"ShootingStarSpawner", "as_entity"},
            {"SplashBubbleGenerator", "as_entity"},
            {"LogicalLiquidStreamSound", "as_logicalstaticsound"},
            {"WetEffect", "as_entity"},
            {"Alien", "as_monster"},
            {"Lahamu", "as_monster"},
            {"Ammit", "as_monster"},
            {"Anubis", "as_monster"},
            {"ApepPart", "as_monster"},
            {"ApepHead", "as_apeppart"},
            {"Bat", "as_monster"},
            {"Bee", "as_monster"},
            {"Bodyguard", "as_npc"},
            {"CatMummy", "as_monster"},
            {"Caveman", "as_walkingmonster"},
            {"Quillback", "as_walkingmonster"},
            {"CavemanShopkeeper", "as_walkingmonster"},
            {"Cobra", "as_monster"},
            {"Crabman", "as_monster"},
            {"Critter", "as_monster"},
            {"CritterButterfly", "as_critter"},
            {"CritterCrab", "as_critter"},
            {"CritterDrone", "as_critter"},
            {"CritterBeetle", "as_critter"},
            {"CritterFirefly", "as_critter"},
            {"CritterFish", "as_critter"},
            {"CritterLocust", "as_critter"},
            {"CritterPenguin", "as_critter"},
            {"CritterSlime", "as_critter"},
            {"CritterSnail", "as_critter"},
            {"Crocman", "as_walkingmonster"},
            {"EggplantMinister", "as_monster"},
            {"Jiangshi", "as_monster"},
            {"Firebug", "as_monster"},
            {"FirebugUnchained", "as_monster"},
            {"FireFrog", "as_frog"},
            {"Fish", "as_monster"},
            {"Frog", "as_monster"},
            {"Ghist", "as_monster"},
            {"Ghost", "as_monster"},
            {"GiantFish", "as_monster"},
            {"GiantFly", "as_monster"},
            {"GiantFrog", "as_monster"},
            {"Spider", "as_monster"},
            {"GoldMonkey", "as_monster"},
            {"Grub", "as_monster"},
            {"HangSpider", "as_monster"},
            {"Hermitcrab", "as_monster"},
            {"HornedLizard", "as_monster"},
            {"Hundun", "as_monster"},
            {"Beg", "as_npc"},
            {"HundunHead", "as_monster"},
            {"Imp", "as_monster"},
            {"JumpDog", "as_monster"},
            {"Kingu", "as_monster"},
            {"Lamassu", "as_monster"},
            {"Lavamander", "as_monster"},
            {"Leprechaun", "as_walkingmonster"},
            {"RoomOwner", "as_monster"},
            {"MagmaMan", "as_monster"},
            {"Mantrap", "as_monster"},
            {"Terra", "as_monster"},
            {"MegaJellyfish", "as_monster"},
            {"Tun", "as_roomowner"},
            {"Mole", "as_monster"},
            {"Monkey", "as_monster"},
            {"Mosquito", "as_monster"},
            {"Mummy", "as_monster"},
            {"Necromancer", "as_walkingmonster"},
            {"Octopus", "as_walkingmonster"},
            {"VanHorsing", "as_npc"},
            {"Olmite", "as_walkingmonster"},
            {"OsirisHand", "as_monster"},
            {"OsirisHead", "as_monster"},
            {"Pet", "as_monster"},
            {"ProtoShopkeeper", "as_monster"},
            {"Skeleton", "as_monster"},
            {"Robot", "as_walkingmonster"},
            {"Scarab", "as_monster"},
            {"Scorpion", "as_monster"},
            {"Shopkeeper", "as_roomowner"},
            {"NPC", "as_monster"},
            {"ForestSister", "as_npc"},
            {"Monster", "as_powerupcapable"},
            {"Sorceress", "as_walkingmonster"},
            {"Waddler", "as_roomowner"},
            {"Tadpole", "as_monster"},
            {"Tiamat", "as_monster"},
            {"WalkingMonster", "as_monster"},
            {"UFO", "as_monster"},
            {"Vampire", "as_monster"},
            {"Vlad", "as_vampire"},
            {"WitchDoctor", "as_walkingmonster"},
            {"WitchDoctorSkull", "as_monster"},
            {"Yama", "as_monster"},
            {"Yang", "as_roomowner"},
            {"YetiKing", "as_monster"},
            {"YetiQueen", "as_monster"},
            {"Axolotl", "as_mount"},
            {"Mount", "as_powerupcapable"},
            {"Mech", "as_mount"},
            {"Qilin", "as_mount"},
            {"Rockdog", "as_mount"},
        };
}

void LuaConsole::on_history_request(ImGuiInputTextCallbackData* data)
{
    bool modifier = !alt_keys || ImGui::GetIO().KeyCtrl;
    std::optional<size_t> prev_history_pos = history_pos;
    if (!history_pos.has_value())
    {
        if (!history.empty())
        {
            history_pos = history.size() - 1;
        }
    }
    else if (modifier && data->EventKey == ImGuiKey_UpArrow && history_pos.value() == history.size() - 1 && data->BufTextLen == 0)
    {
        prev_history_pos = -1;
    }
    else if (modifier && data->EventKey == ImGuiKey_DownArrow && history_pos.value() == history.size() - 1)
    {
        data->DeleteChars(0, data->BufTextLen);
    }
    else if (modifier && data->EventKey == ImGuiKey_UpArrow && history_pos.value() > 0)
    {
        history_pos.value()--;
    }
    else if (modifier && data->EventKey == ImGuiKey_DownArrow && history_pos.value() < history.size() - 1)
    {
        history_pos.value()++;
    }

    if (prev_history_pos != history_pos)
    {
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history[history_pos.value()].command.c_str());
        set_scroll_to_history_item = history_pos.value();
        highlight_history = true;
        if (!alt_keys)
        {
            if (prev_history_pos > history_pos)
                data->CursorPos = 0;
            else
                data->CursorPos = data->BufTextLen;
        }
    }
}
bool LuaConsole::on_completion(ImGuiInputTextCallbackData* data)
{
    std::string_view front{data->Buf, (size_t)data->CursorPos};
    std::string_view to_complete{};
    {
        auto rit = std::find_if(front.rbegin(), front.rend(), [](auto c)
                                { return !std::isalnum(c) && c != '.' && c != ':' && c != '_' && c != '[' && c != ']'; });

        auto it = rit.base();
        to_complete = std::string_view{it, front.end()};
    }

    const auto [to_complete_end, to_complete_base] = [](std::string_view str, std::string_view delims = ".:")
    {
        auto last_delim = str.find_last_of(delims);
        if (last_delim != std::string_view::npos)
        {
            auto pre = str.substr(0, last_delim);
            str = str.substr(last_delim + 1);

            return std::pair{str, pre};
        }
        return std::pair{str, std::string_view{}};
    }(to_complete);

    completion_options.clear();
    completion_error.clear();

    if (!to_complete_end.empty() || !to_complete_base.empty())
    {
        // Gather candidates for completion, this has to actually access variables so it can fail
        std::vector<std::string_view> possible_options;
        try
        {
            possible_options = [this](std::string_view _to_complete_end, std::string_view _to_complete_base)
            {
                if (_to_complete_base.empty())
                {
                    using namespace std::string_view_literals;
                    static constexpr std::array additional_options{
                        "cls"sv,
                        "and"sv,
                        "break"sv,
                        "do"sv,
                        "else"sv,
                        "elseif"sv,
                        "end"sv,
                        "false"sv,
                        "for"sv,
                        "function"sv,
                        "if"sv,
                        "in"sv,
                        "local"sv,
                        "nil"sv,
                        "not"sv,
                        "or"sv,
                        "repeat"sv,
                        "return"sv,
                        "then"sv,
                        "true"sv,
                        "until"sv,
                        "while"sv,
                    };

                    std::vector<std::string_view> possibleoptions;

                    for (std::string_view opt : additional_options)
                    {
                        if (opt.starts_with(_to_complete_end))
                        {
                            possibleoptions.push_back(opt);
                        }
                    }

                    for (const auto& [k, v] : lua)
                    {
                        if (k.get_type() == sol::type::string)
                        {
                            const std::string_view str = k.as<std::string_view>();
                            if (str.starts_with(_to_complete_end) && (!str.starts_with("__") || _to_complete_end.starts_with("__")))
                            {
                                possibleoptions.push_back(str);
                            }
                        }
                    }
                    return possibleoptions;
                }
                else
                {
                    std::vector<std::string_view> possibleoptions;

                    // Need to collect these in a vector, otherwise the state somehow breaks
                    std::vector<sol::userdata> source_obj{};
                    std::vector<sol::table> source{};

                    {
                        const auto obj = execute_lua(lua, fmt::format("return {}", _to_complete_base));
                        const auto obj_type = obj.get_type();
                        if (obj_type == sol::type::table)
                        {
                            source.push_back(obj);
                        }
                        else if (obj_type == sol::type::userdata)
                        {
                            source_obj.push_back(obj);
                            source.push_back(obj.get<sol::userdata>()[sol::metatable_key]);
                        }
                        else
                        {
                            return possibleoptions;
                        }
                    }

                    const bool grab_all = _to_complete_end.empty();
                    while (true)
                    {
                        for (const auto& [k, v] : source.back())
                        {
                            if (k.get_type() == sol::type::string)
                            {
                                const std::string_view str = k.as<std::string_view>();
                                if ((grab_all || str.starts_with(_to_complete_end)) && (!str.starts_with("__") || _to_complete_end.starts_with("__")))
                                {
                                    possibleoptions.push_back(str);
                                }
                            }
                        }

                        auto name_member = source.back()["__name"];
                        if (name_member != sol::nil)
                        {
                            // Transform e.g. "sol.Player*" to "Player"
                            std::string_view name = name_member.get<std::string_view>();
                            if (name.starts_with("sol."))
                            {
                                name = name.substr(4);
                            }
                            if (name.ends_with("*"))
                            {
                                name = name.substr(0, name.size() - 1);
                            }

                            // Should not need to create std::string here, but otherwise we get a nil returned
                            if (entity_down_cast_map.contains(name))
                            {
                                auto down_cast = lua["Entity"][entity_down_cast_map.at(name)];
                                source_obj.push_back(down_cast(source_obj.back()));
                                source.push_back(source_obj.back()[sol::metatable_key].get<sol::table>());
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    return possibleoptions;
                }
            }(to_complete_end, to_complete_base);
        }
        catch (const sol::error& e)
        {
            completion_error = e.what();
            return true;
        }

        to_complete = to_complete_end;
        if (possible_options.empty())
        {
            completion_options = fmt::format("No matches found for tab-completion of '{}'...", to_complete);
        }
        else if (possible_options.size() == 1)
        {
            data->DeleteChars((int)(to_complete.data() - data->Buf), (int)to_complete.size());

            std::string_view option = possible_options[0];
            data->InsertChars(data->CursorPos, option.data(), option.data() + option.size());
        }
        else
        {
            size_t overlap{0};
            auto first = possible_options.front();

            while (true)
            {
                bool all_match{true};
                for (std::string_view option : possible_options)
                {
                    if (overlap >= option.size() || option[overlap] != first[overlap])
                    {
                        all_match = false;
                        break;
                    }
                }
                if (!all_match)
                {
                    break;
                }
                overlap++;
            }

            std::string_view option_overlap = first.substr(0, overlap);
            if (!option_overlap.empty())
            {
                if (!to_complete.empty())
                {
                    data->DeleteChars((int)(to_complete.data() - data->Buf), (int)to_complete.size());
                }
                data->InsertChars(data->CursorPos, option_overlap.data(), option_overlap.data() + option_overlap.size());
            }

            if (possible_options.size() > 20)
            {
                possible_options.resize(20);
                possible_options.push_back("More than 20 completion options, output is truncated...");
            }
            for (std::string_view option : possible_options)
            {
                completion_options += option;
                completion_options += "\n";
            }
            completion_options.pop_back();
        }
        return true;
    }
    else if (!to_complete_end.empty() && !to_complete_base.empty())
    {
        completion_options = fmt::format("Need at least 1 character for tab-completion, given {}...", to_complete_end);
        return true;
    }
    return false;
}

bool LuaConsole::pre_draw()
{
    has_new_history = false;
    if (toggled)
    {
        auto& io = ImGui::GetIO();
        auto& style = ImGui::GetStyle();
        auto base = ImGui::GetMainViewport();

        if (!options.empty())
        {
            auto width = (size.x != 0 ? size.x : base->Size.x) - 300.0f;
            auto opos = pos.x + width;
            ImGui::SetNextWindowSize({300, size.y != 0 ? size.y : base->Size.y});
            ImGui::SetNextWindowPos({opos, pos.y});
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
            ImGui::Begin("Console Options", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);
            render_options();
            ImGui::End();
            ImGui::PopStyleVar();
            ImGui::SetNextWindowSize((size.x != 0 ? ImVec2(width, size.y) : base->Size));
        }
        else
        {
            ImGui::SetNextWindowSize(size.x != 0 ? size : base->Size);
        }
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowViewport(base->ID);
        ImGui::Begin(
            "Console Overlay",
            NULL,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

        std::string_view input_view{console_input};
        const size_t num_lines = std::count(input_view.begin(), input_view.end(), '\n') + 1;

        const std::string& completion_text = completion_options.empty() ? completion_error : completion_options;
        const float completion_size = completion_text.empty() ? 0.0f : style.ItemSpacing.y + ImGui::CalcTextSize(completion_text.c_str(), nullptr, false, io.DisplaySize.x).y;
        const float input_size = 2.0f * style.ItemSpacing.y + num_lines * ImGui::GetTextLineHeight();
        const float footer_height_to_reserve = style.ItemSpacing.y + input_size + completion_size;
        ImGui::BeginChild("Results Region", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        static const ImVec4 trans{0.0f, 0.0f, 0.0f, 0.0f};

        for (size_t i = 0; i < history.size(); i++)
        {
            auto& item = history[i];

            {
                ImVec4 color{0.7f, 0.7f, 0.7f, 1.0f};
                if (history_pos == i)
                {
                    color = ImVec4{0.4f, 0.8f, 0.4f, 1.0f};
                }

                int num = 1;
                const char* str;
                for (str = item.command.c_str(); *str; ++str)
                    num += *str == '\n';

                ImGui::PushStyleColor(ImGuiCol_Button, trans);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                ImGui::PushID((int)i);
                ImGui::PushID(item.command.c_str());
                if (ImGui::Button("> ##CopyCommandToClipboard", {16.0f, ImGui::GetTextLineHeight() * num}))
                    ImGui::SetClipboardText(item.command.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Copy command to clipboard!");
                ImGui::PopID();
                ImGui::PopID();
                ImGui::PopStyleVar();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextWrapped("%s", item.command.c_str());
                ImGui::PopStyleColor();
            }

            if (highlight_history && i == set_scroll_to_history_item)
            {
                ImGui::SetScrollHereY(0.0f);
                last_force_scroll = set_scroll_to_history_item;
                set_scroll_to_history_item = std::nullopt;
            }
            for (const auto& results : item.messages)
            {
                int num = 1;
                const char* str;
                for (str = results.message.c_str(); *str; ++str)
                    num += *str == '\n';

                ImGui::PushStyleColor(ImGuiCol_Button, trans);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                ImGui::PushID((int)i);
                ImGui::PushID(results.message.c_str());
                if (ImGui::Button("< ##CopyResultToClipboard", {16.0f, ImGui::GetTextLineHeight() * num}))
                    ImGui::SetClipboardText(results.message.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Copy result to clipboard!");
                ImGui::PopID();
                ImGui::PopID();
                ImGui::PopStyleVar();
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, results.color);
                ImGui::TextWrapped("%s", results.message.c_str());
                ImGui::PopStyleColor();
            }
        }

        if (scroll_to_bottom)
        {
            ImGui::SetScrollHereY(1.0f);
            last_force_scroll = static_cast<size_t>(-1);
            scroll_to_bottom = false;
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();

        if (!completion_error.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.7f, 0.3f, 0.3f, 1.0f});
            ImGui::TextWrapped("%s", completion_error.c_str());
            ImGui::PopStyleColor();
        }
        else if (!completion_options.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.7f, 0.7f, 0.7f, 1.0f});
            ImGui::TextWrapped("%s", completion_options.c_str());
            ImGui::PopStyleColor();
        }

        if (set_focus)
        {
            ImGui::SetKeyboardFocusHere();
            set_focus = false;
        }

        auto input_callback = [](ImGuiInputTextCallbackData* data)
        {
            static bool do_tab_completion{false};

            LuaConsole* self = static_cast<LuaConsole*>(data->UserData);
            bool modifier = !self->alt_keys || ImGui::GetIO().KeyCtrl;

            if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter)
            {
                if (data->EventChar == '\t')
                {
                    do_tab_completion = true;
                }
                else if (data->EventChar != 0)
                {
                    do_tab_completion = false;
                    if (modifier && data->EventChar == '\n' && self->last_force_scroll.has_value())
                    {
                        if (self->last_force_scroll.value() < self->history.size())
                        {
                            self->set_scroll_to_history_item = self->last_force_scroll;
                        }
                        else
                        {
                            self->scroll_to_bottom = true;
                        }
                    }
                    self->highlight_history = false;
                }
            }
            else
            {
                if (do_tab_completion && data->Buf[data->CursorPos - 1] == '\t')
                {
                    const auto prev_pos = data->CursorPos;
                    data->DeleteChars(data->CursorPos - 1, 1);
                    if (prev_pos == data->CursorPos)
                    {
                        data->CursorPos--;
                    }
                    if (!self->on_completion(data))
                    {
                        data->InsertChars(data->CursorPos, "\t");
                        data->CursorPos = prev_pos;
                    }
                    do_tab_completion = false;
                }

                const bool up_arrow_pressed = ImGui::IsKeyPressed(ImGuiKey_UpArrow);
                const bool down_arrow_pressed = ImGui::IsKeyPressed(ImGuiKey_DownArrow);
                const std::string_view buf_view(data->Buf, data->BufTextLen);

                if (modifier && (up_arrow_pressed || down_arrow_pressed) && (data->CursorPos == 0 || data->CursorPos == data->BufTextLen || self->alt_keys))
                {
                    data->EventKey = up_arrow_pressed
                                         ? ImGuiKey_UpArrow
                                         : ImGuiKey_DownArrow;
                    self->on_history_request(data);
                }
                else if (up_arrow_pressed && (buf_view.find_first_of("\n") == std::string::npos || self->prev_cursor_pos <= buf_view.find_first_of("\n")))
                {
                    data->CursorPos = 0;
                }
                else if (down_arrow_pressed && (buf_view.find_last_of("\n") == std::string::npos || self->prev_cursor_pos > buf_view.find_last_of("\n")))
                {
                    data->CursorPos = data->BufTextLen;
                }
            }

            self->prev_cursor_pos = data->CursorPos;
            return 0;
        };

        const float indent_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), std::numeric_limits<float>::max(), -1.0f, "00", nullptr, nullptr).x + 2.0f;
        ImGui::PushItemWidth(ImGui::GetWindowWidth() - indent_size);
        ImGui::Indent(indent_size);
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackCharFilter;
        if (!alt_keys)
            input_text_flags |= ImGuiInputTextFlags_CtrlEnterForNewLine;
        if (ImGui::InputTextMultiline("##ConsoleInput", console_input, IM_ARRAYSIZE(console_input), ImVec2{-1, -1}, input_text_flags, input_callback, this))
        {
            if (console_input[0] != '\0')
            {
                using namespace std::string_view_literals;
                if (console_input == "cls"sv || console_input == "clr"sv || console_input == "clear"sv)
                {
                    history_pos = std::nullopt;
                    history.clear();
                }
                else if (console_input == "reset"sv || console_input == "reload"sv)
                {
                    LuaBackend::clear();
                }
                else if (console_input == "quit"sv)
                {
                    std::exit(0);
                }
                else if (console_input == "enable"sv)
                {
                    set_enabled(true);
                }
                else if (console_input == "disable"sv)
                {
                    set_enabled(false);
                }
                else if (console_input == "safe"sv)
                {
                    hide_unsafe_libraries(lua);
                    unsafe = false;
                }
                else if (console_input == "unsafe"sv)
                {
                    expose_unsafe_libraries(lua);
                    unsafe = true;
                }
                else
                {
                    std::size_t messages_before = messages.size();

                    auto results = execute(console_input);

                    std::vector<ScriptMessage> result_message;
                    std::move(messages.begin() + messages_before, messages.end(), std::back_inserter(result_message));
                    messages.erase(messages.begin() + messages_before, messages.end());

                    if (!results.str.empty())
                    {
                        ImVec4 color{1.0f, 1.0f, 1.0f, 1.0f};
                        if (results.error)
                        {
                            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                            set_error(results.str);
                        }
                        result_message.push_back({std::move(results.str), {}, color});
                    }

                    history_pos = std::nullopt;
                    push_history(console_input, std::move(result_message));
                }
                std::memset(console_input, 0, IM_ARRAYSIZE(console_input));
                completion_options.clear();
                scroll_to_bottom = true;
            }
            set_focus = true;
        }
        ImGui::Unindent(indent_size);
        ImGui::PopItemWidth();

        ImGui::SetWindowPos({base->Pos.x + base->Size.x / 2 - ImGui::GetWindowWidth() / 2, base->Pos.y + base->Size.y / 2 - base->Size.y / 2}, ImGuiCond_Always);
        auto drawlist = ImGui::GetWindowDrawList();
        int num = 1;
        const char* str;
        for (str = console_input; *str; ++str)
            num += *str == '\n';
        for (int i = 1; i <= num; ++i)
        {
            std::string buf = fmt::format("{}", i);
            auto linesize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), std::numeric_limits<float>::max(), -1.0f, buf.c_str(), nullptr, nullptr);
            float numx = pos.x != 0 ? pos.x + indent_size - linesize.x - 2.0f : base->Pos.x + indent_size - linesize.x - 2.0f;
            float numy = pos.y != 0 ? pos.y + size.y - linesize.y * (num - i + 2) + 9.0f : base->Pos.y + base->Size.y - linesize.y * (num - i + 2) + 9.0f;
            drawlist->AddText(ImVec2(numx, numy), ImColor(1.0f, 1.0f, 1.0f, .5f), buf.c_str());
        }
        ImGui::End();
    }

    return true;
}

const std::filesystem::path& LuaConsole::get_root_path() const
{
    static std::filesystem::path root_path{"."};
    return root_path;
}

void LuaConsole::register_command(LuaBackend* provider, std::string command_name, sol::function cmd)
{
    lua[command_name] = std::move(cmd);
    console_commands_list[std::move(command_name)] = provider;
}
void LuaConsole::unregister_command(LuaBackend* provider, std::string command_name)
{
    if (console_commands_list[command_name] == provider)
    {
        lua[command_name] = sol::nil;
        console_commands_list.erase(std::move(command_name));
    }
}

ConsoleResult LuaConsole::execute(std::string str, bool raw)
{
    sol::protected_function_result res;

    if (!str.starts_with("return") && !raw)
    {
        res = execute_raw("return " + str);
        if (!res.valid())
            res = execute_raw(std::move(str));
    }
    else
    {
        res = execute_raw(std::move(str));
    }

    if (!res.valid())
    {
        sol::error err = res;
        return ConsoleResult{err.what(), true};
    }

    if (res.get_type() == sol::type::nil || res.get_type() == sol::type::none)
        return ConsoleResult{"", false};

    LuaBackend::push_calling_backend(this);
    ON_SCOPE_EXIT(LuaBackend::pop_calling_backend(this));

    sol::protected_function dump_string = lua["dump_string"];
    sol::protected_function_result out = dump_string(res, 2);
    if (!out.valid())
    {
        sol::error err = out;
        return ConsoleResult{err.what(), true};
    }
    return ConsoleResult{out, false};
}

sol::protected_function_result LuaConsole::execute_raw(std::string str)
{
    return execute_lua(lua, str, true);
}

void LuaConsole::toggle()
{
    toggled = !toggled;
    set_focus = toggled;
    scroll_to_bottom = true;
}

void LuaConsole::push_history(std::string history_item, std::vector<ScriptMessage> result_item)
{
    while (history_item.ends_with('\n'))
    {
        history_item.pop_back();
    }

    if (!history_item.empty())
    {
        has_new_history = true;
        scroll_to_bottom = true;
        history.push_back(ConsoleHistoryItem{
            std::move(history_item),
            std::move(result_item)});
        if (history.size() > max_history)
        {
            history.erase(history.begin());
        }
    }
}

std::string LuaConsole::dump_api()
{
    std::set<std::string> excluded_keys{"meta", "__require", "__script_id", "TYPE_MAP", "get_script_id"};

    sol::state dummy_state;
    dummy_state.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package, sol::lib::debug);
    require_serpent_lua(dummy_state);

    for (auto& [key, value] : lua)
    {
        std::string key_str = key.as<std::string>();
        if (key_str.starts_with("sol."))
        {
            excluded_keys.insert(std::move(key_str));
        }
    }

    for (auto& [key, value] : dummy_state["_G"].get<sol::table>())
    {
        std::string key_str = key.as<std::string>();
        excluded_keys.insert(std::move(key_str));
    }

    sol::table opts = dummy_state.create_table();
    opts["comment"] = false;
    sol::function serpent = dummy_state["serpent"]["block"];

    std::map<std::string, std::string> sorted_output;
    for (auto& [key, value] : lua)
    {
        std::string key_str = key.as<std::string>();
        if (!excluded_keys.contains(key_str))
        {
            std::string value_str = serpent(value, opts).get<std::string>();
            std::regex re("__", std::regex::extended);
            if (value_str.starts_with("\"function"))
            {
                continue;
            }
            else if (value_str.starts_with("\"userdata"))
            {
                continue;
            }
            else if (std::regex_search(value_str.c_str(), re))
            {
                continue;
            }
            else if (key_str.find_first_of("abcdefghijklmnopqrstuvwxyz") != std::string::npos)
            {
                continue;
            }
            sorted_output[std::move(key_str)] = std::move(value_str);
        }
    }

    std::string api;
    for (auto& [key, value] : sorted_output)
        api += fmt::format("{} = {}\n", key, value);

    const static std::regex reg(R"("(userdata|function):\s[0-9A-F]+")");
    api = std::regex_replace(api, reg, "\"$1\"");

    return api;
}

unsigned int LuaConsole::get_input_lines() const
{
    int num = 1;
    const char* str;
    for (str = console_input; *str; ++str)
        num += *str == '\n';
    return num;
}
