#[cxx::bridge]
pub mod ffi {
    #[derive(Debug)]
    struct AnimationMap {
        padding: [u8; 0x40],
    }
    #[derive(Debug)]
    pub struct Rect {
        masks: i32,
        up_minus_down: f32,
        side: f32,
        up_plus_down: f32,
        field_10: u8,
        field_11: u8,
        field_12: u16,
    }
    #[derive(Debug)]
    pub struct EntityDB {
        create_func: usize,
        destroy_func: usize,
        field_10: i32,
        /* Entity id (ENT_...) */
        id: i32,
        search_flags: u32,
        width: f32,
        height: f32,
        field_24: u8,
        field_25: u8,
        field_26: i16,
        rect_collision: Rect,
        field_3C: i32,
        field_40: i32,
        field_44: i32,
        field_48: i64,
        field_50: i32,
        friction: f32,
        elasticity: f32,
        weight: f32,
        field_60: u8,
        acceleration: f32,
        max_speed: f32,
        sprint_factor: f32,
        jump: f32,

        /* ??? */
        _a: f32,
        _b: f32,
        _c: f32,
        _d: f32,

        texture: i32,
        technique: i32,
        tile_x: i32,
        tile_y: i32,
        damage: u8,
        life: u8,
        field_96: u8,
        field_97: u8,
        field_98: u8,
        description: i32,
        field_a0: i32,
        field_a4: i32,
        field_a8: f32,
        field_AC: i32,
        animations: AnimationMap,
        attachOffsetX: f32,
        attachOffsetY: f32,
        init: u8,
    }

    pub struct EntityItem {
        pub name: String,
        pub id: u16,
    }

    unsafe extern "C++" {
        include!("cxx/entity.hpp");
        fn list_entities(ptr: usize) -> Vec<EntityItem>;
    }
}

use crate::{
    memory::{read_u64, Memory},
    search::{decode_pc, find_inst},
};
use cached::proc_macro::cached;
use hex_literal::hex;

#[cached]
fn entities_ptr() -> usize {
    let mem = Memory::get();
    mem.at_exe(decode_pc(
        mem.exe(),
        find_inst(
            mem.exe(),
            &hex!("48 B8 02 55 A7 74 52 9D 51 43"),
            mem.after_bundle,
        ) - 7,
    ))
}

pub unsafe fn list_entities() -> Vec<ffi::EntityItem> {
    let entity_ptr = entities_ptr();
    ffi::list_entities(read_u64(entity_ptr))
}
