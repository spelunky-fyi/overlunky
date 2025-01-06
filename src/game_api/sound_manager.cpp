#include "sound_manager.hpp"

#include <Windows.h> // for GetProcAddress, GetModuleHandle, HMODULE
#include <algorithm> // for clamp
#include <cstdint>   // for uint32_t
#include <exception> // for exception
#include <memory>    // for remove_if, unique_ptr
#include <mutex>     // for lock_guard, mutex
#include <regex>     // for regex, regex_replace, regex_search

#include "entity.hpp"             //
#include "logger.h"               // for DEBUG
#include "overloaded.hpp"         // for overloaded
#include "script/lua_backend.hpp" //
#include "search.hpp"             // for get_address

#define SOL_ALL_SAFETIES_ON 1

FMOD::FMOD_MODE operator|(FMOD::FMOD_MODE lhs, FMOD::FMOD_MODE rhs)
{
    return static_cast<FMOD::FMOD_MODE>(
        static_cast<std::underlying_type<FMOD::FMOD_MODE>::type>(lhs) |
        static_cast<std::underlying_type<FMOD::FMOD_MODE>::type>(rhs));
}

FMODStudio::LoadBankFlags operator|(FMODStudio::LoadBankFlags lhs, FMODStudio::LoadBankFlags rhs)
{
    return static_cast<FMODStudio::LoadBankFlags>(
        static_cast<std::underlying_type<FMODStudio::LoadBankFlags>::type>(lhs) |
        static_cast<std::underlying_type<FMODStudio::LoadBankFlags>::type>(rhs));
}

struct SoundCallbackData
{
    FMOD::Channel* handle;
    SoundCallbackFunction callback;
};
std::mutex s_SoundCallbacksMutex;
std::vector<SoundCallbackData> s_SoundCallbacks;
FMOD::FMOD_RESULT ChannelControlCallback(
    FMOD::ChannelControl* channel_control,
    FMOD::ChannelControlType channel_control_type,
    FMOD::ChannelControlCallbackType channel_control_callback_type,
    void*,
    void*)
{
    if (channel_control_type == FMOD::ChannelControlType::Channel && channel_control_callback_type == FMOD::ChannelControlCallbackType::End)
    {
        SoundCallbackFunction snd_callback;

        {
            FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(channel_control);

            std::lock_guard lock{s_SoundCallbacksMutex};
            auto it = std::find_if(
                s_SoundCallbacks.begin(),
                s_SoundCallbacks.end(),
                [channel](const SoundCallbackData& callback)
                { return callback.handle == channel; });
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
    return FMOD::FMOD_RESULT::OK;
}

struct EventCallbackData
{
    struct Callback
    {
        std::uint32_t id;
        EventCallbackFunction cb;
    };

    FMODStudio::EventDescription* handle;
    std::unordered_map<FMODStudio::EventCallbackType, std::vector<Callback>> callbacks;
    std::unordered_map<FMODStudio::EventInstance*, SoundCallbackFunction> specific_callbacks;
    SoundManager* sound_manager;

    inline static FMODStudio::EventInstanceGetDescription* EventInstanceGetDescription{nullptr};
};
std::mutex s_EventCallbacksMutex;
std::uint32_t current_callback_id{0};
std::vector<EventCallbackData> s_EventCallbacks;
std::unordered_map<std::uint32_t, FMODStudio::EventDescription*> s_EventCallbackIdToEventDescription;
FMOD::FMOD_RESULT EventInstanceCallback(FMODStudio::EventCallbackType callback_type, FMODStudio::EventInstance* instance, void*)
{
    FMODStudio::EventDescription* event;
    if (FMOD_CHECK_CALL(EventCallbackData::EventInstanceGetDescription(instance, &event)))
    {
        SoundManager* sound_manager{nullptr};
        std::vector<EventCallbackFunction> evt_callbacks;
        SoundCallbackFunction snd_callback;

        {
            std::lock_guard lock{s_EventCallbacksMutex};
            auto it = std::find_if(
                s_EventCallbacks.begin(), s_EventCallbacks.end(), [event](const EventCallbackData& callback)
                { return callback.handle == event; });
            if (it != s_EventCallbacks.end())
            {
                sound_manager = it->sound_manager;

                for (auto& [type, cbs] : it->callbacks)
                {
                    if (type & callback_type)
                    {
                        // TODO: Expose SoundStarted/SoundStopped?
                        for (auto& cb : cbs)
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

        for (const EventCallbackFunction& evt_callback : evt_callbacks)
        {
            evt_callback(PlayingSound{instance, sound_manager});
        }
        if (snd_callback)
        {
            snd_callback();
        }
    }
    return FMOD::FMOD_RESULT::OK;
}

CustomBank::CustomBank(const CustomBank& rhs)
    : m_FmodHandle{ rhs.m_FmodHandle }, m_SoundManager{ rhs.m_SoundManager }
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [this](FMOD::Bank* bank)
                { m_SoundManager->acquire_bank(bank); },
                [](std::monostate) {},
            },
            rhs.m_FmodHandle);
    }
}
CustomBank::CustomBank(CustomBank&& rhs) noexcept
{
    std::swap(m_FmodHandle, rhs.m_FmodHandle);
    std::swap(m_SoundManager, rhs.m_SoundManager);
}
CustomBank::CustomBank(FMOD::Bank* fmod_bank, SoundManager* sound_manager)
    : m_FmodHandle{ fmod_bank }, m_SoundManager{ sound_manager }
{
}
CustomBank::~CustomBank()
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [this](FMOD::Bank* bank)
                { m_SoundManager->acquire_bank(bank); },
                [](std::monostate) {},
            },
            m_FmodHandle);
    }
}
std::optional<FMODStudio::LoadingState> CustomBank::getLoadingState()
{
    return m_SoundManager->get_bank_loading_state(*this);
}
bool CustomBank::loadSampleData()
{
    return m_SoundManager->load_bank_sample_data(*this);
}
bool CustomBank::unloadSampleData()
{
    return m_SoundManager->unload_bank_sample_data(*this);
}
std::optional<FMODStudio::LoadingState> CustomBank::getSampleLoadingState()
{
    return m_SoundManager->get_bank_sample_loading_state(*this);
}
bool CustomBank::unload()
{
    return std::visit(
        overloaded{
            [=, this](FMOD::Bank* bank)
            { return m_SoundManager->unload_bank(bank); },
            [](std::monostate)
            { return false; }},
        m_FmodHandle);
}
bool CustomBank::isValid()
{
    return m_SoundManager->bank_is_valid(*this);
}

CustomSound::CustomSound(const CustomSound& rhs)
    : m_FmodHandle{rhs.m_FmodHandle}, m_SoundManager{rhs.m_SoundManager}
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [this](FMOD::Sound* sound)
                { m_SoundManager->acquire_sound(sound); },
                [](FMODStudio::EventDescription*) {},
                [](std::monostate) {}
            },
            rhs.m_FmodHandle);
    }
}
CustomSound::CustomSound(CustomSound&& rhs) noexcept
{
    std::swap(m_FmodHandle, rhs.m_FmodHandle);
    std::swap(m_SoundManager, rhs.m_SoundManager);
}
CustomSound::CustomSound(FMOD::Sound* fmod_sound, SoundManager* sound_manager)
    : m_FmodHandle{fmod_sound}, m_SoundManager{sound_manager}
{
}
CustomSound::CustomSound(FMODStudio::EventDescription* fmod_event, SoundManager* sound_manager)
    : m_FmodHandle{fmod_event}, m_SoundManager{sound_manager}
{
}

CustomSound::~CustomSound()
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [this](FMOD::Sound* sound)
                { m_SoundManager->acquire_sound(sound); },
                [](FMODStudio::EventDescription*) {},
                [](std::monostate) {},
            },
            m_FmodHandle);
    }
}

PlayingSound CustomSound::play()
{
    return play(false, SOUND_TYPE::Sfx);
}
PlayingSound CustomSound::play(bool paused)
{
    return play(paused, SOUND_TYPE::Sfx);
}
PlayingSound CustomSound::play(bool paused, SOUND_TYPE sound_type)
{
    return std::visit(
        overloaded{
            [=, this](FMOD::Sound* sound)
            { return m_SoundManager->play_sound(sound, paused, sound_type == SOUND_TYPE::Music); },
            [=, this](FMODStudio::EventDescription* event)
            { return m_SoundManager->play_event(event, paused, sound_type == SOUND_TYPE::Music); },
            [](std::monostate)
            {
                return PlayingSound{nullptr, nullptr};
            },
        },
        m_FmodHandle);
}

std::unordered_map<VANILLA_SOUND_PARAM, const char*> CustomSound::get_parameters()
{
    return std::visit(
        overloaded{
            [](FMOD::Sound*)
            { return std::unordered_map<VANILLA_SOUND_PARAM, const char*>{}; },
            [this](FMODStudio::EventDescription* event)
            { return m_SoundManager->get_parameters(event); },
            [](std::monostate)
            { return std::unordered_map<VANILLA_SOUND_PARAM, const char*>{}; },
        },
        m_FmodHandle);
}

PlayingSound::PlayingSound(FMOD::Channel* fmod_channel, SoundManager* sound_manager)
    : m_FmodHandle{fmod_channel}, m_SoundManager{sound_manager}
{
}
PlayingSound::PlayingSound(FMODStudio::EventInstance* fmod_event, SoundManager* sound_manager)
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
bool PlayingSound::set_looping(SOUND_LOOP_MODE loop_mode)
{
    return m_SoundManager->set_looping(*this, loop_mode);
}
bool PlayingSound::set_callback(SoundCallbackFunction callback)
{
    return m_SoundManager->set_callback(*this, std::move(callback));
}

std::unordered_map<VANILLA_SOUND_PARAM, const char*> PlayingSound::get_parameters()
{
    return m_SoundManager->get_parameters(*this);
}
std::optional<float> PlayingSound::get_parameter(VANILLA_SOUND_PARAM parameter_index)
{
    return m_SoundManager->get_parameter(*this, parameter_index);
}
bool PlayingSound::set_parameter(VANILLA_SOUND_PARAM parameter_index, float value)
{
    return m_SoundManager->set_parameter(*this, parameter_index, value);
}

CustomEventDescription::CustomEventDescription(const CustomEventDescription& rhs)
    : m_FmodHandle{rhs.m_FmodHandle}, m_SoundManager{rhs.m_SoundManager}
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [](FMODStudio::EventDescription*) {},
                [](std::monostate) {}},
            rhs.m_FmodHandle);
    }
}
CustomEventDescription::CustomEventDescription(CustomEventDescription&& rhs) noexcept
{
    std::swap(m_FmodHandle, rhs.m_FmodHandle);
    std::swap(m_SoundManager, rhs.m_SoundManager);
}
CustomEventDescription::CustomEventDescription(FMODStudio::EventDescription* fmod_event, SoundManager* sound_manager)
    : m_FmodHandle{fmod_event}, m_SoundManager{sound_manager}
{
}
CustomEventDescription::~CustomEventDescription()
{
    if (m_SoundManager != nullptr)
    {
        std::visit(
            overloaded{
                [](FMODStudio::EventDescription*) {},
                [](std::monostate) {},
            },
            m_FmodHandle);
    }
}
CustomEventInstance CustomEventDescription::createInstance()
{
    return std::visit(
        overloaded{
            [=, this](FMODStudio::EventDescription* event)
            { return m_SoundManager->create_event_instance(event); },
            [](std::monostate)
            {
                return CustomEventInstance{nullptr, nullptr};
            },
        },
        m_FmodHandle);
}
bool CustomEventDescription::releaseAllInstances()
{
    return std::visit(
        overloaded{
            [=, this](FMODStudio::EventDescription* event)
            { return m_SoundManager->release_all_event_instances(event); },
            [](std::monostate)
            {
                return false;
            },
        },
        m_FmodHandle);
}
bool CustomEventDescription::loadSampleData()
{
    return m_SoundManager->load_event_sample_data(*this);
}
bool CustomEventDescription::unloadSampleData()
{
    return m_SoundManager->unload_event_sample_data(*this);
}
std::optional<FMODStudio::LoadingState> CustomEventDescription::getSampleLoadingState()
{
    return m_SoundManager->get_event_sample_loading_state(*this);
}
std::optional<FMODStudio::ParameterDescription> CustomEventDescription::getParameterDescriptionByName(std::string name)
{
    return m_SoundManager->event_get_parameter_description_by_name(*this, name);
}
bool CustomEventDescription::isValid()
{
    return m_SoundManager->event_description_is_valid(*this);
}

CustomEventInstance::CustomEventInstance(FMODStudio::EventInstance* fmod_event, SoundManager* sound_manager)
    : m_FmodHandle{fmod_event}, m_SoundManager{sound_manager}
{
}
bool CustomEventInstance::start()
{
    return m_SoundManager->start(*this);
}
bool CustomEventInstance::stop()
{
    return m_SoundManager->stop(*this, FMODStudio::StopMode::AllowFadeOut);
}
bool CustomEventInstance::stop(FMODStudio::StopMode mode)
{
    return m_SoundManager->stop(*this, mode);
}
std::optional<FMODStudio::PlaybackState> CustomEventInstance::get_playback_state()
{
    return m_SoundManager->get_playback_state(*this);
}
bool CustomEventInstance::set_pause(bool pause)
{
    return m_SoundManager->set_pause(*this, pause);
}
std::optional<bool> CustomEventInstance::get_pause()
{
    return m_SoundManager->get_pause(*this);
}
bool CustomEventInstance::key_off()
{
    return m_SoundManager->key_off(*this);
}
bool CustomEventInstance::set_pitch(float pitch)
{
    return m_SoundManager->set_pitch(*this, pitch);
}
std::optional<std::vector<float>> CustomEventInstance::get_pitch()
{
    return m_SoundManager->get_pitch(*this);
}
bool CustomEventInstance::set_timeline_position(int position)
{
    return m_SoundManager->set_timeline_position(*this, position);
}
std::optional<int> CustomEventInstance::get_timeline_position()
{
    return m_SoundManager->get_timeline_position(*this);
}
bool CustomEventInstance::set_volume(float volume)
{
    return m_SoundManager->set_volume(*this, volume);
}
std::optional<std::vector<float>> CustomEventInstance::get_volume()
{
    return m_SoundManager->get_volume(*this);
}
std::optional<std::vector<float>> CustomEventInstance::get_parameter_by_name(std::string name)
{
    return m_SoundManager->get_parameter_by_name(*this, name);
}
bool CustomEventInstance::set_parameter_by_name(std::string name, float value)
{
    return m_SoundManager->set_parameter_by_name(*this, name, value, false);
}
bool CustomEventInstance::set_parameter_by_name(std::string name, float value, bool ignoreseekspeed)
{
    return m_SoundManager->set_parameter_by_name(*this, name, value, ignoreseekspeed);
}
bool CustomEventInstance::set_parameter_by_name_with_label(std::string name, std::string label)
{
    return m_SoundManager->set_parameter_by_name_with_label(*this, name, label, false);
}
bool CustomEventInstance::set_parameter_by_name_with_label(std::string name, std::string label, bool ignoreseekspeed)
{
    return m_SoundManager->set_parameter_by_name_with_label(*this, name, label, ignoreseekspeed);
}
std::optional<std::vector<float>> CustomEventInstance::get_parameter_by_id(FMODStudio::ParameterId id)
{
    return m_SoundManager->get_parameter_by_id(*this, id);
}
bool CustomEventInstance::set_parameter_by_id(FMODStudio::ParameterId id, float value)
{
    return m_SoundManager->set_parameter_by_id(*this, id, value, false);
}
bool CustomEventInstance::set_parameter_by_id(FMODStudio::ParameterId id, float value, bool ignoreseekspeed)
{
    return m_SoundManager->set_parameter_by_id(*this, id, value, ignoreseekspeed);
}
bool CustomEventInstance::set_parameter_by_id_with_label(FMODStudio::ParameterId id, std::string label)
{
    return m_SoundManager->set_parameter_by_id_with_label(*this, id, label, false);
}
bool CustomEventInstance::set_parameter_by_id_with_label(FMODStudio::ParameterId id, std::string label, bool ignoreseekspeed)
{
    return m_SoundManager->set_parameter_by_id_with_label(*this, id, label, ignoreseekspeed);
}
bool CustomEventInstance::release()
{
    return m_SoundManager->release(*this);
}
bool CustomEventInstance::is_valid()
{
    return m_SoundManager->event_instance_is_valid(*this);
}

FMODpathGUIDmap::FMODpathGUIDmap(const FMODpathGUIDmap& rhs)
    : m_PathGUIDmap{rhs.m_PathGUIDmap}, m_SoundManager{rhs.m_SoundManager}
{
}
FMODpathGUIDmap::FMODpathGUIDmap(FMODpathGUIDmap&& rhs) noexcept
{
    std::swap(m_PathGUIDmap, rhs.m_PathGUIDmap);
    std::swap(m_SoundManager, rhs.m_SoundManager);
}
FMODpathGUIDmap::FMODpathGUIDmap(std::unordered_map<std::string, FMOD::FMOD_GUID> m_PathGUIDmap, SoundManager* sound_manager)
    : m_PathGUIDmap{m_PathGUIDmap}, m_SoundManager{sound_manager}
{
}

FMODpathGUIDmap::~FMODpathGUIDmap() = default;

CustomEventDescription FMODpathGUIDmap::get_event_desc_from_path(std::string path)
{
    return m_SoundManager->pathguidmap_lookup_event_id_by_path(*this, path);
}

struct SoundManager::Sound
{
    std::uint32_t ref_count;
    DecodedAudioBuffer buffer;
    std::string path;
    FMOD::Sound* fmod_sound{nullptr};
};

struct SoundManager::Bank
{
    std::uint32_t ref_count;
    std::string path;
    FMOD::Bank* fmod_bank{nullptr};
};

SoundManager::SoundManager(DecodeAudioFile* decode_function)
    : m_DecodeFunction{decode_function}
{
    m_IsInit = true;
#define AUDIO_INIT_ERROR(format, ...) \
    DEBUG(format, __VA_ARGS__);       \
    // what's the point of this then?
    // m_IsInit = false;

    if (HMODULE fmod_studio = GetModuleHandle("fmodstudio.dll"))
    {
        m_SoundData.Parameters = (const EventParameters*)get_address("fmod_event_properties"sv);
        m_SoundData.Events = (const EventMap*)get_address("fmod_event_map"sv);

        if (m_SoundData.Parameters != nullptr && m_SoundData.Events != nullptr)
        {
            for (const auto& [id, event] : *m_SoundData.Events)
            {
                m_SoundData.FmodEventToEvent[event.Event] = &event;
                m_SoundData.NameToEvent[event.Name] = &event;
            }

            m_FmodStudioSystem = *(FMODStudio::System**)get_address("fmod_studio"sv);
            auto get_core_system = reinterpret_cast<FMODStudio::GetCoreSystem*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetCoreSystem"));
            {
                auto err = get_core_system(m_FmodStudioSystem, &m_FmodSystem);
                if (err != FMOD::FMOD_RESULT::OK)
                {
                    AUDIO_INIT_ERROR("Could not get Fmod System, custom audio won't work...");
                }
            }

            auto flush_commands = reinterpret_cast<FMODStudio::FlushCommands*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_FlushCommands"));
            auto get_bus = reinterpret_cast<FMODStudio::GetBus*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetBus"));
            auto lock_channel_group = reinterpret_cast<FMODStudio::LockChannelGroup*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bus_LockChannelGroup"));
            auto get_channel_group = reinterpret_cast<FMODStudio::GetChannelGroup*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bus_GetChannelGroup"));

            auto get_channel_group_from_bus_name = [=](const char* bus_name, FMOD::ChannelGroup** channel_group)
            {
                FMODStudio::Bus* bus{nullptr};
                auto err = get_bus(m_FmodStudioSystem, bus_name, &bus);
                if (err != FMOD::FMOD_RESULT::OK)
                {
                    AUDIO_INIT_ERROR("Could not get bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
                }
                else
                {
                    err = lock_channel_group(bus);
                    if (err != FMOD::FMOD_RESULT::OK)
                    {
                        AUDIO_INIT_ERROR("Could not lock channel group for bus '{}', custom audio volume won't be synced with game volume properly...", bus_name);
                    }
                    else
                    {
                        err = flush_commands(m_FmodStudioSystem);
                        if (err != FMOD::FMOD_RESULT::OK)
                        {
                            AUDIO_INIT_ERROR(
                                "Could not flush commands after locking channel group for bus '{}', custom audio volume won't be synced with game volume "
                                "properly...",
                                bus_name);
                        }
                        else
                        {
                            err = get_channel_group(bus, channel_group);
                            if (err != FMOD::FMOD_RESULT::OK)
                            {
                                AUDIO_INIT_ERROR(
                                    "Could not obtain channel group for bus '{}', custom audio volume won't be synced with game volume properly...",
                                    bus_name);
                            }
                        }
                    }
                }
            };
            get_channel_group_from_bus_name("bus:/Master_SUM/Master_SFX", &m_SfxChannelGroup);
            get_channel_group_from_bus_name("bus:/Master_SUM/Master_BGM", &m_MusicChannelGroup);

            m_StudioParseID =
                reinterpret_cast<FMODStudio::ParseID*>(GetProcAddress(fmod_studio, "FMOD_Studio_ParseID"));

            m_SystemLoadBankFile =
                reinterpret_cast<FMODStudio::SystemLoadBankFile*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_LoadBankFile"));
            m_SystemGetBank =
                reinterpret_cast<FMODStudio::SystemGetBank*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetBank"));
            m_SystemGetEventByID =
                reinterpret_cast<FMODStudio::SystemGetEventByID*>(GetProcAddress(fmod_studio, "FMOD_Studio_System_GetEventByID"));

            m_BankGetLoadingState =
                reinterpret_cast<FMODStudio::BankGetLoadingState*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bank_GetLoadingState"));
            m_BankLoadSampleData =
                reinterpret_cast<FMODStudio::BankLoadSampleData*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bank_LoadSampleData"));
            m_BankUnloadSampleData =
                reinterpret_cast<FMODStudio::BankUnloadSampleData*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bank_UnloadSampleData"));
            m_BankGetSampleLoadingState =
                reinterpret_cast<FMODStudio::BankGetSampleLoadingState*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bank_GetSampleLoadingState"));
            m_BankUnload =
                reinterpret_cast<FMODStudio::BankUnload*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bank_Unload"));
            m_BankIsValid =
                reinterpret_cast<FMODStudio::BankIsValid*>(GetProcAddress(fmod_studio, "FMOD_Studio_Bank_IsValid"));

            m_EventCreateInstance =
                reinterpret_cast<FMODStudio::EventDescriptionCreateInstance*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_CreateInstance"));
            m_EventDescriptionReleaseAllInstances = reinterpret_cast<FMODStudio::EventDescriptionReleaseAllInstances*>(
                GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_ReleaseAllInstances"));
            m_EventDescriptionLoadSampleData = reinterpret_cast<FMODStudio::EventDescriptionLoadSampleData*>(
                GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_LoadSampleData"));
            m_EventDescriptionUnloadSampleData = reinterpret_cast<FMODStudio::EventDescriptionUnloadSampleData*>(
                GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_UnloadSampleData"));
            m_EventDescriptionGetSampleLoadingState = reinterpret_cast<FMODStudio::EventDescriptionGetSampleLoadingState*>(
                GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_GetSampleLoadingState"));
            m_EventDescriptionGetParameterDescriptionByName = reinterpret_cast<FMODStudio::EventDescriptionGetParameterDescriptionByName*>(
                GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_GetParameterDescriptionByName"));
            m_EventDescriptionGetParameterDescriptionByID = reinterpret_cast<FMODStudio::EventDescriptionGetParameterDescriptionByID*>(
                GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_GetParameterDescriptionByID"));
            m_EventDescriptionSetCallback =
                reinterpret_cast<FMODStudio::EventDescriptionSetCallback*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_SetCallback"));
            m_EventDescriptionIsValid =
                reinterpret_cast<FMODStudio::EventDescriptionIsValid*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventDescription_IsValid"));

            m_EventInstanceStart = reinterpret_cast<FMODStudio::EventInstanceStart*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_Start"));
            m_EventInstanceStop = reinterpret_cast<FMODStudio::EventInstanceStop*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_Stop"));
            m_EventInstanceGetPlaybackState =
                reinterpret_cast<FMODStudio::EventInstanceGetPlaybackState*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetPlaybackState"));
            m_EventInstanceSetPaused =
                reinterpret_cast<FMODStudio::EventInstanceSetPaused*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetPaused"));
            m_EventInstanceGetPaused =
                reinterpret_cast<FMODStudio::EventInstanceGetPaused*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetPaused"));
            m_EventInstanceKeyOff =
                reinterpret_cast<FMODStudio::EventInstanceKeyOff*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_KeyOff"));
            m_EventInstanceSetPitch =
                reinterpret_cast<FMODStudio::EventInstanceSetPitch*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetPitch"));
            m_EventInstanceGetPitch =
                reinterpret_cast<FMODStudio::EventInstanceGetPitch*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetPitch"));
            m_EventInstanceSetTimelinePosition =
                reinterpret_cast<FMODStudio::EventInstanceSetTimelinePosition*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetTimelinePosition"));
            m_EventInstanceGetTimelinePosition =
                reinterpret_cast<FMODStudio::EventInstanceGetTimelinePosition*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetTimelinePosition"));
            m_EventInstanceSetVolume =
                reinterpret_cast<FMODStudio::EventInstanceSetVolume*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetVolume"));
            m_EventInstanceGetVolume =
                reinterpret_cast<FMODStudio::EventInstanceGetVolume*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetVolume"));
            m_EventInstanceSetCallback =
                reinterpret_cast<FMODStudio::EventInstanceSetCallback*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetCallback"));
            m_EventInstanceSetUserData =
                reinterpret_cast<FMODStudio::EventInstanceSetUserData*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetUserData"));
            m_EventInstanceGetUserData =
                reinterpret_cast<FMODStudio::EventInstanceGetUserData*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetUserData"));
            m_EventInstanceGetDescription =
                reinterpret_cast<FMODStudio::EventInstanceGetDescription*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetDescription"));
            m_EventInstanceSetParameterByName =
                reinterpret_cast<FMODStudio::EventInstanceSetParameterByName*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetParameterByName"));
            m_EventInstanceGetParameterByName =
                reinterpret_cast<FMODStudio::EventInstanceGetParameterByName*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetParameterByName"));
            m_EventInstanceSetParameterByNameWithLabel =
                reinterpret_cast<FMODStudio::EventInstanceSetParameterByNameWithLabel*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetParameterByNameWithLabel"));
            m_EventInstanceGetParameterByID =
                reinterpret_cast<FMODStudio::EventInstanceGetParameterByID*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_GetParameterByID"));
            m_EventInstanceSetParameterByID =
                reinterpret_cast<FMODStudio::EventInstanceSetParameterByID*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetParameterByID"));
            m_EventInstanceSetParameterByIDWithLabel =
                reinterpret_cast<FMODStudio::EventInstanceSetParameterByIDWithLabel*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_SetParameterByIDWithLabel"));
            m_EventInstanceRelease =
                reinterpret_cast<FMODStudio::EventInstanceRelease*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_Release"));
            m_EventInstanceIsValid =
                reinterpret_cast<FMODStudio::EventInstanceIsValid*>(GetProcAddress(fmod_studio, "FMOD_Studio_EventInstance_IsValid"));

            EventCallbackData::EventInstanceGetDescription = m_EventInstanceGetDescription;
        }
        else
        {
            AUDIO_INIT_ERROR(
                "Could not game's fmod data, custom audio won't work...");
        }
    }
    else
    {
        AUDIO_INIT_ERROR("fmodstudio.dll was not loaded, custom audio won't work...");
    }

    if (HMODULE fmod = GetModuleHandle("fmod.dll"))
    {
        m_CreateSound = reinterpret_cast<FMOD::CreateSound*>(GetProcAddress(fmod, "FMOD_System_CreateSound"));
        m_ReleaseSound = reinterpret_cast<FMOD::ReleaseSound*>(GetProcAddress(fmod, "FMOD_Sound_Release"));
        m_PlaySound = reinterpret_cast<FMOD::PlaySound*>(GetProcAddress(fmod, "FMOD_System_PlaySound"));

        m_ChannelIsPlaying = reinterpret_cast<FMOD::ChannelIsPlaying*>(GetProcAddress(fmod, "FMOD_Channel_IsPlaying"));
        m_ChannelStop = reinterpret_cast<FMOD::ChannelStop*>(GetProcAddress(fmod, "FMOD_Channel_Stop"));
        m_ChannelSetPaused = reinterpret_cast<FMOD::ChannelSetPaused*>(GetProcAddress(fmod, "FMOD_Channel_SetPaused"));
        m_ChannelSetMute = reinterpret_cast<FMOD::ChannelSetMute*>(GetProcAddress(fmod, "FMOD_Channel_SetMute"));
        m_ChannelSetPitch = reinterpret_cast<FMOD::ChannelSetPitch*>(GetProcAddress(fmod, "FMOD_Channel_SetPitch"));
        m_ChannelSetPan = reinterpret_cast<FMOD::ChannelSetPan*>(GetProcAddress(fmod, "FMOD_Channel_SetPan"));
        m_ChannelSetVolume = reinterpret_cast<FMOD::ChannelSetVolume*>(GetProcAddress(fmod, "FMOD_Channel_SetVolume"));
        m_ChannelSetMode = reinterpret_cast<FMOD::ChannelSetMode*>(GetProcAddress(fmod, "FMOD_Channel_SetMode"));
        m_ChannelSetCallback = reinterpret_cast<FMOD::ChannelSetCallback*>(GetProcAddress(fmod, "FMOD_Channel_SetCallback"));
        m_ChannelSetUserData = reinterpret_cast<FMOD::ChannelSetUserData*>(GetProcAddress(fmod, "FMOD_Channel_SetUserData"));
        m_ChannelGetUserData = reinterpret_cast<FMOD::ChannelGetUserData*>(GetProcAddress(fmod, "FMOD_Channel_GetUserData"));
    }
    else
    {
        AUDIO_INIT_ERROR("fmod.dll was not loaded, custom audio won't work...");
    }

#undef AUDIO_INIT_ERROR
}
SoundManager::~SoundManager()
{
    for (Sound& sound : m_SoundStorage)
    {
        m_ReleaseSound(sound.fmod_sound);
    }
    for (Bank& bank : m_BankStorage)
    {
        m_BankUnload(bank.fmod_bank);
    }
}

CustomSound SoundManager::get_sound(std::string path)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [&path](const Sound& sound)
                           { return sound.path == path; });
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
    catch (std::exception& except)
    {
        DEBUG("Failed loading audio file {}\n{}", path, except.what());
        return CustomSound{nullptr, nullptr};
    }

    Sound new_sound;
    new_sound.ref_count = 1;
    new_sound.buffer = std::move(buffer);
    new_sound.path = std::move(path);

    FMOD::FMOD_MODE mode =
        (FMOD::FMOD_MODE)(FMOD::FMOD_MODE::MODE_CREATESAMPLE | FMOD::FMOD_MODE::MODE_OPENMEMORY_POINT | FMOD::FMOD_MODE::MODE_OPENRAW | FMOD::FMOD_MODE::MODE_IGNORETAGS | FMOD::FMOD_MODE::MODE_LOOP_OFF);

    FMOD::CREATESOUNDEXINFO create_sound_exinfo{};
    create_sound_exinfo.cbsize = sizeof(create_sound_exinfo);
    create_sound_exinfo.length = (std::uint32_t)new_sound.buffer.data_size - 32;
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

    auto data = (const char*)new_sound.buffer.data.get() + 16; // 16 bytes padding in front
    FMOD::FMOD_RESULT err = m_CreateSound(m_FmodSystem, data, mode, &create_sound_exinfo, &new_sound.fmod_sound);
    if (err != FMOD::FMOD_RESULT::OK)
    {
        return CustomSound{nullptr, nullptr};
    }

    m_SoundStorage.push_back(std::move(new_sound));
    return CustomSound{m_SoundStorage.back().fmod_sound, this};
}
CustomSound SoundManager::get_sound(const char* path)
{
    return get_sound(std::string{path});
}
CustomSound SoundManager::get_existing_sound(std::string_view path)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [&path](const Sound& sound)
                           { return sound.path == path; });
    if (it != m_SoundStorage.end())
    {
        it->ref_count++;
        return CustomSound{it->fmod_sound, this};
    }
    return CustomSound{nullptr, nullptr};
}
void SoundManager::acquire_sound(FMOD::Sound* fmod_sound)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound](const Sound& sound)
                           { return sound.fmod_sound == fmod_sound; });
    if (it == m_SoundStorage.end())
    {
        DEBUG("Trying to acquire sound that does not exist...");
        return;
    }

    it->ref_count++;
}
void SoundManager::release_sound(FMOD::Sound* fmod_sound)
{
    auto it = std::find_if(m_SoundStorage.begin(), m_SoundStorage.end(), [fmod_sound](const Sound& sound)
                           { return sound.fmod_sound == fmod_sound; });
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
PlayingSound SoundManager::play_sound(FMOD::Sound* fmod_sound, bool paused, bool as_music)
{
    FMOD::Channel* channel{nullptr};
    m_PlaySound(m_FmodSystem, fmod_sound, as_music ? m_MusicChannelGroup : m_SfxChannelGroup, paused, &channel);
    return PlayingSound{channel, this};
}

CustomEventDescription SoundManager::pathguidmap_lookup_event_id_by_path(FMODpathGUIDmap map, std::string path)
{
    auto it = map.m_PathGUIDmap.find(path);
    if (it != map.m_PathGUIDmap.end())
    {
        return get_event_description_by_id(&it->second);
    }
    DEBUG("Could not find event path {} in FMODpathGUIDmap", path);
    return CustomEventDescription{nullptr, nullptr};
}

CustomEventDescription SoundManager::get_event_description_by_id_string(std::string guid_string)
{
    FMOD::FMOD_GUID guid;
    if (FMOD_CHECK_CALL(m_StudioParseID(guid_string.c_str(), &guid)))
    {
        return get_event_description_by_id(&guid);
    }
    return CustomEventDescription{nullptr, nullptr};
}

CustomEventDescription SoundManager::get_event_description_by_id(FMODStudio::FMOD_GUID* guid)
{
    FMODStudio::EventDescription* fmod_event;
    if (FMOD_CHECK_CALL(m_SystemGetEventByID(m_FmodStudioSystem, guid, &fmod_event)))
    {
        return CustomEventDescription{fmod_event, this};
    }
    DEBUG("Could not get event for GUID {{{:X}-{:X}-{:X}-{:X}{:X}-{:X}{:X}{:X}{:X}{:X}{:X}}}",
        guid->Data1, guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
    return CustomEventDescription{nullptr, nullptr};
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
PlayingSound SoundManager::play_event(FMODStudio::EventDescription* fmod_event, bool paused, [[maybe_unused]] bool as_music) // TODO: fix as_music being not used
{
    FMODStudio::EventInstance* instance{nullptr};
    m_EventCreateInstance(fmod_event, &instance);
    if (!paused)
    {
        m_EventInstanceStart(instance);
    }
    return PlayingSound{instance, this};
}

CustomBank SoundManager::get_bank(std::string path)
{
    DEBUG("Loading bank file from path {}", path);
    auto it = std::find_if(m_BankStorage.begin(), m_BankStorage.end(), [&path](const Bank& bank)
        { return bank.path == path; });
    if (it != m_BankStorage.end())
    {
        it->ref_count++;
        return CustomBank{ it->fmod_bank, this };
    }

    FMODStudio::LoadBankFlags load_bank_flags = (FMODStudio::LoadBankFlags)(FMODStudio::LoadBankFlags::Nonblocking);

    Bank new_bank;
    new_bank.ref_count = 1;
    new_bank.path = std::move(path);

    FMOD::FMOD_RESULT err = m_SystemLoadBankFile(m_FmodStudioSystem, new_bank.path.c_str(), load_bank_flags, &new_bank.fmod_bank);
    if (err != FMOD::FMOD_RESULT::OK)
    {
        DEBUG("Failed loading bank file {}\nFMOD result: {}", new_bank.path, FMOD::ErrStr(err));
        return CustomBank{ nullptr, nullptr };
    }

    DEBUG("Successfully loaded bank file {}", new_bank.path);
    m_BankStorage.push_back(std::move(new_bank));
    return CustomBank{ m_BankStorage.back().fmod_bank, this };
}
CustomBank SoundManager::get_bank(const char* path)
{
    return get_bank(std::string{ path });
}
CustomBank SoundManager::get_existing_bank(std::string_view path)
{
    auto it = std::find_if(m_BankStorage.begin(), m_BankStorage.end(), [&path](const Bank& bank)
        { return bank.path == path; });
    if (it != m_BankStorage.end())
    {
        it->ref_count++;
        return CustomBank{ it->fmod_bank, this };
    }
    return CustomBank{ nullptr, nullptr };
}
void SoundManager::acquire_bank(FMOD::Bank* fmod_bank)
{
    auto it = std::find_if(m_BankStorage.begin(), m_BankStorage.end(), [fmod_bank](const Bank& bank)
        { return bank.fmod_bank == fmod_bank; });
    if (it == m_BankStorage.end())
    {
        DEBUG("Trying to acquire bank that does not exist...");
        return;
    }

    it->ref_count++;
}
std::optional<FMODStudio::LoadingState> SoundManager::get_bank_loading_state(CustomBank custom_bank)
{
    return std::visit(
        overloaded{
                   [this](FMODStudio::Bank* bank)
                   {
                        FMODStudio::LoadingState value;
                        if (FMOD_CHECK_CALL(m_BankGetLoadingState(bank, &value)))
                        {
                            return std::optional<FMODStudio::LoadingState>{value};
                        }
                        return std::optional<FMODStudio::LoadingState>{};
                   },
                   [](std::monostate)
                   { return std::optional<FMODStudio::LoadingState>{}; } },
        custom_bank.m_FmodHandle);
}
bool SoundManager::load_bank_sample_data(CustomBank custom_bank)
{
    return std::visit(
        overloaded{ [this](FMODStudio::Bank* bank)
                   { return FMOD_CHECK_CALL(m_BankLoadSampleData(bank)); },
                   [](std::monostate)
                   { return false; } },
        custom_bank.m_FmodHandle);
}
bool SoundManager::unload_bank_sample_data(CustomBank custom_bank)
{
    return std::visit(
        overloaded{[this](FMODStudio::Bank* bank)
                   { return FMOD_CHECK_CALL(m_BankUnloadSampleData(bank)); },
                   [](std::monostate)
                   { return false; } },
        custom_bank.m_FmodHandle);
}
std::optional<FMODStudio::LoadingState> SoundManager::get_bank_sample_loading_state(CustomBank custom_bank)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::Bank* bank)
            {
                FMODStudio::LoadingState value;
                if (FMOD_CHECK_CALL(m_BankGetSampleLoadingState(bank, &value)))
                {
                    return std::optional<FMODStudio::LoadingState>{value};
                }
                return std::optional<FMODStudio::LoadingState>{};
            },
            [](std::monostate)
            { return std::optional<FMODStudio::LoadingState>{}; }},
        custom_bank.m_FmodHandle);
}
bool SoundManager::unload_bank(FMOD::Bank* fmod_bank)
{
    auto it = std::find_if(m_BankStorage.begin(), m_BankStorage.end(), [fmod_bank](const Bank& bank)
                           { return bank.fmod_bank == fmod_bank; });
    if (it == m_BankStorage.end())
    {
        DEBUG("Trying to unload bank that does not exist...");
        return false;
    }

    if (it->ref_count == 1)
    {
        auto res = FMOD_CHECK_CALL(m_BankUnload(it->fmod_bank));
        if (res)
        {
            m_BankStorage.erase(it);
        }
        return res;
    }
    else
    {
        it->ref_count--;
        return false;
    }
}
bool SoundManager::bank_is_valid(CustomBank custom_bank)
{
    return std::visit(
        overloaded{[this](FMODStudio::Bank* fmod_bank)
                   { return m_BankIsValid(fmod_bank); },
                   [](std::monostate)
                   { return false; }},
        custom_bank.m_FmodHandle);
}

CustomEventInstance SoundManager::create_event_instance(FMODStudio::EventDescription* fmod_event)
{
    FMODStudio::EventInstance* instance{nullptr};
    m_EventCreateInstance(fmod_event, &instance);
    return CustomEventInstance{instance, this};
}
bool SoundManager::release_all_event_instances(FMODStudio::EventDescription* fmod_event)
{
    return FMOD_CHECK_CALL(m_EventDescriptionReleaseAllInstances(fmod_event));
}
bool SoundManager::load_event_sample_data(CustomEventDescription fmod_event)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventDescription* event_desc)
                   { return FMOD_CHECK_CALL(m_EventDescriptionLoadSampleData(event_desc)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event.m_FmodHandle);
}
bool SoundManager::unload_event_sample_data(CustomEventDescription fmod_event)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventDescription* event_desc)
                   { return FMOD_CHECK_CALL(m_EventDescriptionUnloadSampleData(event_desc)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event.m_FmodHandle);
}
std::optional<FMODStudio::LoadingState> SoundManager::get_event_sample_loading_state(CustomEventDescription fmod_event)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::EventDescription* event_desc)
            {
                FMODStudio::LoadingState value;
                if (FMOD_CHECK_CALL(m_EventDescriptionGetSampleLoadingState(event_desc, &value)))
                {
                    return std::optional<FMODStudio::LoadingState>{value};
                }
                return std::optional<FMODStudio::LoadingState>{};
            },
            [](std::monostate)
            { return std::optional<FMODStudio::LoadingState>{}; }},
        fmod_event.m_FmodHandle);
}
std::optional<FMODStudio::ParameterDescription> SoundManager::event_get_parameter_description_by_name(CustomEventDescription fmod_event, std::string name)
{
    return std::visit(
        overloaded{
            [this, name](FMODStudio::EventDescription* event_desc)
            {
                FMODStudio::ParameterDescription param_desc;
                if (FMOD_CHECK_CALL(m_EventDescriptionGetParameterDescriptionByName(event_desc, name.c_str(), &param_desc)))
                {
                    return std::optional<FMODStudio::ParameterDescription>{param_desc};
                }
                return std::optional<FMODStudio::ParameterDescription>{};
            },
            [](std::monostate)
            { return std::optional<FMODStudio::ParameterDescription>{}; }},
        fmod_event.m_FmodHandle);
}
bool SoundManager::event_description_is_valid(CustomEventDescription fmod_event)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventDescription* event_desc)
                   { return m_EventDescriptionIsValid(event_desc); },
                   [](std::monostate)
                   { return false; }},
        fmod_event.m_FmodHandle);
}

bool SoundManager::start(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceStart(event_instance)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::stop(CustomEventInstance fmod_event_instance, FMODStudio::StopMode mode)
{
    return std::visit(
        overloaded{[this, mode](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceStop(event_instance, mode)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<FMODStudio::PlaybackState> SoundManager::get_playback_state(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::EventInstance* event_instance)
            {
                FMODStudio::PlaybackState playback_state;
                if (FMOD_CHECK_CALL(m_EventInstanceGetPlaybackState(event_instance, &playback_state)))
                {
                    return std::optional<FMODStudio::PlaybackState>{playback_state};
                }
                return std::optional<FMODStudio::PlaybackState>{};
            },
            [](std::monostate)
            { return std::optional<FMODStudio::PlaybackState>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_pause(CustomEventInstance fmod_event_instance, bool pause)
{
    return std::visit(
        overloaded{[this, pause](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetPaused(event_instance, pause)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<bool> SoundManager::get_pause(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::EventInstance* event_instance)
            {
                BOOL paused;
                if (FMOD_CHECK_CALL(m_EventInstanceGetPaused(event_instance, &paused)))
                {
                    return std::optional<bool>{paused};
                }
                return std::optional<bool>{};
            },
            [](std::monostate)
            { return std::optional<bool>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::key_off(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceKeyOff(event_instance)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<std::vector<float>> SoundManager::get_pitch(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::EventInstance* event_instance)
            {
                float pitch;
                float finalpitch;
                if (FMOD_CHECK_CALL(m_EventInstanceGetPitch(event_instance, &pitch, &finalpitch)))
                {
                    auto pitch_arr = std::array<float, 2>{};
                    return std::optional<std::vector<float>>{{pitch, finalpitch}};
                }
                return std::optional<std::vector<float>>{};
            },
            [](std::monostate)
            { return std::optional<std::vector<float>>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_pitch(CustomEventInstance fmod_event_instance, float pitch)
{
    return std::visit(
        overloaded{[this, pitch](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetPitch(event_instance, pitch)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<int> SoundManager::get_timeline_position(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::EventInstance* event_instance)
            {
                int position;
                if (FMOD_CHECK_CALL(m_EventInstanceGetTimelinePosition(event_instance, &position)))
                {
                    return std::optional<int>{position};
                }
                return std::optional<int>{};
            },
            [](std::monostate)
            { return std::optional<int>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_timeline_position(CustomEventInstance fmod_event_instance, int position)
{
    return std::visit(
        overloaded{[this, position](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetTimelinePosition(event_instance, position)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<std::vector<float>> SoundManager::get_volume(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{
            [this](FMODStudio::EventInstance* event_instance)
            {
                float volume;
                float finalvolume;
                if (FMOD_CHECK_CALL(m_EventInstanceGetVolume(event_instance, &volume, &finalvolume)))
                {
                    return std::optional<std::vector<float>>{{volume, finalvolume}};
                }
                return std::optional<std::vector<float>>{};
            },
            [](std::monostate)
            { return std::optional<std::vector<float>>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_volume(CustomEventInstance fmod_event_instance, float volume)
{
    return std::visit(
        overloaded{[this, volume](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetVolume(event_instance, volume)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<std::vector<float>> SoundManager::get_parameter_by_name(CustomEventInstance fmod_event_instance, std::string name)
{
    return std::visit(
        overloaded{
            [this, name](FMODStudio::EventInstance* event_instance)
            {
                float value;
                float finalvalue;
                if (FMOD_CHECK_CALL(m_EventInstanceGetParameterByName(event_instance, name.c_str(), &value, &finalvalue)))
                {
                    return std::optional<std::vector<float>>{{value, finalvalue}};
                }
                return std::optional<std::vector<float>>{};
            },
            [](std::monostate)
            { return std::optional<std::vector<float>>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_parameter_by_name(CustomEventInstance fmod_event_instance, std::string name, float value, bool ignoreseekspeed)
{
    return std::visit(
        overloaded{[this, name, value, ignoreseekspeed](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetParameterByName(event_instance, name.c_str(), value, ignoreseekspeed)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_parameter_by_name_with_label(CustomEventInstance fmod_event_instance, std::string name, std::string label, bool ignoreseekspeed)
{
    return std::visit(
        overloaded{[this, name, label, ignoreseekspeed](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetParameterByNameWithLabel(event_instance, name.c_str(), label.c_str(), ignoreseekspeed)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
std::optional<std::vector<float>> SoundManager::get_parameter_by_id(CustomEventInstance fmod_event_instance, FMODStudio::ParameterId id)
{
    return std::visit(
        overloaded{
            [this, id](FMODStudio::EventInstance* event_instance)
            {
                float value;
                float finalvalue;
                if (FMOD_CHECK_CALL(m_EventInstanceGetParameterByID(event_instance, id, &value, &finalvalue)))
                {
                    return std::optional<std::vector<float>>{{value, finalvalue}};
                }
                return std::optional<std::vector<float>>{};
            },
            [](std::monostate)
            { return std::optional<std::vector<float>>{}; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_parameter_by_id(CustomEventInstance fmod_event_instance, FMODStudio::ParameterId id, float value, bool ignoreseekspeed)
{
    return std::visit(
        overloaded{[this, id, value, ignoreseekspeed](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetParameterByID(event_instance, id, value, ignoreseekspeed)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::set_parameter_by_id_with_label(CustomEventInstance fmod_event_instance, FMODStudio::ParameterId id, std::string label, bool ignoreseekspeed)
{
    return std::visit(
        overloaded{[this, id, label, ignoreseekspeed](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetParameterByIDWithLabel(event_instance, id, label.c_str(), ignoreseekspeed)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::release(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventInstance* event_instance)
                   { return FMOD_CHECK_CALL(m_EventInstanceRelease(event_instance)); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}
bool SoundManager::event_instance_is_valid(CustomEventInstance fmod_event_instance)
{
    return std::visit(
        overloaded{[this](FMODStudio::EventInstance* event_instance)
                   { return m_EventInstanceIsValid(event_instance); },
                   [](std::monostate)
                   { return false; }},
        fmod_event_instance.m_FmodHandle);
}

FMODpathGUIDmap SoundManager::create_fmod_path_guid_map(std::string_view path)
{
    if (std::ifstream guid_file = std::ifstream(std::string{path}))
    {
        std::regex re("(\\{.*\\}) (event:.*)", std::regex_constants::icase);
        std::string line;
        std::smatch matches;
        std::unordered_map<std::string, FMOD::FMOD_GUID> newmap;

        while (std::getline(guid_file, line))
        {
            if (std::regex_search(line, matches, re))
            {
                const char* FMOD_guid = matches[1].str().c_str();
                std::string FMOD_event_path = matches[2].str();

                FMOD::FMOD_GUID guid;
                if (FMOD_CHECK_CALL(m_StudioParseID(FMOD_guid, &guid)))
                {
                    auto it = newmap.find(FMOD_event_path);
                    if (it != newmap.end())
                    {
                        it->second = guid;
                    }
                    else
                    {
                        newmap[FMOD_event_path] = std::move(guid);
                    }
                }
                else
                {
                    DEBUG("Failed to parse FMOD GUID string {}", FMOD_guid);
                }
            }
        }
        if (!newmap.empty())
        {
            return FMODpathGUIDmap{newmap, this};
        }
    }
    DEBUG("Failed to create FMOD path GUID map.");
    return FMODpathGUIDmap{nullptr, nullptr};
}

bool SoundManager::is_playing(PlayingSound playing_sound)
{
    return std::visit(
        overloaded{[this](FMOD::Channel* channel)
                   {
                       FMOD::BOOL is_playing{false};
                       return FMOD_CHECK_CALL(m_ChannelIsPlaying(channel, &is_playing)) && is_playing;
                   },
                   [this](FMODStudio::EventInstance* event)
                   {
                       FMODStudio::PlaybackState playback_state;
                       return FMOD_CHECK_CALL(m_EventInstanceGetPlaybackState(event, &playback_state)) &&
                              playback_state == FMODStudio::PlaybackState::Playing;
                   },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::stop(PlayingSound playing_sound)
{
    return std::visit(
        overloaded{[this](FMOD::Channel* channel)
                   { return FMOD_CHECK_CALL(m_ChannelStop(channel)); },
                   [this](FMODStudio::EventInstance* event)
                   { return FMOD_CHECK_CALL(m_EventInstanceStop(event, FMODStudio::StopMode::AllowFadeOut)); },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_pause(PlayingSound playing_sound, bool pause)
{
    return std::visit(
        overloaded{[this, pause](FMOD::Channel* channel)
                   { return FMOD_CHECK_CALL(m_ChannelSetPaused(channel, pause)); },
                   [this, pause](FMODStudio::EventInstance* event)
                   {
                       FMOD::BOOL paused{false};
                       if (!pause && FMOD_CHECK_CALL(m_EventInstanceGetPaused(event, &paused)) && !paused)
                       {
                           return FMOD_CHECK_CALL(m_EventInstanceStart(event));
                       }
                       return FMOD_CHECK_CALL(m_EventInstanceSetPaused(event, true));
                   },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_mute(PlayingSound playing_sound, bool mute)
{
    return std::visit(
        overloaded{[this, mute](FMOD::Channel* channel)
                   { return FMOD_CHECK_CALL(m_ChannelSetMute(channel, mute)); },
                   [](FMODStudio::EventInstance*)
                   { return false; }, // Not available on the even interface
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_pitch(PlayingSound playing_sound, float pitch)
{
    return std::visit(
        overloaded{[this, pitch](FMOD::Channel* channel)
                   { return FMOD_CHECK_CALL(m_ChannelSetPitch(channel, pitch)); },
                   [this, pitch](FMODStudio::EventInstance* event)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetPitch(event, pitch)); },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_pan(PlayingSound playing_sound, float pan)
{
    return std::visit(
        overloaded{[this, pan](FMOD::Channel* channel)
                   { return FMOD_CHECK_CALL(m_ChannelSetPan(channel, pan)); },
                   [](FMODStudio::EventInstance*)
                   { return false; }, // Not available on the even interface
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_volume(PlayingSound playing_sound, float volume)
{
    return std::visit(
        overloaded{[this, volume](FMOD::Channel* channel)
                   { return FMOD_CHECK_CALL(m_ChannelSetVolume(channel, std::clamp(volume, -1.0f, 1.0f))); },
                   [this, volume](FMODStudio::EventInstance* event)
                   { return FMOD_CHECK_CALL(m_EventInstanceSetVolume(event, volume)); },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_looping(PlayingSound playing_sound, SOUND_LOOP_MODE loop_mode)
{
    return std::visit(
        overloaded{[this, loop_mode](FMOD::Channel* channel)
                   {
                       switch (loop_mode)
                       {
                       case SOUND_LOOP_MODE::Off:
                           return FMOD_CHECK_CALL(m_ChannelSetMode(channel, FMOD::FMOD_MODE::MODE_LOOP_OFF));
                       case SOUND_LOOP_MODE::Loop:
                           return FMOD_CHECK_CALL(m_ChannelSetMode(channel, FMOD::FMOD_MODE::MODE_LOOP_NORMAL));
                       case SOUND_LOOP_MODE::Bidirectional:
                           return FMOD_CHECK_CALL(m_ChannelSetMode(channel, FMOD::FMOD_MODE::MODE_LOOP_BIDI));
                       }
                       return false;
                   },
                   [](FMODStudio::EventInstance*)
                   { return false; },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_callback(PlayingSound playing_sound, SoundCallbackFunction callback)
{
    if (FMOD::Channel** channel = std::get_if<FMOD::Channel*>(&playing_sound.m_FmodHandle))
    {
        std::lock_guard lock{s_SoundCallbacksMutex};
        if (FMOD_CHECK_CALL(m_ChannelSetCallback(*channel, &ChannelControlCallback)))
        {
            s_SoundCallbacks.push_back(SoundCallbackData{*channel, std::move(callback)});
            return true;
        }
    }
    else if (FMODStudio::EventInstance** instance = std::get_if<FMODStudio::EventInstance*>(&playing_sound.m_FmodHandle))
    {
        FMODStudio::EventDescription* event;
        if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(*instance, &event)))
        {
            if (FMOD_CHECK_CALL(m_EventInstanceSetCallback(*instance, &EventInstanceCallback, FMODStudio::EventCallbackType::Stopped)))
            {
                std::lock_guard lock{s_EventCallbacksMutex};
                auto it = std::find_if(
                    s_EventCallbacks.begin(),
                    s_EventCallbacks.end(),
                    [event](const EventCallbackData& _callback)
                    { return _callback.handle == event; });
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
SoundManager::set_callback(FMODStudio::EventDescription* fmod_event, EventCallbackFunction callback, FMODStudio::EventCallbackType types)
{
    std::lock_guard lock{s_EventCallbacksMutex};
    auto it = std::find_if(
        s_EventCallbacks.begin(), s_EventCallbacks.end(), [fmod_event](const EventCallbackData& _callback)
        { return _callback.handle == fmod_event; });
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
    FMODStudio::EventDescription* fmod_event = s_EventCallbackIdToEventDescription[id];
    auto it = std::find_if(
        s_EventCallbacks.begin(), s_EventCallbacks.end(), [fmod_event](const EventCallbackData& callback)
        { return callback.handle == fmod_event; });
    if (it != s_EventCallbacks.end())
    {
        for (auto& [type, cbs] : it->callbacks)
        {
            cbs.erase(std::remove_if(cbs.begin(), cbs.end(), [id](const EventCallbackData::Callback& cb)
                                     { return cb.id == id; }),
                      cbs.end());
        }
    }
    s_EventCallbackIdToEventDescription.erase(id);
}

std::unordered_map<VANILLA_SOUND_PARAM, const char*> SoundManager::get_parameters(PlayingSound playing_sound)
{
    return std::visit(
        overloaded{[]([[maybe_unused]] FMOD::Channel* channel)
                   { return std::unordered_map<VANILLA_SOUND_PARAM, const char*>{}; },
                   [this](FMODStudio::EventInstance* instance)
                   {
                       FMODStudio::EventDescription* event;
                       if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(instance, &event)))
                       {
                           return get_parameters(event);
                       }
                       return std::unordered_map<VANILLA_SOUND_PARAM, const char*>{};
                   },
                   [](std::monostate)
                   { return std::unordered_map<VANILLA_SOUND_PARAM, const char*>{}; }},
        playing_sound.m_FmodHandle);
}
std::unordered_map<VANILLA_SOUND_PARAM, const char*> SoundManager::get_parameters(FMODStudio::EventDescription* fmod_event)
{
    if (const EventDescription* event = m_SoundData.FmodEventToEvent[fmod_event])
    {
        std::unordered_map<VANILLA_SOUND_PARAM, const char*> parameters;
        for (size_t i = 0; i < m_SoundData.Parameters->ParameterNames.size(); i++)
        {
            if (event->HasParameter[i])
            {
                parameters[static_cast<uint32_t>(i)] = m_SoundData.Parameters->ParameterNames[i].c_str();
            }
        }
        return parameters;
    }
    return std::unordered_map<VANILLA_SOUND_PARAM, const char*>{};
}
std::optional<float> SoundManager::get_parameter(PlayingSound playing_sound, VANILLA_SOUND_PARAM parameter_index)
{
    return std::visit(
        overloaded{[]([[maybe_unused]] FMOD::Channel* channel)
                   { return std::optional<float>{}; },
                   [this, parameter_index](FMODStudio::EventInstance* instance)
                   {
                       FMODStudio::EventDescription* event;
                       if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(instance, &event)))
                       {
                           if (const EventDescription* event_desc = m_SoundData.FmodEventToEvent[event])
                           {
                               float value;
                               if (event_desc->HasParameter[parameter_index] &&
                                   FMOD_CHECK_CALL(
                                       m_EventInstanceGetParameterByID(instance, event_desc->Parameters[parameter_index], &value, nullptr)))
                               {
                                   return std::optional<float>{value};
                               }
                           }
                       }
                       return std::optional<float>{};
                   },
                   [](std::monostate)
                   { return std::optional<float>{}; }},
        playing_sound.m_FmodHandle);
}
bool SoundManager::set_parameter(PlayingSound playing_sound, VANILLA_SOUND_PARAM parameter_index, float value)
{
    return std::visit(
        overloaded{[]([[maybe_unused]] FMOD::Channel* channel)
                   { return false; },
                   [this, parameter_index, value](FMODStudio::EventInstance* instance)
                   {
                       FMODStudio::EventDescription* event;
                       if (FMOD_CHECK_CALL(m_EventInstanceGetDescription(instance, &event)))
                       {
                           if (const EventDescription* event_desc = m_SoundData.FmodEventToEvent[event])
                           {
                               return event_desc->HasParameter[parameter_index] &&
                                      FMOD_CHECK_CALL(
                                          m_EventInstanceSetParameterByID(instance, event_desc->Parameters[parameter_index], value, false));
                           }
                       }
                       return false;
                   },
                   [](std::monostate)
                   { return false; }},
        playing_sound.m_FmodHandle);
}

int32_t sound_name_to_id(const VANILLA_SOUND s_name)
{
    static std::vector<std::string> sound_names;
    if (sound_names.empty())
    {
        auto sound_mgr = LuaBackend::get_calling_backend()->sound_manager;
        sound_mgr->for_each_event_name(
            [](std::string event_name)
            {
                sound_names.push_back(std::move(event_name));
            });
    }

    for (auto& sound : sound_names)
    {
        if (sound == s_name)
        {
            return int32_t(&sound - &sound_names[0]) + 1;
        }
    }
    return -1;
}

SoundMeta* play_sound_by_id(uint32_t sound_id, uint32_t source_uid)
{
    if (sound_id == -1)
        return nullptr;

    using play_sound = SoundMeta*(int32_t);
    static auto play_sound_func = (play_sound*)get_address("play_sound");

    Entity* source = get_entity_ptr(source_uid);
    SoundMeta* sound_info{nullptr};

    if (source_uid == ~0 || source != nullptr) // don't play the sound if the entity is not valid
    {
        sound_info = play_sound_func(sound_id);
        if (source != nullptr)
            source->set_as_sound_source(sound_info);
    }
    return sound_info;
}

SoundMeta* play_sound(VANILLA_SOUND sound, uint32_t source_uid)
{
    auto sound_id = sound_name_to_id(sound);
    return play_sound_by_id(sound_id, source_uid);
}

SoundMeta* construct_soundmeta(VANILLA_SOUND sound, bool background_sound)
{
    return construct_soundmeta(sound_name_to_id(sound), background_sound);
}

SoundMeta* construct_soundmeta(uint32_t sound_id, bool background_sound)
{
    using construct_soundposition_ptr_fun_t = SoundMeta*(uint32_t id, bool background_sound);
    static const auto construct_soundposition_ptr_call = (construct_soundposition_ptr_fun_t*)get_address("construct_soundmeta");
    return construct_soundposition_ptr_call(sound_id, background_sound);
}
