#pragma once

#include <sol/forward.hpp>

#include <string>

#include "math.hpp"

class ScriptImpl;

using uColor = uint32_t;

class GuiDrawContext
{
  public:
    GuiDrawContext(class ScriptImpl* script, struct ImDrawList* draw_list);

    /// Draws a line on screen
    void draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color);
    /// Draws a rectangle on screen from top-left to bottom-right.
    void draw_rect(float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color);
    /// Draws a rectangle on screen from top-left to bottom-right.
    void draw_rect(AABB rect, float thickness, float rounding, uColor color);
    /// Draws a filled rectangle on screen from top-left to bottom-right.
    void draw_rect_filled(float x1, float y1, float x2, float y2, float rounding, uColor color);
    /// Draws a filled rectangle on screen from top-left to bottom-right.
    void draw_rect_filled(AABB rect, float rounding, uColor color);
    /// Draws a circle on screen
    void draw_circle(float x, float y, float radius, float thickness, uColor color);
    /// Draws a filled circle on screen
    void draw_circle_filled(float x, float y, float radius, uColor color);
    /// Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
    void draw_text(float x, float y, float size, std::string text, uColor color);
    /// Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
    void draw_image(int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color);
    /// Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
    void draw_image(int image, AABB rect, AABB uv_rect, uColor color);
    /// Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
    void draw_image_rotated(int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py);
    /// Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
    void draw_image_rotated(int image, AABB rect, AABB uv_rect, uColor color, float angle, float px, float py);

    /// Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the
    /// [ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to
    /// autosize in center. Use just a `##Label` as title to hide titlebar.
    /// **Important: Keep all your labels unique!** If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to
    /// give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text... The window api is probably evolving
    /// still, this is just the first draft. Felt cute, might delete later!
    /// Returns false if the window was closed from the X.
    bool window(std::string title, float x, float y, float w, float h, bool movable, sol::function callback);
    /// Add some text to window, automatically wrapped
    void win_text(std::string text);
    /// Add a separator line to window
    void win_separator();
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
    /// Pop unique identifier from the stack. Put after the input.
    void win_popid();
    /// Draw image to window.
    void win_image(int image, int width, int height);

  private:
    class ScriptImpl* script;
    struct ImDrawList* draw_list;
};

namespace NGui
{
void register_usertypes(sol::state& lua, ScriptImpl* script);
};
