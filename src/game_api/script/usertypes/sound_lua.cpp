#include "sound_lua.hpp"

#include "script/script_impl.hpp"
#include "sound_manager.hpp"

#include <sol/sol.hpp>

namespace NSound
{
void register_usertypes(sol::state& lua, LuaBackend* script)
{
    /// Loads a sound from disk relative to this script, ownership might be shared with other code that loads the same file. Returns nil if file can't be found
    lua["create_sound"] = [script](std::string path) -> sol::optional<CustomSound>
    {
        if (CustomSound sound = script->sound_manager->get_sound((script->get_root_path() / path).string()))
        {
            return sound;
        }
        return sol::nullopt;
    };

    /// Gets an existing sound, either if a file at the same path was already loaded or if it is already loaded by the game
    lua["get_sound"] = [script](std::string path_or_vanilla_sound) -> sol::optional<CustomSound>
    {
        if (CustomSound event = script->sound_manager->get_event(path_or_vanilla_sound))
        {
            return event;
        }
        else if (CustomSound sound = script->sound_manager->get_existing_sound((script->get_root_path() / path_or_vanilla_sound).string()))
        {
            return sound;
        }
        return sol::nullopt;
    };

    /// Returns unique id for the callback to be used in [clear_vanilla_sound_callback](#clear_vanilla_sound_callback).
    /// Sets a callback for a vanilla sound which lets you hook creation or playing events of that sound
    /// Callbacks are executed on another thread, so avoid touching any global state, only the local Lua state is protected
    /// If you set such a callback and then play the same sound yourself you have to wait until receiving the STARTED event before changing any
    /// properties on the sound. Otherwise you may cause a deadlock.
    lua["set_vanilla_sound_callback"] = [script](VANILLA_SOUND name, VANILLA_SOUND_CALLBACK_TYPE types, sol::function cb) -> CallbackId
    {
        auto safe_cb = [&, cb = std::move(cb)](PlayingSound sound)
        {
            std::lock_guard gil_guard{script->gil};
            if (script->get_enabled())
                script->handle_function(cb, sound);
        };
        std::uint32_t id = script->sound_manager->set_callback(name, std::move(safe_cb), static_cast<FMODStudio::EventCallbackType>(types));
        script->vanilla_sound_callbacks.push_back(id);
        return id;
    };
    /// Clears a previously set callback
    lua["clear_vanilla_sound_callback"] = [script](CallbackId id)
    {
        script->sound_manager->clear_callback(id);
        auto it = std::find(script->vanilla_sound_callbacks.begin(), script->vanilla_sound_callbacks.end(), id);
        if (it != script->vanilla_sound_callbacks.end())
        {
            script->vanilla_sound_callbacks.erase(it);
        }
    };

    auto play = sol::overload(
        static_cast<PlayingSound (CustomSound::*)()>(&CustomSound::play),
        static_cast<PlayingSound (CustomSound::*)(bool)>(&CustomSound::play),
        static_cast<PlayingSound (CustomSound::*)(bool, SOUND_TYPE)>(&CustomSound::play));
    /// Handle to a loaded sound, can be used to play the sound and receive a `PlayingSound` for more control
    /// It is up to you to not release this as long as any sounds returned by `CustomSound:play()` are still playing
    lua.new_usertype<CustomSound>("CustomSound", "play", play, "get_parameters", &CustomSound::get_parameters);
    /* CustomSound
        PlayingSound play(bool start_paused, SOUND_TYPE sound_type)
        map<VANILLA_SOUND_PARAM,string> get_parameters()
        */
    auto sound_set_callback = [script](PlayingSound* sound, sol::function callback)
    {
        auto safe_cb = [&, callback = std::move(callback)]()
        {
            std::lock_guard gil_guard{script->gil};
            if (script->get_enabled())
                script->handle_function(callback);
        };
        sound->set_callback(std::move(safe_cb));
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
        std::move(sound_set_callback),
        "get_parameters",
        &PlayingSound::get_parameters,
        "get_parameter",
        &PlayingSound::get_parameter,
        "set_parameter",
        &PlayingSound::set_parameter);
    /* PlayingSound
        bool is_playing()
        bool stop()
        bool set_pause(bool pause)
        bool set_mute(bool mute)
        bool set_pitch(float pitch)
        bool set_pan(float pan)
        bool set_volume(float volume)
        bool set_looping(SOUND_LOOP_MODE looping)
        bool set_callback(function callback)
        map<VANILLA_SOUND_PARAM,string> get_parameters()
        optional<float> get_parameter(VANILLA_SOUND_PARAM param)
        bool set_parameter(VANILLA_SOUND_PARAM param, float value)
        */

    /// Third parameter to `CustomSound:play()`, specifies which group the sound will be played in and thus how the player controls its volume
    lua.create_named_table("SOUND_TYPE", "SFX", 0, "MUSIC", 1);
    /// Paramater to `PlayingSound:set_looping()`, specifies what type of looping this sound should do
    lua.create_named_table("SOUND_LOOP_MODE", "OFF", 0, "LOOP", 1, "BIDIRECTIONAL", 2);
    /// Paramater to `get_sound()`, which returns a handle to a vanilla sound, and `set_vanilla_sound_callback()`,
    lua.create_named_table("VANILLA_SOUND"
                           //, "BGM_BGM_TITLE", BGM/BGM_title
                           //, "", ...check__[vanilla_sounds.txt]\[https://github.com/spelunky-fyi/overlunky/tree/main/docs/game_data/vanilla_sounds.txt\]...
                           //, "FX_FX_DM_BANNER", FX/FX_dm_banner
    );
    script->sound_manager->for_each_event_name(
        [&lua](std::string event_name)
        {
            std::string clean_event_name = event_name;
            std::transform(
                clean_event_name.begin(), clean_event_name.end(), clean_event_name.begin(), [](unsigned char c)
                { return std::toupper(c); });
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
                           //, "", ...check__[vanilla_sound_params.txt]\[https://github.com/spelunky-fyi/overlunky/tree/main/docs/game_data/vanilla_sound_params.txt\]...
                           //, "CURRENT_LAYER2", 37
    );
    script->sound_manager->for_each_parameter_name(
        [&lua](std::string parameter_name, std::uint32_t id)
        {
            std::transform(parameter_name.begin(), parameter_name.end(), parameter_name.begin(), [](unsigned char c)
                           { return std::toupper(c); });
            lua["VANILLA_SOUND_PARAM"][std::move(parameter_name)] = id;
        });
}
}; // namespace NSound
