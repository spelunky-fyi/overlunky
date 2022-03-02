#pragma once

#include <cstdint>
#include <optional>

enum class GameSetting
{
    Gameplay_PreferredPet = 20,
    Gameplay_ScreenShake = 21,
    Gameplay_InstantRestart = 22,

    HudAndText_HudVisibility = 23,
    HudAndText_HudSize = 24,

    HudAndText_LevelTimer = 25,
    HudAndText_TimerDetail = 26,
    HudAndText_LevelNumber = 27,
    HudAndText_AngryShopkeeperIcon = 28,

    HudAndText_ButtonPrompts = 30,
    HudAndText_ButtonIcons = 31,

    HudAndText_TextBoxes = 33,
    HudAndText_TextDuration = 34,
    HudAndText_TextOpacity = 35,
    HudAndText_LevelFeelings = 36,
    HudAndText_DialogueText = 37,
    HudAndText_KaliText = 38,
    HudAndText_GhostText = 39,

    Language_Language = 40,

    Online_InputDelay = 43,
};

std::optional<std::uint8_t> get_setting(GameSetting setting);
