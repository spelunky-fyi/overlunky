#pragma once

#include <cstdint> // for uint8_t, uint32_t
#include <string>  // for string
#include <utility> // for pair

#include "aliases.hpp" // for TEXTURE, WORLD_SHADER
#include "color.hpp"   // for Color
#include "math.hpp"    // for Quad, AABB

namespace sol
{
class state;
} // namespace sol

struct TextRenderingInfo;

class VanillaRenderContext
{
  public:
    /// Draw text using the built-in renderer
    /// Use in combination with ON.RENDER_✱ events. See vanilla_rendering.lua in the example scripts.
    void draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle);
    void draw_text(const TextRenderingInfo* tri, Color color);

    /// Measure the provided text using the built-in renderer
    std::pair<float, float> draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer with angle, px/py is pivot for the rotatnion where 0,0 is center 1,1 is top right corner etc.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle, float px, float py);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color);

    /// Draw a texture in screen coordinates from top-left to bottom-right using the built-in renderer. `source` - the coordinates in the texture, `dest` - the coordinates on the screen
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color);

    /// Draw rectangle in screen coordinates from top-left to bottom-right using the built-in renderer.
    /// `thickness` is only used when `filled = false`, px/py is pivot for the rotatnion where 0,0 is center 1,1 is top right corner etc.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_rect(const AABB& rect, Color color, bool filled, float thickness, float angle, float px, float py);

    /// Draw rectangle in screen coordinates from top-left to bottom-right using the built-in renderer.
    /// `thickness` is only used when `filled = false`
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_rect(const Quad& dest, Color color, bool filled, float thickness);

    /// Draw triangle in screen coordinates using the built-in renderer.
    /// `thickness` is only used when `filled = false`
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_triangle(const Triangle& triangle, Color color, bool filled, float thickness);

    /// Draws a line on screen using the built-in renderer from point `A` to point `B`.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_line(const Vec2& A, const Vec2& B, Color color, float thickness);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    /// For more control use the version taking a Quad instead
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    /// For more control use the version taking a Quad instead
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& dest, Color color);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer with angle, px/py is pivot for the rotatnion where 0,0 is center 1,1 is top right corner etc.
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    /// For more control use the version taking a Quad instead
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& dest, Color color, float angle, float px, float py);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
    /// The `shader` parameter controls how to render the texture
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color, WORLD_SHADER shader);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    void draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer.  `source` - the coordinates in the texture, `dest` - the coordinates on the screen
    /// The `shader` parameter controls how to render the texture
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    void draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color, WORLD_SHADER shader);

    /// Draw a texture in world coordinates from top-left to bottom-right using the built-in renderer.  `source` - the coordinates in the texture, `dest` - the coordinates on the screen
    /// Use in combination with ON.RENDER_PRE_DRAW_DEPTH event
    void draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color);

    AABB bounding_box;
};

namespace NVanillaRender
{
void register_usertypes(sol::state& lua);
};
