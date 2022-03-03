#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

enum class GAME_SETTING
{
    WINDOW_SCALE = 0,
    RESOLUTION_SCALE = 1,
    RESOLUTIONX = 2,
    RESOLUTIONY = 3,
    FREQUENCY_NUMERATOR = 4,
    FREQUENCY_DENOMINATOR = 5,
    WINDOW_MODE = 6,
    VSYNC = 7,
    MONITOR = 8,
    VFX = 9,
    BRIGHTNESS = 10,
    SOUND_ENABLED = 11,
    SOUND_VOLUME = 12,
    MUSIC_ENABLED = 13,
    MUSIC_VOLUME = 14,
    MASTER_ENABLED = 15,
    MASTER_VOLUME = 16,
    OVERSCAN = 17,
    CURRENT_PROFILE = 18,
    PREV_LANGUAGE = 19,
    DAMSEL_STYLE = 20,
    SCREEN_SHAKE = 21,
    INSTANT_RESTART = 22,
    HUD_STYLE = 23,
    HUD_SIZE = 24,
    LEVEL_TIMER = 25,
    TIMER_DETAIL = 26,
    LEVEL_NUMBER = 27,
    ANGRY_SHOPKEEPER = 28,
    CLASSIC_AGGRO_MUSIC = 29,
    BUTTON_PROMPTS = 30,
    BUTTON_TEXTURE = 31,
    FEAT_POPUPS = 32,
    TEXTBOX_SIZE = 33,
    TEXTBOX_DURATION = 34,
    TEXTBOX_OPACITY = 35,
    LEVEL_FEELINGS = 36,
    DIALOG_TEXT = 37,
    KALI_TEXT = 38,
    GHOST_TEXT = 39,
    LANGUAGE = 40,
    BRIGHT_FLASHES = 41,
    CROSSPLAY = 42,
    INPUT_DELAY = 43,
    OUTPUT_DELAY = 44,
    PSEUDONYMIZATION = 45,
    CROSSPROGRESS_ENABLED = 46,
    CROSSPROGRESS_AUTOSYNC = 47,
};

std::optional<std::uint32_t> get_setting(GAME_SETTING setting);
std::optional<std::uint32_t> get_setting_default(GAME_SETTING setting);
std::vector<std::pair<std::string_view, GAME_SETTING>> get_settings_names_and_indices();
