#pragma once

#include <sol/forward.hpp> // for function
#include <string>          // for string
#include <vector>          // for vector

#include "aliases.hpp" // for uColor, IMAGE
#include "color.hpp"   // for Color
#include "imgui_internal.h"
#include "math.hpp" // for Vec2, AABB (ptr only)

using GUI_CONDITION = int;

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
    ~GuiDrawContext();

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
    /// Create a new widget window. The window functions are just wrappers for [ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there.
    /// **Important: Keep all your widget ids unique!** If you need inputs with the same visible label, add `##SomeUniqueId` after the label,
    /// or use `win_pushid` to give things unique ids. ImGui doesn't know what you interacted with if all your widgets have the same id.
    /// See [ImGui docs](https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-about-the-id-stack-system) for more information about unique ids.
    /// `title`: Title and id of the window. Avoid changing the id of an active window. Every time the id changes, ImGui will discard the old window and create a new one. Use something like `##SomeUniqueId` as the title to hide the title bar.
    /// `x`, `y`: Initial position of the window, in screen coordinates.
    /// `w`, `h`: Initial size of the window, in screen coordinates.
    /// `movable`: Whether the user can move and resize the window.
    /// `callback`: Add all `win_*` widgets in here. Signature is `nil function(GuiDrawContext ctx, Vec2 pos, Vec2 size, bool collapsed)`.
    /// Set `x`, `y`, `w`, and `h` to `0, 0, 0, 0` to autosize in center of screen.
    /// Returns false when the window is closed by the user.
    bool window(std::string title, float x, float y, float w, float h, bool movable, sol::function callback);
    /// Create a new widget window. The window functions are just wrappers for [ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there.
    /// **Important: Keep all your widget ids unique!** If you need inputs with the same visible label, add `##SomeUniqueId` after the label,
    /// or use `win_pushid` to give things unique ids. ImGui doesn't know what you interacted with if all your widgets have the same id.
    /// See [ImGui docs](https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-about-the-id-stack-system) for more information about unique ids.
    /// `title`: Title and id of the window. Avoid changing the id of an active window. Every time the id changes, ImGui will discard the old window and create a new one.
    /// `x`, `y`: Position of the window, in screen coordinates.
    /// `w`, `h`: Size of the window, in screen coordinates.
    /// `collapsed`: Collapsed state of the window.
    /// `pos_cond`: GUI_CONDITION for applying the position.
    /// `size_cond`: GUI_CONDITION for applying the size.
    /// `collapsed_cond`: GUI_CONDITION for applying the collapsed state.
    /// `flags`: Mask of GUI_WINDOW_FLAG.
    /// `callback`: Add all `win_*` widgets in here. Signature is `nil function(GuiDrawContext ctx, Vec2 pos, Vec2 size, bool collapsed)`.
    /// Set `x`, `y`, `w`, and `h` to `0, 0, 0, 0` to autosize in center of screen.
    /// Returns false when the window is closed by the user.
    bool window(std::string title, float x, float y, float w, float h, bool collapsed, GUI_CONDITION pos_cond, GUI_CONDITION size_cond, GUI_CONDITION collapsed_cond, int flags, sol::function callback);
    /// Add a child window with its own scrolling and clipping. `flags` are a mask of GUI_WINDOW_FLAG.
    void win_child(std::string id, float w, float h, bool border, int flags, sol::function callback);
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
    bool win_button(std::string label);
    /// Add a button
    bool win_button(std::string label, float width, float height);
    /// Add a text field
    std::string win_input_text(std::string label, std::string value);
    /// Add an integer field
    int win_input_int(std::string label, int value);
    /// Add a float field
    float win_input_float(std::string label, float value);
    /// Add an integer slider.
    int win_slider_int(std::string label, int value, int min, int max);
    /// Add an integer slider. `flags` are a mask of GUI_SLIDER_FLAG.
    int win_slider_int(std::string label, int value, int min, int max, std::string format, int flags);
    /// Add an integer dragfield.
    int win_drag_int(std::string label, int value, int min, int max);
    /// Add an integer dragfield. `flags` are a mask of GUI_SLIDER_FLAG.
    int win_drag_int(std::string label, int value, int min, int max, float speed, std::string format, int flags);
    /// Add a float slider.
    float win_slider_float(std::string label, float value, float min, float max);
    /// Add a float slider. `flags` are a mask of GUI_SLIDER_FLAG.
    float win_slider_float(std::string label, float value, float min, float max, std::string format, int flags);
    /// Add a float dragfield.
    float win_drag_float(std::string label, float value, float min, float max);
    /// Add a float dragfield. `flags` are a mask of GUI_SLIDER_FLAG.
    float win_drag_float(std::string label, float value, float min, float max, float speed, std::string format, int flags);
    /// Add a checkbox
    bool win_check(std::string label, bool value);
    /// Add a combo box
    int win_combo(std::string label, int selected, std::string opts);
    /// Add a color editor
    Color win_color_editor(std::string label, Color value, bool can_edit_alpha);
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
    /// Add a tab bar. Only create tab items inside the callback.
    void win_tab_bar(std::string id, sol::function callback);
    /// Add a tab bar. Only create tab items inside the callback. `flags` are a mask of GUI_TAB_BAR_FLAG.
    void win_tab_bar(std::string id, int flags, sol::function callback);
    /// Add a tab item. Only use this inside a tab bar callback. Put contents in the callback function. Returns false when the tab is closed.
    bool win_tab_item(std::string label, bool closeable, sol::function callback);
    /// Add a tab item. Only use this inside a tab bar callback. Put contents in the callback function. `flags` are a mask of GUI_TAB_ITEM_FLAG. Returns false when the tab is closed.
    bool win_tab_item(std::string label, bool closeable, int flags, sol::function callback);
    /// Add a tab item button. Only use this inside a tab bar callback. Returns true when clicked.
    bool win_tab_item_button(std::string label);
    /// Add a tab item button. Only use this inside a tab bar callback. `flags` are a mask of GUI_TAB_ITEM_FLAG. Returns true when clicked.
    bool win_tab_item_button(std::string label, int flags);
    /// Add a menu bar. Only create menus inside the callback.
    void win_menu_bar(sol::function callback);
    /// Add a menu. Only use this inside a menu bar or menu callback. Can contain most widgets, including nested menus.
    void win_menu(std::string label, sol::function callback);
    /// Add a menu. Only use this inside a menu bar or menu callback. Can contain most widgets, including nested menus.
    void win_menu(std::string label, bool enabled, sol::function callback);
    /// Add a menu item. Only use this inside a menu callback. Returns true when clicked.
    bool win_menu_item(std::string label);
    /// Add a menu item. Only use this inside a menu callback. Returns true when clicked. Shortcut text is only visual and does not create a shortcut key.
    bool win_menu_item(std::string label, std::optional<std::string> shortcut, bool checked, bool enabled);
    /// Widgets created in the callback are grouped together into a box starting at the current horizontal position. Calls affecting single items, such as `win_tooltip`, will treat the whole group as one item.
    void win_group(sol::function callback);
    /// Add a dummy item that fills space in the window.
    void win_dummy(float width, float height);
    /// Indent contents, or unindent if negative
    void win_indent(float width);
    /// Sets next item width (width>1: width in pixels, width<0: to the right of window, -1<width<1: fractional, multiply by available window width)
    void win_width(float width);
    /// Disable user interactions and dim widgets inside callback.
    void win_disabled(sol::function callback);
    /// Disable user interactions and dim widgets inside callback. Widgets are not disabled if `disabled` is false.
    void win_disabled(bool disabled, std::optional<sol::function> callback);
    // TODO: "Cursor" may be a confusing name.
    // TODO: What should I do about the different coordinate systems for these calls?
    Vec2 win_get_cursor_pos();
    // TODO
    void win_set_cursor_pos(float x, float y);
    // TODO
    void win_set_cursor_pos(Vec2 p);
    // TODO
    Vec2 win_get_cursor_screen_pos();
    // TODO
    void win_set_cursor_screen_pos(float x, float y);
    // TODO
    void win_set_cursor_screen_pos(Vec2 p);
    // TODO
    Vec2 win_get_content_region_max();
    // TODO
    AABB win_get_item_rect();

  private:
    class LuaBackend* backend;
    DRAW_LAYER drawlist;
    ImGuiContext& g;
    ImGuiStackSizes stack_sizes;
};

namespace NGui
{
void register_usertypes(sol::state& lua);
};
