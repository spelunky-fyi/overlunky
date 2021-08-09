#include "lua_console.hpp"

#include "lua_vm.hpp"
#include "rpc.hpp"
#include "script_util.hpp"

#include "lua_libs/lua_libs.hpp"

#include <sol/sol.hpp>

LuaConsole::LuaConsole(SoundManager* sound_manager)
    : LuaBackend(sound_manager, this)
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
            {"UnchainedSpikeBall", "as_movable"},
            {"WoodenlogTrap", "as_movable"},
            {"Player", "as_movable"},
            {"Floor", "as_entity"},
            {"PoleDeco", "as_floor"},
            {"Altar", "as_floor"},
            {"Arrowtrap", "as_floor"},
            {"BigSpearTrap", "as_floor"},
            {"ForceField", "as_floor"},
            {"ConveyorBelt", "as_transferfloor"},
            {"CityOfGoldDoor", "as_decorateddoor"},
            {"DecoratedDoor", "as_exitdoor"},
            {"EggShipDoor", "as_door"},
            {"Door", "as_floor"},
            {"ExitDoor", "as_door"},
            {"LockedDoor", "as_door"},
            {"MainExit", "as_exitdoor"},
            {"Generator", "as_floor"},
            {"HorizontalForceField", "as_floor"},
            {"LaserTrap", "as_floor"},
            {"TotemTrap", "as_floor"},
            {"MotherStatue", "as_floor"},
            {"Pipe", "as_transferfloor"},
            {"QuickSand", "as_floor"},
            {"SlidingWallCeiling", "as_floor"},
            {"SparkTrap", "as_floor"},
            {"SpikeballTrap", "as_floor"},
            {"StickyTrap", "as_floor"},
            {"TeleportingBorder", "as_floor"},
            {"TentacleBottom", "as_floor"},
            {"TimedForceField", "as_forcefield"},
            {"LavaGlow", "as_movable"},
            {"OlmecFloater", "as_movable"},
            {"Flame", "as_movable"},
            {"LightEmitter", "as_movable"},
            {"Container", "as_movable"},
            {"Coffin", "as_container"},
            {"WallTorch", "as_torch"},
            {"AxolotlShot", "as_movable"},
            {"Spear", "as_movable"},
            {"Bomb", "as_movable"},
            {"Boombox", "as_movable"},
            {"Boomerang", "as_movable"},
            {"Bullet", "as_movable"},
            {"Gun", "as_movable"},
            {"YellowCape", "as_cape"},
            {"Chain", "as_movable"},
            {"Chest", "as_movable"},
            {"ClimbableRope", "as_movable"},
            {"CloneGunShot", "as_timedshot"},
            {"CookFire", "as_movable"},
            {"AcidBubble", "as_movable"},
            {"Claw", "as_movable"},
            {"StretchChain", "as_movable"},
            {"CursedPot", "as_movable"},
            {"EggSac", "as_movable"},
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
            {"GiantClamTop", "as_movable"},
            {"FlyHead", "as_movable"},
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
            {"LightShot", "as_movable"},
            {"Leaf", "as_movable"},
            {"LightArrow", "as_arrow"},
            {"Mattock", "as_movable"},
            {"Arrow", "as_movable"},
            {"Shield", "as_movable"},
            {"MiniGameAsteroid", "as_movable"},
            {"MiniGameShip", "as_movable"},
            {"OlmecCannon", "as_movable"},
            {"RollingItem", "as_movable"},
            {"PlayerBag", "as_movable"},
            {"PlayerGhost", "as_lightemitter"},
            {"GhostBreath", "as_movable"},
            {"Pot", "as_container"},
            {"Backpack", "as_movable"},
            {"AnkhPowerup", "as_movable"},
            {"KapalaPowerup", "as_movable"},
            {"ParachutePowerup", "as_movable"},
            {"TrueCrownPowerup", "as_movable"},
            {"PunishBall", "as_movable"},
            {"ScepterShot", "as_lightemitter"},
            {"SpecialShot", "as_lightemitter"},
            {"SkullDropTrap", "as_movable"},
            {"Switch", "as_movable"},
            {"SnapTrap", "as_movable"},
            {"Spark", "as_flame"},
            {"Teleporter", "as_movable"},
            {"Telescope", "as_movable"},
            {"Tentacle", "as_chain"},
            {"TiamatShot", "as_lightemitter"},
            {"TorchFlame", "as_flame"},
            {"TV", "as_movable"},
            {"UdjatSocket", "as_movable"},
            {"Ushabti", "as_movable"},
            {"VladsCape", "as_cape"},
            {"FlameSize", "as_flame"},
            {"WebGun", "as_gun"},
            {"WebShot", "as_movable"},
            {"Liquid", "as_entity"},
            {"Lava", "as_liquid"},
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
            {"Monster", "as_movable"},
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
            {"Mount", "as_movable"},
            {"Mech", "as_mount"},
            {"Qilin", "as_mount"},
            {"Rockdog", "as_mount"},
        };
}

void LuaConsole::on_history_request(ImGuiInputTextCallbackData* data)
{
    std::optional<size_t> prev_history_pos = history_pos;
    if (!history_pos.has_value())
    {
        if (!history.empty())
        {
            history_pos = history.size() - 1;
        }
    }
    else if (data->EventKey == ImGuiKey_UpArrow && history_pos.value() == history.size() - 1 && data->BufTextLen == 0)
    {
        prev_history_pos = -1;
    }
    else if (data->EventKey == ImGuiKey_DownArrow && history_pos.value() == history.size() - 1)
    {
        data->DeleteChars(0, data->BufTextLen);
    }
    else if (data->EventKey == ImGuiKey_UpArrow && history_pos.value() > 0)
    {
        history_pos.value()--;
    }
    else if (data->EventKey == ImGuiKey_DownArrow && history_pos.value() < history.size() - 1)
    {
        history_pos.value()++;
    }

    if (prev_history_pos != history_pos)
    {
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, history[history_pos.value()].command.c_str());
        set_scroll_to_history_item = history_pos.value();
        highlight_history = true;
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
            possible_options = [this](std::string_view to_complete_end, std::string_view to_complete_base)
            {
                if (to_complete_base.empty())
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

                    std::vector<std::string_view> possible_options;

                    for (std::string_view opt : additional_options)
                    {
                        if (opt.starts_with(to_complete_end))
                        {
                            possible_options.push_back(opt);
                        }
                    }

                    for (const auto& [k, v] : lua)
                    {
                        if (k.get_type() == sol::type::string)
                        {
                            const std::string_view str = k.as<std::string_view>();
                            if (str.starts_with(to_complete_end) && (!str.starts_with("__") || to_complete_end.starts_with("__")))
                            {
                                possible_options.push_back(str);
                            }
                        }
                    }
                    return possible_options;
                }
                else
                {
                    std::vector<std::string_view> possible_options;

                    // Need to collect these in a vector, otherwise the state somehow breaks
                    std::vector<sol::userdata> source_obj{};
                    std::vector<sol::table> source{};

                    {
                        const auto obj = execute_lua(lua, fmt::format("return {}", to_complete_base));
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
                            return possible_options;
                        }
                    }

                    const bool grab_all = to_complete_end.empty();
                    while (true)
                    {
                        for (const auto& [k, v] : source.back())
                        {
                            if (k.get_type() == sol::type::string)
                            {
                                const std::string_view str = k.as<std::string_view>();
                                if ((grab_all || str.starts_with(to_complete_end)) && (!str.starts_with("__") || to_complete_end.starts_with("__")))
                                {
                                    possible_options.push_back(str);
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
                    return possible_options;
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
    if (enabled)
    {
        auto& io = ImGui::GetIO();
        auto& style = ImGui::GetStyle();

        const float window_height = io.DisplaySize.y - style.ItemSpacing.y * 2.0f;
        ImGui::SetNextWindowSize({io.DisplaySize.x, window_height});
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

        for (size_t i = 0; i < history.size(); i++)
        {
            auto& item = history[i];

            {
                ImVec4 color{0.7f, 0.7f, 0.7f, 1.0f};
                if (history_pos == i)
                {
                    color = ImVec4{0.4f, 0.8f, 0.4f, 1.0f};
                }

                ImGui::TextUnformatted("> ");
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
            if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter)
            {
                if (data->EventChar == '\t')
                {
                    do_tab_completion = true;
                }
                else if (data->EventChar != 0)
                {
                    do_tab_completion = false;
                    if (data->EventChar == '\n' && self->last_force_scroll.has_value())
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
                    const auto prev_cursor_pos = data->CursorPos;
                    data->DeleteChars(data->CursorPos - 1, 1);
                    if (prev_cursor_pos == data->CursorPos)
                    {
                        data->CursorPos--;
                    }
                    if (!self->on_completion(data))
                    {
                        data->InsertChars(data->CursorPos, "\t");
                        data->CursorPos = prev_cursor_pos;
                    }
                    do_tab_completion = false;
                }

                const bool up_arrow_pressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow));
                const bool down_arrow_pressed = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow));

                if ((up_arrow_pressed || down_arrow_pressed) && (data->CursorPos == 0 || data->CursorPos == data->BufTextLen))
                {
                    data->EventKey = up_arrow_pressed
                                         ? ImGuiKey_UpArrow
                                         : ImGuiKey_DownArrow;
                    self->on_history_request(data);
                }
            }

            return 0;
        };

        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine;
        if (ImGui::InputTextMultiline("", console_input, IM_ARRAYSIZE(console_input), ImVec2{-1, -1}, input_text_flags, input_callback, this))
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
                else
                {
                    std::size_t messages_before = messages.size();

                    std::string results = execute(console_input);

                    std::vector<ScriptMessage> result_message;
                    std::move(messages.begin() + messages_before, messages.end(), std::back_inserter(result_message));
                    messages.erase(messages.begin() + messages_before, messages.end());

                    if (!results.empty())
                    {
                        ImVec4 color{1.0f, 1.0f, 1.0f, 1.0f};
                        if (results.starts_with("sol:"))
                        {
                            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        }
                        result_message.push_back({std::move(results), {}, color});
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
        ImGui::PopItemWidth();

        ImGui::SetWindowPos({io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, io.DisplaySize.y / 2 - window_height / 2}, ImGuiCond_Always);
        ImGui::End();
    }

    return true;
}

void LuaConsole::set_enabled(bool)
{
}
bool LuaConsole::get_enabled() const
{
    return true;
}

bool LuaConsole::get_unsafe() const
{
    return true;
}
const char* LuaConsole::get_name() const
{
    return "lua_console";
}
const char* LuaConsole::get_id() const
{
    return "dev/lua_console";
}
const char* LuaConsole::get_path() const
{
    return "console_proxy.lua";
}
const char* LuaConsole::get_root() const
{
    return ".";
}
const std::filesystem::path& LuaConsole::get_root_path() const
{
    static std::filesystem::path root_path{"."};
    return root_path;
}

void LuaConsole::register_command(LuaBackend* provider, std::string command_name, sol::function cmd)
{
    lua[command_name] = std::move(cmd);
    console_commands[std::move(command_name)] = provider;
}
void LuaConsole::unregister_command(LuaBackend* provider, std::string command_name)
{
    if (console_commands[command_name] == provider)
    {
        lua[command_name] = sol::nil;
        console_commands.erase(std::move(command_name));
    }
}

std::string LuaConsole::execute(std::string code)
{
    if (!code.starts_with("return"))
    {
        std::string ret = execute_raw("return " + code);
        if (!ret.starts_with("sol: "))
        {
            return ret;
        }
    }
    return execute_raw(std::move(code));
}
std::string LuaConsole::execute_raw(std::string code)
{
    try
    {
        auto ret = execute_lua(lua, code);
        if (ret.get_type() == sol::type::nil || ret.get_type() == sol::type::none)
        {
            return "";
        }
        else
        {
            sol::function serpent = lua["serpent"]["block"];
            return serpent(ret);
        }
    }
    catch (const sol::error& e)
    {
        return e.what();
    }
}

void LuaConsole::toggle()
{
    enabled = !enabled;
    set_focus = enabled;
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
            if (value_str.starts_with("\"function"))
            {
                value_str = "function(...) end";
            }
            else if (value_str.starts_with("\"userdata"))
            {
                value_str = "{}";
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
