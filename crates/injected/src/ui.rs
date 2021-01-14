use crate::{db::ffi::EntityItem, ui::ffi::EntityMemory, models::State};


#[cxx::bridge]
pub mod ffi {

    #[derive(Debug)]
    pub struct Color {
        pub r: f32,
        pub g: f32,
        pub b: f32,
        pub a: f32
    }

    #[derive(Debug)]
    pub struct Inventory {
        pub money: u32,
        pub bombs: u8,
        pub ropes: u8
    }

    #[derive(Debug)]
    pub struct EntityMemory {
        pub create_func: usize,
        pub destroy_func: usize,
        pub overlay: usize,
        pub some_items_func: usize,
        pub items_ptr: usize,
        pub items_size: u32,
        pub items_count: u32,
        pub flags: u32,
        pub more_flags: u32,
        pub uid: u32,
        pub animation: u32,
        pub x: f32,
        pub y: f32,
        pub w: f32,
        pub h: f32,
        pub f50: f32,
        pub f54: f32,
        pub color: Color,
        pub offsetx: f32,
        pub offsety: f32,
        pub hitboxx: f32,
        pub hitboxy: f32,
        pub duckmask: u32,
        pub i7c: i32,
        pub p80: usize,
        pub p88: usize,
        pub tilew: f32,
        pub tileh: f32,
        pub camera_layer: u8,
        pub b99: u8,
        pub b9a: u8,
        pub b9b: u8,
        pub i9c: u32,
        pub pa0: usize,
        pub ia8: i32,
        pub iac: i32,
        pub pb0: usize,
        pub ib8: i32,
        pub ibc: i32,
        pub anim_func: usize,
        pub ic8: i32,
        pub icc: i32,
        pub movex: f32,
        pub movey: f32,
        pub buttons: u32,
        pub stand_counter: u32,
        pub fe0: f32,
        pub ie4: i32,
        pub owner_uid: u32,
        pub last_owner_uid: u32,
        pub animation_func: usize,
        pub idle_counter: u32,
        pub standing_on_uid: u32,
        pub velocityx: f32,
        pub velocityy: f32,
        pub holding_uid: u32,
        pub state: u8,
        pub last_state: u8,
        pub move_state: u8,
        pub health: u8,
        pub i110: i32,
        pub some_state: u32,
        pub i118: i32,
        pub i11c: i32,
        pub i120: i32,
        pub i124: i32,
        pub inside: u32,
        pub timer: u32,
        pub has_backpack: u32,
        pub i134: i32,
        pub inventory_ptr: usize,
        pub p140: usize,
        pub i148: i32,
        pub i14c: i32,
        pub i150: usize,
        pub p158: usize,
        pub p160: usize,
        pub i168: i32,
        pub i16c: i32,
        pub jump_flags: u32,
        pub some_timer: u8,
        pub can_use: u8,
        pub b176: u8,
        pub b177: u8
    }

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
        unsafe fn set_hud_flags(flags: u8);
        unsafe fn get_hud_flags() -> u8;
        unsafe fn set_pause(pause: u8);
        unsafe fn player_status();
        unsafe fn get_entity_ptr(id: u32) -> usize;
        unsafe fn get_entity_type(id: u32) -> i32;
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
                let (ix, iy) = item.position();
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
                    found.push((item.unique_id(), distance, item._memory()));
                }
            }
            if !found.is_empty() {
                found.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap());
                let picked = found.first().unwrap();
                let entity = picked.2;
                log::debug!("{:#x?}", entity);
                return picked.0
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

pub unsafe fn get_hud_flags() -> u8 {
    let state = State::new();
    state.flags()
}

pub unsafe fn set_hud_flags(flags: u8) {
    let state = State::new();
    state.set_flags(flags);
}

pub unsafe fn set_pause(pause: u8) {
    let state = State::new();
    state.set_pause(pause);
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

pub unsafe fn get_entity_ptr(id: u32) -> usize {
    if id == 0 {
        return 0
    }
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            for item in state.layer(player.layer()).items() {
                if item.unique_id() == id {
                    return item.ptr()
                }
            }
        }
        None => {}
    }
    0
}

unsafe fn get_entity_type(id: u32) -> i32 {
    if id == 0 {
        return 0
    }
    let state = State::new();
    match state.items().player(0) {
        Some(player) => {
            for item in state.layer(player.layer()).items() {
                if item.unique_id() == id {
                    return item._type().id
                }
            }
        }
        None => {}
    }
    0
}