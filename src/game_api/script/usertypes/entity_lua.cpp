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

#include "color.hpp"                     // for Color, Color::a, Color::b, Color::g
#include "containers/game_allocator.hpp" // for game_allocator
#include "custom_types.hpp"              // for get_custom_types_map
#include "entities_chars.hpp"            // for Player
#include "entity.hpp"                    // for Entity, EntityDB, Animation, Rect
#include "items.hpp"                     // for Inventory
#include "math.hpp"                      // for Quad, AABB
#include "movable.hpp"                   // for Movable, Movable::falling_timer
#include "render_api.hpp"                // for RenderInfo, RenderInfo::flip_horiz...
#include "script/lua_backend.hpp"        // for LuaBackend
#include "script/safe_cb.hpp"            // for make_safe_cb

namespace NEntity
{
void register_usertypes(sol::state& lua)
{
    /// Used in EntityDB
    lua.new_usertype<Animation>(
        "Animation",
        "id",
        &Animation::id,
        "first_tile",
        &Animation::first_tile,
        "num_tiles",
        &Animation::count,
        "interval",
        &Animation::interval,
        "repeat_mode",
        &Animation::repeat);

    /// Used in Entity and [get_type](#get_type)<br/>
    /// Stores static common data for an ENT_TYPE. You can also clone entity types with the copy constructor to create new custom entities with different common properties. [This tool](https://dregu.github.io/Spelunky2ls/animation.html) can be helpful when messing with the animations. The default values are also listed in [entities.json](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/entities.json).
    auto entitydb_type = lua.new_usertype<EntityDB>("EntityDB", sol::constructors<EntityDB(EntityDB&), EntityDB(ENT_TYPE)>{});
    entitydb_type["id"] = &EntityDB::id;
    entitydb_type["search_flags"] = &EntityDB::search_flags;
    entitydb_type["width"] = &EntityDB::width;
    entitydb_type["height"] = &EntityDB::height;
    entitydb_type["draw_depth"] = &EntityDB::draw_depth;
    entitydb_type["offsetx"] = &EntityDB::default_offsetx;
    entitydb_type["offsety"] = &EntityDB::default_offsety;
    entitydb_type["hitboxx"] = &EntityDB::default_hitboxx;
    entitydb_type["hitboxy"] = &EntityDB::default_hitboxy;
    entitydb_type["default_shape"] = &EntityDB::default_shape;
    entitydb_type["default_hitbox_enabled"] = &EntityDB::default_hitbox_enabled;
    entitydb_type["collision2_mask"] = &EntityDB::collision2_mask;
    entitydb_type["collision_mask"] = &EntityDB::collision_mask;
    entitydb_type["default_flags"] = &EntityDB::default_flags;
    entitydb_type["default_more_flags"] = &EntityDB::default_more_flags;
    entitydb_type["properties_flags"] = &EntityDB::properties_flags;
    entitydb_type["friction"] = &EntityDB::friction;
    entitydb_type["elasticity"] = &EntityDB::elasticity;
    entitydb_type["weight"] = &EntityDB::weight;
    entitydb_type["acceleration"] = &EntityDB::acceleration;
    entitydb_type["max_speed"] = &EntityDB::max_speed;
    entitydb_type["sprint_factor"] = &EntityDB::sprint_factor;
    entitydb_type["jump"] = &EntityDB::jump;
    entitydb_type["default_color"] = &EntityDB::default_color;
    entitydb_type["glow_red"] = &EntityDB::glow_red;
    entitydb_type["glow_green"] = &EntityDB::glow_green;
    entitydb_type["glow_blue"] = &EntityDB::glow_blue;
    entitydb_type["glow_alpha"] = &EntityDB::glow_alpha;
    entitydb_type["texture"] = &EntityDB::texture_id;
    entitydb_type["tilex"] = &EntityDB::tile_x;
    entitydb_type["tiley"] = &EntityDB::tile_y;
    entitydb_type["damage"] = &EntityDB::damage;
    entitydb_type["life"] = &EntityDB::life;
    entitydb_type["sacrifice_value"] = &EntityDB::sacrifice_value;
    entitydb_type["blood_content"] = &EntityDB::blood_content;
    entitydb_type["leaves_corpse_behind"] = &EntityDB::leaves_corpse_behind;
    entitydb_type["description"] = &EntityDB::description;
    entitydb_type["sound_killed_by_player"] = &EntityDB::sound_killed_by_player;
    entitydb_type["sound_killed_by_other"] = &EntityDB::sound_killed_by_other;
    entitydb_type["animations"] = &EntityDB::animations;
    entitydb_type["default_special_offsetx"] = &EntityDB::default_special_offsetx;
    entitydb_type["default_special_offsety"] = &EntityDB::default_special_offsety;

    /// Some information used to render the entity, can not be changed, used in Entity
    lua.new_usertype<RenderInfo>(
        "RenderInfo",
        "x",
        &RenderInfo::x,
        "y",
        &RenderInfo::y,
        "offset_x",
        &RenderInfo::offset_x,
        "offset_y",
        &RenderInfo::offset_y,
        "shader",
        &RenderInfo::shader,
        "source",
        &RenderInfo::source,
        "destination",
        sol::property(
            [](const RenderInfo& ri) -> Quad
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
        "angle",
        &RenderInfo::angle1,
        "animation_frame",
        &RenderInfo::animation_frame,
        "render_inactive",
        &RenderInfo::render_inactive,
        "brightness",
        &RenderInfo::brightness,
        "texture_num",
        sol::readonly(&RenderInfo::texture_num),
        "get_entity",
        &RenderInfo::get_entity,
        "set_normal_map_texture",
        &RenderInfo::set_normal_map_texture,
        "get_second_texture",
        [](const RenderInfo& ri) -> std::optional<TEXTURE>
        {
            if (!ri.texture_names[1] || ri.texture_num < 2)
            {
                return std::nullopt;
            }
            return ::get_texture(std::string_view(*ri.texture_names[1])) /**/;
        },
        "get_third_texture",
        [](const RenderInfo& ri) -> std::optional<TEXTURE>
        {
            if (!ri.texture_names[2] || ri.texture_num < 3)
            {
                return std::nullopt;
            }
            return ::get_texture(std::string_view(*ri.texture_names[2])) /**/;
        },
        "set_second_texture",
        &RenderInfo::set_second_texture,
        "set_third_texture",
        &RenderInfo::set_third_texture,
        "set_texture_num",
        &RenderInfo::set_texture_num);

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
        static_cast<bool (Entity::*)(Entity*) const>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(AABB) const>(&Entity::overlaps_with),
        static_cast<bool (Entity::*)(float, float, float, float) const>(&Entity::overlaps_with));

    auto kill_recursive = sol::overload(
        static_cast<void (Entity::*)(bool, Entity*)>(&Entity::kill_recursive),
        static_cast<void (Entity::*)(bool, Entity*, std::optional<uint32_t>, std::vector<ENT_TYPE>, RECURSIVE_MODE)>(&Entity::kill_recursive));
    auto destroy_recursive = sol::overload(
        static_cast<void (Entity::*)()>(&Entity::destroy_recursive),
        static_cast<void (Entity::*)(std::optional<uint32_t>, std::vector<ENT_TYPE>, RECURSIVE_MODE)>(&Entity::destroy_recursive));

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
    // entity_type["abs_x"] = &Entity::abs_x;
    /// NoDoc
    entity_type["abs_x"] = sol::property([](Entity& e) -> float
                                         { return e.abs_position().x; });
    // entity_type["abs_y"] = &Entity::abs_y;
    /// NoDoc
    entity_type["abs_y"] = sol::property([](Entity& e) -> float
                                         { return e.abs_position().y; });
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
    /// optional unknown - game usually sets it to 0, doesn't appear to have any special effect (needs more reverse engineering)
    entity_type["set_draw_depth"] = [](Entity& ent, uint8_t draw_depth, sol::optional<uint8_t> unknown) -> void
    { ent.set_draw_depth(draw_depth, unknown.value_or(0)); }; // for backward compatibility

    entity_type["reset_draw_depth"] = &Entity::reset_draw_depth;
    entity_type["friction"] = &Entity::friction;
    entity_type["set_enable_turning"] = &Entity::set_enable_turning;

    auto liberate_from_shop = sol::overload(&Entity::liberate_from_shop, [](Entity& ent) // for backward compatibility)
                                            { ent.liberate_from_shop(true); });

    entity_type["liberate_from_shop"] = liberate_from_shop;
    entity_type["get_held_entity"] = &Entity::get_held_entity;
    entity_type["set_layer"] = &Entity::set_layer;
    entity_type["apply_layer"] = &Entity::apply_layer;
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
    entity_type["is_cursed"] = &Entity::is_cursed;
    entity_type["is_movable"] = &Entity::is_movable;
    entity_type["can_be_pushed"] = &Entity::can_be_pushed;
    entity_type["kill_recursive"] = kill_recursive;
    entity_type["destroy_recursive"] = destroy_recursive;
    entity_type["update"] = &Entity::update_state_machine;
    entity_type["flip"] = &Entity::flip;
    entity_type["remove_item"] = &Entity::remove_item;
    entity_type["apply_db"] = &Entity::apply_db;
    entity_type["get_absolute_velocity"] = &Entity::get_absolute_velocity;
    entity_type["get_hitbox"] = &Entity::get_hitbox;
    entity_type["attach"] = &Entity::attach;
    entity_type["detach"] = &Entity::detach;

    /* Entity
    // user_data
    // You can put any arbitrary lua object here for custom entities or player stats, which is then saved across level transitions for players and carried items, mounts etc... This field is local to the script and multiple scripts can write different things in the same entity. The data is saved right before ON.PRE_LOAD_SCREEN from a level and loaded right before ON.POST_LOAD_SCREEN to a level or transition. It is not available yet in post_entity_spawn, but that is a good place to initialize it for new custom entities. See example for more.
    */

    auto damage = sol::overload(
        &Movable::broken_damage,
        &Movable::damage);
    auto light_on_fire = sol::overload(
        [](Movable& ent)              // for backwards compatibility
        { ent.light_on_fire(0x64); }, // kind of standard value that the game uses
        &Movable::light_on_fire);
    auto add_money = sol::overload(
        [](Movable& ent, int amount) // for backwards compatibility
        {static const auto coin = to_id("ENT_TYPE_ITEM_GOLDCOIN"); 
        ent.collect_treasure(amount, coin); },                         // adds a coin to the table cause the collected_money_count is expected to increase
        &Movable::collect_treasure);

    auto movable_type = lua.new_usertype<Movable>("Movable", sol::base_classes, sol::bases<Entity>());
    movable_type["move"] = &Movable::move;
    movable_type["movex"] = &Movable::movex;
    movable_type["movey"] = &Movable::movey;
    movable_type["buttons"] = &Movable::buttons;
    movable_type["buttons_previous"] = &Movable::buttons_previous;
    movable_type["stand_counter"] = &Movable::stand_counter;
    movable_type["jump_height_multiplier"] = &Movable::jump_height_multiplier;
    movable_type["price"] = &Movable::price;
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
    /// NoDoc
    movable_type["airtime"] = &Movable::falling_timer;
    movable_type["falling_timer"] = &Movable::falling_timer;
    movable_type["dark_shadow_timer"] = &Movable::onfire_effect_timer;
    movable_type["onfire_effect_timer"] = &Movable::onfire_effect_timer;
    movable_type["exit_invincibility_timer"] = &Movable::exit_invincibility_timer;
    movable_type["invincibility_frames_timer"] = &Movable::invincibility_frames_timer;
    movable_type["frozen_timer"] = &Movable::frozen_timer;
    movable_type["is_poisoned"] = &Movable::is_poisoned;
    movable_type["poison"] = &Movable::poison;
    movable_type["is_button_pressed"] = &Movable::is_button_pressed;
    movable_type["is_button_held"] = &Movable::is_button_held;
    movable_type["is_button_released"] = &Movable::is_button_released;
    movable_type["stun"] = &Movable::stun;

    auto freeze = sol::overload(&Movable::freeze, [](Movable& ent, uint8_t frame_count) // for backwards compatibility
                                { ent.freeze(frame_count, false); });

    auto set_cursed = sol::overload(&Movable::set_cursed, [](Movable& ent, bool b) // for backwards compatibility
                                    { ent.set_cursed(b, true); });

    movable_type["freeze"] = freeze;
    movable_type["light_on_fire"] = light_on_fire;
    movable_type["set_cursed"] = set_cursed;
    movable_type["drop"] = &Movable::drop;
    movable_type["pick_up"] = &Movable::pick_up;
    movable_type["standing_on"] = &Movable::standing_on;
    /// NoDoc
    movable_type["add_money"] = add_money;
    movable_type["collect_treasure"] = &Movable::collect_treasure;
    movable_type["can_jump"] = &Movable::can_jump;
    movable_type["is_on_fire"] = &Movable::is_on_fire;
    movable_type["is_powerup_capable"] = &Movable::is_powerup_capable;
    movable_type["can_be_picked_up_by"] = &Movable::can_be_picked_up_by;
    movable_type["can_break_block"] = &Movable::can_break_block;
    movable_type["break_block"] = &Movable::break_block;
    movable_type["damage"] = damage;
    movable_type["get_all_behaviors"] = &Movable::get_all_behaviors;
    movable_type["set_behavior"] = &Movable::set_behavior;
    movable_type["get_behavior"] = &Movable::get_behavior;
    movable_type["set_gravity"] = &Movable::set_gravity;
    movable_type["reset_gravity"] = &Movable::reset_gravity;
    movable_type["set_position"] = &Movable::set_position;
    movable_type["process_input"] = &Movable::process_input;
    movable_type["calculate_jump_velocity"] = &Movable::calculate_jump_velocity;
    movable_type["apply_velocity"] = &Movable::apply_velocity;
    movable_type["get_damage"] = &Movable::get_damage;
    movable_type["attack"] = &Movable::attack;
    movable_type["thrown_into"] = &Movable::thrown_into;
    movable_type["get_damage_sound"] = &Movable::get_damage_sound;
    movable_type["copy_extra_info"] = &Movable::copy_extra_info;
    movable_type["cutscene"] = sol::readonly(&Movable::cutscene_behavior);
    movable_type["clear_cutscene"] = [](Movable& movable) -> void
    {
        delete movable.cutscene_behavior;
        movable.cutscene_behavior = nullptr;
    };

    lua.new_usertype<CutsceneBehavior>("CutsceneBehavior", sol::no_constructor);

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
    for (auto& elm : get_custom_types_vector())
    {
        lua["ENT_TYPE"][elm.second] = (uint32_t)elm.first;
    }

    lua.create_named_table("RECURSIVE_MODE", "EXCLUSIVE", RECURSIVE_MODE::EXCLUSIVE, "INCLUSIVE", RECURSIVE_MODE::INCLUSIVE, "NONE", RECURSIVE_MODE::NONE);
    /* RECURSIVE_MODE
    // EXCLUSIVE
    // In this mode the provided ENT_TYPE and MASK will not be affected nor will entities attached to them
    // INCLUSIVE
    // In this mode the provided ENT_TYPE and MASK will be the only affected entities, anything outside of the specified mask or type will not be touched including entities attached to them
    // For this mode you have to specify at least one mask or ENT_TYPE, otherwise nothing will be affected
    // NONE
    // Ignores provided ENT_TYPE and MASK and affects all the entities
    */

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
    // All the BG_* entities excluded from `BG` (MASK.BG &#124; MASK.SHADOW) will get you all BG_* entities plus one extra decoration mentioned above
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
    // Short for (MASK.WATER &#124; MASK.LAVA)
    // ANY
    // Value of 0, treated by all the functions as ANY mask
    */

    /// 16bit bitmask used in Movable::damage. Can be many things, like 0x2024 = hit by a burning object that was thrown by an explosion.
    lua.create_named_table(
        "DAMAGE_TYPE",
        "GENERIC",
        0x1,
        "WHIP",
        0x2,
        "THROW",
        0x4,
        "ARROW",
        0x8,
        "SWORD",
        0x10,
        "FIRE",
        0x20,
        "POISON",
        0x40,
        "POISON_TICK",
        0x80,
        "CURSE",
        0x100,
        "FALL",
        0x200,
        "LASER",
        0x400,
        "ICE_BREAK",
        0x800,
        "STOMP",
        0x1000,
        "EXPLOSION",
        0x2000,
        "VOODOO",
        0x4000);
    /* DAMAGE_TYPE
    // GENERIC
    // enemy contact, rope hit, spikes(-1 damage), anubisshot, forcefield, dagger shot, spear trap...
    // THROW
    // rock, bullet, monkey, yeti
    // FIRE
    // fire, fireball, lava
    // POISON
    // applies the status effect, not damage
    // POISON_TICK
    // actual damage from being poisoned for a while
    // CURSE
    // witchskull, catmummy directly, but not cloud
    // LASER
    // laser trap, ufo, not dagger
    // ICE_BREAK
    // damage or fall when frozen
    // EXPLOSION
    // also from lava
    */
}
}; // namespace NEntity
