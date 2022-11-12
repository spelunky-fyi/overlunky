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

    /// Returns unique id for the callback to be used in [clear_vanilla_sound_callback](#clear_vanilla_sound_callback).
    /// Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
    /// Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
    /// If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any properties on the sound. Otherwise you may cause a deadlock.
    /// The callback signature is nil on_vanilla_sound(PlayingSound sound)
    lua["set_vanilla_sound_callback"] = [](VANILLA_SOUND name, VANILLA_SOUND_CALLBACK_TYPE types, sol::function cb) -> CallbackId
    {
        auto backend_id = LuaBackend::get_calling_backend_id();
        auto safe_cb = [backend_id, cb = std::move(cb)](PlayingSound sound)
        {
            auto backend = LuaBackend::get_backend(backend_id);
            if (backend->get_enabled())
                backend->handle_function(cb, std::make_unique<PlayingSound>(sound));
        };
        auto backend = LuaBackend::get_backend(backend_id);
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
        auto backend_id = LuaBackend::get_calling_backend_id();
        auto safe_cb = [backend_id, callback = std::move(callback)]()
        {
            auto backend = LuaBackend::get_backend(backend_id);
            if (backend->get_enabled())
                backend->handle_function(callback);
        };
        sound->set_callback(std::move(safe_cb));
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
        //"left_channel",
        //&SoundMeta::left_channel, // TODO: index 0-37 instead of 1-38
        //"right_channel",
        //&SoundMeta::right_channel,
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
