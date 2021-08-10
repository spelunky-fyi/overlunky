#include "vanilla_render_lua.hpp"

#include "render_api.hpp"
#include "script/lua_backend.hpp"

VanillaRenderContext::VanillaRenderContext(LuaBackend* _backend)
    : backend(_backend)
{
}

void VanillaRenderContext::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
{
    RenderAPI::get().draw_text(text, x, y, scale_x, scale_y, color, alignment, fontstyle);
}

std::pair<float, float> VanillaRenderContext::draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle)
{
    return RenderAPI::get().draw_text_size(text, scale_x, scale_y, fontstyle);
}

void VanillaRenderContext::draw_texture(uint32_t texture_id, uint8_t row, uint8_t column, float render_at_x, float render_at_y, float render_width, float render_height, Color color)
{
    RenderAPI::get().draw_texture(texture_id, row, column, render_at_x, render_at_y, render_width, render_height, color);
}

namespace NVanillaRender
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<VanillaRenderContext>(
        "VanillaRenderContext",
        "draw_text",
        &VanillaRenderContext::draw_text,
        "draw_text_size",
        &VanillaRenderContext::draw_text_size,
        "draw_texture",
        &VanillaRenderContext::draw_texture);
};
} // namespace NVanillaRender
