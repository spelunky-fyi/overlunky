#include "entities_fx_lua.hpp"

#include <algorithm>   // for max
#include <new>         // for operator new
#include <sol/sol.hpp> // for proxy_key_t, state, basic_table_core::new...
#include <string>      // for operator==, allocator
#include <tuple>       // for get
#include <type_traits> // for move, declval
#include <utility>     // for min, max

#include "entities_fx.hpp"  // for FxSaleContainer, Button, FxAnkhRotatingSpark
#include "entity.hpp"       // for Entity
#include "illumination.hpp" // IWYU pragma: keep

class Movable;

namespace NEntitiesFX
{
void register_usertypes(sol::state& lua)
{
    lua["Entity"]["as_liquidsurface"] = &Entity::as<LiquidSurface>;
    lua["Entity"]["as_olmecfloater"] = &Entity::as<OlmecFloater>;
    lua["Entity"]["as_eggshipcenterjetflame"] = &Entity::as<EggshipCenterJetFlame>;
    lua["Entity"]["as_minigameshipoffset"] = &Entity::as<MiniGameShipOffset>;
    lua["Entity"]["as_button"] = &Entity::as<Button>;
    lua["Entity"]["as_fxtornjournalpage"] = &Entity::as<FxTornJournalPage>;
    lua["Entity"]["as_fxmainexitdoor"] = &Entity::as<FxMainExitDoor>;
    lua["Entity"]["as_birdies"] = &Entity::as<Birdies>;
    lua["Entity"]["as_explosion"] = &Entity::as<Explosion>;
    lua["Entity"]["as_fxouroborooccluder"] = &Entity::as<FxOuroboroOccluder>;
    lua["Entity"]["as_fxouroborodragonpart"] = &Entity::as<FxOuroboroDragonPart>;
    lua["Entity"]["as_rubble"] = &Entity::as<Rubble>;
    lua["Entity"]["as_fxcompass"] = &Entity::as<FxCompass>;
    lua["Entity"]["as_sleepbubble"] = &Entity::as<SleepBubble>;
    lua["Entity"]["as_movingicon"] = &Entity::as<MovingIcon>;
    lua["Entity"]["as_fxsalecontainer"] = &Entity::as<FxSaleContainer>;
    lua["Entity"]["as_fxpickupeffect"] = &Entity::as<FxPickupEffect>;
    lua["Entity"]["as_fxshotgunblast"] = &Entity::as<FxShotgunBlast>;
    lua["Entity"]["as_fxjetpackflame"] = &Entity::as<FxJetpackFlame>;
    lua["Entity"]["as_fxplayerindicator"] = &Entity::as<FxPlayerIndicator>;
    lua["Entity"]["as_fxspringtrapring"] = &Entity::as<FxSpringtrapRing>;
    lua["Entity"]["as_fxwitchdoctorhint"] = &Entity::as<FxWitchdoctorHint>;
    lua["Entity"]["as_fxnecromancerankh"] = &Entity::as<FxNecromancerANKH>;
    lua["Entity"]["as_fxwebbedeffect"] = &Entity::as<FxWebbedEffect>;
    lua["Entity"]["as_fxunderwaterbubble"] = &Entity::as<FxUnderwaterBubble>;
    lua["Entity"]["as_fxwaterdrop"] = &Entity::as<FxWaterDrop>;
    lua["Entity"]["as_fxkingusliding"] = &Entity::as<FxKinguSliding>;
    lua["Entity"]["as_fxalienblast"] = &Entity::as<FxAlienBlast>;
    lua["Entity"]["as_fxsparksmall"] = &Entity::as<FxSparkSmall>;
    lua["Entity"]["as_fxtiamathead"] = &Entity::as<FxTiamatHead>;
    lua["Entity"]["as_fxtiamattorso"] = &Entity::as<FxTiamatTorso>;
    lua["Entity"]["as_fxtiamattail"] = &Entity::as<FxTiamatTail>;
    lua["Entity"]["as_fxvatbubble"] = &Entity::as<FxVatBubble>;
    lua["Entity"]["as_fxhundunneckpiece"] = &Entity::as<FxHundunNeckPiece>;
    lua["Entity"]["as_fxjellyfishstar"] = &Entity::as<FxJellyfishStar>;
    lua["Entity"]["as_fxquicksand"] = &Entity::as<FxQuickSand>;
    lua["Entity"]["as_fxsorceressattack"] = &Entity::as<FxSorceressAttack>;
    lua["Entity"]["as_fxlamassuattack"] = &Entity::as<FxLamassuAttack>;
    lua["Entity"]["as_fxfireflylight"] = &Entity::as<FxFireflyLight>;
    lua["Entity"]["as_fxempress"] = &Entity::as<FxEmpress>;
    lua["Entity"]["as_fxankhrotatingspark"] = &Entity::as<FxAnkhRotatingSpark>;
    lua["Entity"]["as_fxankhbrokenpiece"] = &Entity::as<FxAnkhBrokenPiece>;
    lua["Entity"]["as_megajellyfisheye"] = &Entity::as<MegaJellyfishEye>;

    lua.new_usertype<LiquidSurface>(
        "LiquidSurface",
        "glow_radius",
        &LiquidSurface::glow_radius,
        "sine_pos",
        &LiquidSurface::sine_pos,
        "sine_pos_increment",
        &LiquidSurface::sine_pos_increment,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<OlmecFloater>(
        "OlmecFloater",
        "both_floaters_intact",
        &OlmecFloater::both_floaters_intact,
        "on_breaking",
        &OlmecFloater::on_breaking,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<EggshipCenterJetFlame>(
        "EggshipCenterJetFlame",
        "sound",
        &EggshipCenterJetFlame::sound,
        "emitted_light",
        &EggshipCenterJetFlame::emitted_light,
        "particle",
        &EggshipCenterJetFlame::particle,
        "smoke_on",
        &EggshipCenterJetFlame::smoke_on,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<MiniGameShipOffset>(
        "MiniGameShipOffset",
        "offset_x",
        &MiniGameShipOffset::offset_x,
        "offset_y",
        &MiniGameShipOffset::offset_y,
        "normal_y_offset",
        &MiniGameShipOffset::normal_y_offset,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Button>(
        "Button",
        "button_sprite",
        &Button::button_sprite,
        "visibility",
        &Button::visibility,
        "is_visible",
        &Button::is_visible,
        "player_trigger",
        &Button::player_trigger,
        "seen",
        &Button::seen,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxTornJournalPage>(
        "FxTornJournalPage",
        "page_number",
        &FxTornJournalPage::page_number,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxMainExitDoor>(
        "FxMainExitDoor",
        "emitted_light",
        &FxMainExitDoor::emitted_light,
        "timer",
        &FxMainExitDoor::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Birdies>(
        "Birdies",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Explosion>(
        "Explosion",
        "emitted_light",
        &Explosion::emitted_light,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxOuroboroOccluder>(
        "FxOuroboroOccluder",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxOuroboroDragonPart>(
        "FxOuroboroDragonPart",
        "speed",
        &FxOuroboroDragonPart::speed,
        "timer",
        &FxOuroboroDragonPart::timer,
        "particle",
        &FxOuroboroDragonPart::particle,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<Rubble>(
        "Rubble",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxCompass>(
        "FxCompass",
        "sine_angle",
        &FxCompass::sine_angle,
        "visibility",
        &FxCompass::visibility,
        "is_active",
        &FxCompass::is_active,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<SleepBubble>(
        "SleepBubble",
        "show_hide_timer",
        &SleepBubble::show_hide_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<MovingIcon>(
        "MovingIcon",
        "movement_timer",
        &MovingIcon::movement_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxSaleContainer>(
        "FxSaleContainer",
        "fx_value",
        &FxSaleContainer::fx_value,
        "fx_icon",
        &FxSaleContainer::fx_icon,
        "fx_button",
        &FxSaleContainer::fx_button,
        "shake_amplitude",
        &FxSaleContainer::shake_amplitude,
        "sound_trigger",
        &FxSaleContainer::sound_trigger,
        "pop_in_out_procentage",
        &FxSaleContainer::pop_in_out_procentage,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxPickupEffect>(
        "FxPickupEffect",
        "spawn_y",
        &FxPickupEffect::spawn_y,
        "visibility",
        &FxPickupEffect::visibility,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxShotgunBlast>(
        "FxShotgunBlast",
        "illumination",
        &FxShotgunBlast::illumination,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxJetpackFlame>(
        "FxJetpackFlame",
        "particle_smoke",
        &FxJetpackFlame::particle_smoke,
        "particle_flame",
        &FxJetpackFlame::particle_flame,
        "sound",
        &FxJetpackFlame::sound,
        "illumination",
        &FxJetpackFlame::illumination,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxPlayerIndicator>(
        "FxPlayerIndicator",
        "attached_to",
        &FxPlayerIndicator::attached_to,
        "pos_x",
        &FxPlayerIndicator::pos_x,
        "pos_y",
        &FxPlayerIndicator::pos_y,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxSpringtrapRing>(
        "FxSpringtrapRing",
        "timer",
        &FxSpringtrapRing::timer,
        "illumination",
        &FxSpringtrapRing::illumination,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxWitchdoctorHint>(
        "FxWitchdoctorHint",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxNecromancerANKH>(
        "FxNecromancerANKH",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxWebbedEffect>(
        "FxWebbedEffect",
        "visible",
        &FxWebbedEffect::visible,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxUnderwaterBubble>(
        "FxUnderwaterBubble",
        "bubble_source_uid",
        &FxUnderwaterBubble::bubble_source_uid,
        "direction",
        &FxUnderwaterBubble::direction,
        "pop",
        &FxUnderwaterBubble::pop,
        "inverted",
        &FxUnderwaterBubble::inverted,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxWaterDrop>(
        "FxWaterDrop",
        "inverted",
        &FxWaterDrop::inverted,
        "droplet_source_uid",
        &FxWaterDrop::droplet_source_uid,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxKinguSliding>(
        "FxKinguSliding",
        "particle",
        &FxKinguSliding::particle,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxAlienBlast>(
        "FxAlienBlast",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxSparkSmall>(
        "FxSparkSmall",
        "timer",
        &FxSparkSmall::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxTiamatHead>(
        "FxTiamatHead",
        "timer",
        &FxTiamatHead::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxTiamatTorso>(
        "FxTiamatTorso",
        "timer",
        &FxTiamatTorso::timer,
        "torso_target_size",
        &FxTiamatTorso::torso_target_size,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxTiamatTail>(
        "FxTiamatTail",
        "angle_two",
        &FxTiamatTail::angle_two,
        "x_pos",
        &FxTiamatTail::x_pos,
        "y_pos",
        &FxTiamatTail::y_pos,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxVatBubble>(
        "FxVatBubble",
        "max_y",
        &FxVatBubble::max_y,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxHundunNeckPiece>(
        "FxHundunNeckPiece",
        "kill_timer",
        &FxHundunNeckPiece::kill_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxJellyfishStar>(
        "FxJellyfishStar",
        "rotation_angle",
        &FxJellyfishStar::rotation_angle,
        "radius",
        &FxJellyfishStar::radius,
        "speed",
        &FxJellyfishStar::speed,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxQuickSand>(
        "FxQuickSand",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxSorceressAttack>(
        "FxSorceressAttack",
        "size",
        &FxSorceressAttack::size,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxLamassuAttack>(
        "FxLamassuAttack",
        "attack_angle",
        &FxLamassuAttack::attack_angle,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxFireflyLight>(
        "FxFireflyLight",
        "illumination",
        &FxFireflyLight::illumination,
        "light_timer",
        &FxFireflyLight::light_timer,
        "cooldown_timer",
        &FxFireflyLight::cooldown_timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxEmpress>(
        "FxEmpress",
        "sine_angle",
        &FxEmpress::sine_angle,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxAnkhRotatingSpark>(
        "FxAnkhRotatingSpark",
        "radius",
        &FxAnkhRotatingSpark::radius,
        "inclination",
        &FxAnkhRotatingSpark::inclination,
        "speed",
        &FxAnkhRotatingSpark::speed,
        "sine_angle",
        &FxAnkhRotatingSpark::sine_angle,
        "size",
        &FxAnkhRotatingSpark::size,
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<FxAnkhBrokenPiece>(
        "FxAnkhBrokenPiece",
        sol::base_classes,
        sol::bases<Entity, Movable>());

    lua.new_usertype<MegaJellyfishEye>(
        "MegaJellyfishEye",
        "timer",
        &MegaJellyfishEye::timer,
        sol::base_classes,
        sol::bases<Entity, Movable>());
}
} // namespace NEntitiesFX
