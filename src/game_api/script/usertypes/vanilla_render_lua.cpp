#include "vanilla_render_lua.hpp"

#include "render_api.hpp"
#include "script/lua_backend.hpp"

void VanillaRenderContext::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
{
    RenderAPI::get().draw_text(text, x, y, scale_x, scale_y, color, alignment, fontstyle);
}

std::pair<float, float> VanillaRenderContext::draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle)
{
    return RenderAPI::get().draw_text_size(text, scale_x, scale_y, fontstyle);
}

void VanillaRenderContext::draw_texture(uint32_t texture_id, uint8_t row, uint8_t column, float x1, float y1, float x2, float y2, Color color)
{
    RenderAPI::get().draw_texture(texture_id, row, column, x1, y1, x2, y2, color);
}

void VanillaRenderContext::draw_texture(uint32_t texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    RenderAPI::get().draw_texture(texture_id, row, column, rect.left, rect.top, rect.right, rect.bottom, color);
}

namespace NVanillaRender
{
void register_usertypes(sol::state& lua)
{
    auto draw_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(uint32_t, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_texture),
        static_cast<void (VanillaRenderContext::*)(uint32_t, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_texture));
    lua.new_usertype<VanillaRenderContext>(
        "VanillaRenderContext",
        "draw_text",
        &VanillaRenderContext::draw_text,
        "draw_text_size",
        &VanillaRenderContext::draw_text_size,
        "draw_texture",
        draw_texture);
};
} // namespace NVanillaRender
