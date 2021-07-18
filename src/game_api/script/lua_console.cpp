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

        const float footer_height_to_reserve = style.ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
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

            for (const auto& result : item.messages)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, result.color);
                ImGui::TextUnformatted(result.message.c_str());
                ImGui::PopStyleColor();
            }
        }

        if (set_scroll)
        {
            ImGui::SetScrollHereY(set_scroll.value());
            set_scroll = std::nullopt;
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();

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
                const std::optional<size_t> prev_history_pos = self->history_pos;
                if (!self->history_pos.has_value())
                {
                    if (!self->history.empty())
                    {
                        self->history_pos = self->history.size() - 1;
                    }
                }
                else if (data->EventKey == ImGuiKey_UpArrow && self->history_pos.value() > 0)
                {
                    self->history_pos.value()--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow && self->history_pos.value() < self->history.size() - 1)
                {
                    self->history_pos.value()++;
                }

                if (prev_history_pos != self->history_pos)
                {
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, self->history[self->history_pos.value()].command.c_str());
                }
            }

            return 0;
        };

        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("", console_input, IM_ARRAYSIZE(console_input), input_text_flags, input_callback, this))
        {
            if (console_input[0] != '\0')
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
                std::memset(console_input, 0, IM_ARRAYSIZE(console_input));

                set_scroll = 1.0f;
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
    set_scroll = 1.0f;
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
