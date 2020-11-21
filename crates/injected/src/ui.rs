use crate::{db::ffi::EntityItem, memory::Memory, models::State};

#[cxx::bridge]
pub mod ffi {
    extern "Rust" {
        unsafe fn spawn_entity(id: usize, x: f32, y: f32, s: bool);
        unsafe fn spawn_door(x: f32, y: f32, w: u8, l: u8, f: u8, t: u8);
        unsafe fn teleport(x: f32, y: f32, s: bool);
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
pub unsafe fn spawn_entity(id: usize, x: f32, y: f32, s: bool) {
    let memory = Memory::new();
    let state = State::new(&memory);

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            if !s {
                log::info!("Spawning {} on {}, {}", id, x + _x, y + _y);
                state
                    .layer(player.layer())
                    .spawn_entity(id, x + _x, y + _y, s);
            } else {
                log::info!("Spawning {} on screen {}, {}", id, x, y);
                state.layer(player.layer()).spawn_entity(id, x, y, s);
            }
        }
        None => {}
    }
}

pub unsafe fn spawn_door(x: f32, y: f32, l: u8, w: u8, f: u8, t: u8) {
    let memory = Memory::new();
    let state = State::new(&memory);

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            log::info!("Spawning door on {}, {}", x + _x, y + _y);
            state
                .layer(player.layer())
                .spawn_door(x + _x, y + _y, l, w, f, t);
        }
        None => {}
    }
}

pub unsafe fn teleport(x: f32, y: f32, s: bool) {
    let memory = Memory::new();
    let state = State::new(&memory);

    match state.items().player(0) {
        Some(player) => {
            log::info!("Teleporting to relative {}, {}, {}", x, y, s);
            player.teleport(x, y, s);
        }
        None => {}
    }
}
