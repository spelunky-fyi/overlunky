#include "settings_api.hpp"

#include "search.hpp"

union SettingValue
{
    std::uint32_t u32;
    std::uint16_t u16;
    std::uint8_t u8;
};
struct SettingData
{
    std::uint8_t u8_00;
    std::uint16_t u16_01;
    std::uint8_t setting;
    std::uint16_t u16_08;
    std::uint32_t u32_0a;
    std::uint16_t u16_0e;
    const char* setting_name;
    SettingValue default_value;
    SettingValue& value;
};
constexpr auto b = sizeof(SettingData);

SettingData* get_setting_data(GAME_SETTING setting)
{
    static auto* settings_map = (std::map<GAME_SETTING, SettingData>*)get_address("settings_map");
    if (settings_map->contains(setting))
    {
        return &settings_map->at(setting);
    }
    else
    {
        static auto* graphics_and_audio_settings_map = (std::map<GAME_SETTING, SettingData>*)get_address("graphics_settings_map");
        if (graphics_and_audio_settings_map->contains(setting))
        {
            return &graphics_and_audio_settings_map->at(setting);
        }
    }

    return nullptr;
}

std::optional<std::uint32_t> get_setting(GAME_SETTING setting)
{
    if (SettingData* data = get_setting_data(setting))
    {
        switch (setting)
        {
        case GAME_SETTING::FREQUENCY_NUMERATOR:
        case GAME_SETTING::FREQUENCY_DENOMINATOR:
            return data->value.u32;
        case GAME_SETTING::RESOLUTIONX:
        case GAME_SETTING::RESOLUTIONY:
            return data->value.u16;
        default:
            return data->value.u8;
        }
    }

    return std::nullopt;
}
std::optional<std::uint32_t> get_setting_default(GAME_SETTING setting)
{
    if (SettingData* data = get_setting_data(setting))
    {
        switch (setting)
        {
        case GAME_SETTING::FREQUENCY_NUMERATOR:
        case GAME_SETTING::FREQUENCY_DENOMINATOR:
            return data->default_value.u32;
        case GAME_SETTING::RESOLUTIONX:
        case GAME_SETTING::RESOLUTIONY:
            return data->default_value.u16;
        default:
            return data->default_value.u8;
        }
    }

    return std::nullopt;
}
std::vector<std::pair<std::string_view, GAME_SETTING>> get_settings_names_and_indices()
{
    std::vector<std::pair<std::string_view, GAME_SETTING>> names_and_indices;
    [[maybe_unused]] std::unordered_set<GAME_SETTING> indices;

    {
        auto* settings_map = (std::map<GAME_SETTING, SettingData>*)get_address("graphics_settings_map");
        for (auto& [setting, data] : *settings_map)
        {
            names_and_indices.push_back({data.setting_name, setting});
            indices.insert(setting);
        }
    }

    {
        auto* settings_map = (std::map<GAME_SETTING, SettingData>*)get_address("settings_map");
        for (auto& [setting, data] : *settings_map)
        {
            names_and_indices.push_back({data.setting_name, setting});
            assert(!indices.contains(setting));
        }
    }

    return names_and_indices;
}
