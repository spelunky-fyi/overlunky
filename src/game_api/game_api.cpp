#include "game_api.hpp"

#include "render_api.hpp"
#include "search.hpp"
#include "state.hpp"

GameAPI* GameAPI::get()
{
    static_assert(sizeof(GameAPI) == 0x60);
    using GetGameAPI = GameAPI*();
    static auto addr = (GetGameAPI*)get_address("get_game_api");
    return addr();
}

float GameAPI::get_current_zoom() const
{
    auto state = State::get().ptr();
    return renderer->current_zoom + get_layer_transition_zoom_offset(state->camera_layer);
}

void GameAPI::set_zoom(std::optional<float> current, std::optional<float> target)
{
    if (current.has_value())
    {
        renderer->current_zoom = current.value(); // - renderer->current_zoom_offset;
    }
    if (target.has_value())
    {
        renderer->target_zoom = target.value(); // - renderer->target_zoom_offset;
    }
}
