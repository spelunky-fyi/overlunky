#include "settings_api.hpp"

#include "search.hpp"

#include <array>

std::optional<std::uint8_t> get_setting(GameSetting setting)
{
    struct SettingsMapNode
    {
        SettingsMapNode* left;
        SettingsMapNode* right;
        SettingsMapNode* parent;
        int value;
        int padding_0;
        std::array<char, 0x28> padding_1;
        std::uint8_t* data;
    };
    constexpr auto a = sizeof(SettingsMapNode);
    using TryEmplace = void(void*, std::pair<SettingsMapNode*, bool>&, int);

    void* settings_map = (void*)get_address("settings_map");
    auto* try_emplace = (TryEmplace*)get_address("settings_map::_Try_emplace");

    std::pair<SettingsMapNode*, bool> emplace_result;
    try_emplace(settings_map, emplace_result, (int)setting);
    if (emplace_result.first->data)
    {
        return *emplace_result.first->data;
    }

    return std::nullopt;
}
