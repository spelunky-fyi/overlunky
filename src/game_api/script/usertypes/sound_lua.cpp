#include "sound_lua.hpp"

#include <algorithm>     // for max, transform, replace
#include <assert.h>      // for assert
#include <cctype>        // for toupper
#include <cstdint>       // for uint32_t
#include <exception>     // for exception
#include <filesystem>    // for operator/, path
#include <fmt/format.h>  // for format_error
#include <functional>    // for _Func_impl_no_alloc<>::_Mybase
#include <locale>        // for num_put
#include <memory>        // for allocator, make_unique
#include <mutex>         // for lock_guard
#include <new>           // for operator new
#include <optional>      // for nullopt
#include <ostream>       // for basic_streambuf, basic_ios
#include <sol/sol.hpp>   // for global_table, proxy_key_t, state
#include <string>        // for string, basic_string, operator==
#include <tuple>         // for get
#include <type_traits>   // for move, remove_reference_t, declval
#include <unordered_map> // for unordered_map
#include <utility>       // for min, max, swap, find
#include <vector>        // for vector, _Vector_iterator, _Vector_...

#include "aliases.hpp"            // for CallbackId, VANILLA_SOUND_CALLBACK...
#include "fmod.hpp"               // for EventCallbackType, Created, Destroyed
#include "logger.h"               // for DEBUG
#include "script/lua_backend.hpp" // for LuaBackend
#include "script/safe_cb.hpp"     // for make_safe_cb
#include "script/sol_helper.hpp"  //
#include "sound_manager.hpp"      // for CustomSound, PlayingSound, SoundMa...
#include "string_aliases.hpp"     // for VANILLA_SOUND

namespace NSound
{
void register_usertypes(sol::state& lua, SoundManager* sound_manager)
{
    assert(sound_manager != nullptr && sound_manager->is_init());
    if (sound_manager == nullptr)
    {
        DEBUG("Audio API is not available!");
        return;
    }
    if (!sound_manager->is_init())
    {
        DEBUG("Audio API is not initialized!");
        return;
    }

    /// Loads a bank from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
    lua["load_bank"] = [](std::string path) -> sol::optional<CustomBank>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomBank bank = backend->sound_manager->get_bank((backend->get_root_path() / path).string()))
        {
            return bank;
        }
        return sol::nullopt;
    };

    /// Gets an existing bank if a file at the same path was already loaded
    lua["get_bank"] = [](std::string path) -> sol::optional<CustomBank>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomBank bank = backend->sound_manager->get_existing_bank((backend->get_root_path() / path).string()))
        {
            return bank;
        }
        return sol::nullopt;
    };

    /// Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
    lua["create_sound"] = [](std::string path) -> sol::optional<CustomSound>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomSound sound = backend->sound_manager->get_sound((backend->get_root_path() / path).string()))
        {
            return sound;
        }
        return sol::nullopt;
    };

    /// Gets an existing sound, either if a file at the same path was already loaded or if it is already loaded by the game
    lua["get_sound"] = [](std::string path_or_vanilla_sound) -> sol::optional<CustomSound>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomSound event = backend->sound_manager->get_event(path_or_vanilla_sound))
        {
            return event;
        }
        else if (CustomSound sound = backend->sound_manager->get_existing_sound((backend->get_root_path() / path_or_vanilla_sound).string()))
        {
            return sound;
        }
        return sol::nullopt;
    };

    /// Gets an existing sound using a human readable FMOD GUID string
    lua["get_event_desc_by_id"] = [](std::string guid_string) -> sol::optional<CustomEventDescription>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomEventDescription event_description = backend->sound_manager->get_event_description_by_id_string(guid_string))
        {
            return event_description;
        }
        return sol::nullopt;
    };

    lua.new_enum("FMOD_LOADING_STATE",
        "UNLOADING",
        FMODStudio::LoadingState::Unloading,
        "UNLOADED",
        FMODStudio::LoadingState::Unloaded,
        "LOADING",
        FMODStudio::LoadingState::Loading,
        "LOADED",
        FMODStudio::LoadingState::Loaded,
        "ERROR",
        FMODStudio::LoadingState::Error);

    lua.new_enum("FMOD_PLAYBACK_STATE",
        "PLAYING",
        FMODStudio::PlaybackState::Playing,
        "STARTING",
        FMODStudio::PlaybackState::Starting,
        "STOPPED",
        FMODStudio::PlaybackState::Stopped,
        "STOPPING",
        FMODStudio::PlaybackState::Stopping,
        "SUSTAINING",
        FMODStudio::PlaybackState::Sustaining);

    lua.new_enum("FMOD_STOP_MODE",
        "ALLOW_FADE_OUT",
        FMODStudio::StopMode::AllowFadeOut,
        "IMMEDIATE",
        FMODStudio::StopMode::Immediate);

    lua.new_enum("FMOD_PARAMETER_TYPE",
        "GAME_CONTROLLED",
        FMODStudio::ParameterType::GameControlled,
        "AUTOMATIC_DISTANCE",
        FMODStudio::ParameterType::AutomaticDistance,
        "AUTOMATIC_EVENT_CONE_ANGLE",
        FMODStudio::ParameterType::AutomaticEventConeAngle,
        "AUTOMATIC_EVENT_ORIENTATION",
        FMODStudio::ParameterType::AutomaticEventOrientation,
        "AUTOMATIC_DIRECTION",
        FMODStudio::ParameterType::AutomaticDirection,
        "AUTOMATIC_ELEVATION",
        FMODStudio::ParameterType::AutomaticElevation,
        "AUTOMATIC_LISTENER_ORIENTATION",
        FMODStudio::ParameterType::AutomaticListenerOrientation,
        "AUTOMATIC_SPEED",
        FMODStudio::ParameterType::AutomaticSpeed);

    lua.new_enum("FMOD_PARAMETER_FLAGS",
        "READ_ONLY",
        FMODStudio::ParameterFlags::ReadOnly,
        "AUTOMATIC",
        FMODStudio::ParameterFlags::Automatic,
        "GLOBAL",
        FMODStudio::ParameterFlags::Global);

    lua.new_usertype<FMODStudio::ParameterId>(
        "ParameterId",
        "data1",
        &FMODStudio::ParameterId::data1,
        "data2",
        &FMODStudio::ParameterId::data2);

    lua.new_usertype<FMODStudio::ParameterDescription>(
        "ParameterDescription",
        "name",
        &FMODStudio::ParameterDescription::name,
        "id",
        &FMODStudio::ParameterDescription::id,
        "minimum",
        &FMODStudio::ParameterDescription::minimum,
        "maximum",
        &FMODStudio::ParameterDescription::maximum,
        "defaultvalue",
        &FMODStudio::ParameterDescription::defaultvalue,
        "type",
        &FMODStudio::ParameterDescription::type,
        "flags",
        &FMODStudio::ParameterDescription::flags);

    lua.new_usertype<CustomEventDescription>(
        "CustomEventDescription",
        "createInstance",
        &CustomEventDescription::createInstance,
        "releaseAllInstances",
        &CustomEventDescription::releaseAllInstances,
        "loadSampleData",
        &CustomEventDescription::loadSampleData,
        "unloadSampleData",
        &CustomEventDescription::unloadSampleData,
        "getSampleLoadingState",
        &CustomEventDescription::getSampleLoadingState,
        "getParameterDescriptionByName",
        &CustomEventDescription::getParameterDescriptionByName,
        "isValid",
        &CustomEventDescription::isValid);

    auto stop = sol::overload(
        static_cast<bool (CustomEventInstance::*)()>(&CustomEventInstance::stop),
        static_cast<bool (CustomEventInstance::*)(FMODStudio::StopMode)>(&CustomEventInstance::stop));
    auto set_parameter_by_name = sol::overload(
        static_cast<bool (CustomEventInstance::*)(std::string, float)>(&CustomEventInstance::set_parameter_by_name),
        static_cast<bool (CustomEventInstance::*)(std::string, float, bool)>(&CustomEventInstance::set_parameter_by_name));
    auto set_parameter_by_name_with_label = sol::overload(
        static_cast<bool (CustomEventInstance::*)(std::string, std::string)>(&CustomEventInstance::set_parameter_by_name_with_label),
        static_cast<bool (CustomEventInstance::*)(std::string, std::string, bool)>(&CustomEventInstance::set_parameter_by_name_with_label));
    auto set_parameter_by_id = sol::overload(
        static_cast<bool (CustomEventInstance::*)(FMODStudio::ParameterId, float)>(&CustomEventInstance::set_parameter_by_id),
        static_cast<bool (CustomEventInstance::*)(FMODStudio::ParameterId, float, bool)>(&CustomEventInstance::set_parameter_by_id));
    auto set_parameter_by_id_with_label = sol::overload(
        static_cast<bool (CustomEventInstance::*)(FMODStudio::ParameterId, std::string)>(&CustomEventInstance::set_parameter_by_id_with_label),
        static_cast<bool (CustomEventInstance::*)(FMODStudio::ParameterId, std::string, bool)>(&CustomEventInstance::set_parameter_by_id_with_label));

    lua.new_usertype<CustomEventInstance>(
        "CustomEventInstance",
        "start",
        &CustomEventInstance::start,
        "stop",
        stop,
        "getPlaybackState",
        &CustomEventInstance::get_playback_state,
        "setPause",
        &CustomEventInstance::set_pause,
        "getPause",
        &CustomEventInstance::get_pause,
        "keyOff",
        &CustomEventInstance::key_off,
        "setPitch",
        &CustomEventInstance::set_pitch,
        "getPitch",
        &CustomEventInstance::get_pitch,
        "setTimelinePosition",
        &CustomEventInstance::set_timeline_position,
        "getTimelinePosition",
        &CustomEventInstance::get_timeline_position,
        "setVolume",
        &CustomEventInstance::set_volume,
        "getVolume",
        &CustomEventInstance::get_volume,
        "release",
        &CustomEventInstance::release,
        "isValid",
        &CustomEventInstance::is_valid,
        "getParameterByName",
        &CustomEventInstance::get_parameter_by_name,
        "setParameterByName",
        set_parameter_by_name,
        "setParameterByNameWithLabel",
        set_parameter_by_name_with_label,
        "setParameterByID",
        set_parameter_by_id,
        "setParameterByIDWithLabel",
        set_parameter_by_id_with_label,
        "release",
        &CustomEventInstance::release,
        "isValid",
        &CustomEventInstance::is_valid);

    lua.new_usertype<CustomBank>(
        "CustomBank",
        "getLoadingState",
        &CustomBank::getLoadingState,
        "getSampleLoadingState",
        &CustomBank::getSampleLoadingState,
        "loadSampleData",
        &CustomBank::loadSampleData,
        "unload",
        &CustomBank::unload,
        "unloadSampleData",
        &CustomBank::unloadSampleData);

    lua.new_usertype<FMODpathGUIDmap>(
        "CustomGUIDstringMap",
        "getEventDescriptionByPath",
        [](FMODpathGUIDmap& pgm, std::string path) -> std::optional<CustomEventDescription>
        {
            if (CustomEventDescription event_description = pgm.get_event_desc_from_path(path))
            {
                return event_description;
            }
            DEBUG("Failed to get CustomEventDescription from FMODpathGUIDmap");
            return sol::nullopt;
        }
    );
    
    /// Creates an FMOD event path to GUID map from disk relative to this script.
    lua["create_fmod_path_guid_map"] = [](std::string path) -> sol::optional<FMODpathGUIDmap>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (FMODpathGUIDmap map = backend->sound_manager->create_fmod_path_guid_map((backend->get_root_path() / path).string()))
        {
            return map;
        }
        return sol::nullopt;
    };

    /// Returns unique id for the callback to be used in [clear_vanilla_sound_callback](#clear_vanilla_sound_callback).
    /// Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
    /// Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
    /// If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any properties on the sound. Otherwise you may cause a deadlock.
    /// <br/>The callback signature is nil on_vanilla_sound(PlayingSound sound)
    lua["set_vanilla_sound_callback"] = [](VANILLA_SOUND name, VANILLA_SOUND_CALLBACK_TYPE types, sol::function cb) -> CallbackId
    {
        static constexpr auto clone_sound = [](const PlayingSound& sound)
        {
            return std::make_unique<PlayingSound>(sound);
        };
        auto safe_cb = make_safe_cb<void(PlayingSound)>(
            std::move(cb),
            FrontBinder{},
            BackBinder{clone_sound});

        auto backend = LuaBackend::get_calling_backend();
        std::uint32_t id = backend->sound_manager->set_callback(name, std::move(safe_cb), static_cast<FMODStudio::EventCallbackType>(types));
        backend->vanilla_sound_callbacks.push_back(id);
        return id;
    };
    /// Clears a previously set callback
    lua["clear_vanilla_sound_callback"] = [](CallbackId id)
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->sound_manager->clear_callback(id);
        auto it = std::find(backend->vanilla_sound_callbacks.begin(), backend->vanilla_sound_callbacks.end(), id);
        if (it != backend->vanilla_sound_callbacks.end())
        {
            backend->vanilla_sound_callbacks.erase(it);
        }
    };

    {
        auto play = sol::overload(
            static_cast<PlayingSound (CustomSound::*)()>(&CustomSound::play),
            static_cast<PlayingSound (CustomSound::*)(bool)>(&CustomSound::play),
            static_cast<PlayingSound (CustomSound::*)(bool, SOUND_TYPE)>(&CustomSound::play));
        auto get_parameters = [](CustomSound& self)
        {
            return sol::as_table(self.get_parameters());
        };
        /// Handle to a loaded sound, can be used to play the sound and receive a `PlayingSound` for more control
        /// It is up to you to not release this as long as any sounds returned by `CustomSound:play()` are still playing
        lua.new_usertype<CustomSound>(
            "CustomSound",
            "play",
            play,
            "get_parameters",
            get_parameters);
    }

    auto set_callback = [](PlayingSound* sound, sol::function callback)
    {
        sound->set_callback(make_safe_cb<void()>(std::move(callback)));
    };
    auto get_parameters = [](PlayingSound& self)
    {
        return sol::as_table(self.get_parameters());
    };
    /// Handle to a playing sound, start the sound paused to make sure you can apply changes before playing it
    /// You can just discard this handle if you do not need extended control anymore
    lua.new_usertype<PlayingSound>(
        "PlayingSound",
        "is_playing",
        &PlayingSound::is_playing,
        "stop",
        &PlayingSound::stop,
        "set_pause",
        &PlayingSound::set_pause,
        "set_mute",
        &PlayingSound::set_mute,
        "set_pitch",
        &PlayingSound::set_pitch,
        "set_pan",
        &PlayingSound::set_pan,
        "set_volume",
        &PlayingSound::set_volume,
        "set_looping",
        &PlayingSound::set_looping,
        "set_callback",
        std::move(set_callback),
        "get_parameters",
        get_parameters,
        "get_parameter",
        &PlayingSound::get_parameter,
        "set_parameter",
        &PlayingSound::set_parameter);

    lua.new_usertype<SoundMeta>(
        "SoundMeta",
        "x",
        &SoundMeta::x,
        "y",
        &SoundMeta::y,
        "left_channel",
        //&SoundMeta::left_channel,
        sol::property([](SoundMeta* sm)
                      { return ZeroIndexArray<float>(sm->left_channel) /**/; }),
        "right_channel",
        //&SoundMeta::right_channel,
        sol::property([](SoundMeta* sm)
                      { return ZeroIndexArray<float>(sm->right_channel) /**/; }),
        "start_over",
        &SoundMeta::start_over,
        "playing",
        &SoundMeta::playing);

    lua.new_usertype<BackgroundSound>(
        "BackgroundSound",
        sol::base_classes,
        sol::bases<SoundMeta>());

    lua["play_sound"] = play_sound;

    /// Third parameter to `CustomSound:play()`, specifies which group the sound will be played in and thus how the player controls its volume
    lua.create_named_table("SOUND_TYPE", "SFX", 0, "MUSIC", 1);
    /// Paramater to `PlayingSound:set_looping()`, specifies what type of looping this sound should do
    lua.create_named_table("SOUND_LOOP_MODE", "OFF", 0, "LOOP", 1, "BIDIRECTIONAL", 2);
    /// Paramater to `get_sound()`, which returns a handle to a vanilla sound, and `set_vanilla_sound_callback()`,
    lua.create_named_table("VANILLA_SOUND"
                           //, "BGM_BGM_TITLE", BGM/BGM_title
                           //, "", ...check__[vanilla_sounds.txt]\[game_data/vanilla_sounds.txt\]...
                           //, "FX_FX_DM_BANNER", FX/FX_dm_banner
    );
    sound_manager->for_each_event_name(
        [&lua](std::string event_name)
        {
            std::string clean_event_name = event_name;
            std::transform(
                clean_event_name.begin(), clean_event_name.end(), clean_event_name.begin(), [](unsigned char c)
                { return (unsigned char)std::toupper(c); });
            std::replace(clean_event_name.begin(), clean_event_name.end(), '/', '_');
            lua["VANILLA_SOUND"][std::move(clean_event_name)] = std::move(event_name);
        });
    /// Bitmask parameter to `set_vanilla_sound_callback()`
    lua.create_named_table(
        "VANILLA_SOUND_CALLBACK_TYPE",
        "CREATED",
        FMODStudio::EventCallbackType::Created,
        "DESTROYED",
        FMODStudio::EventCallbackType::Destroyed,
        "STARTED",
        FMODStudio::EventCallbackType::Started,
        "RESTARTED",
        FMODStudio::EventCallbackType::Restarted,
        "STOPPED",
        FMODStudio::EventCallbackType::Stopped,
        "START_FAILED",
        FMODStudio::EventCallbackType::StartFailed);
    /* VANILLA_SOUND_CALLBACK_TYPE
        // CREATED
        // Params: `PlayingSound vanilla_sound`
        // DESTROYED
        // Params: `PlayingSound vanilla_sound`
        // STARTED
        // Params: `PlayingSound vanilla_sound`
        // RESTARTED
        // Params: `PlayingSound vanilla_sound`
        // STOPPED
        // Params: `PlayingSound vanilla_sound`
        // START_FAILED
        // Params: `PlayingSound vanilla_sound`
        */
    /// Paramater to `PlayingSound:get_parameter()` and `PlayingSound:set_parameter()`
    lua.create_named_table("VANILLA_SOUND_PARAM"
                           //, "POS_SCREEN_X", 0
                           //, "", ...check__[vanilla_sound_params.txt]\[game_data/vanilla_sound_params.txt\]...
                           //, "CURRENT_LAYER2", 37
    );
    sound_manager->for_each_parameter_name(
        [&lua](std::string parameter_name, std::uint32_t id)
        {
            std::transform(parameter_name.begin(), parameter_name.end(), parameter_name.begin(), [](unsigned char c)
                           { return (unsigned char)std::toupper(c); });
            lua["VANILLA_SOUND_PARAM"][std::move(parameter_name)] = id;
        });
}
}; // namespace NSound
