#include "gui_lua.hpp"

#include "file_api.hpp"
#include "script/script_impl.hpp"
#include "script/script_util.hpp"
#include "state.hpp"

#include <imgui.h>
#include <sol/sol.hpp>

#include "logger.h"

const ImVec4 error_color{1.0f, 0.2f, 0.2f, 1.0f};

GuiDrawContext::GuiDrawContext(class ScriptImpl* _script, ImDrawList* _draw_list)
    : script(_script), draw_list{_draw_list}
{
}

void GuiDrawContext::draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color)
{
    ImVec2 a = screenify({x1, y1});
    ImVec2 b = screenify({x2, y2});
    script->draw_list->AddLine(a, b, color, thickness);
};
void GuiDrawContext::draw_rect(float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color)
{
    // check for nan in the vectors because this will cause a crash in ImGui
    if (isnan(x1) || isnan(y1) || isnan(x2) || isnan(y2))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        script->messages.push_back({fmt::format("An argument passed to draw_rect was not a number: {} {} {} {}", x1, y1, x2, y2), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    ImVec2 a = screenify({x1, y1});
    ImVec2 b = screenify({x2, y2});
    script->draw_list->AddRect(a, b, color, rounding, ImDrawCornerFlags_All, thickness);
};
void GuiDrawContext::draw_rect(AABB rect, float thickness, float rounding, uColor color)
{
    draw_rect(rect.left, rect.bottom, rect.right, rect.top, thickness, rounding, color);
}
void GuiDrawContext::draw_rect_filled(float x1, float y1, float x2, float y2, float rounding, uColor color)
{
    if (isnan(x1) || isnan(y1) || isnan(x2) || isnan(y2))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        script->messages.push_back({fmt::format("An argument passed to draw_rect_filled was not a number: {} {} {} {}", x1, y1, x2, y2), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    ImVec2 a = screenify({x1, y1});
    ImVec2 b = screenify({x2, y2});
    // check for nan in the vectors because this will cause a crash in ImGui
    script->draw_list->AddRectFilled(a, b, color, rounding, ImDrawCornerFlags_All);
};
void GuiDrawContext::draw_rect_filled(AABB rect, float rounding, uColor color)
{
    draw_rect_filled(rect.left, rect.bottom, rect.right, rect.top, rounding, color);
}
void GuiDrawContext::draw_circle(float x, float y, float radius, float thickness, uColor color)
{
    ImVec2 a = screenify({x, y});
    float r = screenify(radius);
    // check for nan in the vectors and radius because this will cause a crash in ImGui
    if (isnan(a.x) || isnan(a.y) || isnan(r))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        script->messages.push_back({fmt::format("An argument passed to draw_circle was not a number: {} {} {}", a.x, a.y, r), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    script->draw_list->AddCircle(a, r, color, 0, thickness);
};
void GuiDrawContext::draw_circle_filled(float x, float y, float radius, uColor color)
{
    ImVec2 a = screenify({x, y});
    float r = screenify(radius);
    script->draw_list->AddCircleFilled(a, r, color, 0);
};
void GuiDrawContext::draw_text(float x, float y, float size, std::string text, uColor color)
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
void GuiDrawContext::draw_image(int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)
{
    if (!script->images.contains(image))
        return;
    ImVec2 a = screenify({x1, y1});
    ImVec2 b = screenify({x2, y2});
    ImVec2 uva = ImVec2(uvx1, uvy1);
    ImVec2 uvb = ImVec2(uvx2, uvy2);
    script->draw_list->AddImage(script->images[image]->texture, a, b, uva, uvb, color);
};
void GuiDrawContext::draw_image(int image, AABB rect, AABB uv_rect, uColor color)
{
    draw_image(image, rect.left, rect.bottom, rect.right, rect.top, uv_rect.left, uv_rect.bottom, uv_rect.right, uv_rect.top, color);
}
void GuiDrawContext::draw_image_rotated(int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)
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
void GuiDrawContext::draw_image_rotated(int image, AABB rect, AABB uv_rect, uColor color, float angle, float px, float py)
{
    draw_image_rotated(image, rect.left, rect.bottom, rect.right, rect.top, uv_rect.left, uv_rect.bottom, uv_rect.right, uv_rect.top, color, angle, px, py);
}

bool GuiDrawContext::window(std::string title, float x, float y, float w, float h, bool movable, sol::function callback)
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
void GuiDrawContext::win_text(std::string text)
{
    ImGui::TextWrapped(text.c_str());
};
void GuiDrawContext::win_separator()
{
    ImGui::Separator();
};
void GuiDrawContext::win_inline()
{
    ImGui::SameLine();
};
void GuiDrawContext::win_sameline(float offset, float spacing)
{
    ImGui::SameLine(offset, spacing);
};
bool GuiDrawContext::win_button(std::string text)
{
    if (ImGui::Button(text.c_str()))
    {
        return true;
    }
    return false;
};
std::string GuiDrawContext::win_input_text(std::string label, std::string value)
{
    InputString(label.c_str(), &value, 0, nullptr, nullptr);
    return value;
};
int GuiDrawContext::win_input_int(std::string label, int value)
{
    ImGui::InputInt(label.c_str(), &value);
    return value;
};
float GuiDrawContext::win_input_float(std::string label, float value)
{
    ImGui::InputFloat(label.c_str(), &value);
    return value;
};
int GuiDrawContext::win_slider_int(std::string label, int value, int min, int max)
{
    ImGui::SliderInt(label.c_str(), &value, min, max);
    return value;
};
int GuiDrawContext::win_drag_int(std::string label, int value, int min, int max)
{
    ImGui::DragInt(label.c_str(), &value, 0.5f, min, max);
    return value;
};
float GuiDrawContext::win_slider_float(std::string label, float value, float min, float max)
{
    ImGui::SliderFloat(label.c_str(), &value, min, max);
    return value;
};
float GuiDrawContext::win_drag_float(std::string label, float value, float min, float max)
{
    ImGui::DragFloat(label.c_str(), &value, 0.5f, min, max);
    return value;
};
bool GuiDrawContext::win_check(std::string label, bool value)
{
    ImGui::Checkbox(label.c_str(), &value);
    return value;
};
int GuiDrawContext::win_combo(std::string label, int selected, std::string opts)
{
    int reals = selected - 1;
    ImGui::Combo(label.c_str(), &reals, opts.c_str());
    return reals + 1;
};
void GuiDrawContext::win_pushid(int id)
{
    ImGui::PushID(id);
};
void GuiDrawContext::win_popid()
{
    ImGui::PopID();
};
void GuiDrawContext::win_image(int image, int width, int height)
{
    if (!script->images.contains(image))
        return;

    auto& image_ptr = script->images[image];
    if (width < 1)
        width = image_ptr->width;
    if (height < 1)
        height = image_ptr->height;
    ImGui::Image(image_ptr->texture, ImVec2(static_cast<float>(width), static_cast<float>(height)));
};

namespace NGui
{
void register_usertypes(sol::state& lua, ScriptImpl* script)
{
    auto draw_rect = sol::overload(
        static_cast<void (GuiDrawContext::*)(float, float, float, float, float, float, uColor)>(&GuiDrawContext::draw_rect),
        static_cast<void (GuiDrawContext::*)(AABB, float, float, uColor)>(&GuiDrawContext::draw_rect));
    auto draw_rect_filled = sol::overload(
        static_cast<void (GuiDrawContext::*)(float, float, float, float, float, uColor)>(&GuiDrawContext::draw_rect_filled),
        static_cast<void (GuiDrawContext::*)(AABB, float, uColor)>(&GuiDrawContext::draw_rect_filled));
    auto draw_image = sol::overload(
        static_cast<void (GuiDrawContext::*)(int, float, float, float, float, float, float, float, float, uColor)>(&GuiDrawContext::draw_image),
        static_cast<void (GuiDrawContext::*)(int, AABB, AABB, uColor)>(&GuiDrawContext::draw_image));
    auto draw_image_rotated = sol::overload(
        static_cast<void (GuiDrawContext::*)(int, float, float, float, float, float, float, float, float, uColor, float, float, float)>(&GuiDrawContext::draw_image_rotated),
        static_cast<void (GuiDrawContext::*)(int, AABB, AABB, uColor, float, float, float)>(&GuiDrawContext::draw_image_rotated));
    lua.new_usertype<GuiDrawContext>(
        "GuiDrawContext",
        "draw_line",
        &GuiDrawContext::draw_line,
        "draw_rect",
        draw_rect,
        "draw_rect_filled",
        draw_rect_filled,
        "draw_circle",
        &GuiDrawContext::draw_circle,
        "draw_circle_filled",
        &GuiDrawContext::draw_circle_filled,
        "draw_text",
        &GuiDrawContext::draw_text,
        "draw_image",
        draw_image,
        "draw_image_rotated",
        draw_image_rotated,
        "window",
        &GuiDrawContext::window,
        "win_text",
        &GuiDrawContext::win_text,
        "win_separator",
        &GuiDrawContext::win_separator,
        "win_inline",
        &GuiDrawContext::win_inline,
        "win_sameline",
        &GuiDrawContext::win_sameline,
        "win_button",
        &GuiDrawContext::win_button,
        "win_input_text",
        &GuiDrawContext::win_input_text,
        "win_input_int",
        &GuiDrawContext::win_input_int,
        "win_input_float",
        &GuiDrawContext::win_input_float,
        "win_slider_int",
        &GuiDrawContext::win_slider_int,
        "win_drag_int",
        &GuiDrawContext::win_drag_int,
        "win_slider_float",
        &GuiDrawContext::win_slider_float,
        "win_drag_float",
        &GuiDrawContext::win_drag_float,
        "win_check",
        &GuiDrawContext::win_check,
        "win_combo",
        &GuiDrawContext::win_combo,
        "win_pushid",
        &GuiDrawContext::win_pushid,
        "win_popid",
        &GuiDrawContext::win_popid,
        "win_image",
        &GuiDrawContext::win_image);

    /// Converts a color to int to be used in drawing functions. Use values from `0..255`.
    lua["rgba"] = [script](int r, int g, int b, int a) -> uColor
    {
        return (uColor)(a << 24) + (b << 16) + (g << 8) + (r);
    };
    /// Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.
    /// Example:
    /// ```lua
    /// function on_guiframe(draw_ctx)
    ///     -- get a random color
    ///     color = math.random(0, 0xffffffff)
    ///     -- zoom the font size based on frame
    ///     size = (get_frame() % 199)+1
    ///     text = 'Awesome!'
    ///     -- calculate size of text
    ///     w, h = draw_text_size(size, text)
    ///     -- draw to the center of screen
    ///     draw_ctx:draw_text(0-w/2, 0-h/2, size, text, color)
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
        ImVec2 res = io.DisplaySize;

        auto a = normalize(ImVec2(0, 0));
        auto b = normalize(textsize);
        auto pair = std::make_pair(b.x - a.x, b.y - a.y);
        // nan isn't a valid text size so return 0x0 instead
        if (isnan(pair.first) || isnan(pair.second))
        {
            return std::make_pair(0.0f, 0.0f);
        }
        else
        {
            return pair;
        }
    };
    /// Create image from file. Returns a tuple containing id, width and height.
    lua["create_image"] = [script](std::string path) -> std::tuple<size_t, int, int>
    {
        ScriptImage* image = new ScriptImage;
        image->width = 0;
        image->height = 0;
        image->texture = NULL;
        if (create_d3d11_texture_from_file((script->script_folder / path).string().data(), &image->texture, &image->width, &image->height))
        {
            size_t id = script->images.size();
            script->images[id] = image;
            return std::make_tuple(id, image->width, image->height);
        }
        return std::make_tuple(-1, -1, -1);
    };

    /// Deprecated
    /// Use `DrawGuiContext.draw_line` instead
    lua["draw_line"] = [script](float x1, float y1, float x2, float y2, float thickness, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_line(x1, y1, x2, y2, thickness, color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_rect` instead
    lua["draw_rect"] = [script](float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_rect(x1, y1, x2, y2, thickness, rounding, color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_rect_filled` instead
    lua["draw_rect_filled"] = [script](float x1, float y1, float x2, float y2, float rounding, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_rect_filled(x1, y1, x2, y2, rounding, color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_circle` instead
    lua["draw_circle"] = [script](float x, float y, float radius, float thickness, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_circle(x, y, radius, thickness, color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_circle_filled` instead
    lua["draw_circle_filled"] = [script](float x, float y, float radius, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_circle_filled(x, y, radius, color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_text` instead
    lua["draw_text"] = [script](float x, float y, float size, std::string text, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_text(x, y, size, std::move(text), color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_image` instead
    lua["draw_image"] = [script](int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)
    {
        GuiDrawContext(script, script->draw_list).draw_image(image, x1, y1, x2, y2, uvx1, uvy1, uvx2, uvy2, color);
    };
    /// Deprecated
    /// Use `DrawGuiContext.draw_image_rotated` instead
    lua["draw_image_rotated"] = [script](int image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)
    {
        GuiDrawContext(script, script->draw_list).draw_image_rotated(image, x1, y1, x2, y2, uvx1, uvy1, uvx2, uvy2, color, angle, px, py);
    };

    /// Deprecated
    /// Use `DrawGuiContext.window` instead
    lua["window"] = [script](std::string title, float x, float y, float w, float h, bool movable, sol::function callback)
    {
        GuiDrawContext(script, script->draw_list).window(std::move(title), x, y, w, h, movable, std::move(callback));
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_text` instead
    lua["win_text"] = [script](std::string text)
    {
        GuiDrawContext(script, script->draw_list).win_text(std::move(text));
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_separator` instead
    lua["win_separator"] = [script]()
    {
        GuiDrawContext(script, script->draw_list).win_separator();
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_inline` instead
    lua["win_inline"] = [script]()
    {
        GuiDrawContext(script, script->draw_list).win_inline();
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_sameline` instead
    lua["win_sameline"] = [script](float offset, float spacing)
    {
        GuiDrawContext(script, script->draw_list).win_sameline(offset, spacing);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_button` instead
    lua["win_button"] = [script](std::string text) -> bool
    {
        return GuiDrawContext(script, script->draw_list).win_button(std::move(text));
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_input_text` instead
    lua["win_input_text"] = [script](std::string label, std::string value) -> std::string
    {
        return GuiDrawContext(script, script->draw_list).win_input_text(std::move(label), std::move(value));
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_input_int` instead
    lua["win_input_int"] = [script](std::string label, int value) -> int
    {
        return GuiDrawContext(script, script->draw_list).win_input_int(std::move(label), value);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_input_float` instead
    lua["win_input_float"] = [script](std::string label, float value) -> float
    {
        return GuiDrawContext(script, script->draw_list).win_input_float(std::move(label), value);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_slider_int` instead
    lua["win_slider_int"] = [script](std::string label, int value, int min, int max) -> int
    {
        return GuiDrawContext(script, script->draw_list).win_slider_int(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_drag_int` instead
    lua["win_drag_int"] = [script](std::string label, int value, int min, int max) -> int
    {
        return GuiDrawContext(script, script->draw_list).win_drag_int(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_slider_float` instead
    lua["win_slider_float"] = [script](std::string label, float value, float min, float max) -> float
    {
        return GuiDrawContext(script, script->draw_list).win_slider_float(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_drag_float` instead
    lua["win_drag_float"] = [script](std::string label, float value, float min, float max) -> float
    {
        return GuiDrawContext(script, script->draw_list).win_drag_float(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_check` instead
    lua["win_check"] = [script](std::string label, bool value) -> bool
    {
        return GuiDrawContext(script, script->draw_list).win_check(std::move(label), value);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_combo` instead
    lua["win_combo"] = [script](std::string label, int selected, std::string opts) -> int
    {
        return GuiDrawContext(script, script->draw_list).win_combo(std::move(label), selected, std::move(opts));
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_pushid` instead
    lua["win_pushid"] = [script](int id)
    {
        GuiDrawContext(script, script->draw_list).win_pushid(id);
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_popid` instead
    lua["win_popid"] = [script]()
    {
        GuiDrawContext(script, script->draw_list).win_popid();
    };
    /// Deprecated
    /// Use `DrawGuiContext.win_image` instead
    lua["win_image"] = [script](int image, int width, int height)
    {
        GuiDrawContext(script, script->draw_list).win_image(image, width, height);
    };
}
}; // namespace NGui
