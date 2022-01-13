#pragma once

#include "entity.hpp"
#include "render_api.hpp"
#include <sol/forward.hpp>
#include <string>

class ScriptImpl;

class VanillaRenderContext
{
  public:
    /// Draw text using the built-in renderer. Use in combination with ON.RENDER_✱ events. See vanilla_rendering.lua in the example scripts.
    void draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle);

    /// Measure the provided text using the built-in renderer
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer. Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color);

    AABB bounding_box;
};

namespace NVanillaRender
{
void register_usertypes(sol::state& lua);
};
