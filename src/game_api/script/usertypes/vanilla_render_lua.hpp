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
    /// If you can, consider creating your own TextureRenderingInfo instead
    /// You can then use `:text_size()` and `draw_text` with that one object
    /// `draw_text_size` works by creating new TextureRenderingInfo just to call `:text_size()`, which is not very optimal
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

    /// Draws a line on screen using the built-in renderer from point `A` to point `B`.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_line(const Vec2& A, const Vec2& B, float thickness, Color color);

    /// Draw rectangle in screen coordinates from top-left to bottom-right using the built-in renderer with optional `angle`.
    /// `px`/`py` is pivot for the rotatnion where 0,0 is center 1,1 is top right corner etc. (corner from the AABB, not the visible one from adding the `thickness`)
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_rect(const AABB& rect, float thickness, Color color, std::optional<float> angle, std::optional<float> px, std::optional<float> py);

    void draw_screen_rect_filled(const AABB& rect, Color color, std::optional<float> angle, std::optional<float> px, std::optional<float> py);

    /// Draw triangle in screen coordinates using the built-in renderer.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_triangle(const Triangle& triangle, float thickness, Color color);

    /// Draw filled triangle in screen coordinates using the built-in renderer.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_triangle_filled(const Triangle& triangle, Color color);

    /// Draw a polyline on screen from points using the built-in renderer
    /// Draws from the first to the last point, use `closed` to connect first and last as well
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_poly(std::vector<Vec2> points, float thickness, Color color, bool closed);

    /// Draw quadrilateral in screen coordinates from top-left to bottom-right using the built-in renderer.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_poly(const Quad& points, float thickness, Color color, bool closed);

    /// Draw a convex polygon on screen from points using the built-in renderer
    /// Can probably draw almost any polygon, but the convex one is guaranteed to look correct
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_poly_filled(std::vector<Vec2> points, Color color);

    /// Draw filled quadrilateral in screen coordinates from top-left to bottom-right using the built-in renderer.
    /// Use in combination with ON.RENDER_✱_HUD/PAUSE_MENU/JOURNAL_PAGE events
    void draw_screen_poly_filled(const Quad& points, Color color);

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
