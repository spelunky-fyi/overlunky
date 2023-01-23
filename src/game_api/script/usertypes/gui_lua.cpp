#include "gui_lua.hpp"

#include <Windows.h>        // for GetProcAddress, DWORD, LoadLibraryA
#include <algorithm>        // for max
#include <chrono>           // for system_clock
#include <cmath>            // for isnan, floor
#include <cstddef>          // for NULL
#include <deque>            // for deque
#include <exception>        // for exception
#include <filesystem>       // for operator/, path
#include <fmt/format.h>     // for check_format_string, format, vformat
#include <imgui.h>          // for ImVec2, ImGuiIO, ImDrawList, GetIO
#include <imgui_internal.h> // for GImGui
#include <map>              // for map
#include <new>              // for operator new
#include <sol/sol.hpp>      // for proxy_key_t, data_t, state, property
#include <tuple>            // for get, tuple, make_tuple
#include <type_traits>      // for move, declval, reference_wrapper, ref
#include <unordered_set>    // for unordered_set
#include <utility>          // for max, min, pair, get, make_pair
#include <xinput.h>         // for XINPUT_STATE, XINPUT_CAPABILITIES

#include "file_api.hpp"                   // for create_d3d11_texture_from_file
#include "math.hpp"                       // for Vec2
#include "script.hpp"                     // for ScriptMessage, ScriptImage
#include "script/handle_lua_function.hpp" // for handle_function
#include "script/lua_backend.hpp"         // for LuaBackend
#include "script/script_util.hpp"         // for screenify_fix, screenify, normalize
#include "window_api.hpp"                 // for hide_cursor, show_cursor

typedef DWORD(WINAPI* PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD(WINAPI* PFN_XInputGetState)(DWORD, XINPUT_STATE*);
static bool g_HasGamepad = false;
static bool g_WantUpdateHasGamepad = false;
static HMODULE g_XInputDLL = NULL;
static PFN_XInputGetCapabilities g_XInputGetCapabilities = NULL;
static PFN_XInputGetState g_XInputGetState = NULL;

Vec2::Vec2(const ImVec2& p)
    : x(p.x), y(p.y){};

struct Gamepad : XINPUT_GAMEPAD
{
    /*
    GAMEPAD wButtons; // just for the autodoc
    */
    bool enabled;
};

Gamepad get_gamepad(unsigned int index = 1)
{
    if (g_WantUpdateHasGamepad)
    {
        XINPUT_CAPABILITIES caps;
        g_HasGamepad = g_XInputGetCapabilities ? (g_XInputGetCapabilities(index - 1, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS) : false;
        // g_WantUpdateHasGamepad = false;
    }

    XINPUT_STATE xinput_state;
    if (g_HasGamepad && g_XInputGetState && g_XInputGetState(index - 1, &xinput_state) == ERROR_SUCCESS)
    {
        return {xinput_state.Gamepad, true};
    }
    return Gamepad{{0}};
}

[[maybe_unused]] const ImVec4 error_color{1.0f, 0.2f, 0.2f, 1.0f};

[[maybe_unused]] static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

[[maybe_unused]] static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

GuiDrawContext::GuiDrawContext(LuaBackend* _backend)
    : backend(_backend), g(*GImGui)
{
}

void GuiDrawContext::draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color)
{
    ImVec2 a = screenify_fix({x1, y1});
    ImVec2 b = screenify_fix({x2, y2});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddLine(a, b, color, thickness);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddLine(a, b, color, thickness);
};
void GuiDrawContext::draw_rect(float left, float top, float right, float bottom, float thickness, float rounding, uColor color)
{
    // check for nan in the vectors because this will cause a crash in ImGui
    if (isnan(left) || isnan(top) || isnan(right) || isnan(bottom))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        backend->messages.push_back({fmt::format("An argument passed to draw_rect was not a number: {} {} {} {}", left, top, right, bottom), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    ImVec2 a = screenify_fix({left, top});
    ImVec2 b = screenify_fix({right, bottom});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddRect(a, b, color, rounding, ImDrawCornerFlags_All, thickness);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddRect(a, b, color, rounding, ImDrawCornerFlags_All, thickness);
};
void GuiDrawContext::draw_rect(AABB rect, float thickness, float rounding, uColor color)
{
    draw_rect(rect.left, rect.top, rect.right, rect.bottom, thickness, rounding, color);
}
void GuiDrawContext::draw_rect_filled(float left, float top, float right, float bottom, float rounding, uColor color)
{
    // check for nan in the vectors because this will cause a crash in ImGui
    if (isnan(left) || isnan(top) || isnan(right) || isnan(bottom))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        backend->messages.push_back({fmt::format("An argument passed to draw_rect_filled was not a number: {} {} {} {}", left, top, right, bottom), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    ImVec2 a = screenify_fix({left, top});
    ImVec2 b = screenify_fix({right, bottom});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddRectFilled(a, b, color, rounding, ImDrawCornerFlags_All);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddRectFilled(a, b, color, rounding, ImDrawCornerFlags_All);
};
void GuiDrawContext::draw_rect_filled(AABB rect, float rounding, uColor color)
{
    draw_rect_filled(rect.left, rect.top, rect.right, rect.bottom, rounding, color);
}
void GuiDrawContext::draw_poly(std::vector<Vec2> points, float thickness, uColor color)
{
    auto draw = [&](ImDrawList* dl)
    {
        dl->PathClear();
        for (auto point : points)
        {
            ImVec2 a = screenify_fix({point.x, point.y});
            dl->PathLineToMergeDuplicate(a);
        }
        dl->PathStroke(color, 0, thickness);
    };
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            draw(ImGui::GetForegroundDrawList(vp));
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    draw(list);
}
void GuiDrawContext::draw_poly_filled(std::vector<Vec2> points, uColor color)
{
    auto draw = [&](ImDrawList* dl)
    {
        dl->PathClear();
        for (auto point : points)
        {
            ImVec2 a = screenify_fix({point.x, point.y});
            dl->PathLineToMergeDuplicate(a);
        }
        dl->PathFillConvex(color);
    };
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            draw(ImGui::GetForegroundDrawList(vp));
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    draw(list);
}
void GuiDrawContext::draw_bezier_cubic(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, float thickness, uColor color)
{
    ImVec2 a = screenify_fix({p1.x, p1.y});
    ImVec2 b = screenify_fix({p2.x, p2.y});
    ImVec2 c = screenify_fix({p3.x, p3.y});
    ImVec2 d = screenify_fix({p4.x, p4.y});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddBezierCubic(a, b, c, d, color, thickness);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddBezierCubic(a, b, c, d, color, thickness);
}
void GuiDrawContext::draw_bezier_quadratic(Vec2 p1, Vec2 p2, Vec2 p3, float thickness, uColor color)
{
    ImVec2 a = screenify_fix({p1.x, p1.y});
    ImVec2 b = screenify_fix({p2.x, p2.y});
    ImVec2 c = screenify_fix({p3.x, p3.y});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddBezierQuadratic(a, b, c, color, thickness);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddBezierQuadratic(a, b, c, color, thickness);
}
void GuiDrawContext::draw_triangle(Vec2 p1, Vec2 p2, Vec2 p3, float thickness, uColor color)
{
    ImVec2 a = screenify_fix({p1.x, p1.y});
    ImVec2 b = screenify_fix({p2.x, p2.y});
    ImVec2 c = screenify_fix({p3.x, p3.y});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddTriangle(a, b, c, color, thickness);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddTriangle(a, b, c, color, thickness);
}
void GuiDrawContext::draw_triangle_filled(Vec2 p1, Vec2 p2, Vec2 p3, uColor color)
{
    ImVec2 a = screenify_fix({p1.x, p1.y});
    ImVec2 b = screenify_fix({p2.x, p2.y});
    ImVec2 c = screenify_fix({p3.x, p3.y});
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddTriangleFilled(a, b, c, color);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddTriangleFilled(a, b, c, color);
}
void GuiDrawContext::draw_circle(float x, float y, float radius, float thickness, uColor color)
{
    ImVec2 a = screenify_fix({x, y});
    float r = screenify(radius);
    // check for nan in the vectors and radius because this will cause a crash in ImGui
    if (isnan(a.x) || isnan(a.y) || isnan(r))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        backend->messages.push_back({fmt::format("An argument passed to draw_circle was not a number: {} {} {}", a.x, a.y, r), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddCircle(a, r, color, 0, thickness);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddCircle(a, r, color, 0, thickness);
};
void GuiDrawContext::draw_circle_filled(float x, float y, float radius, uColor color)
{
    ImVec2 a = screenify_fix({x, y});
    float r = screenify(radius);
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddCircleFilled(a, r, color, 0);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddCircleFilled(a, r, color, 0);
};
void GuiDrawContext::draw_text(float x, float y, float size, std::string text, uColor color)
{
    ImVec2 a = screenify_fix({x, y});
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
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddText(font, size, a, color, text.c_str());
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddText(font, size, a, color, text.c_str());
};
void GuiDrawContext::draw_image(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)
{
    if (!backend->images.contains(image))
        return;
    ImVec2 a = screenify_fix({left, top});
    ImVec2 b = screenify_fix({right, bottom});
    ImVec2 uva = ImVec2(uvx1, uvy1);
    ImVec2 uvb = ImVec2(uvx2, uvy2);
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            ImGui::GetForegroundDrawList(vp)->AddImage(backend->images[image]->texture, a, b, uva, uvb, color);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    list->AddImage(backend->images[image]->texture, a, b, uva, uvb, color);
};
void GuiDrawContext::draw_image(IMAGE image, AABB rect, AABB uv_rect, uColor color)
{
    draw_image(image, rect.left, rect.top, rect.right, rect.bottom, uv_rect.left, uv_rect.top, uv_rect.right, uv_rect.bottom, color);
}
void GuiDrawContext::draw_image_rotated(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)
{
    if (!backend->images.contains(image))
        return;
    ImVec2 a = screenify_fix({left, top});
    ImVec2 b = screenify_fix({right, bottom});
    ImVec2 uva = ImVec2(uvx1, uvy1);
    ImVec2 uvb = ImVec2(uvx2, uvy2);
    ImVec2 pivot = {screenify(px), screenify(py)};
    if (drawlist == DRAW_LAYER::FOREGROUND)
    {
        for (auto vp : g.Viewports)
            AddImageRotated(ImGui::GetForegroundDrawList(vp), backend->images[image]->texture, a, b, uva, uvb, color, angle, pivot);
        return;
    }
    auto list = drawlist == DRAW_LAYER::WINDOW ? ImGui::GetWindowDrawList() : backend->draw_list;
    AddImageRotated(list, backend->images[image]->texture, a, b, uva, uvb, color, angle, pivot);
};
void GuiDrawContext::draw_image_rotated(IMAGE image, AABB rect, AABB uv_rect, uColor color, float angle, float px, float py)
{
    draw_image_rotated(image, rect.left, rect.bottom, rect.right, rect.top, uv_rect.left, uv_rect.bottom, uv_rect.right, uv_rect.top, color, angle, px, py);
}

bool GuiDrawContext::window(std::string title, float x, float y, float w, float h, bool movable, sol::function callback)
{
    bool win_open = true;
    ImGui::PushID("backendwindow");
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
        ImVec2 spos = screenify_fix(ImVec2(x, y));
        ImVec2 ssa = screenify_fix(ImVec2(w, h));
        ImVec2 ssb = screenify_fix(ImVec2(0, 0));
        ImVec2 ssize = ImVec2(ssa.x - ssb.x, ssb.y - ssa.y);
        ImGui::SetNextWindowPos(spos, cond);
        ImGui::SetNextWindowSize(ssize, cond);
    }
    flag |= ImGuiWindowFlags_NoDocking;
    ImGui::Begin(title.c_str(), &win_open, flag);
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() / 2);
    auto size = normalize(ImGui::GetWindowSize());
    size.x += 1.0f;
    size.y -= 1.0f;
    size.y *= -1.0f;
    handle_function<void>(backend, callback, this, Vec2(normalize(ImGui::GetWindowPos() - ImGui::GetMainViewport()->Pos)), Vec2(size));
    ImGui::PopItemWidth();
    if (x == 0.0f && y == 0.0f && w == 0.0f && h == 0.0f)
    {
        ImGui::SetWindowPos(
            {ImGui::GetIO().DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2, ImGui::GetIO().DisplaySize.y / 2 - ImGui::GetWindowHeight() / 2},
            cond);
    }
    ImGui::End();
    ImGui::PopID();

    if (win_open && backend->windows.count(title) == 0)
    {
        backend->windows.insert(std::move(title));
        show_cursor();
    }
    else if (!win_open && backend->windows.count(title) != 0)
    {
        backend->windows.erase(title);
        hide_cursor();
    }

    return win_open;
};
void GuiDrawContext::win_text(std::string text)
{
    ImGui::TextWrapped("%s", text.c_str());
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
void GuiDrawContext::win_image(IMAGE image, int width, int height)
{
    if (!backend->images.contains(image))
        return;

    auto& image_ptr = backend->images[image];
    if (width < 1)
        width = image_ptr->width;
    if (height < 1)
        height = image_ptr->height;
    ImGui::Image(image_ptr->texture, ImVec2(static_cast<float>(width), static_cast<float>(height)));
};
bool GuiDrawContext::win_imagebutton(std::string label, IMAGE image, float width, float height, float uvx1, float uvy1, float uvx2, float uvy2)
{
    if (!backend->images.contains(image))
        return false;

    auto& image_ptr = backend->images[image];

    ImVec2 im_size = ImVec2(width, height);

    if (im_size.x < 1)
        im_size.x = static_cast<float>(image_ptr->width);
    if (im_size.y < 1)
        im_size.y = static_cast<float>(image_ptr->height);

    return ImGui::ImageButton(label.c_str(), image_ptr->texture, im_size, ImVec2(uvx1, uvy1), ImVec2(uvx2, uvy2));
};
void GuiDrawContext::win_section(std::string title, sol::function callback)
{
    if (ImGui::CollapsingHeader(title.c_str()))
        handle_function<void>(backend, callback);
};
void GuiDrawContext::win_indent(float width)
{
    if (width > 0)
        ImGui::Indent(width);
    else if (width < 0)
        ImGui::Unindent(-width);
}
void GuiDrawContext::draw_layer(DRAW_LAYER layer)
{
    drawlist = layer;
}

namespace NGui
{
void register_usertypes(sol::state& lua)
{
    const char* xinput_dll_names[] =
        {
            "xinput1_4.dll",   // Windows 8+
            "xinput1_3.dll",   // DirectX SDK
            "xinput9_1_0.dll", // Windows Vista, Windows 7
            "xinput1_2.dll",   // DirectX SDK
            "xinput1_1.dll"    // DirectX SDK
        };
    for (int n = 0; n < IM_ARRAYSIZE(xinput_dll_names); n++)
        if (HMODULE dll = ::LoadLibraryA(xinput_dll_names[n]))
        {
            g_XInputDLL = dll;
            g_XInputGetCapabilities = (PFN_XInputGetCapabilities)::GetProcAddress(dll, "XInputGetCapabilities");
            g_XInputGetState = (PFN_XInputGetState)::GetProcAddress(dll, "XInputGetState");
            break;
        }

    auto draw_rect = sol::overload(
        static_cast<void (GuiDrawContext::*)(float, float, float, float, float, float, uColor)>(&GuiDrawContext::draw_rect),
        static_cast<void (GuiDrawContext::*)(AABB, float, float, uColor)>(&GuiDrawContext::draw_rect));
    auto draw_rect_filled = sol::overload(
        static_cast<void (GuiDrawContext::*)(float, float, float, float, float, uColor)>(&GuiDrawContext::draw_rect_filled),
        static_cast<void (GuiDrawContext::*)(AABB, float, uColor)>(&GuiDrawContext::draw_rect_filled));
    auto draw_image = sol::overload(
        static_cast<void (GuiDrawContext::*)(IMAGE, float, float, float, float, float, float, float, float, uColor)>(&GuiDrawContext::draw_image),
        static_cast<void (GuiDrawContext::*)(IMAGE, AABB, AABB, uColor)>(&GuiDrawContext::draw_image));
    auto draw_image_rotated = sol::overload(
        static_cast<void (GuiDrawContext::*)(IMAGE, float, float, float, float, float, float, float, float, uColor, float, float, float)>(&GuiDrawContext::draw_image_rotated),
        static_cast<void (GuiDrawContext::*)(IMAGE, AABB, AABB, uColor, float, float, float)>(&GuiDrawContext::draw_image_rotated));

    /// Used in [register_option_callback](#register_option_callback) and [set_callback](#set_callback) with ON.GUIFRAME
    auto guidrawcontext_type = lua.new_usertype<GuiDrawContext>("GuiDrawContext");
    guidrawcontext_type["draw_line"] = &GuiDrawContext::draw_line;
    guidrawcontext_type["draw_rect"] = draw_rect;
    guidrawcontext_type["draw_rect_filled"] = draw_rect_filled;
    guidrawcontext_type["draw_triangle"] = &GuiDrawContext::draw_triangle;
    guidrawcontext_type["draw_triangle_filled"] = &GuiDrawContext::draw_triangle_filled;
    guidrawcontext_type["draw_poly"] = &GuiDrawContext::draw_poly;
    guidrawcontext_type["draw_poly_filled"] = &GuiDrawContext::draw_poly_filled;
    guidrawcontext_type["draw_bezier_cubic"] = &GuiDrawContext::draw_bezier_cubic;
    guidrawcontext_type["draw_bezier_quadratic"] = &GuiDrawContext::draw_bezier_quadratic;
    guidrawcontext_type["draw_circle"] = &GuiDrawContext::draw_circle;
    guidrawcontext_type["draw_circle_filled"] = &GuiDrawContext::draw_circle_filled;
    guidrawcontext_type["draw_text"] = &GuiDrawContext::draw_text;
    guidrawcontext_type["draw_image"] = draw_image;
    guidrawcontext_type["draw_image_rotated"] = draw_image_rotated;
    guidrawcontext_type["draw_layer"] = &GuiDrawContext::draw_layer;
    guidrawcontext_type["window"] = &GuiDrawContext::window;
    guidrawcontext_type["win_text"] = &GuiDrawContext::win_text;
    guidrawcontext_type["win_separator"] = &GuiDrawContext::win_separator;
    guidrawcontext_type["win_inline"] = &GuiDrawContext::win_inline;
    guidrawcontext_type["win_sameline"] = &GuiDrawContext::win_sameline;
    guidrawcontext_type["win_button"] = &GuiDrawContext::win_button;
    guidrawcontext_type["win_input_text"] = &GuiDrawContext::win_input_text;
    guidrawcontext_type["win_input_int"] = &GuiDrawContext::win_input_int;
    guidrawcontext_type["win_input_float"] = &GuiDrawContext::win_input_float;
    guidrawcontext_type["win_slider_int"] = &GuiDrawContext::win_slider_int;
    guidrawcontext_type["win_drag_int"] = &GuiDrawContext::win_drag_int;
    guidrawcontext_type["win_slider_float"] = &GuiDrawContext::win_slider_float;
    guidrawcontext_type["win_drag_float"] = &GuiDrawContext::win_drag_float;
    guidrawcontext_type["win_check"] = &GuiDrawContext::win_check;
    guidrawcontext_type["win_combo"] = &GuiDrawContext::win_combo;
    guidrawcontext_type["win_pushid"] = &GuiDrawContext::win_pushid;
    guidrawcontext_type["win_popid"] = &GuiDrawContext::win_popid;
    guidrawcontext_type["win_image"] = &GuiDrawContext::win_image;
    guidrawcontext_type["win_imagebutton"] = &GuiDrawContext::win_imagebutton;
    guidrawcontext_type["win_section"] = &GuiDrawContext::win_section;
    guidrawcontext_type["win_indent"] = &GuiDrawContext::win_indent;

    /// Converts a color to int to be used in drawing functions. Use values from `0..255`.
    lua["rgba"] = [](int r, int g, int b, int a) -> uColor
    {
        return (uColor)(a << 24) + (b << 16) + (g << 8) + (r);
    };
    /// Calculate the bounding box of text, so you can center it etc. Returns `width`, `height` in screen distance.
    lua["draw_text_size"] = [](float size, std::string text) -> std::pair<float, float>
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
        auto pair = std::make_pair(b.x - a.x, b.y - a.y);
        // nan isn't a valid text size so return 0x0 instead
        if (isnan(pair.first) || isnan(pair.second))
        {
            return {};
        }
        else
        {
            return pair;
        }
    };
    /// Create image from file. Returns a tuple containing id, width and height.
    /// Depending on the image size, this can take a moment, preferably don't create them dynamically, rather create all you need in global scope so it will load them as soon as the game starts
    lua["create_image"] = [](std::string path) -> std::tuple<IMAGE, int, int>
    {
        ScriptImage* image = new ScriptImage;
        image->width = 0;
        image->height = 0;
        image->texture = NULL;

        auto backend = LuaBackend::get_calling_backend();
        std::string real_path;
        if (path.starts_with("/"))
            real_path = (std::filesystem::absolute(".") / path.substr(1)).string();
        else
            real_path = (std::filesystem::path(backend->get_root_path()) / path).string();

        if (create_d3d11_texture_from_file(real_path.c_str(), &image->texture, &image->width, &image->height))
        {
            IMAGE id = static_cast<IMAGE>(backend->images.size());
            backend->images[id] = image;
            return std::make_tuple(id, image->width, image->height);
        }
        return std::make_tuple((IMAGE)-1, -1, -1);
    };

    /// Create image from file, cropped to the geometry provided. Returns a tuple containing id, width and height.
    /// Depending on the image size, this can take a moment, preferably don't create them dynamically, rather create all you need in global scope so it will load them as soon as the game starts
    lua["create_image_crop"] = [](std::string path, int x, int y, int w, int h) -> std::tuple<IMAGE, int, int>
    {
        ScriptImage* image = new ScriptImage;
        image->width = 0;
        image->height = 0;
        image->texture = NULL;

        auto backend = LuaBackend::get_calling_backend();
        std::string real_path;
        if (path.starts_with("/"))
            real_path = (std::filesystem::absolute(".") / path.substr(1)).string();
        else
            real_path = (std::filesystem::path(backend->get_root_path()) / path).string();

        if (create_d3d11_texture_from_file(real_path.c_str(), &image->texture, &image->width, &image->height, x, y, w, h))
        {
            IMAGE id = static_cast<IMAGE>(backend->images.size());
            backend->images[id] = image;
            return std::make_tuple(id, image->width, image->height);
        }
        return std::make_tuple((IMAGE)-1, -1, -1);
    };

    /// Get image size from file. Returns a tuple containing width and height.
    lua["get_image_size"] = [](std::string path) -> std::tuple<int, int>
    {
        int width = 0;
        int height = 0;

        auto backend = LuaBackend::get_calling_backend();
        std::string real_path;
        if (path.starts_with("/"))
            real_path = (std::filesystem::absolute(".") / path.substr(1)).string();
        else
            real_path = (std::filesystem::path(backend->get_root_path()) / path).string();

        if (get_image_size_from_file(real_path.c_str(), &width, &height))
            return std::make_tuple(width, height);
        return std::make_tuple(-1, -1);
    };

    /// Current mouse cursor position in screen coordinates.
    lua["mouse_position"] = []() -> std::pair<float, float>
    {
        auto base = ImGui::GetMainViewport();
        auto pos = normalize(ImGui::GetMousePos() - base->Pos);
        return std::make_pair(pos.x, pos.y);
    };

    /// Used in ImGuiIO
    lua.new_usertype<Gamepad>(
        "Gamepad",
        "enabled",
        &Gamepad::enabled,
        "buttons",
        &Gamepad::wButtons,
        "lt",
        sol::property([](Gamepad& p) -> float
                      { return (float)p.bLeftTrigger / 255.f; }),
        "rt",
        sol::property([](Gamepad& p) -> float
                      { return (float)p.bRightTrigger / 255.f; }),
        "lx",
        sol::property([](Gamepad& p) -> float
                      { return (float)p.sThumbLX / 32768.f; }),
        "ly",
        sol::property([](Gamepad& p) -> float
                      { return (float)p.sThumbLY / 32768.f; }),
        "rx",
        sol::property([](Gamepad& p) -> float
                      { return (float)p.sThumbRX / 32768.f; }),
        "ry",
        sol::property([](Gamepad& p) -> float
                      { return (float)p.sThumbRY / 32768.f; }));

    auto keydown = sol::overload(
        [](int keycode)
        {
            return ImGui::IsKeyDown((ImGuiKey)keycode);
        },
        [](char key)
        {
            return ImGui::IsKeyDown((ImGuiKey)(int)key);
        });
    auto keypressed = sol::overload(
        [](int keycode)
        {
            return ImGui::IsKeyPressed((ImGuiKey)keycode, false);
        },
        [](int keycode, bool repeat)
        {
            return ImGui::IsKeyPressed((ImGuiKey)keycode, repeat);
        },
        [](char key)
        {
            return ImGui::IsKeyPressed((ImGuiKey)(int)key, false);
        },
        [](char key, bool repeat)
        {
            return ImGui::IsKeyPressed((ImGuiKey)(int)key, repeat);
        });
    auto keyreleased = sol::overload(
        [](int keycode)
        {
            return ImGui::IsKeyReleased((ImGuiKey)keycode);
        },
        [](char key)
        {
            return ImGui::IsKeyReleased((ImGuiKey)(int)key);
        });
    /// Used in [get_io](#get_io)
    lua.new_usertype<ImGuiIO>(
        "ImGuiIO",
        "displaysize",
        sol::property([](ImGuiIO& io)
                      { return Vec2(io.DisplaySize); }),
        "framerate",
        &ImGuiIO::Framerate,
        "wantkeyboard",
        &ImGuiIO::WantCaptureKeyboard,
        "keysdown",
        sol::property([](ImGuiIO& io)
                      { return std::ref(io.KeysDown) /**/; }),
        "keydown",
        keydown,
        "keypressed",
        keypressed,
        "keyreleased",
        keyreleased,
        "keyctrl",
        &ImGuiIO::KeyCtrl,
        "keyshift",
        &ImGuiIO::KeyShift,
        "keyalt",
        &ImGuiIO::KeyAlt,
        "keysuper",
        &ImGuiIO::KeySuper,
        "wantmouse",
        &ImGuiIO::WantCaptureMouse,
        "mousepos",
        sol::property([](ImGuiIO& io)
                      { return Vec2(io.MousePos); }),
        "mousedown",
        sol::property([](ImGuiIO& io)
                      { return std::ref(io.MouseDown) /**/; }),
        "mouseclicked",
        sol::property([](ImGuiIO& io)
                      { return std::ref(io.MouseClicked) /**/; }),
        "mousedoubleclicked",
        sol::property([](ImGuiIO& io)
                      { return std::ref(io.MouseDoubleClicked) /**/; }),
        "mousewheel",
        &ImGuiIO::MouseWheel,
        "gamepad",
        sol::property([]() -> Gamepad
                      {
        g_WantUpdateHasGamepad = true;
        return get_gamepad(1) /**/; }),
        "gamepads",
        [](unsigned int index)
        {
            g_WantUpdateHasGamepad = true;
            return get_gamepad(index) /**/;
        },
        "showcursor",
        &ImGuiIO::MouseDrawCursor);

    /* ImGuiIO
    // keydown
    // bool keydown(int keycode)
    // bool keydown(char key)
    // keypressed
    // bool keypressed(int keycode, bool repeat = false)
    // bool keypressed(char key, bool repeat = false)
    // keyreleased
    // bool keyreleased(int keycode)
    // bool keyreleased(char key)
    // gamepads
    // Gamepad gamepads(int index)
    // This is the XInput index 1..4, might not be the same as the player slot.
    */

    lua.create_named_table("GAMEPAD", "UP", 0x0001, "DOWN", 0x0002, "LEFT", 0x0004, "RIGHT", 0x0008, "START", 0x0010, "BACK", 0x0020, "LEFT_THUMB", 0x0040, "RIGHT_THUMB", 0x0080, "LEFT_SHOULDER", 0x0100, "RIGHT_SHOULDER", 0x0200, "A", 0x1000, "B", 0x2000, "X", 0x4000, "Y", 0x8000);

    lua.create_named_table("GAMEPAD_FLAG", "UP", 1, "DOWN", 2, "LEFT", 3, "RIGHT", 4, "START", 5, "BACK", 6, "LEFT_THUMB", 7, "RIGHT_THUMB", 8, "LEFT_SHOULDER", 9, "RIGHT_SHOULDER", 10, "A", 13, "B", 14, "X", 15, "Y", 16);

    lua.create_named_table("INPUT_FLAG", "JUMP", 1, "WHIP", 2, "BOMB", 3, "ROPE", 4, "RUN", 5, "DOOR", 6, "MENU", 7, "JOURNAL", 8, "LEFT", 9, "RIGHT", 10, "UP", 11, "DOWN", 12);

    /// Returns: [ImGuiIO](#ImGuiIO) for raw keyboard, mouse and xinput gamepad stuff.
    ///
    /// - Note: The clicked/pressed actions only make sense in `ON.GUIFRAME`.
    /// - Note: Lua starts indexing at 1, you need `keysdown[string.byte('A') + 1]` to find the A key.
    /// - Note: Overlunky/etc will eat all keys it is currently configured to use, your script will only get leftovers.
    /// - Note: Gamepad is basically [XINPUT_GAMEPAD](https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad) but variables are renamed and values are normalized to -1.0..1.0 range.
    // lua["get_io"] = []() -> ImGuiIO
    lua["get_io"] = ImGui::GetIO;

    lua.create_named_table("DRAW_LAYER", "BACKGROUND", DRAW_LAYER::BACKGROUND, "FOREGROUND", DRAW_LAYER::FOREGROUND, "WINDOW", DRAW_LAYER::WINDOW);

    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_line` instead
    lua["draw_line"] = [](float x1, float y1, float x2, float y2, float thickness, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_line(x1, y1, x2, y2, thickness, color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_rect` instead
    lua["draw_rect"] = [](float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_rect(x1, y1, x2, y2, thickness, rounding, color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_rect_filled` instead
    lua["draw_rect_filled"] = [](float x1, float y1, float x2, float y2, float rounding, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_rect_filled(x1, y1, x2, y2, rounding, color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_circle` instead
    lua["draw_circle"] = [](float x, float y, float radius, float thickness, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_circle(x, y, radius, thickness, color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_circle_filled` instead
    lua["draw_circle_filled"] = [](float x, float y, float radius, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_circle_filled(x, y, radius, color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_text` instead
    lua["draw_text"] = [](float x, float y, float size, std::string text, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_text(x, y, size, std::move(text), color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_image` instead
    lua["draw_image"] = [](IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_image(image, x1, y1, x2, y2, uvx1, uvy1, uvx2, uvy2, color);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.draw_image_rotated` instead
    lua["draw_image_rotated"] = [](IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).draw_image_rotated(image, x1, y1, x2, y2, uvx1, uvy1, uvx2, uvy2, color, angle, px, py);
    };

    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.window` instead
    lua["window"] = [](std::string title, float x, float y, float w, float h, bool movable, sol::function callback)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).window(std::move(title), x, y, w, h, movable, std::move(callback));
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_text` instead
    lua["win_text"] = [](std::string text)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_text(std::move(text));
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_separator` instead
    lua["win_separator"] = []()
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_separator();
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_inline` instead
    lua["win_inline"] = []()
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_inline();
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_sameline` instead
    lua["win_sameline"] = [](float offset, float spacing)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_sameline(offset, spacing);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_button` instead
    lua["win_button"] = [](std::string text) -> bool
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_button(std::move(text));
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_input_text` instead
    lua["win_input_text"] = [](std::string label, std::string value) -> std::string
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_input_text(std::move(label), std::move(value));
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_input_int` instead
    lua["win_input_int"] = [](std::string label, int value) -> int
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_input_int(std::move(label), value);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_input_float` instead
    lua["win_input_float"] = [](std::string label, float value) -> float
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_input_float(std::move(label), value);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_slider_int` instead
    lua["win_slider_int"] = [](std::string label, int value, int min, int max) -> int
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_slider_int(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_drag_int` instead
    lua["win_drag_int"] = [](std::string label, int value, int min, int max) -> int
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_drag_int(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_slider_float` instead
    lua["win_slider_float"] = [](std::string label, float value, float min, float max) -> float
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_slider_float(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_drag_float` instead
    lua["win_drag_float"] = [](std::string label, float value, float min, float max) -> float
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_drag_float(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_check` instead
    lua["win_check"] = [](std::string label, bool value) -> bool
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_check(std::move(label), value);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_combo` instead
    lua["win_combo"] = [](std::string label, int selected, std::string opts) -> int
    {
        auto backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend.get()).win_combo(std::move(label), selected, std::move(opts));
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_pushid` instead
    lua["win_pushid"] = [](int id)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_pushid(id);
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_popid` instead
    lua["win_popid"] = []()
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_popid();
    };
    /// Deprecated
    /// Use [GuiDrawContext](#GuiDrawContext)`.win_image` instead
    lua["win_image"] = [](IMAGE image, int width, int height)
    {
        auto backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend.get()).win_image(image, width, height);
    };
}
}; // namespace NGui
