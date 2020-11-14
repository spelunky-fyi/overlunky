use crate::{
    db::ffi::EntityItem,
    models::{Memory, State},
};

#[cxx::bridge]
pub mod ffi {
    extern "Rust" {
        fn spawn_entity(id: usize, x: f32, y: f32);
    }
    extern "C++" {
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
pub unsafe fn spawn_entity(id: usize, x: f32, y: f32) {
    let memory = Memory::new();
    let state = State::new(&memory);

    match state.items().player(0) {
        Some(player) => {
            let (_x, _y) = player.position();
            log::info!("Spawning {} on {}, {}", id, x + _x, y + _y);
            state.layer(player.layer()).spawn_entity(id, x + _x, y + _y);
        }
        None => {}
    }
}
