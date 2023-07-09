#pragma once

#include <sol/forward.hpp> // for function
#include <string>          // for string
#include <vector>          // for vector

#include "aliases.hpp" // for uColor, IMAGE
#include "math.hpp"    // for Vec2, AABB (ptr only)

enum class DRAW_LAYER
{
    BACKGROUND,
    FOREGROUND,
    WINDOW
};

struct ImGuiContext;

namespace sol
{
class state;
} // namespace sol

class GuiDrawContext
{
  public:
    GuiDrawContext(class LuaBackend* script);

    /// Draws a line on screen
    void draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color);
    /// Draws a rectangle on screen from top-left to bottom-right.
    void draw_rect(float left, float top, float right, float bottom, float thickness, float rounding, uColor color);
    /// Draws a rectangle on screen from top-left to bottom-right.
    void draw_rect(AABB rect, float thickness, float rounding, uColor color);
    /// Draws a filled rectangle on screen from top-left to bottom-right.
    void draw_rect_filled(float left, float top, float right, float bottom, float rounding, uColor color);
    /// Draws a filled rectangle on screen from top-left to bottom-right.
    void draw_rect_filled(AABB rect, float rounding, uColor color);
    /// Draws a triangle on screen.
    void draw_triangle(Vec2 p1, Vec2 p2, Vec2 p3, float thickness, uColor color);
    /// Draws a filled triangle on screen.
    void draw_triangle_filled(Vec2 p1, Vec2 p2, Vec2 p3, uColor color);
    /// Draws a polyline on screen.
    void draw_poly(std::vector<Vec2> points, float thickness, uColor color);
    /// Draws a filled convex polyline on screen.
    void draw_poly_filled(std::vector<Vec2> points, uColor color);
    /// Draws a cubic bezier curve on screen.
    void draw_bezier_cubic(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, float thickness, uColor color);
    /// Draws a quadratic bezier curve on screen.
    void draw_bezier_quadratic(Vec2 p1, Vec2 p2, Vec2 p3, float thickness, uColor color);
    /// Draws a circle on screen
    void draw_circle(float x, float y, float radius, float thickness, uColor color);
    /// Draws a filled circle on screen
    void draw_circle_filled(float x, float y, float radius, uColor color);
    /// Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
    void draw_text(float x, float y, float size, std::string text, uColor color);
    /// Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
    void draw_image(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color);
    /// Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
    void draw_image(IMAGE image, AABB rect, AABB uv_rect, uColor color);
    /// Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
    void draw_image_rotated(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py);
    /// Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
    void draw_image_rotated(IMAGE image, AABB rect, AABB uv_rect, uColor color, float angle, float px, float py);
    /// Draw on top of UI windows, including platform windows that may be outside the game area, or only in current widget window. Defaults to main viewport background.
    void draw_layer(DRAW_LAYER layer);

    /// Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the
    /// [ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to
    /// autosize in center. Use just a `##Label` as title to hide titlebar.
    /// **Important: Keep all your labels unique!** If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to
    /// give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text...
    /// Returns false if the window was closed from the X.
    /// <br/>The callback signature is nil win(GuiDrawContext ctx, Vec2 pos, Vec2 size)
    bool window(std::string title, float x, float y, float w, float h, bool movable, sol::function callback);
    /// Add some text to window, automatically wrapped
    void win_text(std::string text);
    /// Add a separator line to window
    void win_separator();
    /// Add a separator text line to window
    void win_separatortext(std::string text);
    /// Add next thing on the same line. This is same as `win_sameline(0, -1)`
    void win_inline();
    /// Add next thing on the same line, with an offset
    void win_sameline(float offset, float spacing);
    /// Add a button
    bool win_button(std::string text);
    /// Add a text field
    std::string win_input_text(std::string label, std::string value);
    /// Add an integer field
    int win_input_int(std::string label, int value);
    /// Add a float field
    float win_input_float(std::string label, float value);
    /// Add an integer slider
    int win_slider_int(std::string label, int value, int min, int max);
    /// Add an integer dragfield
    int win_drag_int(std::string label, int value, int min, int max);
    /// Add an float slider
    float win_slider_float(std::string label, float value, float min, float max);
    /// Add an float dragfield
    float win_drag_float(std::string label, float value, float min, float max);
    /// Add a checkbox
    bool win_check(std::string label, bool value);
    /// Add a combo box
    int win_combo(std::string label, int selected, std::string opts);
    /// Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
    void win_pushid(int id);
    /// Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
    void win_pushid(std::string id);
    /// Pop unique identifier from the stack. Put after the input.
    void win_popid();
    /// Draw image to window.
    void win_image(IMAGE image, float width, float height);
    /// Draw imagebutton to window.
    bool win_imagebutton(std::string label, IMAGE image, float width, float height, float uvx1, float uvy1, float uvx2, float uvy2);
    /// Sets a tooltip to show when hovering the cursor over the previous item.
    void win_tooltip(std::string text);
    /// Add a collapsing accordion section, put contents in the callback function.
    void win_section(std::string title, sol::function callback);
    /// Indent contents, or unindent if negative
    void win_indent(float width);
    /// Sets next item width (width>1: width in pixels, width<0: to the right of window, -1<width<1: fractional, multiply by available window width)
    void win_width(float width);

  private:
    class LuaBackend* backend;
    DRAW_LAYER drawlist;
    ImGuiContext& g;
};

namespace NGui
{
void register_usertypes(sol::state& lua);
};
