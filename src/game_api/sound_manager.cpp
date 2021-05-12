#include "sound_manager.hpp"

#include "logger.h"
#include "memory.hpp"
#include "overloaded.hpp"

#include <mutex>
#include <string>

#include <Windows.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/function.hpp>

struct SoundCallbackData
{
    SoundCallbackData(const SoundCallbackData &) = delete;
    SoundCallbackData &operator=(const SoundCallbackData &) = delete;
    SoundCallbackData(SoundCallbackData &&) = default;
    SoundCallbackData &operator=(SoundCallbackData &&) = default;

    FMOD::Channel *handle;
    SoundCallbackFunction callback;
};
std::mutex s_SoundCallbacksMutex;
std::vector<SoundCallbackData> s_SoundCallbacks;
FMOD::FMOD_RESULT ChannelControlCallback(
    FMOD::ChannelControl *channel_control,
    FMOD::ChannelControlType channel_control_type,
    FMOD::ChannelControlCallbackType channel_control_callback_type,
    void *calback_data_1,
    void *calback_data_2)
{
    if (channel_control_type == FMOD::ChannelControlType::Channel && channel_control_callback_type == FMOD::ChannelControlCallbackType::End)
    {
        SoundCallbackFunction snd_callback;

        {
            FMOD::Channel *channel = reinterpret_cast<FMOD::Channel *>(channel_control);

            std::lock_guard lock{s_SoundCallbacksMutex};
            auto it = std::find_if(
                s_SoundCallbacks.begin(),
                s_SoundCallbacks.end(),
                [channel](const SoundCallbackData &callback) { return callback.handle == channel; });
            if (it != s_SoundCallbacks.end())
            {
                snd_callback = std::move(it->callback);
                s_SoundCallbacks.erase(it);
            }
        }

        if (snd_callback)
        {
            snd_callback();
        }
    }
    // TODO: Cleanup old channels?
    return FMOD::OK;
}

struct EventCallbackData
{
    EventCallbackData(const EventCallbackData &) = delete;
    EventCallbackData &operator=(const EventCallbackData &) = delete;
    EventCallbackData(EventCallbackData &&) = default;
    EventCallbackData &operator=(EventCallbackData &&) = default;

    struct Callback
    {
        std::uint32_t id;
        EventCallbackFunction cb;
    };

    FMODStudio::EventDescription *handle;
    std::unordered_map<FMODStudio::EventCallbackType, std::vector<Callback>> callbacks;
    std::unordered_map<FMODStudio::EventInstance *, SoundCallbackFunction> specific_callbacks;
    SoundManager *sound_manager;

    inline static FMODStudio::EventInstanceGetDescription *EventInstanceGetDescription{nullptr};
};
std::mutex s_EventCallbacksMutex;
std::uint32_t current_callback_id{0};
std::vector<EventCallbackData> s_EventCallbacks;
std::unordered_map<std::uint32_t, FMODStudio::EventDescription *> s_EventCallbackIdToEventDescription;
FMOD::FMOD_RESULT EventInstanceCallback(FMODStudio::EventCallbackType callback_type, FMODStudio::EventInstance *instance, void *callback_data)
{
    FMODStudio::EventDescription *event;
    if (FMOD_CHECK_CALL(EventCallbackData::EventInstanceGetDescription(instance, &event)))
    {
        SoundManager *sound_manager{nullptr};
        std::vector<EventCallbackFunction> evt_callbacks;
        SoundCallbackFunction snd_callback;

        {
            std::lock_guard lock{s_EventCallbacksMutex};
            auto it = std::find_if(
                s_EventCallbacks.begin(), s_EventCallbacks.end(), [event](const EventCallbackData &callback) { return callback.handle == event; });
            if (it != s_EventCallbacks.end())
            {
                sound_manager = it->sound_manager;

                for (auto &[type, cbs] : it->callbacks)
                {
                    if (type & callback_type)
                    {
                        // TODO: Expose SoundStarted/SoundStopped?
                        for (auto &cb : cbs)
                        {
                            evt_callbacks.push_back(cb.cb);
                        }
                    }
                }

                if (callback_type == FMODStudio::EventCallbackType::Stopped)
                {
                    auto specific_it = it->specific_callbacks.find(instance);
                    if (specific_it != it->specific_callbacks.end())
                    {
                        snd_callback = std::move(specific_it->second);
                    }
                    it->specific_callbacks.erase(instance);
                }
            }
        }

        for (const EventCallbackFunction &evt_callback : evt_callbacks)
        {
            evt_callback(PlayingSound{instance, sound_manager});
        }
        if (snd_callback)
        {
            snd_callback();
        }
    }
    return FMOD::OK;
}

CustomSound::CustomSound(const CustomSound &rhs) : m_FmodHandle{rhs.m_FmodHandle}, m_SoundManager{rhs.m_SoundManager}
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [this](FMOD::Sound *sound) { m_SoundManager->acquire_sound(sound); },
                [](FMODStudio::EventDescription *) {},
                [](std::monostate) {},
            },
            rhs.m_FmodHandle);
    }
}
CustomSound::CustomSound(CustomSound &&rhs) noexcept
{
    std::swap(m_FmodHandle, rhs.m_FmodHandle);
    std::swap(m_SoundManager, rhs.m_SoundManager);
}
CustomSound::CustomSound(FMOD::Sound *fmod_sound, SoundManager *sound_manager) : m_FmodHandle{fmod_sound}, m_SoundManager{sound_manager}
{
}
CustomSound::CustomSound(FMODStudio::EventDescription *fmod_event, SoundManager *sound_manager)
    : m_FmodHandle{fmod_event}, m_SoundManager{sound_manager}
{
}

CustomSound::~CustomSound()
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [this](FMOD::Sound *sound) { m_SoundManager->acquire_sound(sound); },
                [this](FMODStudio::EventDescription *) {},
                [](std::monostate) {},
            },
            m_FmodHandle);
    }
}

PlayingSound CustomSound::play()
{
    return play(false, SoundType::Sfx);
}
PlayingSound CustomSound::play(bool paused)
{
    return play(paused, SoundType::Sfx);
}
PlayingSound CustomSound::play(bool paused, SoundType sound_type)
{
    return std::visit(
        overloaded{
            [=](FMOD::Sound *sound) { return m_SoundManager->play_sound(sound, paused, sound_type == SoundType::Music); },
            [=](FMODStudio::EventDescription *event) { return m_SoundManager->play_event(event, paused, sound_type == SoundType::Music); },
            [](std::monostate) {
                return PlayingSound{nullptr, nullptr};
            },
        },
        m_FmodHandle);
}

std::unordered_map<std::uint32_t, const char *> CustomSound::get_parameters()
{
    return std::visit(
        overloaded{
            [](FMOD::Sound *sound) { return std::unordered_map<std::uint32_t, const char *>{}; },
            [this](FMODStudio::EventDescription *event) { return m_SoundManager->get_parameters(event); },
            [](std::monostate) { return std::unordered_map<std::uint32_t, const char *>{}; },
        },
        m_FmodHandle);
}

PlayingSound::PlayingSound(FMOD::Channel *fmod_channel, SoundManager *sound_manager) : m_FmodHandle{fmod_channel}, m_SoundManager{sound_manager}
{
}
PlayingSound::PlayingSound(FMODStudio::EventInstance *fmod_event, SoundManager *sound_manager)
    : m_FmodHandle{fmod_event}, m_SoundManager{sound_manager}
{
}

bool PlayingSound::is_playing()
{
    return m_SoundManager->is_playing(*this);
}
bool PlayingSound::stop()
{
    return m_SoundManager->stop(*this);
}
bool PlayingSound::set_pause(bool pause)
{
    return m_SoundManager->set_pause(*this, pause);
}
bool PlayingSound::set_mute(bool mute)
{
    return m_SoundManager->set_mute(*this, mute);
}
bool PlayingSound::set_pitch(float pitch)
{
    return m_SoundManager->set_pitch(*this, pitch);
}
bool PlayingSound::set_pan(float pan)
{
    return m_SoundManager->set_pan(*this, pan);
}
bool PlayingSound::set_volume(float volume)
{
    return m_SoundManager->set_volume(*this, volume);
}
bool PlayingSound::set_looping(LoopMode loop_mode)
{
    return m_SoundManager->set_looping(*this, loop_mode);
}
bool PlayingSound::set_callback(SoundCallbackFunction callback)
{
    return m_SoundManager->set_callback(*this, std::move(callback));
}

std::unordered_map<std::uint32_t, const char *> PlayingSound::get_parameters()
{
    return m_SoundManager->get_parameters(*this);
}
std::optional<float> PlayingSound::get_parameter(std::uint32_t parameter_index)
{
    return m_SoundManager->get_parameter(*this, parameter_index);
}
bool PlayingSound::set_parameter(std::uint32_t parameter_index, float value)
{
    return m_SoundManager->set_parameter(*this, parameter_index, value);
}

struct SoundManager::Sound
{
    std::uint32_t ref_count;
    DecodedAudioBuffer buffer;
    std::string path;
    FMOD::Sound *fmod_sound{nullptr};
};

SoundManager::SoundManager(DecodeAudioFile *decode_function) : m_DecodeFunction{decode_function}
{
    if (HMODULE fmod_studio = GetModuleHandle("fmodstudio.dll"))
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto start = memory.after_bundle;

        auto fmod_studio_system_instruction = find_inst(exe, "\x48\x8d", find_inst(exe, "\x85\xc0\x74\x2b\x44\x8b\xc0"s, start) - 0x10);
        auto fmod_studio_system = *(FMODStudio::System **)memory.at_exe(decode_pc(exe, fmod_studio_system_instruction));

        auto event_properties_instruction = find_inst(exe, "\x48\x8d\xbd\x90\x00\x00\x00\x48\x8d\x74\x24\x60", start) + 0xc;
        m_SoundData.Parameters = (const EventParameters *)memory.at_exe(decode_pc(exe, event_properties_instruction));

        auto event_map_instruction = find_inst(exe, "\x48\x8b\x08\x48\x83\xc1\x18\x48\x8d\x54\x24\x30", start) - 0xc;
        m_SoundData.Events = (const EventMap *)memory.at_exe(decode_pc(exe, event_map_instruction));
        for (const auto &[id, event] : *m_SoundData.Events)
        {
            m_SoundData.FmodEventToEvent[event.Event] = &event;
            m_SoundData.NameToEvent[event.Name] = &event;
        }

        auto get_core_system = reinterpret_cast<FMODStudio::GetCoreSystem *>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetCoreSystem"));
        {
            auto err = get_core_system(fmod_studio_system, &m_FmodSystem);
            if (err != FMOD::OK)
            {
                DEBUG("Could not get Fmod System, custom audio won't work...");
            }
        }

        auto flush_commands = reinterpret_cast<FMODStudio::FlushCommands *>(GetProcAddress(fmod_studio, "FMOD_Studio_System_FlushCommands"));
        auto get_bus = reinterpret_cast<FMODStudio::GetBus *>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetBus"));
        auto lock_channel_group = reinterpret_cast<FMODStudio::LockChannelGroup *>(GetProcAddress(fmod_studio, "FMOD_Studio_Bus_LockChannelGroup"));
        auto get_channel_group = reinterpret_cast<FMODStudio::GetChannelGroup *>(GetProcAddress(fmod_studio, "FMOD_Studio_Bus_GetChannelGroup"));

        auto get_channel_group_from_bus_name = [=](const char *bus_name, FMOD::ChannelGroup **channel_group)
        {
            FMODStudio::Bus *bus{nullptr};
            auto err = get_bus(fmod_studio_system, bus_name, &bus);
            if (err != FMOD::OK)
            {
                DEBUG("Could not get bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
            }
            else
            {
                err = lock_channel_group(bus);
                if (err != FMOD::OK)
                {
                    DEBUG("Could not lock channel group for bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
                }
                else
                {
                    err = flush_commands(fmod_studio_system);
                    if (err != FMOD::OK)
                    {
                        DEBUG(
                            "Could not flush commands after locking channel group for bus '{}', custom audio volume won't be synced with game volume "
                            "properly...",
                            bus_name);
                    }
                    else
                    {
                        err = get_channel_group(bus, channel_group);
                        if (err != FMOD::OK)
                        {
                            DEBUG(
                                "Could not obtain channel group for bus '{}', custom audio volume won't be synced with game volume properly...",
                                bus_name);
                        }
                    }
                }
            }
        };
        get_channel_group_from_bus_name("bus:/Master_SUM/Master_SFX", &m_SfxChannelGroup);
        get_channel_group_from_bus_name("bus:/Master_SUM/Master_BGM", &m_MusicChannelGroup);

        m_EventCreateInstance = reinterpret_cast<FMODStudio::EventDescriptionCreateInstance *>(
            GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_CreateInstance"));
        m_EventDescriptionGetParameterDescriptionByName = reinterpret_cast<FMODStudio::EventDescriptionGetParameterDescriptionByName *>(
            GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_GetParameterDescriptionByName"));
        m_EventDescriptionGetParameterDescriptionByID = reinterpret_cast<FMODStudio::EventDescriptionGetParameterDescriptionByID *>(
            GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_GetParameterDescriptionByID"));
        m_EventDescriptionSetCallback =
            reinterpret_cast<FMODStudio::EventDescriptionSetCallback *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_SetCallback"));

        m_EventInstanceStart = reinterpret_cast<FMODStudio::EventInstanceStart *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_Start"));
        m_EventInstanceStop = reinterpret_cast<FMODStudio::EventInstanceStop *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_Stop"));
        m_EventInstanceGetPlaybackState =
            reinterpret_cast<FMODStudio::EventInstanceGetPlaybackState *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetPlaybackState"));
        m_EventInstanceSetPaused =
            reinterpret_cast<FMODStudio::EventInstanceSetPaused *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetPaused"));
        m_EventInstanceGetPaused =
            reinterpret_cast<FMODStudio::EventInstanceGetPaused *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetPaused"));
        m_EventInstanceSetPitch =
            reinterpret_cast<FMODStudio::EventInstanceSetPitch *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetPitch"));
        m_EventInstanceSetVolume =
            reinterpret_cast<FMODStudio::EventInstanceSetVolume *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetVolume"));
        m_EventInstanceSetCallback =
            reinterpret_cast<FMODStudio::EventInstanceSetCallback *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetCallback"));
        m_EventInstanceSetUserData =
            reinterpret_cast<FMODStudio::EventInstanceSetUserData *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetUserData"));
        m_EventInstanceGetUserData =
            reinterpret_cast<FMODStudio::EventInstanceGetUserData *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetUserData"));
        m_EventInstanceGetDescription =
            reinterpret_cast<FMODStudio::EventInstanceGetDescription *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetDescription"));
        m_EventInstanceGetParameterByID =
            reinterpret_cast<FMODStudio::EventInstanceGetParameterByID *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetParameterByID"));
        m_EventInstanceSetParameterByID =
            reinterpret_cast<FMODStudio::EventInstanceSetParameterByID *>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetParameterByID"));

        EventCallbackData::EventInstanceGetDescription = m_EventInstanceGetDescription;
    }
    else
    {
        DEBUG("fmodstudio.dll was not loaded, custom audio won't work...");
    }

    if (HMODULE fmod = GetModuleHandle("fmod.dll"))
    {
        m_CreateSound = reinterpret_cast<FMOD::CreateSound *>(GetProcAddress(fmod, "FMOD_System_CreateSound"));
        m_ReleaseSound = reinterpret_cast<FMOD::ReleaseSound *>(GetProcAddress(fmod, "FMOD_Sound_Release"));
        m_PlaySound = reinterpret_cast<FMOD::PlaySound *>(GetProcAddress(fmod, "FMOD_System_PlaySound"));

        m_ChannelIsPlaying = reinterpret_cast<FMOD::ChannelIsPlaying *>(GetProcAddress(fmod, "FMOD_Channel_IsPlaying"));
        m_ChannelStop = reinterpret_cast<FMOD::ChannelStop *>(GetProcAddress(fmod, "FMOD_Channel_Stop"));
        m_ChannelSetPaused = reinterpret_cast<FMOD::ChannelSetPaused *>(GetProcAddress(fmod, "FMOD_Channel_SetPaused"));
        m_ChannelSetMute = reinterpret_cast<FMOD::ChannelSetMute *>(GetProcAddress(fmod, "FMOD_Channel_SetMute"));
        m_ChannelSetPitch = reinterpret_cast<FMOD::ChannelSetPitch *>(GetProcAddress(fmod, "FMOD_Channel_SetPitch"));
        m_ChannelSetPan = reinterpret_cast<FMOD::ChannelSetPan *>(GetProcAddress(fmod, "FMOD_Channel_SetPan"));
        m_ChannelSetVolume = reinterpret_cast<FMOD::ChannelSetVolume *>(GetProcAddress(fmod, "FMOD_Channel_SetVolume"));
        m_ChannelSetMode = reinterpret_cast<FMOD::ChannelSetMode *>(GetProcAddress(fmod, "FMOD_Channel_SetMode"));
        m_ChannelSetCallback = reinterpret_cast<FMOD::ChannelSetCallback *>(GetProcAddress(fmod, "FMOD_Channel_SetCallback"));
        m_ChannelSetUserData = reinterpret_cast<FMOD::ChannelSetUserData *>(GetProcAddress(fmod, "FMOD_Channel_SetUserData"));
        m_ChannelGetUserData = reinterpret_cast<FMOD::ChannelGetUserData *>(GetProcAddress(fmod, "FMOD_Channel_GetUserData"));
    }
    else
    {
        DEBUG("fmod.dll was not loaded, custom audio won't work...");
    }
}
SoundManager::~SoundManager()
{
    for (Sound &sound : m_SoundStorage)
    {
        m_ReleaseSound(sound.fmod_sound);
    }
}

CustomSound SoundManager::get_sound(std::string path)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [&path](const Sound &sound) { return sound.path == path; });
    if (it != m_SoundStorage.end())
    {
        it->ref_count++;
        return CustomSound{it->fmod_sound, this};
    }

    DecodedAudioBuffer buffer;
    try
    {
        buffer = m_DecodeFunction(path.c_str());
    }
    catch (std::exception &except)
    {
        DEBUG("Failed loading audio file {}\n{}", path, except.what());
        return CustomSound{nullptr, nullptr};
    }

    Sound new_sound;
    new_sound.ref_count = 1;
    new_sound.buffer = std::move(buffer);
    new_sound.path = std::move(path);

    FMOD::FMOD_MODE mode =
        (FMOD::FMOD_MODE)(FMOD::MODE_CREATESAMPLE | FMOD::MODE_OPENMEMORY_POINT | FMOD::MODE_OPENRAW | FMOD::MODE_IGNORETAGS | FMOD::MODE_LOOP_OFF);

    FMOD::CREATESOUNDEXINFO create_sound_exinfo{};
    create_sound_exinfo.cbsize = sizeof(create_sound_exinfo);
    create_sound_exinfo.length = (std::uint32_t)new_sound.buffer.data_size;
    create_sound_exinfo.numchannels = new_sound.buffer.num_channels;
    create_sound_exinfo.defaultfrequency = new_sound.buffer.frequency;
    create_sound_exinfo.format = [path](SoundFormat format)
    {
        switch (format)
        {
        default:
            DEBUG("Sound format is not supported for file {}...", path);
            return FMOD::SOUND_FORMAT::NONE;
        case SoundFormat::PCM_8:
            return FMOD::SOUND_FORMAT::PCM8;
        case SoundFormat::PCM_16:
            return FMOD::SOUND_FORMAT::PCM16;
        case SoundFormat::PCM_24:
            return FMOD::SOUND_FORMAT::PCM24;
        case SoundFormat::PCM_32:
            return FMOD::SOUND_FORMAT::PCM32;
        case SoundFormat::PCM_FLOAT:
            return FMOD::SOUND_FORMAT::PCMFLOAT;
        }
    }(new_sound.buffer.format);

    FMOD::FMOD_RESULT err = m_CreateSound(m_FmodSystem, (const char *)new_sound.buffer.data.get(), mode, &create_sound_exinfo, &new_sound.fmod_sound);
    if (err != FMOD::OK)
    {
        return CustomSound{nullptr, nullptr};
    }

    m_SoundStorage.push_back(std::move(new_sound));
    return CustomSound{m_SoundStorage.back().fmod_sound, this};
}
CustomSound SoundManager::get_sound(const char *path)
{
    return get_sound(std::string{path});
}
CustomSound SoundManager::get_existing_sound(std::string_view path)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [&path](const Sound &sound) { return sound.path == path; });
    if (it != m_SoundStorage.end())
    {
        it->ref_count++;
        return CustomSound{it->fmod_sound, this};
    }
    return CustomSound{nullptr, nullptr};
}
void SoundManager::acquire_sound(FMOD::Sound *fmod_sound)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound](const Sound &sound) { return sound.fmod_sound == fmod_sound; });
    if (it == m_SoundStorage.end())
    {
        DEBUG("Trying to acquire sound that does not exist...");
        return;
    }

    it->ref_count++;
}
void SoundManager::release_sound(FMOD::Sound *fmod_sound)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound](const Sound &sound) { return sound.fmod_sound == fmod_sound; });
    if (it == m_SoundStorage.end())
    {
        DEBUG("Trying to release sound that does not exist...");
        return;
    }

    // TODO: Really worth releasing or should we just keep this for eternity?
    if (it->ref_count == 1)
    {
        m_ReleaseSound(it->fmod_sound);
        m_SoundStorage.erase(it);
    }
    else
    {
        it->ref_count--;
    }
}
PlayingSound SoundManager::play_sound(FMOD::Sound *fmod_sound, bool paused, bool as_music)
{
    FMOD::Channel *channel{nullptr};
    m_PlaySound(m_FmodSystem, fmod_sound, as_music ? m_MusicChannelGroup : m_SfxChannelGroup, paused, &channel);
    return PlayingSound{channel, this};
}

CustomSound SoundManager::get_event(std::string_view event_name)
{
    auto it = m_SoundData.NameToEvent.find(event_name);
    if (it != m_SoundData.NameToEvent.end())
    {
        return CustomSound{it->second->Event, this};
    }
    return CustomSound{nullptr, nullptr};
}
PlayingSound SoundManager::play_event(FMODStudio::EventDescription *fmod_event, bool paused, bool as_music)
{
    FMODStudio::EventInstance *instance{nullptr};
    m_EventCreateInstance(fmod_event, &instance);
    if (!paused)
    {
        m_EventInstanceStart(instance);
    }
    return PlayingSound{instance, this};
}

bool SoundManager::is_playing(PlayingSound playing_sound)
{
    return std::visit(
        overloaded{
            [this](FMOD::Channel *channel)
            {
                FMOD::BOOL is_playing{false};
                return FMOD_CHECK_CALL(m_ChannelIsPlaying(channel, &is_playing)) && is_playing;
            },
            [this](FMODStudio::EventInstance *event)
            {
                FMODStudio::PlaybackState playback_state;
                return FMOD_CHECK_CALL(m_EventInstanceGetPlaybackState(event, &playback_state)) &&
                       playback_state == FMODStudio::PlaybackState::Playing;
            },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::stop(PlayingSound playing_sound)
{
    return std::visit(
        overloaded{
            [this](FMOD::Channel *channel) { return FMOD_CHECK_CALL(m_ChannelStop(channel)); },
            [this](FMODStudio::EventInstance *event) { return FMOD_CHECK_CALL(m_EventInstanceStop(event, FMODStudio::StopMode::AllowFadeOut)); },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_pause(PlayingSound playing_sound, bool pause)
{
    return std::visit(
        overloaded{
            [this, pause](FMOD::Channel *channel) { return FMOD_CHECK_CALL(m_ChannelSetPaused(channel, pause)); },
            [this, pause](FMODStudio::EventInstance *event)
            {
                FMOD::BOOL paused{false};
                if (!pause && FMOD_CHECK_CALL(m_EventInstanceGetPaused(event, &paused)) && !paused)
                {
                    return FMOD_CHECK_CALL(m_EventInstanceStart(event));
                }
                return FMOD_CHECK_CALL(m_EventInstanceSetPaused(event, true));
            },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_mute(PlayingSound playing_sound, bool mute)
{
    return std::visit(
        overloaded{
            [this, mute](FMOD::Channel *channel) { return FMOD_CHECK_CALL(m_ChannelSetMute(channel, mute)); },
            [this](FMODStudio::EventInstance *event) { return false; }, // Not available on the even interface
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_pitch(PlayingSound playing_sound, float pitch)
{
    return std::visit(
        overloaded{
            [this, pitch](FMOD::Channel *channel) { return FMOD_CHECK_CALL(m_ChannelSetPitch(channel, pitch)); },
            [this, pitch](FMODStudio::EventInstance *event) { return FMOD_CHECK_CALL(m_EventInstanceSetPitch(event, pitch)); },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_pan(PlayingSound playing_sound, float pan)
{
    return std::visit(
        overloaded{
            [this, pan](FMOD::Channel *channel) { return FMOD_CHECK_CALL(m_ChannelSetPan(channel, pan)); },
            [](FMODStudio::EventInstance *) { return false; }, // Not available on the even interface
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_volume(PlayingSound playing_sound, float volume)
{
    return std::visit(
        overloaded{
            [this, volume](FMOD::Channel *channel) { return FMOD_CHECK_CALL(m_ChannelSetVolume(channel, std::clamp(volume, -1.0f, 1.0f))); },
            [this, volume](FMODStudio::EventInstance *event) { return FMOD_CHECK_CALL(m_EventInstanceSetVolume(event, volume)); },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_looping(PlayingSound playing_sound, LoopMode loop_mode)
{
    return std::visit(
        overloaded{
            [this, loop_mode](FMOD::Channel *channel)
            {
                switch (loop_mode)
                {
                case LoopMode::Off:
                    return FMOD_CHECK_CALL(m_ChannelSetMode(channel, FMOD::MODE_LOOP_OFF));
                case LoopMode::Loop:
                    return FMOD_CHECK_CALL(m_ChannelSetMode(channel, FMOD::MODE_LOOP_NORMAL));
                case LoopMode::Bidirectional:
                    return FMOD_CHECK_CALL(m_ChannelSetMode(channel, FMOD::MODE_LOOP_BIDI));
                }
            },
            [](FMODStudio::EventInstance *event) { return false; },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
    return false;
}
bool SoundManager::set_callback(PlayingSound playing_sound, SoundCallbackFunction callback)
{
    if (FMOD::Channel **channel = std::get_if<FMOD::Channel *>(&playing_sound.m_FmodHandle))
    {
        std::lock_guard lock{s_SoundCallbacksMutex};
        if (FMOD_CHECK_CALL(m_ChannelSetCallback(*channel, &ChannelControlCallback)))
        {
            s_SoundCallbacks.push_back(SoundCallbackData{*channel, std::move(callback)});
            return true;
        }
    }
    else if (FMODStudio::EventInstance **instance = std::get_if<FMODStudio::EventInstance *>(&playing_sound.m_FmodHandle))
    {
        FMODStudio::EventDescription *event;
        if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(*instance, &event)))
        {
            if (FMOD_CHECK_CALL(m_EventInstanceSetCallback(*instance, &EventInstanceCallback, FMODStudio::EventCallbackType::Stopped)))
            {
                std::lock_guard lock{s_EventCallbacksMutex};
                auto it = std::find_if(
                    s_EventCallbacks.begin(),
                    s_EventCallbacks.end(),
                    [event](const EventCallbackData &callback) { return callback.handle == event; });
                if (it != s_EventCallbacks.end())
                {
                    it->specific_callbacks[*instance] = std::move(callback);
                }
                else
                {
                    s_EventCallbacks.push_back(EventCallbackData{event, {}, {{*instance, std::move(callback)}}, this});
                }
                return true;
            }
        }
    }
    return false;
}

std::uint32_t SoundManager::set_callback(std::string_view event_name, EventCallbackFunction callback, FMODStudio::EventCallbackType types)
{
    return set_callback(m_SoundData.NameToEvent[event_name]->Event, std::move(callback), types);
}
std::uint32_t
SoundManager::set_callback(FMODStudio::EventDescription *fmod_event, EventCallbackFunction callback, FMODStudio::EventCallbackType types)
{
    std::lock_guard lock{s_EventCallbacksMutex};
    auto it = std::find_if(
        s_EventCallbacks.begin(), s_EventCallbacks.end(), [fmod_event](const EventCallbackData &callback) { return callback.handle == fmod_event; });
    if (it == s_EventCallbacks.end())
    {
        s_EventCallbacks.push_back(EventCallbackData{fmod_event, {}, {}, this});
        it = s_EventCallbacks.end() - 1;
        FMOD_CHECK_CALL(m_EventDescriptionSetCallback(fmod_event, &EventInstanceCallback, FMODStudio::EventCallbackType::All));
    }

    std::uint32_t callback_id = current_callback_id++;
    for (int i = 0; i < FMODStudio::EventCallbackType::Num; i++)
    {
        FMODStudio::EventCallbackType flag = static_cast<FMODStudio::EventCallbackType>(1 << i);
        if (flag & types)
        {
            it->callbacks[flag].push_back({callback_id, callback});
        }
    }

    s_EventCallbackIdToEventDescription[callback_id] = fmod_event;
    return callback_id;
}
void SoundManager::clear_callback(std::uint32_t id)
{
    std::lock_guard lock{s_EventCallbacksMutex};
    FMODStudio::EventDescription *fmod_event = s_EventCallbackIdToEventDescription[id];
    auto it = std::find_if(
        s_EventCallbacks.begin(), s_EventCallbacks.end(), [fmod_event](const EventCallbackData &callback) { return callback.handle == fmod_event; });
    if (it != s_EventCallbacks.end())
    {
        for (auto &[type, cbs] : it->callbacks)
        {
            cbs.erase(std::remove_if(cbs.begin(), cbs.end(), [id](const EventCallbackData::Callback &cb) { return cb.id == id; }), cbs.end());
        }
    }
    s_EventCallbackIdToEventDescription.erase(id);
}

std::unordered_map<std::uint32_t, const char *> SoundManager::get_parameters(PlayingSound playing_sound)
{
    return std::visit(
        overloaded{
            [](FMOD::Channel *channel) { return std::unordered_map<std::uint32_t, const char *>{}; },
            [this](FMODStudio::EventInstance *instance)
            {
                FMODStudio::EventDescription *event;
                if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(instance, &event)))
                {
                    return get_parameters(event);
                }
                return std::unordered_map<std::uint32_t, const char *>{};
            },
            [](std::monostate) { return std::unordered_map<std::uint32_t, const char *>{}; }},
        playing_sound.m_FmodHandle);
}
std::unordered_map<std::uint32_t, const char *> SoundManager::get_parameters(FMODStudio::EventDescription *fmod_event)
{
    if (const EventDescription *event = m_SoundData.FmodEventToEvent[fmod_event])
    {
        std::unordered_map<std::uint32_t, const char *> parameters;
        for (size_t i = 0; i < m_SoundData.Parameters->ParameterNames.size(); i++)
        {
            if (event->HasParameter[i])
            {
                parameters[i] = m_SoundData.Parameters->ParameterNames[i].c_str();
            }
        }
        return parameters;
    }
    return std::unordered_map<std::uint32_t, const char *>{};
}
std::optional<float> SoundManager::get_parameter(PlayingSound playing_sound, std::uint32_t parameter_index)
{
    return std::visit(
        overloaded{
            [](FMOD::Channel *channel) { return std::optional<float>{}; },
            [this, parameter_index](FMODStudio::EventInstance *instance)
            {
                FMODStudio::EventDescription *event;
                if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(instance, &event)))
                {
                    if (const EventDescription *event_desc = m_SoundData.FmodEventToEvent[event])
                    {
                        float value;
                        if (event_desc->HasParameter[parameter_index] &&
                            FMOD_CHECK_CALL(m_EventInstanceGetParameterByID(instance, event_desc->Parameters[parameter_index], &value, nullptr)))
                        {
                            return std::optional<float>{value};
                        }
                    }
                }
                return std::optional<float>{};
            },
            [](std::monostate) { return std::optional<float>{}; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_parameter(PlayingSound playing_sound, std::uint32_t parameter_index, float value)
{
    return std::visit(
        overloaded{
            [](FMOD::Channel *channel) { return false; },
            [this, parameter_index, value](FMODStudio::EventInstance *instance)
            {
                FMODStudio::EventDescription *event;
                if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(instance, &event)))
                {
                    if (const EventDescription *event_desc = m_SoundData.FmodEventToEvent[event])
                    {
                        return event_desc->HasParameter[parameter_index] &&
                               FMOD_CHECK_CALL(m_EventInstanceSetParameterByID(instance, event_desc->Parameters[parameter_index], value, false));
                    }
                }
                return false;
            },
            [](std::monostate) { return false; }},
        playing_sound.m_FmodHandle);
}
