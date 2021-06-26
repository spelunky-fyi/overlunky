#include "gui_lua.hpp"

#include "file_api.hpp"
#include "script/script_impl.hpp"
#include "script/script_util.hpp"
#include "state.hpp"

#include <imgui.h>
#include <sol/sol.hpp>

namespace NGui
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    /// Converts a color to int to be used in drawing functions. Use values from `0..255`.
    lua["rgba"] = [](int r, int g, int b, int a) -> uColor
    { return (ImU32)(a << 24) + (b << 16) + (g << 8) + (r); };
    /// Draws a line on screen
    lua["draw_line"] = [script](float x1, float y1, float x2, float y2, float thickness, ImU32 color)
    {
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        script->draw_list->AddLine(a, b, color, thickness);
    };
    /// Draws a rectangle on screen from top-left to bottom-right.
    lua["draw_rect"] = [script](float x1, float y1, float x2, float y2, float thickness, float rounding, ImU32 color)
    {
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        // check for nan in the vectors because this will cause a crash in ImGui
        if (isnan(a.x) || isnan(a.y) || isnan(b.x) || isnan(b.y))
        {
            return;
        }
        script->draw_list->AddRect(a, b, color, rounding, ImDrawCornerFlags_All, thickness);
    };
    /// Draws a filled rectangle on screen from top-left to bottom-right.
    lua["draw_rect_filled"] = [script](float x1, float y1, float x2, float y2, float rounding, ImU32 color)
    {
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        script->draw_list->AddRectFilled(a, b, color, rounding, ImDrawCornerFlags_All);
    };
    /// Draws a circle on screen
    lua["draw_circle"] = [script](float x, float y, float radius, float thickness, ImU32 color)
    {
        ImVec2 a = screenify({x, y});
        float r = screenify(radius);
        script->draw_list->AddCircle(a, r, color, 0, thickness);
    };
    /// Draws a filled circle on screen
    lua["draw_circle_filled"] = [script](float x, float y, float radius, ImU32 color)
    {
        ImVec2 a = screenify({x, y});
        float r = screenify(radius);
        script->draw_list->AddCircleFilled(a, r, color, 0);
    };
    /// Draws text in screen coordinates `x`, `y`, anchored top-left. Text size 0 uses the default 18.
    lua["draw_text"] = [script](float x, float y, float size, std::string text, ImU32 color)
    {
        ImVec2 a = screenify({x, y});
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->Fonts.back();
        for (auto pickfont : io.Fonts->Fonts)
        {
            if (floor(size) <= floor(pickfont->FontSize))
            {
                font = pickfont;
                break;
            }
        }
        script->draw_list->AddText(font, size, a, color, text.c_str());
    };
    /// Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.
    /// Example:
    /// ```lua
    /// function on_guiframe()
    ///     -- get a random color
    ///     color = math.random(0, 0xffffffff)
    ///     -- zoom the font size based on frame
    ///     size = (get_frame() % 199)+1
    ///     text = 'Awesome!'
    ///     -- calculate size of text
    ///     w, h = draw_text_size(size, text)
    ///     -- draw to the center of screen
    ///     draw_text(0-w/2, 0-h/2, size, text, color)
    /// end
    /// ```
    lua["draw_text_size"] = [script](float size, std::string text) -> std::pair<float, float>
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->Fonts.back();
        for (auto pickfont : io.Fonts->Fonts)
        {
            if (floor(size) <= floor(pickfont->FontSize))
            {
                font = pickfont;
                break;
            }
        }
        ImVec2 textsize = font->CalcTextSizeA(size, 9999.0, 9999.0, text.c_str());
        auto a = normalize(ImVec2(0, 0));
        auto b = normalize(textsize);
        return std::make_pair(b.x - a.x, b.y - a.y);
    };
    /// Create image from file. Returns a tuple containing id, width and height.
    lua["create_image"] = [script](std::string path) -> std::tuple<int, int, int>
    {
        ScriptImage* image = new ScriptImage;
        image->width = 0;
        image->height = 0;
        image->texture = NULL;
        if (create_d3d11_texture_from_file((script->script_folder / path).string().data(), &image->texture, &image->width, &image->height))
        {
            int id = script->images.size();
            script->images[id] = image;
            return std::make_tuple(id, image->width, image->height);
        }
        return std::make_tuple(-1, -1, -1);
    };
    /// Draws an image on screen from top-left to bottom-right. Use UV coordinates `0, 0, 1, 1` to just draw the whole image.
    lua["draw_image"] = [script](int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, ImU32 color)
    {
        if (!script->images.contains(image))
            return;
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        ImVec2 uva = ImVec2(uvx1, uvy1);
        ImVec2 uvb = ImVec2(uvx2, uvy2);
        script->draw_list->AddImage(script->images[image]->texture, a, b, uva, uvb, color);
    };
    /// Same as `draw_image` but rotates the image by angle in radians around the pivot offset from the center of the rect (meaning `px=py=0` rotates around the center)
    lua["draw_image_rotated"] = [script](int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, ImU32 color, float angle, float px, float py)
    {
        if (!script->images.contains(image))
            return;
        ImVec2 a = screenify({x1, y1});
        ImVec2 b = screenify({x2, y2});
        ImVec2 uva = ImVec2(uvx1, uvy1);
        ImVec2 uvb = ImVec2(uvx2, uvy2);
        ImVec2 pivot = {screenify(px), screenify(py)};
        AddImageRotated(script->draw_list, script->images[image]->texture, a, b, uva, uvb, color, angle, pivot);
    };

    /// Create a new widget window. Put all win_ widgets inside the callback function. The window functions are just wrappers for the
    /// [ImGui](https://github.com/ocornut/imgui/) widgets, so read more about them there. Use screen position and distance, or `0, 0, 0, 0` to
    /// autosize in center. Use just a `##Label` as title to hide titlebar.
    /// **Important: Keep all your labels unique!** If you need inputs with the same label, add `##SomeUniqueLabel` after the text, or use pushid to
    /// give things unique ids. ImGui doesn't know what you clicked if all your buttons have the same text... The window api is probably evolving
    /// still, this is just the first draft. Felt cute, might delete later!
    /// Returns false if the window was closed from the X.
    lua["window"] = [script](std::string title, float x, float y, float w, float h, bool movable, sol::function callback)
    {
        bool win_open = true;
        ImGui::PushID("scriptwindow");
        ImGuiCond cond = (movable ? ImGuiCond_Appearing : ImGuiCond_Always);
        ImGuiCond flag = (movable ? 0 : ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        if (title == "" || title.find("##") == 0)
        {
            flag |= ImGuiWindowFlags_NoTitleBar;
        }
        if (x == 0.0f && y == 0.0f && w == 0.0f && h == 0.0f)
        {
            ImGui::SetNextWindowSize(ImVec2(400, -1), cond);
        }
        else
        {
            ImVec2 spos = screenify(ImVec2(x, y));
            ImVec2 ssa = screenify(ImVec2(w, h));
            ImVec2 ssb = screenify(ImVec2(0, 0));
            ImVec2 ssize = ImVec2(ssa.x - ssb.x, ssb.y - ssa.y);
            ImGui::SetNextWindowPos(spos, cond);
            ImGui::SetNextWindowSize(ssize, cond);
        }
        ImGui::Begin(title.c_str(), &win_open, flag);
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() / 2);
        script->handle_function(callback);
        ImGui::PopItemWidth();
        if (x == 0.0f && y == 0.0f && w == 0.0f && h == 0.0f)
        {
            ImGui::SetWindowPos(
                {ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, ImGui::GetIO().DisplaySize.y / 2 - ImGui::GetWindowHeight() / 2},
                cond);
        }
        ImGui::End();
        ImGui::PopID();

        if (win_open && script->windows.count(title) == 0)
        {
            script->windows.insert(std::move(title));
            show_cursor();
        }
        else if (!win_open && script->windows.count(title) != 0)
        {
            script->windows.erase(title);
            hide_cursor();
        }

        return win_open;
    };
    /// Add some text to window, automatically wrapped
    lua["win_text"] = [](std::string text)
    { ImGui::TextWrapped(text.c_str()); };
    /// Add a separator line to window
    lua["win_separator"] = []()
    { ImGui::Separator(); };
    /// Add next thing on the same line. This is same as `win_sameline(0, -1)`
    lua["win_inline"] = []()
    { ImGui::SameLine(); };
    /// Add next thing on the same line, with an offset
    lua["win_sameline"] = [](float offset, float spacing)
    { ImGui::SameLine(offset, spacing); };
    /// Add a button
    lua["win_button"] = [](std::string text) -> bool
    {
        if (ImGui::Button(text.c_str()))
        {
            return true;
        }
        return false;
    };
    /// Add a text field
    lua["win_input_text"] = [](std::string label, std::string value) -> std::string
    {
        InputString(label.c_str(), &value, 0, nullptr, nullptr);
        return value;
    };
    /// Add an integer field
    lua["win_input_int"] = [](std::string label, int value) -> int
    {
        ImGui::InputInt(label.c_str(), &value);
        return value;
    };
    /// Add a float field
    lua["win_input_float"] = [](std::string label, float value) -> float
    {
        ImGui::InputFloat(label.c_str(), &value);
        return value;
    };
    /// Add an integer slider
    lua["win_slider_int"] = [](std::string label, int value, int min, int max) -> int
    {
        ImGui::SliderInt(label.c_str(), &value, min, max);
        return value;
    };
    /// Add an integer dragfield
    lua["win_drag_int"] = [](std::string label, int value, int min, int max) -> int
    {
        ImGui::DragInt(label.c_str(), &value, 0.5f, min, max);
        return value;
    };
    /// Add an float slider
    lua["win_slider_float"] = [](std::string label, float value, float min, float max) -> float
    {
        ImGui::SliderFloat(label.c_str(), &value, min, max);
        return value;
    };
    /// Add an float dragfield
    lua["win_drag_float"] = [](std::string label, float value, float min, float max) -> float
    {
        ImGui::DragFloat(label.c_str(), &value, 0.5f, min, max);
        return value;
    };
    /// Add a checkbox
    lua["win_check"] = [](std::string label, bool value) -> bool
    {
        ImGui::Checkbox(label.c_str(), &value);
        return value;
    };
    /// Add a combo box
    lua["win_combo"] = [](std::string label, int selected, std::string opts) -> int
    {
        int reals = selected - 1;
        ImGui::Combo(label.c_str(), &reals, opts.c_str());
        return reals + 1;
    };
    /// Add unique identifier to the stack, to distinguish identical inputs from each other. Put before the input.
    lua["win_pushid"] = [](int id)
    { ImGui::PushID(id); };
    /// Pop unique identifier from the stack. Put after the input.
    lua["win_popid"] = []()
    { ImGui::PopID(); };
    /// Draw image to window.
    lua["win_image"] = [script](int image, int width, int height)
    {
        if (!script->images.contains(image))
            return;

        auto& image_ptr = script->images[image];
        if (width < 1)
            width = image_ptr->width;
        if (height < 1)
            height = image_ptr->height;
        ImGui::Image(image_ptr->texture, ImVec2(width, height));
    };
}
}; // namespace NGui
