#include "bucket.hpp"

#include "containers/game_allocator.hpp"
#include "entities_chars.hpp"
#include "game_manager.hpp"
#include "items.hpp"
#include "memory.hpp"
#include "screen.hpp"
#include "state.hpp"

Bucket* Bucket::get()
{
    static auto bucket_offset = get_address("hundun_door_control") - 12; // another random CCCC with some space for a pointer
    auto bucket = reinterpret_cast<Bucket*>(memory_read<size_t>(bucket_offset));
    if ((size_t)bucket != 0xCCCCCCCCCCCCCCCC)
        return bucket;
    auto new_bucket = new Bucket();
    write_mem_prot(bucket_offset, new_bucket, true);
    new_bucket->pause_api = new PauseAPI();
    new_bucket->io = new SharedIO();
    return new_bucket;
}

PAUSE_TYPE PauseAPI::get_pause()
{
    pause = (PAUSE_TYPE)(State::get().ptr()->pause | ((uint32_t)pause & ~0x3f));
    return pause;
}

void PauseAPI::set_pause(PAUSE_TYPE flags)
{
    auto state = State::get().ptr();
    pause = flags;
    state->pause = (uint8_t)(((uint32_t)flags) & 0x3f);
}

bool PauseAPI::check_trigger(PAUSE_TRIGGER& trigger, PAUSE_SCREEN& screen)
{
    bool match = false;
    auto state = State::get().ptr();

    if (state->loading == 2 && (trigger & PAUSE_TRIGGER::SCREEN) != PAUSE_TRIGGER::NONE && (screen == PAUSE_SCREEN::NONE || (screen & (PAUSE_SCREEN)(1 << state->screen_next)) != PAUSE_SCREEN::NONE))
        match = true;

    if ((trigger & PAUSE_TRIGGER::FADE_START) != PAUSE_TRIGGER::NONE && state->fade_timer > 0 && state->fade_timer == state->fade_length && state->fade_timer != last_fade_timer)
        match = true;

    if ((trigger & PAUSE_TRIGGER::FADE_END) != PAUSE_TRIGGER::NONE && state->fade_timer == 1 && state->fade_timer != last_fade_timer)
        match = true;

    if ((trigger & PAUSE_TRIGGER::EXIT) != PAUSE_TRIGGER::NONE && (state->screen == 12 || state->screen == 11) && (state->level_flags & (1 << 20)) && !(last_level_flags & (1 << 20)))
        match = true;

    if (match && (trigger & PAUSE_TRIGGER::ONCE) != PAUSE_TRIGGER::NONE)
        trigger = PAUSE_TRIGGER::NONE;

    if (match && last_trigger_frame == get_global_update_count())
        match = false;

    return match;
}

bool PauseAPI::loading()
{
    auto state = State::get().ptr();
    auto gm = get_game_manager();
    bool loading = state->loading > 0 || state->fade_timer > 0 || (state->screen == 4 && gm->screen_menu->menu_text_opacity < 1) || (state->screen == 9 && (state->screen_character_select->topleft_woodpanel_esc_slidein == 0 || state->screen_character_select->start_pressed)) || state->logic->ouroboros;
    if ((state->loading == 3 && (state->fade_timer <= 1 || state->fade_length == 0)) || (state->loading == 1 && state->fade_timer == state->fade_length))
        loading = false;
    return loading;
}

bool PauseAPI::event(PAUSE_TYPE pause_event)
{
    bool block = false;
    std::optional<bool> force;
    auto state = State::get().ptr();

    if (skip_fade)
    {
        state->fade_enabled = false;
        state->fade_value = 0;
        state->fade_timer = 0;
    }

    if (skip)
    {
        state->pause &= ~((uint8_t)pause_type & 0x3f);
        return block;
    }

    if (((pause_type & pause_event) != PAUSE_TYPE::NONE || ((uint8_t)pause_type & 0x3f && pause_event == PAUSE_TYPE::PRE_UPDATE)) && (ignore_screen_trigger & (PAUSE_SCREEN)(1 << state->screen)) == PAUSE_SCREEN::NONE)
    {
        if (check_trigger(pause_trigger, pause_screen))
        {
            set_paused(true);
            force = true;
            last_trigger_frame = get_global_update_count();
        }
        if (check_trigger(unpause_trigger, unpause_screen))
        {
            set_paused(false);
            force = false;
            last_trigger_frame = get_global_update_count();
        }
        last_fade_timer = state->fade_timer;
        last_level_flags = state->level_flags;
    }

    auto is_loading = loading();

    if ((get_pause() & pause_event) != PAUSE_TYPE::NONE)
        block = true;

    if ((ignore_screen & (PAUSE_SCREEN)(1 << state->screen)) != PAUSE_SCREEN::NONE)
        block = false;

    if ((ignore_screen & PAUSE_SCREEN::LOADING) != PAUSE_SCREEN::NONE && is_loading)
        block = false;

    if ((ignore_screen & PAUSE_SCREEN::EXIT) != PAUSE_SCREEN::NONE && (state->screen == 12 || state->screen == 11) && (state->level_flags & (1 << 20)))
        block = false;

    if (force.has_value())
    {
        block = force.value();
        if ((pause_type & PAUSE_TYPE::FORCE_STATE) != PAUSE_TYPE::NONE && !is_loading && (uint8_t)pause_type & 0x3f)
            set_paused(block);
    }

    if ((pause & PAUSE_TYPE::FORCE_STATE) != PAUSE_TYPE::NONE)
        set_paused(true);

    if (update_camera && ((block && (pause_event == PAUSE_TYPE::PRE_UPDATE || pause_event == PAUSE_TYPE::PRE_GAME_LOOP)) || ((pause_event == PAUSE_TYPE::PRE_UPDATE && (uint8_t)pause_type & 0x3f) && state->pause > 0)) && ((state->pause & 1) == 0 || (uint8_t)pause_type & 1))
        update_camera_position();

    if ((pause_type & pause_event) != PAUSE_TYPE::NONE)
        blocked = block;

    return block;
}

void PauseAPI::post_loop()
{
    auto state = State::get().ptr();
    if (skip)
        state->pause |= (uint8_t)pause_type & 0x3f;
    skip = false;
}

bool PauseAPI::pre_input()
{
    static const auto bucket = Bucket::get();

    if (!bucket->overlunky)
        return false;

    auto gm = get_game_manager();
    auto state = State::get().ptr();

    if (bucket->pause_api->modifiers_block & bucket->pause_api->modifiers_down)
    {
        if (bucket->pause_api->modifiers_clear_input)
        {
            gm->game_props->buttons_menu = 0;
            gm->game_props->buttons_menu_previous = 0;
            for (size_t i = 0; i < state->items->player_count; ++i)
            {
                if (gm->game_props->input_index[i] > 4)
                    continue;
                state->player_inputs->player_slots[i].buttons = 0;
                state->player_inputs->player_slots[i].buttons_gameplay = 0;
                gm->game_props->buttons[i] = 0;
                gm->game_props->buttons_previous[i] = 0;
                if (state->items->players[i])
                {
                    state->items->players[i]->buttons = 0;
                    state->items->players[i]->buttons_previous = 0;
                    state->items->players[i]->movex = 0;
                    state->items->players[i]->movey = 0;
                }
            }
        }
        return true;
    }

    return false;
}

void PauseAPI::post_input()
{
    static const auto bucket = Bucket::get();

    if (!bucket->overlunky)
        return;

    auto gm = get_game_manager();
    auto kb = get_raw_input()->keyboard;

    if (((bucket->overlunky->keys["toggle_pause"] & 32) || (bucket->overlunky->keys["frame_advance"] & 32) || (bucket->overlunky->keys["frame_advance_alt"] & 32)) && kb[39].down && !kb[32].down)
        gm->game_props->buttons_menu &= ~1;
}
