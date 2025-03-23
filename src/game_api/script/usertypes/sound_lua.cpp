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

    /// Parameter to `load_bank()`, used to control bank loading.
    lua.new_enum("FMOD_LOAD_BANK_FLAGS",
        "NORMAL",
        FMODStudio::LoadBankFlags::Normal,
        "NONBLOCKING",
        FMODStudio::LoadBankFlags::Nonblocking,
        "DECOMPRESS_SAMPLES",
        FMODStudio::LoadBankFlags::DecompressSamples,
        "UNENCRYPTED",
        FMODStudio::LoadBankFlags::Unencrypted);
    /* FMOD_LOAD_BANK_FLAGS
        // NORMAL
        // Standard behavior. The function will not return until the bank has finished loading.
        // NONBLOCKING
        // Loading occurs asychronously rather than immediately. Minimal performace impact. Probably the flag you should use in most cases.
        // DECOMPRESS_SAMPLES
        // Force samples to decompress into memory when loaded, instead of staying compressed. Use NORMAL or NONBLOCKING instead.
        // UNENCRYPTED
        // Ignore the encryption key specified by Studio::System::setAdvancedSettings when loading this bank. Use NORMAL or NONBLOCKING instead.
        */

    /// The loading state of various FMOD Studio objects, such as banks and non-streaming sample data.
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

    /// The playback state of various FMOD Studio objects, used for `CustomEventInstance:get_playback_state()`.
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

    /// Stop modes for an event instance.
    lua.new_enum("FMOD_STOP_MODE",
        "ALLOW_FADE_OUT",
        FMODStudio::StopMode::AllowFadeOut,
        "IMMEDIATE",
        FMODStudio::StopMode::Immediate);
    /* FMOD_STOP_MODE
        // ALLOW_FADE_OUT
        // Allow the events ADHSR modulators to complete their release, and DSP effect tails to play out.
        // IMMEDIATE
        // Stops the event instance immediately.
        */

    /// FMOD Studio flags describing the behavior of a parameter.
    lua.new_enum("FMOD_PARAMETER_FLAGS",
        "READ_ONLY",
        FMODStudio::ParameterFlags::ReadOnly,
        "AUTOMATIC",
        FMODStudio::ParameterFlags::Automatic,
        "GLOBAL",
        FMODStudio::ParameterFlags::Global);

    /// FMOD Studio event parameter types.
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

    /// Loads a bank from disk relative to this script, ownership might be shared with other code that loads the same file.
    /// Returns nil if the file can't be found. Loading a bank file will load the banks metadata, but not non-streaming
    /// sample data. Once a bank has finished loading, all metadata can be accessed meaning that event descriptions can
    /// be found with `get_event_by_id` or using `create_fmod_guid_map` and calling `FMODguidMap:getEvent()`.
    /// The banks loading state can be queried using `CustomBank:getLoadingState()` which will return an `FMOD_LOADING_STATE`.
    lua["load_bank"] = [](std::string path, FMODStudio::LoadBankFlags flags) -> sol::optional<CustomBank>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomBank bank = backend->sound_manager->get_bank((backend->get_root_path() / path).string(), flags))
        {
            return bank;
        }
        return sol::nullopt;
    };

    /// Gets an existing loaded bank if a file at the same path was already loaded
    lua["get_bank"] = [](std::string path) -> sol::optional<CustomBank>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomBank bank = backend->sound_manager->get_existing_bank((backend->get_root_path() / path).string()))
        {
            return bank;
        }
        return sol::nullopt;
    };

    /// Gets a `CustomEventDescription` if the event description is loaded using an FMOD GUID string. The string representation
    /// must be formatted as 32 digits seperated by hyphens and enclosed in braces: {00000000-0000-0000-0000-000000000000}.
    lua["get_event_by_id"] = [](std::string guid_string) -> sol::optional<CustomEventDescription>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (CustomEventDescription event_description = backend->sound_manager->get_event_by_id_string(guid_string))
        {
            return event_description;
        }
        return sol::nullopt;
    };

    /// FMOD Studio event parameter identifier. Can be retrieved with
    /// `CustomEventDescription:getParameterDescriptionByName().id`, or
    /// directly by using `CustomEventDescription:getParameterIDByName()`.
    /// Can be used with `CustomEventInstance:set_parameter_by_id()`,
    /// `CustomEventInstance:set_parameter_by_id_with_label()`, and
    /// `CustomEventInstance:get_parameter_by_id()`. This is useful
    /// if you need to get or update an event parameter at a high
    /// frequency, since setting and getting parameters by ID do
    /// not require FMOD to perform a name to ID lookup internally
    /// like FMOD does when setting or getting parameters by name.
    lua.new_usertype<FMODStudio::ParameterId>(
        "ParameterId",
        "data1",
        sol::readonly(&FMODStudio::ParameterId::data1),
        "data2",
        sol::readonly(&FMODStudio::ParameterId::data2));

    /// FMOD Studio descriptor for an event parameter.
    lua.new_usertype<FMODStudio::ParameterDescription>(
        "ParameterDescription",
        "name",
        sol::readonly(&FMODStudio::ParameterDescription::name),
        "id",
        sol::readonly(&FMODStudio::ParameterDescription::id),
        "minimum",
        sol::readonly(&FMODStudio::ParameterDescription::minimum),
        "maximum",
        sol::readonly(&FMODStudio::ParameterDescription::maximum),
        "defaultvalue",
        sol::readonly(&FMODStudio::ParameterDescription::defaultvalue),
        "type",
        sol::readonly(&FMODStudio::ParameterDescription::type),
        "flags",
        sol::readonly(&FMODStudio::ParameterDescription::flags));
    
    /// Handle to a loaded FMOD Bank. Unloading a bank will destroy all objects loaded from it, and unload all sample data.
    /// Can be used to load and unload the non-streaming sample data of all events in the bank. However you can also
    /// control the loading state of non-streaming sample data for individual events with a `CustomEventDescription`.
    lua.new_usertype<CustomBank>(
        "CustomBank",
        "getLoadingState",
        &CustomBank::getLoadingState,
        "loadSampleData",
        &CustomBank::loadSampleData,
        "unloadSampleData",
        &CustomBank::unloadSampleData,
        "getSampleLoadingState",
        &CustomBank::getSampleLoadingState,
        "unload",
        &CustomBank::unload,
        "isValid",
        &CustomBank::isValid);

    /// Handle to an FMOD event description, can be used to create a `CustomEventInstance` with
    /// `CustomEventDescription:createInstance()`. Also can be used to load and unload non-streaming
    /// sample data for the Event, and release all instances of the event. You can also get parameter
    /// IDs using `CustomEventDescription:getParameterDescriptionByName()`.
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
        "getParameterIDByName",
        &CustomEventDescription::getParameterIDByName,
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

    /// Handle to an FMOD event instance. Can be used to start and stop an event, or set the events parameters. Once you
    /// are done with an event instance and no longer need to change its playback state or parameters, you should call
    /// `CustomEventInstance:release` so the event is marked for release and released when it stops playing to free resources.
    /// Generally though, it is best practice to call `CustomEventInstance:release` immediately after `CustomEventInstance:start`
    /// unless you want to play the event instance multiple times or explicitly start and stop it later.
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

    /// An `FMODguidMap` can be used to resolve FMOD GUIDs for events and snapshots from paths using the GUIDs.txt exported
    /// from an FMOD Studio Project. By default FMOD studio uses a strings bank to do this, however the games master bank and
    /// strings bank cannot be rebuilt to include the names and paths of new events or snapshots. `FMODguidMap` is a
    /// workaround for this, and allows you to get a `CustomEventDescription` from a path with `FMODguidMap:getEvent()`.
    /// `FMODguidMap:getEvent()` expects the path to be formatted similarly to event:/UI/Cancel or snapshot:/IngamePause.
    lua.new_usertype<FMODguidMap>(
        "FMODguidMap",
        "getEvent",
        [](FMODguidMap& guidmap, std::string path) -> std::optional<CustomEventDescription>
        {
            if (CustomEventDescription event_description = guidmap.get_event(path))
            {
                return event_description;
            }
            DEBUG("Failed to get CustomEventDescription from FMODguidMap");
            return sol::nullopt;
        }
    );
    /// Creates an `FMODguidMap` by parsing a GUIDs.txt exported from FMOD Studio from disk relative to this script. This is useful
    /// if you want to use a human readable FMOD event path to create a `CustomEventDescription` instead of using an FMOD GUID string.
    lua["create_fmod_guid_map"] = [](std::string path) -> sol::optional<FMODguidMap>
    {
        auto backend = LuaBackend::get_calling_backend();
        if (FMODguidMap map = backend->sound_manager->create_fmod_guid_map((backend->get_root_path() / path).string()))
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
        "sound_info",
        &SoundMeta::sound_info,
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
        &SoundMeta::playing,
        "start",
        &SoundMeta::start);

    lua.new_usertype<BackgroundSound>(
        "BackgroundSound",
        sol::base_classes,
        sol::bases<SoundMeta>());

    lua.new_usertype<SoundInfo>(
        "SoundInfo",
        "sound_id",
        sol::readonly(&SoundInfo::sound_id),
        "sound_name",
        sol::property([](SoundInfo& si)                 // -> VANILLA_SOUND
                      { return si.sound_name /**/; })); // return copy, so it's read only

    auto play_sound = sol::overload(static_cast<SoundMeta* (*)(SOUNDID, uint32_t)>(::play_sound), static_cast<SoundMeta* (*)(VANILLA_SOUND, uint32_t)>(::play_sound));

    /// Use source_uid to make the sound be played at the location of that entity, set it -1 to just play it "everywhere"
    /// Returns SoundMeta, beware that the sound can't be stopped (`start_over` and `playing` are unavailable). Should only be used for sfx.
    lua["play_sound"] = play_sound;

    // lua["convert_sound_id"] = convert_sound_id;
    /// NoDoc
    lua["convert_sound_id"] = sol::overload([](SOUNDID id) -> const VANILLA_SOUND&
                                            { auto back_end = LuaBackend::get_calling_backend(); 
        return back_end->sound_manager->convert_sound_id(id); },
                                            [](VANILLA_SOUND sound) -> SOUNDID
                                            {auto back_end = LuaBackend::get_calling_backend(); 
        return back_end->sound_manager->convert_sound_id(sound); });

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
