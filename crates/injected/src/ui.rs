use crate::{db::ffi::EntityItem, models::State};


#[cxx::bridge]
pub mod ffi {
    extern "Rust" {
        unsafe fn spawn_entity(id: usize, x: f32, y: f32, s: bool, vx: f32, vy: f32);
        unsafe fn spawn_door(x: f32, y: f32, w: u8, l: u8, f: u8, t: u8);
        unsafe fn spawn_backdoor(x: f32, y: f32);
        unsafe fn teleport(x: f32, y: f32, s: bool, vx: f32, vy: f32);
        unsafe fn godmode(g: bool);
        unsafe fn zoom(level: f32);
        unsafe fn list_items();
        unsafe fn player_status();
    }
    unsafe extern "C++" {
        include!("cxx/ui.hpp");
        fn create_box(_: Vec<String>, _: Vec<u16>);
        fn init_hooks(_: usize) -> Result<bool>;
    }
}

pub unsafe fn create_box(items: &Vec<EntityItem>) {
    let names = items.iter().map(|item| item.name.clone()).collect();
    let ids = items.iter().map(|item| item.id).collect();
    ffi::create_box(names, ids);
}

// TODO: expose this to RPC
pub unsafe fn spawn_entity(id: usize, x: f32, y: f32, s: bool, vx: f32, vy: f32) {
    let state = State::new();

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            if !s {
                log::debug!("Spawning {} on {}, {}", id, x + _x, y + _y);
                state
                    .layer(player.layer())
                    .spawn_entity(id, x + _x, y + _y, s, vx, vy);
            } else {
                log::debug!("Spawning {} on screen {}, {}", id, x, y);
                state.layer(player.layer()).spawn_entity(id, x, y, s, vx, vy);
            }
        }
        None => {}
    }
}

pub unsafe fn spawn_door(x: f32, y: f32, l: u8, w: u8, f: u8, t: u8) {
    let state = State::new();

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            log::debug!("Spawning door on {}, {}", x + _x, y + _y);
            state
                .layer(player.layer())
                .spawn_door(x + _x, y + _y, l, w, f, t);
            state
                .layer(player.layer())
                .spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0);
            state
                .layer(player.layer())
                .spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0);
        }
        None => {}
    }
}

pub unsafe fn spawn_backdoor(x: f32, y: f32) {
    let state = State::new();

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            log::debug!("Spawning backdoor on {}, {}", x + _x, y + _y);
            state
                .layer(0)
                .spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0);
            state
                .layer(1)
                .spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0);
            state
                .layer(0)
                .spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0);
            state
                .layer(1)
                .spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0);
            state
                .layer(0)
                .spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0);
            state
                .layer(1)
                .spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0);
        }
        None => {}
    }
}

pub unsafe fn teleport(x: f32, y: f32, s: bool, vx: f32, vy: f32) {
    let state = State::new();

    match state.items().player(0) {
        Some(player) => {
            log::debug!("Teleporting to relative {}, {}, {}", x, y, s);
            player.teleport(x, y, s, vx, vy);
        }
        None => {}
    }
}

pub unsafe fn godmode(g: bool) {
    State::new().godmode(g);
}

pub unsafe fn zoom(level: f32) {
    State::new().zoom(level);
}

pub unsafe fn list_items() {
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            for item in state.layer(player.layer()).items() {
                log::debug!(
                    "Item: {} {:x}, position: {:?}",
                    item.unique_id(),
                    item._type().search_flags,
                    item.position_self()
                );
            }
        }
        None => {}
    }
}

pub unsafe fn player_status() {
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            let status = player.status();
            log::debug!("Player status: {:?}", [status.rope(), status.bomb()]);
            status.set_rope(99);
            status.set_bomb(99);
        }
        None => {}
    }
}
