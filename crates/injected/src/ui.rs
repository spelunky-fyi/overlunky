use crate::{db::ffi::EntityItem, models::State};


#[cxx::bridge]
pub mod ffi {
    extern "Rust" {
        unsafe fn spawn_entity(id: usize, x: f32, y: f32, s: bool, vx: f32, vy: f32, snap: bool) -> u32;
        unsafe fn spawn_door(x: f32, y: f32, w: u8, l: u8, f: u8, t: u8);
        unsafe fn spawn_backdoor(x: f32, y: f32);
        unsafe fn teleport(x: f32, y: f32, s: bool, vx: f32, vy: f32, snap: bool);
        unsafe fn godmode(g: bool);
        unsafe fn zoom(level: f32);
        unsafe fn list_items();
        unsafe fn get_entity_at(x: f32, y: f32, s: bool, r: f32, mask: u32) -> u32;
        unsafe fn move_entity(id: u32, x: f32, y: f32, s: bool, vx: f32, vy: f32, snap: bool);
        unsafe fn get_entity_flags(id: u32) -> u32;
        unsafe fn set_entity_flags(id: u32, flags: u32);
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
pub unsafe fn spawn_entity(id: usize, x: f32, y: f32, s: bool, vx: f32, vy: f32, snap: bool) -> u32 {
    let state = State::new();

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            if !s {
                log::debug!("Spawning {} on {}, {}", id, x + _x, y + _y);
                return state
                    .layer(player.layer())
                    .spawn_entity(id, x + _x, y + _y, s, vx, vy, snap).unique_id();
            } else {
                log::debug!("Spawning {} on screen {}, {}", id, x, y);
                return state.layer(player.layer()).spawn_entity(id, x, y, s, vx, vy, snap).unique_id();
            }
        }
        None => 0
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
                .spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
            state
                .layer(player.layer())
                .spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
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
                .spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0, true);
            state
                .layer(1)
                .spawn_entity(26, x + _x, y + _y, false, 0.0, 0.0, true);
            state
                .layer(0)
                .spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
            state
                .layer(1)
                .spawn_entity(37, x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
            state
                .layer(0)
                .spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
            state
                .layer(1)
                .spawn_entity(775, x + _x, y + _y, false, 0.0, 0.0, true);
        }
        None => {}
    }
}

pub unsafe fn teleport(x: f32, y: f32, s: bool, vx: f32, vy: f32, snap: bool) {
    let state = State::new();

    match state.items().player(0) {
        Some(player) => {
            log::debug!("Teleporting to relative {}, {}, {}", x, y, s);
            player.teleport(x, y, s, vx, vy, snap);
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

pub unsafe fn get_entity_at(mut x: f32, mut y: f32, s: bool, r: f32, mask: u32) -> u32 {
    let state = State::new();
    if s {
        let (rx, ry) = state.click_position(x, y);
        x = rx; y = ry;
    }
    log::debug!("Items at {:?}:", (x, y));
    match state.items().player(0) {
        Some(player) => {
            let mut found = Vec::new();
            for item in state.layer(player.layer()).items() {
                let (ix, iy) = item.position_self();
                let flags = item._type().search_flags;
                let distance = ((x-ix).powi(2) + (y-iy).powi(2)).sqrt();
                if mask & flags > 0 && distance < r {
                    log::debug!(
                        "Item: {}, type: {:x}, position: {:?}, distance: {}, {:x?}",
                        item.unique_id(),
                        item._type().search_flags,
                        item.position_self(),
                        distance,
                        item.ptr()
                    );
                    found.push((item.unique_id(), distance));
                }
            }
            if !found.is_empty() {
                found.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap());
                return found.first().unwrap().0
            }
            0
        }
        None => 0
    }
}

pub unsafe fn move_entity(id: u32, x: f32, y: f32, s: bool, vx: f32, vy: f32, snap: bool) {
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            for item in state.layer(player.layer()).items() {
                if item.unique_id() == id {
                    item.teleport(x, y, s, vx, vy, snap);
                }
            }
        }
        None => {}
    }
}

pub unsafe fn get_entity_flags(id: u32) -> u32 {
    if id == 0 {
        return 0
    }
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            for item in state.layer(player.layer()).items() {
                if item.unique_id() == id {
                    return item.flags()
                }
            }
        }
        None => {}
    }
    0
}

pub unsafe fn set_entity_flags(id: u32, flags: u32) {
    if id == 0 {
        return
    }
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            for item in state.layer(player.layer()).items() {
                if item.unique_id() == id {
                    item.set_flags(flags);
                }
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
