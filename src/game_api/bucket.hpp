#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "aliases.hpp"

using BucketItem = std::variant<bool, int64_t, float, std::string>;
using KEY = int64_t;

struct RecoverableMemory
{
    size_t address;
    char* old_data;
    size_t size;
    bool prot_used;
};

struct EditedMemory
{
    std::vector<RecoverableMemory> mem;
    bool dirty;
};

struct Overlunky
{
    /// Current Overlunky options. Read only.
    std::map<std::string, BucketItem> options;
    /// Write [some select options](https://github.com/search?q=repo%3Aspelunky-fyi%2Foverlunky+legal_options+language%3AC%2B%2B&type=code&l=C%2B%2B) here to change Overlunky options.
    std::map<std::string, BucketItem> set_options;
    /// Current Overlunky key bindings. Read only. You can use this to bind some custom feature to the same unknown key as currently bound by the user.
    std::map<std::string, KEY> keys;
    /// Disable some key bindings in Overlunky, whatever key they are actually bound to. Remember this might not be bound to the default any more, so only use this if you also plan on overriding the current keybinding, or just need to disable some feature and don't care what key it is bound on.
    std::unordered_set<std::string> ignore_keys;
    /// Disable keys that may or may not be used by Overlunky. You should probably write the keycodes you need here just in case if you think using OL will interfere with your keybinds.
    std::unordered_set<KEY> ignore_keycodes;
    /// TODO: Disable Overlunky features. Doesn't do anything yet.
    std::unordered_set<std::string> ignore_features;
    /// Currently selected uid in the entity picker or -1 if nothing is selected.
    int32_t selected_uid;
    /// Set currently selected uid in the entity picker or -1 to clear selection.
    std::optional<int32_t> set_selected_uid;
    /// Currently hovered entity uid or -1 if nothing is hovered.
    int32_t hovered_uid;
    /// Currently selected uids in the entity finder.
    std::vector<uint32_t> selected_uids;
    /// Set currently selected uids in the entity finder.
    std::optional<std::vector<uint32_t>> set_selected_uids;
};

struct PauseAPI
{
    /// Current pause state bitmask. Use custom PAUSE_TYPE.PRE_✱ (or multiple) to freeze the game at the specified callbacks automatically. Checked after the matching ON update callbacks, so can be set on the same callback you want to block at the latest. Vanilla PAUSE flags will be forwarded to state.pause, but use of vanilla PAUSE flags is discouraged and might not work with other PauseAPI features.
    PAUSE_TYPE pause{PAUSE_TYPE::NONE};
    /// Pause mask to toggle when using the PauseAPI methods to set or get pause state.
    PAUSE_TYPE pause_type{PAUSE_TYPE::PRE_UPDATE};

    /// Bitmask for conditions when the current `pause_type` should be automatically enabled in `pause`, can have multiple conditions.
    PAUSE_TRIGGER pause_trigger{PAUSE_TRIGGER::NONE};
    /// Bitmask to only enable PAUSE_TRIGGER.SCREEN during specific SCREEN, or any screen when NONE.
    PAUSE_SCREEN pause_screen{PAUSE_SCREEN::NONE};

    /// Bitmask for conditions when the current `pause_type` should be automatically disabled in `pause`, can have multiple conditions.
    PAUSE_TRIGGER unpause_trigger{PAUSE_TRIGGER::NONE};
    /// Bitmask to only enable PAUSE_TRIGGER.SCREEN during specific SCREEN, or any screen when NONE.
    PAUSE_SCREEN unpause_screen{PAUSE_SCREEN::NONE};

    /// Global frame stamp when one of the triggers was last triggered, used to prevent running them again on the same frame on unpause.
    int64_t last_trigger_frame{0};
    /// Fade timer stamp when fade triggers were last checked.
    int64_t last_fade_timer{0};
    /// Used to detect changes in state.level_flags for triggers.
    uint32_t last_level_flags{0};

    /// Bitmask for game SCREEN where the PRE_✱ pause types are ignored, even though enabled in `pause`. Can also use the special cases [FADE, EXIT] to unfreeze temporarily during fades (or other screen transitions where player input is probably impossible) or the level exit walk of shame.
    PAUSE_SCREEN ignore_screen{PAUSE_SCREEN::NONE};
    /// Bitmask for game SCREEN where the triggers are ignored.
    PAUSE_SCREEN ignore_screen_trigger{PAUSE_SCREEN::NONE};

    /// Set to true to unfreeze the game for one update cycle. Sets back to false after ON.POST_GAME_LOOP, so it can be used to check if current frame is a frame advance frame.
    bool skip{false};
    /// Set to true to enable normal camera movement when the game is paused or frozen on a callback by PauseAPI.
    bool update_camera{false};
    /// Is true when PauseAPI is freezing the game.
    bool blocked{false};
    /// Set to true to skip all fade transitions, forcing fade_timer and fade_value to 0 on every update.
    bool skip_fade{false};
    /// Set to true to run pause logic and triggers only in the last API instance in the chain (Playlunky) when multiple instances of the API (Overlunky and Playlunky) are injected.
    bool last_instance{false};

    /// Bitmask of modifier KEYs that are currently held
    uint32_t modifiers_down{0};
    /// Bitmask of modifier KEYs that will block all game input
    uint32_t modifiers_block{0};
    /// Enable to clear affected input when modifiers are held, disable to ignore all input events, i.e. keep held button state as it was before pressing the modifier key
    bool modifiers_clear_input{false};

    /// Get the current pause flags
    PAUSE_TYPE get_pause();
    /// Set the current pause flags
    void set_pause(PAUSE_TYPE flags);
    /// Enable/disable the current pause_type flags in pause state
    bool set_paused(bool enable = true)
    {
        if (enable)
            set_pause(get_pause() | pause_type);
        else
            set_pause(get_pause() & (~pause_type));
        return paused();
    }
    /// Is the game currently paused and that pause state matches any of the current the pause_type
    bool paused()
    {
        return get_pause() != PAUSE_TYPE::NONE && (get_pause() & pause_type) != PAUSE_TYPE::NONE;
    }
    /// Toggles pause state
    bool toggle()
    {
        if (paused())
            set_paused(false);
        else
            set_paused(true);
        return paused();
    }
    /// Sets skip
    void frame_advance()
    {
        skip = true;
    }
    /// Is the game currently loading and PAUSE_SCREEN.LOADING would be triggered, based on state.loading and some arbitrary checks.
    bool loading();

    void apply()
    {
        set_pause(pause);
    }
    bool event(PAUSE_TYPE event);
    bool check_trigger(PAUSE_TRIGGER& trigger, PAUSE_SCREEN& screen) const;
    void pre_loop()
    {
        blocked = false;
    }
    void post_loop();
    bool pre_input();
    void post_input();
};

struct SharedIO
{
    std::optional<bool> WantCaptureKeyboard;
    std::optional<bool> WantCaptureMouse;
};

class Bucket
{
  public:
    static Bucket* get();

    /// You can store arbitrary simple values here in Playlunky to be read in on Overlunky script for example.
    std::unordered_map<std::string, BucketItem> data;
    /// Access Overlunky options here, nil if Overlunky is not loaded.
    Overlunky* overlunky{nullptr};
    // Used by state.cpp to determine if patches should still be applied.
    bool patches_applied{false};
    // Used by Overlunky to restart adventure runs with same seed, updated by PRE_LOAD_SCREEN
    std::pair<int64_t, int64_t> adventure_seed{0, 0};
    // Used by memory for recoverable memory interoperability
    std::unordered_map<std::string, EditedMemory> original_memory;
    /// PauseAPI is used by Overlunky and can be used to control the Overlunky pause options from scripts. Can be accessed from the global `pause` more easily.
    PauseAPI* pause_api;
    // Used by blockable PRE callbacks to forward BLOCKED events from Overlunky to Playlunky
    bool forward_blocked_events{false};
    // Set to true when the callback was blocked by Overlunky and should also be blocked in Playlunky
    bool blocked_event{false};
    /// Shared part of ImGuiIO to block keyboard/mouse input across API instances.
    SharedIO* io{nullptr};
    /// Number of API instances present
    int count{0};
    // holds next id for new custom string
    STRINGID next_stringid{0};
    // Holds all the custom strings added thru add_string
    std::unordered_map<STRINGID, std::u16string> custom_strings;
    // uid to custom string id for shop items
    std::unordered_map<uint32_t, STRINGID> custom_shopitem_names;

  private:
    Bucket() = default;
    Bucket(const Bucket&) = delete;
    Bucket& operator=(const Bucket&) = delete;
};
