#include "settings_api.hpp"

#include <assert.h>      // for assert
#include <cstring>       // for strstr, size_t
#include <functional>    // for equal_to
#include <map>           // for map, _Tree_iterator, _Tree_const_iterator
#include <new>           // for operator new
#include <type_traits>   // for hash, move, conditional_t
#include <unordered_set> // for unordered_set, _Uset_traits<>::allocator_type

#include "game_api.hpp" //
#include "search.hpp"   // for get_address

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

static std::map<GAME_SETTING, uint32_t> g_original_settings;
static bool g_lock_settings{false};

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

void save_original_setting(GAME_SETTING setting)
{
    if (g_original_settings.contains(setting))
        return;
    if (auto original = get_setting(setting))
        g_original_settings[setting] = original.value();
}

void restore_original_settings()
{
    for (auto& [k, v] : g_original_settings)
        set_setting(k, v);
    g_original_settings.clear();
    g_lock_settings = true;
}

void unlock_settings()
{
    g_lock_settings = false;
}

bool set_setting(GAME_SETTING setting, std::uint32_t value)
{
    if (g_lock_settings)
        return false;

    if (SettingData* data = get_setting_data(setting))
    {
        switch (setting)
        {
        case GAME_SETTING::FREQUENCY_NUMERATOR:
        case GAME_SETTING::FREQUENCY_DENOMINATOR:
            data->value.u32 = value;
            return true;
        case GAME_SETTING::RESOLUTIONX:
        case GAME_SETTING::RESOLUTIONY:
            data->value.u16 = value & 0xffff;
            return data->value.u16 == value;
        default:
            data->value.u8 = value & 0xff;
            return data->value.u8 == value;
        }
    }

    return false;
}
std::optional<std::uint32_t> get_setting(GAME_SETTING setting)
{
    if ((setting == GAME_SETTING::FREQUENCY_NUMERATOR || setting == GAME_SETTING::FREQUENCY_DENOMINATOR) && get_setting(GAME_SETTING::WINDOW_MODE) == 1u)
    {
        auto game_api = GameAPI::get();
        if (setting == GAME_SETTING::FREQUENCY_NUMERATOR)
            return game_api->renderer->fps;
        else
            return game_api->renderer->fps_denominator;
    }

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
        static auto* settings_map = (std::map<GAME_SETTING, SettingData>*)get_address("graphics_settings_map");
        for (auto& [setting, data] : *settings_map)
        {
            names_and_indices.push_back({data.setting_name, setting});
            indices.insert(setting);
        }
    }

    {
        static auto* settings_map = (std::map<GAME_SETTING, SettingData>*)get_address("settings_map");
        for (auto& [setting, data] : *settings_map)
        {
            if (strstr(data.setting_name, "damsel"))
            {
                names_and_indices.push_back({"pet_style", setting});
            }
            else
            {
                names_and_indices.push_back({data.setting_name, setting});
            }
            assert(!indices.contains(setting));
        }
    }

    return names_and_indices;
}
