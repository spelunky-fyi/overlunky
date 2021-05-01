#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "fmod.hpp"
#include "audio_buffer.hpp"

namespace sol {
    template<bool b>
    class basic_reference;
    using reference = basic_reference<false>;
    template<typename T, bool, typename H>
    class basic_protected_function;
    using safe_function = basic_protected_function<reference, false, reference>;
    using protected_function = safe_function;
    using function = protected_function;
}


class SoundManager;
class PlayingSound;

using SoundCallbackFunction = sol::function;

enum class LoopMode {
    Off,
    Loop,
    Bidirectional
};

enum class SoundType {
    Sfx,
    Music
};

class CustomSound
{
    friend class SoundManager;

public:
    CustomSound(const CustomSound& rhs);
    CustomSound(CustomSound&& rhs) noexcept;
    CustomSound& operator=(const CustomSound& rhs) = delete;
    CustomSound& operator=(CustomSound&& rhs) = delete;
    ~CustomSound();

    operator bool() { return m_SoundManager != nullptr; }

    PlayingSound play();
    PlayingSound play(bool paused);
    PlayingSound play(bool paused, SoundType sound_type);

private:
    CustomSound(FMOD::Sound* fmod_sound, SoundManager* sound_manager);

    FMOD::Sound* m_FmodSound{ nullptr };
    SoundManager* m_SoundManager{ nullptr };
};

class PlayingSound {
    friend class SoundManager;

public:
    PlayingSound(const PlayingSound& rhs) = default;
    PlayingSound(PlayingSound&& rhs) noexcept = default;
    PlayingSound& operator=(const PlayingSound& rhs) = default;
    PlayingSound& operator=(PlayingSound&& rhs) noexcept = default;
    ~PlayingSound() = default;

    bool is_playing();
    bool stop();
    bool set_pause(bool pause);
    bool set_mute(bool mute);
    bool set_pitch(float pitch);
    bool set_pan(float pan);
    bool set_volume(float volume);
    bool set_looping(LoopMode loop_mode);
    bool set_callback(SoundCallbackFunction&& callback);

private:
    PlayingSound(FMOD::Channel* fmod_channel, SoundManager* sound_manager);

    FMOD::Channel* m_FmodChannel{ nullptr };
    SoundManager* m_SoundManager{ nullptr };
};

class SoundManager
{
public:
    SoundManager(DecodeAudioFile* decode_function);
    ~SoundManager();

    SoundManager(const SoundManager&) = delete;
    SoundManager(SoundManager&&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    SoundManager& operator=(SoundManager&&) = delete;

    CustomSound get_sound(std::string path);
    CustomSound get_sound(const char* path);
    void acquire_sound(FMOD::Sound* fmod_sound);
    void release_sound(FMOD::Sound* fmod_sound);
    PlayingSound play_sound(FMOD::Sound* fmod_sound, bool paused, bool as_music);

    bool is_playing(PlayingSound playing_sound);
    bool stop(PlayingSound playing_sound);
    bool set_pause(PlayingSound playing_sound, bool pause);
    bool set_mute(PlayingSound playing_sound, bool mute);
    bool set_pitch(PlayingSound playing_sound, float pitch);
    bool set_pan(PlayingSound playing_sound, float pan);
    bool set_volume(PlayingSound playing_sound, float volume);
    bool set_looping(PlayingSound playing_sound, LoopMode loop_mode);
    bool set_callback(PlayingSound playing_sound, SoundCallbackFunction&& callback);

private:
    DecodeAudioFile* m_DecodeFunction{ nullptr };

    void* m_FmodSystem{ nullptr };

    FMOD::CreateSound* m_CreateSound{ nullptr };
    FMOD::ReleaseSound* m_ReleaseSound{ nullptr };
    FMOD::PlaySound* m_PlaySound{ nullptr };

    FMOD::ChannelIsPlaying* m_ChannelIsPlaying{ nullptr };
    FMOD::ChannelStop* m_ChannelStop{ nullptr };
    FMOD::ChannelSetPaused* m_ChannelSetPaused{ nullptr };
    FMOD::ChannelSetMute* m_ChannelSetMute{ nullptr };
    FMOD::ChannelSetPitch* m_ChannelSetPitch{ nullptr };
    FMOD::ChannelSetPan* m_ChannelSetPan{ nullptr };
    FMOD::ChannelSetVolume* m_ChannelSetVolume{ nullptr };
    FMOD::ChannelSetMode* m_ChannelSetMode{ nullptr };
    FMOD::ChannelSetCallback* m_ChannelSetCallback{ nullptr };
    FMOD::ChannelSetUserData* m_ChannelSetUserData{ nullptr };
    FMOD::ChannelGetUserData* m_ChannelGetUserData{ nullptr };

    FMOD::ChannelGroup* m_SfxChannelGroup{ nullptr };
    FMOD::ChannelGroup* m_MusicChannelGroup{ nullptr };

    using EventId = std::uint32_t;
    struct EventProperties {
        std::string PropertyNames[38];
    };
    struct EventDescription {
        FMOD::EventDescription* Event;
        EventId Id;
        std::string Name;
        std::uint64_t _ull[46];
    };
    using EventMap = std::unordered_map<EventId, EventDescription>;
    struct SoundData {
        const EventProperties* Properties;
        const EventMap* Events;
    };
    static_assert(sizeof(EventDescription) == 0x1a0);
    SoundData m_SoundData;

    struct Sound;
    std::vector<Sound> m_SoundStorage;
};
