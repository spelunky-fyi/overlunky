#include "lua_console.hpp"

#include "lua_libs/lua_libs.hpp"
#include "rpc.hpp"
#include "script_util.hpp"

#include <sol/sol.hpp>

LuaConsole::LuaConsole(SoundManager* sound_manager)
    : LuaBackend(sound_manager, this)
{
    require_serpent_lua(lua);
}

void LuaConsole::on_history_request(ImGuiInputTextCallbackData* data)
{
    const std::optional<size_t> prev_history_pos = history_pos;
    if (!history_pos.has_value())
    {
        if (!history.empty())
        {
            history_pos = history.size() - 1;
        }
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
    }
}
void LuaConsole::on_completion(ImGuiInputTextCallbackData* data)
{
    std::string_view front{ data->Buf, (size_t)data->CursorPos };
    std::string_view completion_segment{};
    {
        auto rit = std::find_if(front.rbegin(), front.rend(), [](auto c) {
            return !std::isalnum(c) && c != '.' && c != ':' && c != '_';
        });

        auto it = rit.base();
        completion_segment = std::string_view{ it, front.end() };
    }

    completion_options.clear();

    if (completion_segment.size() >= 3)
    {
        std::vector<std::string_view> options;
        for (auto& [k, v] : lua)
        {
            if (k.get_type() == sol::type::string && k.as<std::string_view>().starts_with(completion_segment))
            {
                options.push_back(k.as<std::string_view>());
            }
        }

        if (options.empty())
        {
            completion_options = fmt::format("No matches found for tab-completion of '{}'...", completion_segment);
        }
        else if (options.size() == 1)
        {
            data->DeleteChars((int)(completion_segment.data() - data->Buf), (int)completion_segment.size());

            std::string_view option = options[0];
            data->InsertChars(data->CursorPos, option.data(), option.data() + option.size());
        }
        else
        {
            size_t overlap{ 1 };
            auto first = options.front();

            while (true)
            {
                bool all_match{ true };
                for (std::string_view option : options)
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
                data->DeleteChars((int)(completion_segment.data() - data->Buf), (int)completion_segment.size());
                data->InsertChars(data->CursorPos, option_overlap.data(), option_overlap.data() + option_overlap.size());
            }

            for (std::string_view option : options)
            {
                completion_options += option;
                completion_options += "; ";
            }
            completion_options.pop_back();
        }
    }
    else
    {
        completion_options = "Need at least 3 characters for tab-completion...";
    }
}

bool LuaConsole::pre_draw()
{
    if (enabled)
    {
        auto& io = ImGui::GetIO();
        auto& style = ImGui::GetStyle();

        const float window_height = io.DisplaySize.y - style.ItemSpacing.y * 2.0f;
        ImGui::SetNextWindowSize({ io.DisplaySize.x, window_height });
        ImGui::Begin(
            "Console Overlay",
            NULL,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

        const float completion_size = completion_options.empty() ? 0.0f : style.ItemSpacing.y + ImGui::CalcTextSize(completion_options.c_str(), nullptr, false, io.DisplaySize.x).y;
        const float footer_height_to_reserve = style.ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() + completion_size;
        ImGui::BeginChild("Results Region", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        for (size_t i = 0; i < history.size(); i++)
        {
            auto& item = history[i];

            {
                ImVec4 color{ 0.7f,0.7f,0.7f,1.0f };
                if (history_pos == i)
                {
                    color = ImVec4{ 0.4f,0.8f,0.4f,1.0f };
                }

                ImGui::TextUnformatted("> ");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item.command.c_str());
                ImGui::PopStyleColor();
            }

            if (i == set_scroll_to_history_item)
            {
                ImGui::SetScrollHereY(0.0f);
                set_scroll_to_history_item = std::nullopt;
            }

            for (const auto& result : item.messages)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, result.color);
                ImGui::TextUnformatted(result.message.c_str());
                ImGui::PopStyleColor();
            }
        }

        if (scroll_to_bottom)
        {
            ImGui::SetScrollHereY(1.0f);
            scroll_to_bottom = false;
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();

        if (!completion_options.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.7f,0.7f,0.7f,1.0f });
            ImGui::TextWrapped(completion_options.c_str());
            ImGui::PopStyleColor();
        }

        if (set_focus)
        {
            ImGui::SetKeyboardFocusHere();
            set_focus = false;
        }

        auto input_callback = [](ImGuiInputTextCallbackData* data)
        {
            LuaConsole* self = static_cast<LuaConsole*>(data->UserData);
            if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
            {
                self->on_history_request(data);
            }
            else if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
            {
                self->on_completion(data);
            }

            return 0;
        };

        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("", console_input, IM_ARRAYSIZE(console_input), input_text_flags, input_callback, this))
        {
            if (console_input[0] != '\0')
            {
                using namespace std::string_view_literals;
                if (console_input == "cls"sv || console_input == "clr"sv || console_input == "clear"sv)
                {
                    history_pos = std::nullopt;
                    history.clear();
                }
                else
                {
                    std::string result = execute(console_input);

                    std::vector<ScriptMessage> result_message;
                    std::move(messages.begin(), messages.end(), std::back_inserter(result_message));
                    messages.clear();

                    if (!result.empty())
                    {
                        ImVec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
                        if (result.starts_with("sol:"))
                        {
                            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                        }
                        result_message.push_back({
                            std::move(result),
                            {},
                            color
                            });
                    }

                    history_pos = std::nullopt;
                    history.push_back(ConsoleHistoryItem{
                        console_input,
                        std::move(result_message)
                        });
                    if (history.size() > max_history)
                    {
                        history.erase(history.begin());
                    }
                }
                std::memset(console_input, 0, IM_ARRAYSIZE(console_input));
                scroll_to_bottom = true;
            }
            set_focus = true;
        }
        ImGui::PopItemWidth();

        ImGui::SetWindowPos({ io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, io.DisplaySize.y / 2 - window_height / 2 }, ImGuiCond_Always);
        ImGui::End();
    }

    return true;
}

void LuaConsole::set_enabled(bool enabled)
{}
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
    return "lua_console";
}
const char* LuaConsole::get_root() const
{
    return ".";
}
const std::filesystem::path& LuaConsole::get_root_path() const
{
    static std::filesystem::path root_path{ "." };
    return root_path;
}

void LuaConsole::register_command(std::string provider_name, std::string command_name, sol::function cmd)
{
    lua[command_name] = std::move(cmd);
    console_commands[std::move(command_name)] = std::move(provider_name);
}
void LuaConsole::unregister_command(std::string provider_name, std::string command_name)
{
    if (console_commands[command_name] == provider_name)
    {
        lua[command_name] = sol::nil;
        console_commands.erase(std::move(command_name));
    }
}

std::string LuaConsole::execute(std::string code)
{
    try {
        if (!code.starts_with("return"))
        {
            try {
                return execute_raw("return " + code);
            }
            catch (const sol::error& e)
            {
                return execute_raw(std::move(code));
            }
        }
        else
        {
            return execute_raw(std::move(code));
        }
    }
    catch (const sol::error& e)
    {
        return e.what();
    }
}
std::string LuaConsole::execute_raw(std::string code)
{
    auto ret = lua.safe_script(code);
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

void LuaConsole::toggle()
{
    enabled = !enabled;
    set_focus = enabled;
    scroll_to_bottom = true;
}

std::string LuaConsole::dump_api()
{
    std::set<std::string> excluded_keys{ "meta" };

    sol::state dummy_state;
    dummy_state.open_libraries(sol::lib::math, sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::coroutine, sol::lib::package, sol::lib::debug);
    require_serpent_lua(dummy_state);

    for (auto& [key, value] : lua["_G"].get<sol::table>())
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
    for (auto& [key, value] : lua["_G"].get<sol::table>())
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
                value_str = {};
            }
            sorted_output[std::move(key_str)] = std::move(value_str);
        }
    }

    std::string api;
    for (auto& [key, value] : sorted_output)
        api += fmt::format("{} = {}\n", key, value);

    const static std::regex reg(R"("function:\s[0-9A-F]+")");
    api = std::regex_replace(api, reg, R"("function:")");

    return api;
}
