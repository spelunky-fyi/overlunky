#pragma once

#include "entity.hpp"
#include <sol/forward.hpp>
#include <string>

class ScriptImpl;

class VanillaRenderContext
{
  public:
    /// Draw text using the built-in renderer. Use in combination with ON.RENDER_PRE_HUD or ON.RENDER_POST_HUD. See vanilla_rendering.lua in the example scripts.
    void draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle);

    /// Measure the provided text using the built-in renderer
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);

    /// Draw a texture in screen coordinates `x`, `y` using the built-in renderer. Use in combination with ON.RENDER_PRE_HUD or ON.RENDER_POST_HUD
    void draw_texture(uint32_t texture_id, uint8_t row, uint8_t column, float render_at_x, float render_at_y, float render_width, float render_height, Color color);
};

namespace NVanillaRender
{
void register_usertypes(sol::state& lua);
};
