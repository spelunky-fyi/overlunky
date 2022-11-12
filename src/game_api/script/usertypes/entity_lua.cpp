#include "entity_lua.hpp"

#include <algorithm>   // for max
#include <cstdint>     // for uint16_t, int8_t, uint32_t, uint8_t
#include <exception>   // for exception
#include <map>         // for map, _Tree_const_iterator
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, data_t, state, property
#include <string>      // for operator==, allocator, string
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max, swap, pair
#include <vector>      // for _Vector_iterator, vector, _Vector_...

#include "color.hpp"              // for Color, Color::a, Color::b, Color::g
#include "custom_types.hpp"       // for get_custom_types_map
#include "entity.hpp"             // for Entity, EntityDB, Animation, Rect
#include "math.hpp"               // for Quad, AABB
#include "movable.hpp"            // for Movable, Movable::falling_timer
#include "render_api.hpp"         // for RenderInfo, RenderInfo::flip_horiz...
#include "script/lua_backend.hpp" // for LuaBackend

namespace NEntity
{
void register_usertypes(sol::state& lua)
{
    auto color_type = lua.new_usertype<Color>("Color", sol::constructors<Color(), Color(const Color&), Color(float, float, float, float)>{}, sol::meta_function::equal_to, &Color::operator==);
    color_type["r"] = &Color::r;
    color_type["g"] = &Color::g;
    color_type["b"] = &Color::b;
    color_type["a"] = &Color::a;
    color_type["white"] = &Color::white;
    color_type["silver"] = &Color::silver;
    color_type["gray"] = &Color::gray;
    color_type["black"] = &Color::black;
    color_type["red"] = &Color::red;
    color_type["maroon"] = &Color::maroon;
    color_type["yellow"] = &Color::yellow;
    color_type["olive"] = &Color::olive;
    color_type["lime"] = &Color::lime;
    color_type["green"] = &Color::green;
    color_type["aqua"] = &Color::aqua;
    color_type["teal"] = &Color::teal;
    color_type["blue"] = &Color::blue;
    color_type["navy"] = &Color::navy;
    color_type["fuchsia"] = &Color::fuchsia;
    color_type["purple"] = &Color::purple;
    color_type["get_rgba"] = &Color::get_rgba;
    color_type["set_rgba"] = &Color::set_rgba;
    color_type["get_ucolor"] = &Color::get_ucolor;
    color_type["set_ucolor"] = &Color::set_ucolor;

    lua.new_usertype<Animation>(
        "Animation",
        "id",
        &Animation::key,
        "first_tile",
        &Animation::texture,
        "num_tiles",
        &Animation::count,
        "interval",
        &Animation::interval,
        "repeat_mode",
        &Animation::repeat);

    auto entitydb_type = lua.new_usertype<EntityDB>("EntityDB", sol::constructors<EntityDB(EntityDB&), EntityDB(ENT_TYPE)>{});
    entitydb_type["id"] = &EntityDB::id;
    entitydb_type["search_flags"] = &EntityDB::search_flags;
    entitydb_type["width"] = &EntityDB::width;
    entitydb_type["height"] = &EntityDB::height;
    entitydb_type["offsetx"] = &EntityDB::default_offsetx;
    entitydb_type["offsety"] = &EntityDB::default_offsety;
    entitydb_type["hitboxx"] = &EntityDB::default_hitboxx;
    entitydb_type["hitboxy"] = &EntityDB::default_hitboxy;
    entitydb_type["draw_depth"] = &EntityDB::draw_depth;
    entitydb_type["friction"] = &EntityDB::friction;
    entitydb_type["elasticity"] = &EntityDB::elasticity;
    entitydb_type["weight"] = &EntityDB::weight;
    entitydb_type["acceleration"] = &EntityDB::acceleration;
    entitydb_type["max_speed"] = &EntityDB::max_speed;
    entitydb_type["sprint_factor"] = &EntityDB::sprint_factor;
    entitydb_type["jump"] = &EntityDB::jump;
    entitydb_type["glow_red"] = &EntityDB::glow_red;
    entitydb_type["glow_green"] = &EntityDB::glow_green;
    entitydb_type["glow_blue"] = &EntityDB::glow_blue;
    entitydb_type["glow_alpha"] = &EntityDB::glow_alpha;
    entitydb_type["damage"] = &EntityDB::damage;
    entitydb_type["life"] = &EntityDB::life;
    entitydb_type["blood_content"] = &EntityDB::blood_content;
    entitydb_type["texture"] = &EntityDB::texture;
    entitydb_type["animations"] = &EntityDB::animations;
    entitydb_type["properties_flags"] = &EntityDB::properties_flags;
    entitydb_type["default_flags"] = &EntityDB::default_flags;
    entitydb_type["default_more_flags"] = &EntityDB::default_more_flags;
    entitydb_type["leaves_corpse_behind"] = &EntityDB::leaves_corpse_behind;
    entitydb_type["sound_killed_by_player"] = &EntityDB::sound_killed_by_player;
    entitydb_type["sound_killed_by_other"] = &EntityDB::sound_killed_by_other;
    entitydb_type["description"] = &EntityDB::description;
    entitydb_type["tilex"] = &EntityDB::tile_x;
    entitydb_type["tiley"] = &EntityDB::tile_y;

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

    auto get_user_data = [](Entity& entity) -> sol::object
    {
        auto backend = LuaBackend::get_calling_backend();
        if (sol::object user_data = backend->get_user_data(entity))
        {
            return user_data;
        }
        return sol::nil;
    };
    auto set_user_data = [](Entity& entity, sol::object user_data) -> void
    {
        auto backend = LuaBackend::get_calling_backend();
        backend->set_user_data(entity, user_data);
    };
    auto user_data = sol::property(get_user_data, set_user_data);

    auto overlaps_with = sol::overload(
        static_cast<bool (Entity::*)(Entity*)>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(AABB)>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(float, float, float, float)>(&Entity::overlaps_with));

    auto entity_type = lua.new_usertype<Entity>("Entity");
    entity_type["type"] = &Entity::type;
    entity_type["overlay"] = std::move(overlay);
    entity_type["flags"] = &Entity::flags;
    entity_type["more_flags"] = &Entity::more_flags;
    entity_type["uid"] = &Entity::uid;
    entity_type["animation_frame"] = &Entity::animation_frame;
    entity_type["draw_depth"] = &Entity::draw_depth;
    entity_type["x"] = &Entity::x;
    entity_type["y"] = &Entity::y;
    entity_type["layer"] = &Entity::layer;
    entity_type["width"] = &Entity::w;
    entity_type["height"] = &Entity::h;
    entity_type["special_offsetx"] = &Entity::special_offsetx;
    entity_type["special_offsety"] = &Entity::special_offsety;
    entity_type["tile_width"] = &Entity::tilew;
    entity_type["tile_height"] = &Entity::tileh;
    entity_type["angle"] = &Entity::angle;
    entity_type["color"] = &Entity::color;
    entity_type["hitboxx"] = &Entity::hitboxx;
    entity_type["hitboxy"] = &Entity::hitboxy;
    entity_type["shape"] = &Entity::shape;
    entity_type["hitbox_enabled"] = &Entity::hitbox_enabled;
    entity_type["offsetx"] = &Entity::offsetx;
    entity_type["offsety"] = &Entity::offsety;
    entity_type["rendering_info"] = &Entity::rendering_info;
    entity_type["user_data"] = std::move(user_data);
    entity_type["topmost"] = topmost;
    entity_type["topmost_mount"] = topmost_mount;
    entity_type["overlaps_with"] = overlaps_with;
    entity_type["get_texture"] = &Entity::get_texture;
    entity_type["set_texture"] = &Entity::set_texture;
    entity_type["set_draw_depth"] = &Entity::set_draw_depth;
    entity_type["set_enable_turning"] = &Entity::set_enable_turning;
    entity_type["liberate_from_shop"] = &Entity::liberate_from_shop;
    entity_type["get_held_entity"] = &Entity::get_held_entity;
    entity_type["set_layer"] = &Entity::set_layer;
    entity_type["remove"] = &Entity::remove;
    entity_type["respawn"] = &Entity::respawn;
    entity_type["kill"] = &Entity::kill;
    entity_type["destroy"] = &Entity::destroy;
    entity_type["activate"] = &Entity::activate;
    entity_type["perform_teleport"] = &Entity::perform_teleport;
    entity_type["trigger_action"] = &Entity::trigger_action;
    entity_type["get_metadata"] = &Entity::get_metadata;
    entity_type["apply_metadata"] = &Entity::apply_metadata;
    entity_type["set_invisible"] = &Entity::set_invisible;
    entity_type["get_items"] = &Entity::get_items;
    entity_type["is_in_liquid"] = &Entity::is_in_liquid;
    /* Entity
    // user_data
    // You can put any arbitrary lua object here for custom entities or player stats, which is then saved across level transitions for players and carried items, mounts etc... This field is local to the script and multiple scripts can write different things in the same entity. The data is saved right before ON.PRE_LOAD_SCREEN from a level and loaded right before ON.POST_LEVEL_GENERATION.
    */

    auto damage = sol::overload(
        static_cast<void (Movable::*)(uint32_t, int8_t, uint16_t, float, float)>(&Movable::broken_damage),
        static_cast<void (Movable::*)(uint32_t, int8_t, uint16_t, float, float, uint16_t)>(&Movable::damage));
    auto light_on_fire = sol::overload(
        static_cast<void (Movable::*)()>(&Movable::light_on_fire_broken),
        static_cast<void (Movable::*)(uint8_t)>(&Movable::light_on_fire));

    auto movable_type = lua.new_usertype<Movable>("Movable", sol::base_classes, sol::bases<Entity>());
    movable_type["move"] = &Movable::move;
    movable_type["movex"] = &Movable::movex;
    movable_type["movey"] = &Movable::movey;
    movable_type["buttons"] = &Movable::buttons;
    movable_type["buttons_previous"] = &Movable::buttons_previous;
    movable_type["stand_counter"] = &Movable::stand_counter;
    movable_type["jump_height_multiplier"] = &Movable::jump_height_multiplier;
    movable_type["owner_uid"] = &Movable::owner_uid;
    movable_type["last_owner_uid"] = &Movable::last_owner_uid;
    movable_type["current_animation"] = &Movable::current_animation;
    movable_type["idle_counter"] = &Movable::idle_counter;
    movable_type["standing_on_uid"] = &Movable::standing_on_uid;
    movable_type["velocityx"] = &Movable::velocityx;
    movable_type["velocityy"] = &Movable::velocityy;
    movable_type["holding_uid"] = &Movable::holding_uid;
    movable_type["state"] = &Movable::state;
    movable_type["last_state"] = &Movable::last_state;
    movable_type["move_state"] = &Movable::move_state;
    movable_type["health"] = &Movable::health;
    movable_type["stun_timer"] = &Movable::stun_timer;
    movable_type["stun_state"] = &Movable::stun_state;
    movable_type["lock_input_timer"] = &Movable::lock_input_timer;
    movable_type["some_state"] = &Movable::some_state;
    movable_type["wet_effect_timer"] = &Movable::wet_effect_timer;
    movable_type["poison_tick_timer"] = &Movable::poison_tick_timer;
    movable_type["airtime"] = &Movable::falling_timer;
    movable_type["falling_timer"] = &Movable::falling_timer;
    movable_type["is_poisoned"] = &Movable::is_poisoned;
    movable_type["poison"] = &Movable::poison;
    movable_type["dark_shadow_timer"] = &Movable::onfire_effect_timer;
    movable_type["onfire_effect_timer"] = &Movable::onfire_effect_timer;
    movable_type["exit_invincibility_timer"] = &Movable::exit_invincibility_timer;
    movable_type["invincibility_frames_timer"] = &Movable::invincibility_frames_timer;
    movable_type["frozen_timer"] = &Movable::frozen_timer;
    movable_type["is_button_pressed"] = &Movable::is_button_pressed;
    movable_type["is_button_held"] = &Movable::is_button_held;
    movable_type["is_button_released"] = &Movable::is_button_released;
    movable_type["price"] = &Movable::price;
    movable_type["stun"] = &Movable::stun;
    movable_type["freeze"] = &Movable::freeze;
    movable_type["light_on_fire"] = light_on_fire;
    movable_type["set_cursed"] = &Movable::set_cursed;
    movable_type["drop"] = &Movable::drop;
    movable_type["pick_up"] = &Movable::pick_up;
    movable_type["can_jump"] = &Movable::can_jump;
    movable_type["standing_on"] = &Movable::standing_on;
    movable_type["add_money"] = &Movable::add_money;
    movable_type["is_on_fire"] = &Movable::is_on_fire;
    movable_type["damage"] = damage;
    movable_type["get_all_behaviors"] = &Movable::get_all_behaviors;
    movable_type["set_behavior"] = &Movable::set_behavior;
    movable_type["get_behavior"] = &Movable::get_behavior;
    movable_type["set_gravity"] = &Movable::set_gravity;
    movable_type["reset_gravity"] = &Movable::reset_gravity;

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
    lua.create_named_table("SHAPE", "RECTANGLE", SHAPE::RECTANGLE, "CIRCLE", SHAPE::CIRCLE);
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
