#include "gui_lua.hpp"

#include "file_api.hpp"
#include "script/lua_backend.hpp"
#include "script/script_util.hpp"
#include "state.hpp"

#include <imgui.h>
#include <sol/sol.hpp>

#include "logger.h"

#include <xinput.h>
typedef DWORD(WINAPI* PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD(WINAPI* PFN_XInputGetState)(DWORD, XINPUT_STATE*);
static bool g_HasGamepad = false;
static bool g_WantUpdateHasGamepad = false;
static HMODULE g_XInputDLL = NULL;
static PFN_XInputGetCapabilities g_XInputGetCapabilities = NULL;
static PFN_XInputGetState g_XInputGetState = NULL;

struct Gamepad : XINPUT_GAMEPAD
{
    bool enabled;
};

Gamepad get_gamepad()
{
    if (g_WantUpdateHasGamepad)
    {
        XINPUT_CAPABILITIES caps;
        g_HasGamepad = g_XInputGetCapabilities ? (g_XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS) : false;
        // g_WantUpdateHasGamepad = false;
    }

    XINPUT_STATE xinput_state;
    if (g_HasGamepad && g_XInputGetState && g_XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
    {
        return {xinput_state.Gamepad, true};
    }
    return Gamepad{{0}};
}

const ImVec4 error_color{1.0f, 0.2f, 0.2f, 1.0f};

GuiDrawContext::GuiDrawContext(LuaBackend* _backend, ImDrawList* _draw_list)
    : backend(_backend), draw_list{_draw_list}
{
}

void GuiDrawContext::draw_line(float x1, float y1, float x2, float y2, float thickness, uColor color)
{
    ImVec2 a = screenify({x1, y1});
    ImVec2 b = screenify({x2, y2});
    backend->draw_list->AddLine(a, b, color, thickness);
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
    ImVec2 a = screenify({left, top});
    ImVec2 b = screenify({right, bottom});
    backend->draw_list->AddRect(a, b, color, rounding, ImDrawCornerFlags_All, thickness);
};
void GuiDrawContext::draw_rect(AABB rect, float thickness, float rounding, uColor color)
{
    draw_rect(rect.left, rect.top, rect.right, rect.bottom, thickness, rounding, color);
}
void GuiDrawContext::draw_rect_filled(float left, float top, float right, float bottom, float rounding, uColor color)
{
    if (isnan(left) || isnan(top) || isnan(right) || isnan(bottom))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        backend->messages.push_back({fmt::format("An argument passed to draw_rect_filled was not a number: {} {} {} {}", left, top, right, bottom), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    ImVec2 a = screenify({left, top});
    ImVec2 b = screenify({right, bottom});
    // check for nan in the vectors because this will cause a crash in ImGui
    backend->draw_list->AddRectFilled(a, b, color, rounding, ImDrawCornerFlags_All);
};
void GuiDrawContext::draw_rect_filled(AABB rect, float rounding, uColor color)
{
    draw_rect_filled(rect.left, rect.top, rect.right, rect.bottom, rounding, color);
}
void GuiDrawContext::draw_circle(float x, float y, float radius, float thickness, uColor color)
{
    ImVec2 a = screenify({x, y});
    float r = screenify(radius);
    // check for nan in the vectors and radius because this will cause a crash in ImGui
    if (isnan(a.x) || isnan(a.y) || isnan(r))
    {
#ifdef SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS
        backend->messages.push_back({fmt::format("An argument passed to draw_circle was not a number: {} {} {}", a.x, a.y, r), std::chrono::system_clock::now(), error_color});
#endif
        return;
    }
    backend->draw_list->AddCircle(a, r, color, 0, thickness);
};
void GuiDrawContext::draw_circle_filled(float x, float y, float radius, uColor color)
{
    ImVec2 a = screenify({x, y});
    float r = screenify(radius);
    backend->draw_list->AddCircleFilled(a, r, color, 0);
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
    backend->draw_list->AddText(font, size, a, color, text.c_str());
};
void GuiDrawContext::draw_image(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)
{
    if (!backend->images.contains(image))
        return;
    ImVec2 a = screenify({left, top});
    ImVec2 b = screenify({right, bottom});
    ImVec2 uva = ImVec2(uvx1, uvy1);
    ImVec2 uvb = ImVec2(uvx2, uvy2);
    backend->draw_list->AddImage(backend->images[image]->texture, a, b, uva, uvb, color);
};
void GuiDrawContext::draw_image(IMAGE image, AABB rect, AABB uv_rect, uColor color)
{
    draw_image(image, rect.left, rect.top, rect.right, rect.bottom, uv_rect.left, uv_rect.top, uv_rect.right, uv_rect.bottom, color);
}
void GuiDrawContext::draw_image_rotated(IMAGE image, float left, float top, float right, float bottom, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)
{
    if (!backend->images.contains(image))
        return;
    ImVec2 a = screenify({left, top});
    ImVec2 b = screenify({right, bottom});
    ImVec2 uva = ImVec2(uvx1, uvy1);
    ImVec2 uvb = ImVec2(uvx2, uvy2);
    ImVec2 pivot = {screenify(px), screenify(py)};
    AddImageRotated(backend->draw_list, backend->images[image]->texture, a, b, uva, uvb, color, angle, pivot);
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
        ImVec2 spos = screenify(ImVec2(x, y));
        ImVec2 ssa = screenify(ImVec2(w, h));
        ImVec2 ssb = screenify(ImVec2(0, 0));
        ImVec2 ssize = ImVec2(ssa.x - ssb.x, ssb.y - ssa.y);
        ImGui::SetNextWindowPos(spos, cond);
        ImGui::SetNextWindowSize(ssize, cond);
    }
    flag |= ImGuiWindowFlags_NoDocking;
    ImGui::Begin(title.c_str(), &win_open, flag);
    ImGui::PushItemWidth(-ImGui::GetWindowWidth() / 2);
    backend->handle_function(callback);
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
    lua["create_image"] = [](std::string path) -> std::tuple<IMAGE, int, int>
    {
        ScriptImage* image = new ScriptImage;
        image->width = 0;
        image->height = 0;
        image->texture = NULL;

        LuaBackend* backend = LuaBackend::get_calling_backend();
        if (create_d3d11_texture_from_file((std::filesystem::path{backend->get_root_path()} / path).string().data(), &image->texture, &image->width, &image->height))
        {
            IMAGE id = static_cast<IMAGE>(backend->images.size());
            backend->images[id] = image;
            return std::make_tuple(id, image->width, image->height);
        }
        return std::make_tuple((IMAGE)-1, -1, -1);
    };
    /// Current mouse cursor position in screen coordinates.
    lua["mouse_position"] = []() -> std::pair<float, float>
    {
        auto pos = normalize(ImGui::GetMousePos());
        return std::make_pair(pos.x, pos.y);
    };
    lua.new_usertype<ImVec2>(
        "ImVec2",
        "x",
        &ImVec2::x,
        "y",
        &ImVec2::y);

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
            return ImGui::IsKeyDown(keycode);
        },
        [](char key)
        {
            return ImGui::IsKeyDown((int)key);
        });
    auto keypressed = sol::overload(
        [](int keycode)
        {
            return ImGui::IsKeyPressed(keycode, false);
        },
        [](int keycode, bool repeat)
        {
            return ImGui::IsKeyPressed(keycode, repeat);
        },
        [](char key)
        {
            return ImGui::IsKeyPressed((int)key, false);
        },
        [](char key, bool repeat)
        {
            return ImGui::IsKeyPressed((int)key, repeat);
        });
    auto keyreleased = sol::overload(
        [](int keycode)
        {
            return ImGui::IsKeyReleased(keycode);
        },
        [](char key)
        {
            return ImGui::IsKeyReleased((int)key);
        });
    lua.new_usertype<ImGuiIO>(
        "ImGuiIO",
        "displaysize",
        &ImGuiIO::DisplaySize,
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
        &ImGuiIO::MousePos,
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
        sol::property([]()
                      {
                          g_WantUpdateHasGamepad = true;
                          return get_gamepad() /**/; }));

    /// Returns: [ImGuiIO](#ImGuiIO) for raw keyboard, mouse and xinput gamepad stuff. This is kinda bare and might change.
    /// - Note: The clicked/pressed actions only make sense in `ON.GUIFRAME`.
    /// - Note: Lua starts indexing at 1, you need `keysdown[string.byte('A') + 1]` to find the A key.
    /// - Note: Overlunky/etc will eat all keys it is currently configured to use, your script will only get leftovers.
    /// - Note: `gamepad` is basically [XINPUT_GAMEPAD](https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad) but variables are renamed and values are normalized to -1.0..1.0 range.
    lua["get_io"] = ImGui::GetIO;

    /// Deprecated
    /// Use `GuiDrawContext.draw_line` instead
    lua["draw_line"] = [](float x1, float y1, float x2, float y2, float thickness, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_line(x1, y1, x2, y2, thickness, color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_rect` instead
    lua["draw_rect"] = [](float x1, float y1, float x2, float y2, float thickness, float rounding, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_rect(x1, y1, x2, y2, thickness, rounding, color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_rect_filled` instead
    lua["draw_rect_filled"] = [](float x1, float y1, float x2, float y2, float rounding, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_rect_filled(x1, y1, x2, y2, rounding, color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_circle` instead
    lua["draw_circle"] = [](float x, float y, float radius, float thickness, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_circle(x, y, radius, thickness, color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_circle_filled` instead
    lua["draw_circle_filled"] = [](float x, float y, float radius, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_circle_filled(x, y, radius, color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_text` instead
    lua["draw_text"] = [](float x, float y, float size, std::string text, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_text(x, y, size, std::move(text), color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_image` instead
    lua["draw_image"] = [](IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_image(image, x1, y1, x2, y2, uvx1, uvy1, uvx2, uvy2, color);
    };
    /// Deprecated
    /// Use `GuiDrawContext.draw_image_rotated` instead
    lua["draw_image_rotated"] = [](IMAGE image, float x1, float y1, float x2, float y2, float uvx1, float uvy1, float uvx2, float uvy2, uColor color, float angle, float px, float py)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).draw_image_rotated(image, x1, y1, x2, y2, uvx1, uvy1, uvx2, uvy2, color, angle, px, py);
    };

    /// Deprecated
    /// Use `GuiDrawContext.window` instead
    lua["window"] = [](std::string title, float x, float y, float w, float h, bool movable, sol::function callback)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).window(std::move(title), x, y, w, h, movable, std::move(callback));
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_text` instead
    lua["win_text"] = [](std::string text)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_text(std::move(text));
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_separator` instead
    lua["win_separator"] = []()
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_separator();
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_inline` instead
    lua["win_inline"] = []()
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_inline();
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_sameline` instead
    lua["win_sameline"] = [](float offset, float spacing)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_sameline(offset, spacing);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_button` instead
    lua["win_button"] = [](std::string text) -> bool
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_button(std::move(text));
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_input_text` instead
    lua["win_input_text"] = [](std::string label, std::string value) -> std::string
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_input_text(std::move(label), std::move(value));
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_input_int` instead
    lua["win_input_int"] = [](std::string label, int value) -> int
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_input_int(std::move(label), value);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_input_float` instead
    lua["win_input_float"] = [](std::string label, float value) -> float
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_input_float(std::move(label), value);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_slider_int` instead
    lua["win_slider_int"] = [](std::string label, int value, int min, int max) -> int
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_slider_int(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_drag_int` instead
    lua["win_drag_int"] = [](std::string label, int value, int min, int max) -> int
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_drag_int(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_slider_float` instead
    lua["win_slider_float"] = [](std::string label, float value, float min, float max) -> float
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_slider_float(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_drag_float` instead
    lua["win_drag_float"] = [](std::string label, float value, float min, float max) -> float
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_drag_float(std::move(label), value, min, max);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_check` instead
    lua["win_check"] = [](std::string label, bool value) -> bool
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_check(std::move(label), value);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_combo` instead
    lua["win_combo"] = [](std::string label, int selected, std::string opts) -> int
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        return GuiDrawContext(backend, backend->draw_list).win_combo(std::move(label), selected, std::move(opts));
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_pushid` instead
    lua["win_pushid"] = [](int id)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_pushid(id);
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_popid` instead
    lua["win_popid"] = []()
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_popid();
    };
    /// Deprecated
    /// Use `GuiDrawContext.win_image` instead
    lua["win_image"] = [](IMAGE image, int width, int height)
    {
        LuaBackend* backend = LuaBackend::get_calling_backend();
        GuiDrawContext(backend, backend->draw_list).win_image(image, width, height);
    };
}
}; // namespace NGui
