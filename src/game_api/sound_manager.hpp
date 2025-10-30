#pragma once

#include <array>
#include <cstddef> // IWYU pragma: keep
#include <cstdint>
#include <functional>
#include <list>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "aliases.hpp"
#include "audio_buffer.hpp"
#include "fmod.hpp"
#include "string_aliases.hpp" // for VANILLA_SOUND

class SoundManager;
class PlayingSound;

using SoundCallbackFunction = std::function<void()>;
using EventCallbackFunction = std::function<void(PlayingSound)>;

enum class SOUND_LOOP_MODE
{
    Off,
    Loop,
    Bidirectional
};

enum class SOUND_TYPE
{
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

    operator bool()
    {
        return m_SoundManager != nullptr;
    }

    PlayingSound play();
    PlayingSound play(bool paused);
    PlayingSound play(bool paused, SOUND_TYPE sound_type);

    std::unordered_map<VANILLA_SOUND_PARAM, const char*> get_parameters();

  private:
    CustomSound(std::nullptr_t, std::nullptr_t)
    {
    }
    CustomSound(FMOD::Sound* fmod_sound, SoundManager* sound_manager);
    CustomSound(FMODStudio::EventDescription* fmod_event, SoundManager* sound_manager);

    std::variant<FMOD::Sound*, FMODStudio::EventDescription*, std::monostate> m_FmodHandle{};
    SoundManager* m_SoundManager{nullptr};
};

using PlayingSoundHandle = std::variant<FMOD::Channel*, FMODStudio::EventInstance*, std::monostate>;
class PlayingSound
{
    friend class SoundManager;
    friend class CustomSound;
    friend FMOD::FMOD_RESULT EventInstanceCallback(FMODStudio::EventCallbackType, FMODStudio::EventInstance*, void*);

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
    bool set_looping(SOUND_LOOP_MODE loop_mode);
    bool set_callback(SoundCallbackFunction callback);

    std::unordered_map<VANILLA_SOUND_PARAM, const char*> get_parameters();
    std::optional<float> get_parameter(VANILLA_SOUND_PARAM parameter_index);
    bool set_parameter(VANILLA_SOUND_PARAM parameter_index, float value);

  private:
    PlayingSound(std::nullptr_t, std::nullptr_t)
    {
    }
    PlayingSound(FMOD::Channel* fmod_channel, SoundManager* sound_manager);
    PlayingSound(FMODStudio::EventInstance* fmod_event, SoundManager* sound_manager);

    PlayingSoundHandle m_FmodHandle{};
    SoundManager* m_SoundManager{nullptr};
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

    bool is_init() const
    {
        return m_IsInit;
    }

    CustomSound get_sound(std::string path);
    CustomSound get_sound(const char* path);
    CustomSound get_existing_sound(std::string_view path);
    void acquire_sound(FMOD::Sound* fmod_sound);
    void release_sound(FMOD::Sound* fmod_sound);
    PlayingSound play_sound(FMOD::Sound* fmod_sound, bool paused, bool as_music);

    CustomSound get_event(std::string_view event_name);
    PlayingSound play_event(FMODStudio::EventDescription* fmod_event, bool paused, bool as_music);

    bool is_playing(PlayingSound playing_sound);
    bool stop(PlayingSound playing_sound);
    bool set_pause(PlayingSound playing_sound, bool pause);
    bool set_mute(PlayingSound playing_sound, bool mute);
    bool set_pitch(PlayingSound playing_sound, float pitch);
    bool set_pan(PlayingSound playing_sound, float pan);
    bool set_volume(PlayingSound playing_sound, float volume);
    bool set_looping(PlayingSound playing_sound, SOUND_LOOP_MODE loop_mode);
    bool set_callback(PlayingSound playing_sound, SoundCallbackFunction callback);

    std::uint32_t set_callback(std::string_view event_name, EventCallbackFunction callback, FMODStudio::EventCallbackType types);
    std::uint32_t set_callback(FMODStudio::EventDescription* fmod_event, EventCallbackFunction callback, FMODStudio::EventCallbackType types);
    void clear_callback(std::uint32_t id);

    std::unordered_map<VANILLA_SOUND_PARAM, const char*> get_parameters(PlayingSound playing_sound);
    std::unordered_map<VANILLA_SOUND_PARAM, const char*> get_parameters(FMODStudio::EventDescription* fmod_event);
    std::optional<float> get_parameter(PlayingSound playing_sound, VANILLA_SOUND_PARAM parameter_index);
    bool set_parameter(PlayingSound playing_sound, VANILLA_SOUND_PARAM parameter_index, float value);

    template <class FunT>
    void for_each_event_name(FunT&& fun)
    {
        for (const auto& [id, event] : *m_SoundData.Events)
        {
            fun(event.Name);
        }
    }
    template <class FunT>
    void for_each_parameter_name(FunT&& fun)
    {
        for (size_t i = 0; i < m_SoundData.Parameters->ParameterNames.size(); i++)
        {
            const auto& parameter_name = m_SoundData.Parameters->ParameterNames[i];
            fun(parameter_name, static_cast<uint32_t>(i));
        }
    }
    SOUNDID convert_sound_id(const VANILLA_SOUND& s_name);
    const VANILLA_SOUND& convert_sound_id(SOUNDID id);

  private:
    bool m_IsInit{false};

    DecodeAudioFile* m_DecodeFunction{nullptr};

    FMOD::System* m_FmodSystem{nullptr};

    FMOD::CreateSound* m_CreateSound{nullptr};
    FMOD::ReleaseSound* m_ReleaseSound{nullptr};
    FMOD::PlaySound* m_PlaySound{nullptr};

    FMOD::ChannelIsPlaying* m_ChannelIsPlaying{nullptr};
    FMOD::ChannelStop* m_ChannelStop{nullptr};
    FMOD::ChannelSetPaused* m_ChannelSetPaused{nullptr};
    FMOD::ChannelSetMute* m_ChannelSetMute{nullptr};
    FMOD::ChannelSetPitch* m_ChannelSetPitch{nullptr};
    FMOD::ChannelSetPan* m_ChannelSetPan{nullptr};
    FMOD::ChannelSetVolume* m_ChannelSetVolume{nullptr};
    FMOD::ChannelSetMode* m_ChannelSetMode{nullptr};
    FMOD::ChannelSetCallback* m_ChannelSetCallback{nullptr};
    FMOD::ChannelSetUserData* m_ChannelSetUserData{nullptr};
    FMOD::ChannelGetUserData* m_ChannelGetUserData{nullptr};

    FMODStudio::EventDescriptionCreateInstance* m_EventCreateInstance{nullptr};
    FMODStudio::EventDescriptionGetParameterDescriptionByID* m_EventDescriptionGetParameterDescriptionByID{nullptr};
    FMODStudio::EventDescriptionGetParameterDescriptionByName* m_EventDescriptionGetParameterDescriptionByName{nullptr};
    FMODStudio::EventDescriptionSetCallback* m_EventDescriptionSetCallback{nullptr};

    FMODStudio::EventInstanceStart* m_EventInstanceStart{nullptr};
    FMODStudio::EventInstanceStop* m_EventInstanceStop{nullptr};
    FMODStudio::EventInstanceGetPlaybackState* m_EventInstanceGetPlaybackState{nullptr};
    FMODStudio::EventInstanceSetPaused* m_EventInstanceSetPaused{nullptr};
    FMODStudio::EventInstanceGetPaused* m_EventInstanceGetPaused{nullptr};
    FMODStudio::EventInstanceSetPitch* m_EventInstanceSetPitch{nullptr};
    FMODStudio::EventInstanceSetVolume* m_EventInstanceSetVolume{nullptr};
    FMODStudio::EventInstanceSetCallback* m_EventInstanceSetCallback{nullptr};
    FMODStudio::EventInstanceSetUserData* m_EventInstanceSetUserData{nullptr};
    FMODStudio::EventInstanceGetUserData* m_EventInstanceGetUserData{nullptr};
    FMODStudio::EventInstanceGetDescription* m_EventInstanceGetDescription{nullptr};
    FMODStudio::EventInstanceGetParameterByID* m_EventInstanceGetParameterByID{nullptr};
    FMODStudio::EventInstanceSetParameterByID* m_EventInstanceSetParameterByID{nullptr};

    FMOD::ChannelGroup* m_SfxChannelGroup{nullptr};
    FMOD::ChannelGroup* m_MusicChannelGroup{nullptr};

    using EventId = std::uint32_t;
    struct EventParameters
    {
        std::array<std::string, 38> ParameterNames;
    };
    struct EventDescription
    {
        FMODStudio::EventDescription* Event;
        EventId Id;
        std::string Name;
        std::array<FMODStudio::ParameterId, 38> Parameters;
        std::array<bool, 38> HasParameter;
        std::uint64_t _ull0;
        std::uint32_t _u0;
        std::uint32_t _u1;
        std::uint32_t _u2;
    };
    using EventMap = std::unordered_map<EventId, EventDescription>;
    struct SoundData
    {
        const EventParameters* Parameters;
        const EventMap* Events;
        std::unordered_map<const FMODStudio::EventDescription*, const EventDescription*> FmodEventToEvent;
        std::unordered_map<std::string_view, const EventDescription*> NameToEvent;
    };
    static_assert(sizeof(EventDescription) == 0x1a0);
    SoundData m_SoundData;

    struct Sound;

    std::vector<Sound> m_SoundStorage;
};

struct SoundInfo
{
    int64_t unknown1;
    SOUNDID sound_id;
    int32_t unknown2; // padding probably
    std::string sound_name;
};

// there is actually base class that consists of up to the left/right channel, used for the sfx
// then normal sounds that you can find in entities, screens itp. have up to the paddings
// finally music in GameManages has the extra bool in BackgroundSound
struct SoundMeta
{
    float x;
    float y;
    SoundInfo* sound_info;  // param to FMOD::Studio::EventInstance::SetParameterByID (this ptr + 0x30)
    uint64_t fmod_param_id; // param to FMOD::Studio::EventInstance::SetParameterByID

    /// Use VANILLA_SOUND_PARAM as index, warning: special case with first index at 0, loop using pairs will get you all results but the key/index will be wrong, ipairs will have correct key/index but will skip the first element
    std::array<float, 38> left_channel;
    /// Use VANILLA_SOUND_PARAM as index warning: special case with first index at 0, loop using pairs will get you all results but the key/index will be wrong, ipairs will have correct key/index but will skip the first element
    std::array<float, 38> right_channel;

    /// when false, current track starts from the beginning, is immediately set back to true
    bool start_over;
    bool play_ending_sequence;
    /// set to false to turn off
    bool playing;
    uint8_t padding1;
    uint32_t padding2;

    virtual void start() = 0;             // just sets music_on to true in most cases
    virtual void kill(bool fade_out) = 0; // fade_out - set's the play_ending_sequence
    virtual void get_name(char16_t* buffor, uint32_t size) = 0;
    virtual ~SoundMeta() = 0;
    virtual void update() = 0; // disabling this function does not progresses the track, does not stop it at the end level etc.
                               // like if you start a level you have one loop and then after you move, it progresses to another one
    virtual bool unknown() = 0;
};

struct BackgroundSound : public SoundMeta
{
    bool destroy_sound; // don't use directly, use the kill function
};

SoundMeta* play_sound(VANILLA_SOUND sound, uint32_t source_uid);
SoundMeta* play_sound(SOUNDID sound_id, uint32_t source_uid);

// could probably be exposed if someone can actually figure out how to properly "register it"?
// it probably also needs to make sure the lua owns the returned object and it will properly delete it
SoundMeta* construct_soundmeta(VANILLA_SOUND sound, bool background_sound);
SoundMeta* construct_soundmeta(SOUNDID sound_id, bool background_sound);
/*
VANILLA_SOUND convert_sound_id(SOUNDID id); // for the autodoc
/// Convert SOUNDID to VANILLA_SOUND and vice versa
SOUNDID convert_sound_id(VANILLA_SOUND sound);
*/
