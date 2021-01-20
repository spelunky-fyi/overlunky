// A small module with PoC codes
use crate::models::{Mount, State};

unsafe fn spawn_caveman_with_turkey() {
    let state = State::get();
    let player = state.items().player(0).unwrap();
    let position = player.position();
    let layer = state.layer(player.layer());
    let turkey: Mount = layer
        .spawn_entity(884, position.0 + 1.0, position.1, false, 0.0, 0.0, false)
        .into();
    let caveman = layer.spawn_entity_over(225, turkey.into(), -0.05, 0.52);
    turkey.carry(caveman);
    turkey.tame(true);
}

unsafe fn player_status() {
    let state = State::get();
    let player = state.items().player(0).unwrap();
    let status = player.status();
    log::debug!("{:?}", [status.rope(), status.bomb()]);
    status.set_rope(99);
    status.set_bomb(99);
    return;
}

unsafe fn list_items() {
    let state = State::get();
    for item in state.layer(0).items() {
        log::debug!(
            "Item: {} {:x}, position: {:?}",
            item.unique_id(),
            item._type().search_flags,
            item.position_self()
        );
    }
}
