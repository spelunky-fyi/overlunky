use crate::models::{Mount, State};

#[allow(dead_code)]
fn poc() {
    // Spawns caveman riding turkey
    unsafe {
        let state = State::new();
        let player = state.items().player(0).unwrap();
        let position = player.position();
        let layer = state.layer(player.layer());
        let turkey: Mount = layer
            .spawn_entity(884, position.0 + 1.0, position.1, false)
            .into();
        let caveman = layer.spawn_entity_over(225, turkey.into(), -0.05, 0.52);
        turkey.carry(caveman);
        turkey.tame(true);

        let status = player.status();
        log::debug!("{:?}", [status.rope(), status.bomb()]);
        status.set_rope(99);
        status.set_bomb(99);
        return;
    }
}
