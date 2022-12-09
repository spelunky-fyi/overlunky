#include "vanilla_render_lua.hpp"

#include <algorithm>   // for max
#include <cstdlib>     // for abs
#include <locale>      // for num_put
#include <new>         // for operator new
#include <optional>    // for nullopt
#include <sol/sol.hpp> // for global_table, proxy_key_t, optional, state
#include <tuple>       // for get
#include <type_traits> // for move, declval

#include "render_api.hpp" // for TextureRenderingInfo, WorldShader, TextRen...
#include "state.hpp"      // for enum_to_layer
#include "texture.hpp"    // for Texture, get_texture

void VanillaRenderContext::draw_text(const std::string& text, float x, float y, float scale_x, float scale_y, Color color, uint32_t alignment, uint32_t fontstyle)
{
    RenderAPI::get().draw_text(text, x, y, scale_x, scale_y, color, alignment, fontstyle);
}

void VanillaRenderContext::draw_text(const TextRenderingInfo* tri, Color color)
{
    RenderAPI::get().draw_text(tri, color);
}

std::pair<float, float> VanillaRenderContext::draw_text_size(const std::string& text, float scale_x, float scale_y, uint32_t fontstyle)
{
    return RenderAPI::get().draw_text_size(text, scale_x, scale_y, fontstyle);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    draw_screen_texture(texture_id, row, column, Quad(AABB(left, top, right, bottom)), color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_screen_texture(texture_id, row, column, Quad(rect), color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle, float px, float py)
{
    Quad dest{rect};
    if (angle != 0)
    {
        constexpr float ratio = 16.0f / 9.0f;
        constexpr float inverse_ratio = 9.0f / 16.0f;

        // fix ratio to 1/1 to properly rotate the coordinates
        const AABB new_rect{rect.left * ratio, rect.top, rect.right * ratio, rect.bottom};
        dest = Quad{new_rect};
        auto pivot = new_rect.center();
        if (px != 0 || py != 0)
        {
            pivot.first += abs(pivot.first - new_rect.left) * px;
            pivot.second += abs(pivot.second - new_rect.bottom) * py;
        }

        dest.rotate(angle, pivot.first, pivot.second);
        // bring back to the 16/9
        dest.bottom_left_x *= inverse_ratio;
        dest.bottom_right_x *= inverse_ratio;
        dest.top_left_x *= inverse_ratio;
        dest.top_right_x *= inverse_ratio;
    }
    draw_screen_texture(texture_id, row, column, dest, color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    float uv_left = (texture->tile_width_fraction * column) + texture->offset_x_weird_math;
    float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
    float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
    float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

    const Quad source(
        // bottom left:
        uv_left,
        uv_bottom,
        // bottom right:
        uv_right,
        uv_bottom,
        // top right:
        uv_right,
        uv_top,
        // top left:
        uv_left,
        uv_top);

    RenderAPI::get().draw_screen_texture(texture, source, dest, color);
}

void VanillaRenderContext::draw_screen_texture(TEXTURE texture_id, const Quad& source, const Quad& quad, Color color)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_screen_texture(texture, source, quad, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, float left, float top, float right, float bottom, Color color)
{
    draw_world_texture(texture_id, row, column, Quad{AABB{left, top, right, bottom}}, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color)
{
    draw_world_texture(texture_id, row, column, Quad{rect}, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const AABB& rect, Color color, float angle, float px, float py)
{
    if (bounding_box.is_valid() && !rect.overlaps_with(bounding_box))
    {
        return;
    }
    Quad new_quad(rect);
    if (angle != 0)
    {
        auto center = rect.center();
        if (px != 0 || py != 0)
        {
            center.first += abs(center.first - rect.left) * px;
            center.second += abs(center.second - rect.bottom) * py;
        }
        new_quad.rotate(angle, center.first, center.second);
    }
    draw_world_texture(texture_id, row, column, new_quad, color);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color, WORLD_SHADER shader)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }

    float uv_left = (texture->tile_width_fraction * column) + texture->offset_x_weird_math;
    float uv_right = uv_left + texture->tile_width_fraction - texture->one_over_width;
    float uv_top = (texture->tile_height_fraction * row) + texture->offset_y_weird_math;
    float uv_bottom = uv_top + texture->tile_height_fraction - texture->one_over_height;

    Quad source = {
        // bottom left:
        uv_left,
        uv_bottom,
        // bottom right:
        uv_right,
        uv_bottom,
        // top right:
        uv_right,
        uv_top,
        // top left:
        uv_left,
        uv_top,
    };
    RenderAPI::get().draw_world_texture(texture, source, dest, color, (WorldShader)shader);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, uint8_t row, uint8_t column, const Quad& dest, Color color)
{
    draw_world_texture(texture_id, row, column, dest, color, (WORLD_SHADER)WorldShader::TextureColor);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color, WORLD_SHADER shader)
{
    auto texture = get_texture(texture_id);
    if (texture == nullptr)
    {
        return;
    }
    RenderAPI::get().draw_world_texture(texture, source, dest, color, (WorldShader)shader);
}

void VanillaRenderContext::draw_world_texture(TEXTURE texture_id, const Quad& source, const Quad& dest, Color color)
{
    draw_world_texture(texture_id, source, dest, color, (WORLD_SHADER)WorldShader::TextureColor);
}

// For the custom constructor
void TextRenderingInfo_ctor(TextRenderingInfo& uninitialized_memory, const std::u16string text, float x, float y, float scale_x, float scale_y, uint32_t alignment, uint32_t fontstyle)
{
    new (&uninitialized_memory) TextRenderingInfo{};
    uninitialized_memory.set_text(text, x, y, scale_x, scale_y, alignment, fontstyle);
}

namespace NVanillaRender
{
void register_usertypes(sol::state& lua)
{
    static const auto set_lut = [](sol::optional<TEXTURE> texture_id, LAYER layer)
    {
        RenderAPI& render_api = RenderAPI::get();
        if (layer != LAYER::BOTH)
        {
            const uint8_t real_layer = enum_to_layer(layer);
            if (texture_id)
            {
                render_api.set_lut(texture_id.value(), real_layer);
            }
            else
            {
                render_api.reset_lut(real_layer);
            }
        }
        else
        {
            if (texture_id)
            {
                render_api.set_lut(texture_id.value(), 0);
                render_api.set_lut(texture_id.value(), 1);
            }
            else
            {
                render_api.reset_lut(0);
                render_api.reset_lut(1);
            }
        }
    };

    /// Force the LUT texture for the given layer (or both) until it is reset.
    /// Pass `nil` in the first parameter to reset
    lua["set_lut"] = [](sol::optional<TEXTURE> texture_id, LAYER layer)
    {
        set_lut(texture_id, layer);
    };
    /// Same as `set_lut(nil, layer)`
    lua["reset_lut"] = [](LAYER layer)
    {
        set_lut(sol::nullopt, layer);
    };

    /// NoDoc: Dev-tool only
    lua["reload_shaders"] = []()
    {
        RenderAPI::get().reload_shaders();
    };

    auto draw_screen_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float, float, float)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_screen_texture));
    auto draw_world_texture = sol::overload(
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, float, float, float, float, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const AABB&, Color, float, float, float)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, uint8_t, uint8_t, const Quad&, Color, WORLD_SHADER)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color)>(&VanillaRenderContext::draw_world_texture),
        static_cast<void (VanillaRenderContext::*)(TEXTURE, const Quad&, const Quad&, Color, WORLD_SHADER)>(&VanillaRenderContext::draw_world_texture));
    auto draw_text = sol::overload(
        static_cast<void (VanillaRenderContext::*)(const std::string&, float, float, float, float, Color, uint32_t, uint32_t)>(&VanillaRenderContext::draw_text),
        static_cast<void (VanillaRenderContext::*)(const TextRenderingInfo*, Color)>(&VanillaRenderContext::draw_text));

    /// Used in [set_callback](#set_callback) ON.RENDER_* callbacks, [set_post_render](#set_post_render), [set_post_render_screen](#set_post_render_screen), [set_pre_render](#set_pre_render), [set_pre_render_screen](#set_pre_render_screen)
    lua.new_usertype<VanillaRenderContext>(
        "VanillaRenderContext",
        "draw_text",
        draw_text,
        "draw_text_size",
        &VanillaRenderContext::draw_text_size,
        "draw_screen_texture",
        draw_screen_texture,
        "draw_world_texture",
        draw_world_texture);

    auto texturerenderinginfo_type = lua.new_usertype<TextureRenderingInfo>("TextureRenderingInfo");
    texturerenderinginfo_type["x"] = &TextureRenderingInfo::x;
    texturerenderinginfo_type["y"] = &TextureRenderingInfo::y;
    texturerenderinginfo_type["destination_bottom_left_x"] = &TextureRenderingInfo::destination_bottom_left_x;
    texturerenderinginfo_type["destination_bottom_left_y"] = &TextureRenderingInfo::destination_bottom_left_y;
    texturerenderinginfo_type["destination_bottom_right_x"] = &TextureRenderingInfo::destination_bottom_right_x;
    texturerenderinginfo_type["destination_bottom_right_y"] = &TextureRenderingInfo::destination_bottom_right_y;
    texturerenderinginfo_type["destination_top_left_x"] = &TextureRenderingInfo::destination_top_left_x;
    texturerenderinginfo_type["destination_top_left_y"] = &TextureRenderingInfo::destination_top_left_y;
    texturerenderinginfo_type["destination_top_right_x"] = &TextureRenderingInfo::destination_top_right_x;
    texturerenderinginfo_type["destination_top_right_y"] = &TextureRenderingInfo::destination_top_right_y;
    texturerenderinginfo_type["set_destination"] = &TextureRenderingInfo::set_destination;
    texturerenderinginfo_type["dest_get_quad"] = &TextureRenderingInfo::dest_get_quad;
    texturerenderinginfo_type["dest_set_quad"] = &TextureRenderingInfo::dest_set_quad;
    texturerenderinginfo_type["source_bottom_left_x"] = &TextureRenderingInfo::source_bottom_left_x;
    texturerenderinginfo_type["source_bottom_left_y"] = &TextureRenderingInfo::source_bottom_left_y;
    texturerenderinginfo_type["source_bottom_right_x"] = &TextureRenderingInfo::source_bottom_right_x;
    texturerenderinginfo_type["source_bottom_right_y"] = &TextureRenderingInfo::source_bottom_right_y;
    texturerenderinginfo_type["source_top_left_x"] = &TextureRenderingInfo::source_top_left_x;
    texturerenderinginfo_type["source_top_left_y"] = &TextureRenderingInfo::source_top_left_y;
    texturerenderinginfo_type["source_top_right_x"] = &TextureRenderingInfo::source_top_right_x;
    texturerenderinginfo_type["source_top_right_y"] = &TextureRenderingInfo::source_top_right_y;
    texturerenderinginfo_type["source_get_quad"] = &TextureRenderingInfo::source_get_quad;
    texturerenderinginfo_type["source_set_quad"] = &TextureRenderingInfo::source_set_quad;

    lua.new_usertype<TextRenderingInfo>(
        "TextRenderingInfo",
        "new",
        sol::initializers(&TextRenderingInfo_ctor),
        "x",
        &TextRenderingInfo::x,
        "y",
        &TextRenderingInfo::y,
        "text_length",
        sol::readonly(&TextRenderingInfo::text_length),
        "width",
        &TextRenderingInfo::width,
        "height",
        &TextRenderingInfo::height,
        "font",
        &TextRenderingInfo::font,
        "text_size",
        &TextRenderingInfo::text_size);

    /* TextRenderingInfo
    // new
    // TextRenderingInfo:new(string text, float x, float y, float scale_x, float scale_y, int alignment, int fontstyle)
    // Creates new TextRenderingInfo that can be used in VanillaRenderContext draw_text
    // For static text, it is better to use one object and call draw_text with it, instead of relaying on draw_text creating this object for you
    // text_length
    // You can also just use `#` operator to get the text lenght
    */

    lua.create_named_table(
        "WORLD_SHADER",
        "COLOR",
        WorldShader::Colors,
        "TEXTURE",
        WorldShader::Texture,
        "TEXTURE_ALPHA_COLOR",
        WorldShader::TextureAlphaColor,
        "TEXTURE_COLOR",
        WorldShader::TextureColor,
        "TEXTURE_COLORS_WARP",
        WorldShader::TextureColorsWarp,
        "DEFERRED_COLOR_TRANSPARENT",
        WorldShader::DeferredColorTransparent,
        "DEFERRED_TEXTURE_COLOR",
        WorldShader::DeferredTextureColor,
        "DEFERRED_TEXTURE_COLOR_POISONED",
        WorldShader::DeferredTextureColor_Poisoned,
        "DEFERRED_TEXTURE_COLOR_CURSED",
        WorldShader::DeferredTextureColor_Cursed,
        "DEFERRED_TEXTURE_COLOR_POISONED_CURSED",
        WorldShader::DeferredTextureColor_PoisonedCursed,
        "DEFERRED_TEXTURE_COLOR_TRANSPARENT",
        WorldShader::DeferredTextureColor_Transparent,
        "DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED",
        WorldShader::DeferredTextureColor_TransparentCorrected,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE",
        WorldShader::DeferredTextureColor_Emissive,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW",
        WorldShader::DeferredTextureColor_EmissiveGlow,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY",
        WorldShader::DeferredTextureColor_EmissiveGlowHeavy,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS",
        WorldShader::DeferredTextureColor_EmissiveGlowBrightness,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW",
        WorldShader::DeferredTextureColor_EmissiveColorizedGlow,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW",
        WorldShader::DeferredTextureColor_EmissiveColorizedGlow_DynamicGlow,
        "DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION",
        WorldShader::DeferredTextureColor_EmissiveColorizedGlow_Saturation);
    /* WORLD_SHADER
    // COLOR
    // Renders a solid color
    // TEXTURE
    // Renders a texture without applying the given color
    // TEXTURE_ALPHA_COLOR
    // Renders a texture by interpreting its red channel as alpha and applying the given color
    // TEXTURE_COLOR
    // The default shader to be used, just renders a texture with transparancy and the given color
    // TEXTURE_COLORS_WARP
    // Renders the texture, with "gamma correction" of the color channels and multiplying everything by the input color alpha only
    // DEFERRED_COLOR_TRANSPARENT
    // Basically same as COLOR but goes through the deferred pipeline
    // DEFERRED_TEXTURE_COLOR
    // Basically same as TEXTURE_COLOR but goes through the deferred pipeline
    // DEFERRED_TEXTURE_COLOR
    // Basically same as TEXTURE_COLOR but goes through the deferred pipeline
    // DEFERRED_TEXTURE_COLOR_POISONED
    // Same as DEFERRED_TEXTURE_COLOR but applies poison color effect
    // DEFERRED_TEXTURE_COLOR_CURSED
    // Same as DEFERRED_TEXTURE_COLOR but applies cursed color effect
    // DEFERRED_TEXTURE_COLOR_POISONED_CURSED
    // Same as DEFERRED_TEXTURE_COLOR but applies poisoned and cursed color effect
    // DEFERRED_TEXTURE_COLOR_TRANSPARENT
    // Basically same as DEFERRED_TEXTURE_COLOR
    // DEFERRED_TEXTURE_COLOR_TRANSPARENT_CORRECTED
    // Same as DEFERRED_TEXTURE_COLOR_TRANSPARENT but applies gamma correction to alpha channel
    // DEFERRED_TEXTURE_COLOR_EMISSIVE
    // Same as DEFERRED_TEXTURE_COLOR but renders to the emissive channel
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW
    // Same as DEFERRED_TEXTURE_COLOR but renders to the emissive channel with glow
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY
    // Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW but renders to the emissive channel with heavy glow
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_BRIGHTNESS
    // Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_GLOW_HEAVY but renders glow on top of the texture
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW
    // Same as DEFERRED_TEXTURE_COLOR but renders heavy glow behind the texture
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_DYNAMIC_GLOW
    // Basically same as DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW
    // DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW_SATURATION
    // Same as DEFERRED_TEXTURE_COLOR_EMISSIVE_COLORIZED_GLOW but renders texture as solid color
    */
};
} // namespace NVanillaRender
