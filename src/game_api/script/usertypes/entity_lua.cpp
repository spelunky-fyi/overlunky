#include "entity_lua.hpp"

#include "color.hpp"
#include "custom_types.hpp"
#include "movable.hpp"
#include "render_api.hpp"

#include <sol/sol.hpp>

namespace NEntity
{
void register_usertypes(sol::state& lua)
{
    lua.new_usertype<Color>(
        "Color",
        sol::constructors<Color(), Color(const Color&), Color(float, float, float, float)>{},
        sol::meta_function::equal_to,
        &Color::operator==,
        "r",
        &Color::r,
        "g",
        &Color::g,
        "b",
        &Color::b,
        "a",
        &Color::a,
        "white",
        &Color::white,
        "silver",
        &Color::silver,
        "gray",
        &Color::gray,
        "black",
        &Color::black,
        "red",
        &Color::red,
        "maroon",
        &Color::maroon,
        "yellow",
        &Color::yellow,
        "olive",
        &Color::olive,
        "lime",
        &Color::lime,
        "green",
        &Color::green,
        "aqua",
        &Color::aqua,
        "teal",
        &Color::teal,
        "blue",
        &Color::blue,
        "navy",
        &Color::navy,
        "fuchsia",
        &Color::fuchsia,
        "purple",
        &Color::purple,
        "get_rgba",
        &Color::get_rgba,
        "set_rgba",
        &Color::set_rgba,
        "get_ucolor",
        &Color::get_ucolor,
        "set_ucolor",
        &Color::set_ucolor);
    lua.new_usertype<Animation>(
        "Animation",
        "first_tile",
        &Animation::texture,
        "num_tiles",
        &Animation::count,
        "interval",
        &Animation::interval,
        "repeat_mode",
        &Animation::repeat);
    lua.new_usertype<EntityDB>(
        "EntityDB",
        "id",
        &EntityDB::id,
        "search_flags",
        &EntityDB::search_flags,
        "width",
        &EntityDB::width,
        "height",
        &EntityDB::height,
        "offsetx",
        sol::property([](EntityDB& e) -> float
                      { return e.rect_collision.offsetx; }),
        "offsety",
        sol::property([](EntityDB& e) -> float
                      { return e.rect_collision.offsety; }),
        "hitboxx",
        sol::property([](EntityDB& e) -> float
                      { return e.rect_collision.hitboxx; }),
        "hitboxy",
        sol::property([](EntityDB& e) -> float
                      { return e.rect_collision.hitboxy; }),
        "draw_depth",
        &EntityDB::draw_depth,
        "friction",
        &EntityDB::friction,
        "elasticity",
        &EntityDB::elasticity,
        "weight",
        &EntityDB::weight,
        "acceleration",
        &EntityDB::acceleration,
        "max_speed",
        &EntityDB::max_speed,
        "sprint_factor",
        &EntityDB::sprint_factor,
        "jump",
        &EntityDB::jump,
        "glow_red",
        &EntityDB::glow_red,
        "glow_green",
        &EntityDB::glow_green,
        "glow_blue",
        &EntityDB::glow_blue,
        "glow_alpha",
        &EntityDB::glow_alpha,
        "damage",
        &EntityDB::damage,
        "life",
        &EntityDB::life,
        "blood_content",
        &EntityDB::blood_content,
        "texture",
        &EntityDB::texture,
        "animations",
        &EntityDB::animations,
        "properties_flags",
        &EntityDB::properties_flags,
        "default_flags",
        &EntityDB::default_flags,
        "default_more_flags",
        &EntityDB::default_more_flags,
        "leaves_corpse_behind",
        &EntityDB::leaves_corpse_behind,
        "sound_killed_by_player",
        &EntityDB::sound_killed_by_player,
        "sound_killed_by_other",
        &EntityDB::sound_killed_by_other,
        "description",
        &EntityDB::description,
        "tilex",
        &EntityDB::tile_x,
        "tiley",
        &EntityDB::tile_y);

    /// Some information used to render the entity, can not be changed
    lua.new_usertype<RenderInfo>(
        "RenderInfo",
        "x",
        &RenderInfo::x,
        "y",
        &RenderInfo::y,
        "shader",
        &RenderInfo::shader,
        "source",
        &RenderInfo::source,
        "destination",
        sol::property([](const RenderInfo& ri) -> Quad
                      { return Quad{
                            ri.destination_bottom_left_x,
                            ri.destination_bottom_left_y,
                            ri.destination_bottom_right_x,
                            ri.destination_bottom_right_y,
                            ri.destination_top_right_x,
                            ri.destination_top_right_y,
                            ri.destination_top_left_x,
                            ri.destination_top_left_y,
                        }; }),
        "tilew",
        &RenderInfo::tilew,
        "tileh",
        &RenderInfo::tileh,
        "facing_left",
        &RenderInfo::flip_horizontal,
        "render_inactive",
        &RenderInfo::render_inactive);

    auto get_overlay = [&lua](Entity& entity)
    {
        return lua["cast_entity"](entity.overlay);
    };
    auto set_overlay = [](Entity& entity, Entity* overlay)
    {
        return entity.overlay = overlay;
    };
    auto overlay = sol::property(get_overlay, set_overlay);
    auto topmost = [&lua](Entity& entity)
    {
        return lua["cast_entity"](entity.topmost());
    };
    auto topmost_mount = [&lua](Entity& entity)
    {
        return lua["cast_entity"](entity.topmost_mount());
    };
    auto overlaps_with = sol::overload(
        static_cast<bool (Entity::*)(Entity*)>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(AABB)>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(float, float, float, float)>(&Entity::overlaps_with));
    lua.new_usertype<Entity>(
        "Entity",
        "type",
        &Entity::type,
        "overlay",
        std::move(overlay),
        "flags",
        &Entity::flags,
        "more_flags",
        &Entity::more_flags,
        "uid",
        &Entity::uid,
        "animation_frame",
        &Entity::animation_frame,
        "draw_depth",
        &Entity::draw_depth,
        "x",
        &Entity::x,
        "y",
        &Entity::y,
        "layer",
        &Entity::layer,
        "width",
        &Entity::w,
        "height",
        &Entity::h,
        "special_offsetx",
        &Entity::special_offsetx,
        "special_offsety",
        &Entity::special_offsety,
        "tile_width",
        &Entity::tilew,
        "tile_height",
        &Entity::tileh,
        "angle",
        &Entity::angle,
        "color",
        &Entity::color,
        "hitboxx",
        &Entity::hitboxx,
        "hitboxy",
        &Entity::hitboxy,
        "offsetx",
        &Entity::offsetx,
        "offsety",
        &Entity::offsety,
        "rendering_info",
        &Entity::rendering_info,
        "topmost",
        topmost,
        "topmost_mount",
        topmost_mount,
        "overlaps_with",
        overlaps_with,
        "get_texture",
        &Entity::get_texture,
        "set_texture",
        &Entity::set_texture,
        "set_draw_depth",
        &Entity::set_draw_depth,
        "liberate_from_shop",
        &Entity::liberate_from_shop,
        "get_held_entity",
        &Entity::get_held_entity,
        "set_layer",
        &Entity::set_layer,
        "remove",
        &Entity::remove,
        "respawn",
        &Entity::respawn,
        "kill",
        &Entity::kill,
        "destroy",
        &Entity::destroy,
        "activate",
        &Entity::activate,
        "perform_teleport",
        &Entity::perform_teleport,
        "trigger_action",
        &Entity::trigger_action,
        "get_metadata",
        &Entity::get_metadata,
        "apply_metadata",
        &Entity::apply_metadata,
        "set_invisible",
        &Entity::set_invisible,
        "get_items",
        &Entity::get_items,
        "is_in_liquid",
        &Entity::is_in_liquid);

    auto damage = sol::overload(
        static_cast<void (Movable::*)(uint32_t, int8_t, uint16_t, float, float)>(&Movable::broken_damage),
        static_cast<void (Movable::*)(uint32_t, int8_t, uint16_t, float, float, uint16_t)>(&Movable::damage));
    auto light_on_fire = sol::overload(
        static_cast<void (Movable::*)()>(&Movable::light_on_fire_broken),
        static_cast<void (Movable::*)(uint8_t)>(&Movable::light_on_fire));
    lua.new_usertype<Movable>(
        "Movable",
        "move",
        &Movable::move,
        "movex",
        &Movable::movex,
        "movey",
        &Movable::movey,
        "buttons",
        &Movable::buttons,
        "buttons_previous",
        &Movable::buttons_previous,
        "stand_counter",
        &Movable::stand_counter,
        "jump_height_multiplier",
        &Movable::jump_height_multiplier,
        "owner_uid",
        &Movable::owner_uid,
        "last_owner_uid",
        &Movable::last_owner_uid,
        "idle_counter",
        &Movable::idle_counter,
        "standing_on_uid",
        &Movable::standing_on_uid,
        "velocityx",
        &Movable::velocityx,
        "velocityy",
        &Movable::velocityy,
        "holding_uid",
        &Movable::holding_uid,
        "state",
        &Movable::state,
        "last_state",
        &Movable::last_state,
        "move_state",
        &Movable::move_state,
        "health",
        &Movable::health,
        "stun_timer",
        &Movable::stun_timer,
        "stun_state",
        &Movable::stun_state,
        "lock_input_timer",
        &Movable::lock_input_timer,
        "some_state",
        &Movable::some_state,
        "wet_effect_timer",
        &Movable::wet_effect_timer,
        "poison_tick_timer",
        &Movable::poison_tick_timer,
        "airtime",
        &Movable::falling_timer,
        "falling_timer",
        &Movable::falling_timer,
        "is_poisoned",
        &Movable::is_poisoned,
        "poison",
        &Movable::poison,
        "dark_shadow_timer",
        &Movable::onfire_effect_timer,
        "onfire_effect_timer",
        &Movable::onfire_effect_timer,
        "exit_invincibility_timer",
        &Movable::exit_invincibility_timer,
        "invincibility_frames_timer",
        &Movable::invincibility_frames_timer,
        "frozen_timer",
        &Movable::frozen_timer,
        "is_button_pressed",
        &Movable::is_button_pressed,
        "is_button_held",
        &Movable::is_button_held,
        "is_button_released",
        &Movable::is_button_released,
        "price",
        &Movable::price,
        "stun",
        &Movable::stun,
        "freeze",
        &Movable::freeze,
        "light_on_fire",
        light_on_fire,
        "set_cursed",
        &Movable::set_cursed,
        "drop",
        &Movable::drop,
        "pick_up",
        &Movable::pick_up,
        "can_jump",
        &Movable::can_jump,
        "standing_on",
        &Movable::standing_on,
        "add_money",
        &Movable::add_money,
        "is_on_fire",
        &Movable::is_on_fire,
        "damage",
        damage,
        "get_all_behaviors",
        &Movable::get_all_behaviors,
        "set_behavior",
        &Movable::set_behavior,
        "get_behavior",
        &Movable::get_behavior,
        sol::base_classes,
        sol::bases<Entity>());

    lua["Entity"]["as_entity"] = &Entity::as<Entity>;
    lua["Entity"]["as_movable"] = &Entity::as<Movable>;

    lua.create_named_table("ENT_TYPE"
                           //, "FLOOR_BORDERTILE", 1
                           //, "", ...check__[entities.txt]\[game_data/entities.txt\]...
                           //, "LIQUID_COARSE_LAVA", 915
    );
    for (auto& item : list_entities())
    {
        auto name = item.name.substr(9, item.name.size());
        lua["ENT_TYPE"][name] = item.id;
    }
    for (auto elm : get_custom_types_map())
    {
        lua["ENT_TYPE"][elm.second] = elm.first;
    }

    lua.create_named_table("REPEAT_TYPE", "NO_REPEAT", REPEAT_TYPE::NoRepeat, "LINEAR", REPEAT_TYPE::Linear, "BACK_AND_FORTH", REPEAT_TYPE::BackAndForth);
    lua.create_named_table("BUTTON", "JUMP", 1, "WHIP", 2, "BOMB", 4, "ROPE", 8, "RUN", 16, "DOOR", 32);
    lua.create_named_table(
        "MASK",
        "PLAYER",
        0x1,
        "MOUNT",
        0x2,
        "MONSTER",
        0x4,
        "ITEM",
        0x8,
        "EXPLOSION",
        0x10,
        "ROPE",
        0x20,
        "FX",
        0x40,
        "ACTIVEFLOOR",
        0x80,
        "FLOOR",
        0x100,
        "DECORATION",
        0x200,
        "BG",
        0x400,
        "SHADOW",
        0x800,
        "LOGICAL",
        0x1000,
        "WATER",
        0x2000,
        "LAVA",
        0x4000,
        "LIQUID",
        0x6000,
        "ANY",
        0x0);
    /* MASK
    // PLAYER
    // All CHAR_* entities, only `Player` type
    // MOUNT
    // All MOUNT_* entities, only `Mount` type
    // MONSTER
    // All MONS_* entities, various types, all `Movable`
    // ITEM
    // All ITEM_* entities except: ITEM_POWERUP_*, ITEM_ROPE, ITEM_CLIMBABLE_ROPE, ITEM_UNROLLED_ROPE, ITEM_RUBBLE, ITEM_FLAMETHROWER_FIREBALL, ITEM_CURSING_CLOUD
    // Also includes: FX_JETPACKFLAME, FX_OLMECPART_FLOATER, FX_SMALLFLAME, FX_TELEPORTSHADOW
    // Various types, all `Movable`
    // EXPLOSION
    // Only: FX_EXPLOSION, FX_POWEREDEXPLOSION, FX_MODERNEXPLOSION
    // All `Explosion` type
    // ROPE
    // Only: ITEM_ROPE, ITEM_CLIMBABLE_ROPE, ITEM_UNROLLED_ROPE
    // Various types, all `Movable`
    // FX
    // All FX_* entities except: FX_COMPASS, FX_SPECIALCOMPASS, FX_EXPLOSION, FX_POWEREDEXPLOSION, FX_MODERNEXPLOSION, FX_JETPACKFLAME, FX_OLMECPART_FLOATER, FX_SMALLFLAME, FX_TELEPORTSHADOW, FX_LEADER_FLAG, FX_PLAYERINDICATOR, FX_PLAYERINDICATORPORTRAIT
    // Also includes: DECORATION_CHAINANDBLOCKS_CHAINDECORATION, DECORATION_SLIDINGWALL_CHAINDECORATION, ITEM_RUBBLE, ITEM_FLAMETHROWER_FIREBALL, ITEM_CURSING_CLOUD
    // Various types, all `Movable`
    // ACTIVEFLOOR
    // All ACTIVEFLOOR_* entities
    // Various types, all `Movable`
    // FLOOR
    // All FLOOR_* and FLOORSTYLED_* entities
    // Various types, all `Floor`
    // DECORATION
    // All DECORATION_* entities except: DECORATION_CHAINANDBLOCKS_CHAINDECORATION, DECORATION_SLIDINGWALL_CHAINDECORATION, DECORATION_PALACE_PORTRAIT
    // Also includes: EMBED_GOLD, ENT_TYPE_EMBED_GOLD_BIG
    // Various types, all `Entity`
    // BG
    // All MIDBG* entities and most of the BG_* entities
    // does not include: a lot .. check [default_flags_more_flags.txt](internal/entity.type.default_flags_more_flags.txt) for full list
    // Also includes: DECORATION_PALACE_PORTRAIT
    // Various types, all `Entity`
    // SHADOW
    // All the BG_* entities excluded from `BG` (MASK.BG | MASK.SHADOW) will get you all BG_* entities plus one extra decoration mentioned above
    // Various types, all `Entity`
    // LOGICAL
    // All LOGICAL_* entities
    // Also includes: ITEM_POWERUP_*, FX_COMPASS, FX_SPECIALCOMPASS, FX_LEADER_FLAG, FX_PLAYERINDICATOR, FX_PLAYERINDICATORPORTRAIT
    // Various types, all `Entity`
    // WATER
    // Only: LIQUID_WATER, LIQUID_COARSE_WATER, LIQUID_IMPOSTOR_LAKE
    // Various types, all `Entity`
    // LAVA
    // Only: LIQUID_LAVA, LIQUID_STAGNANT_LAVA, LIQUID_IMPOSTOR_LAVA, LIQUID_COARSE_LAVA
    // Various types, all `Entity`
    // LIQUID
    // Short for (MASK.WATER | MASK.LAVA)
    // ANY
    // Value of 0, treated by all the functions as ANY mask
    */
}
}; // namespace NEntity
