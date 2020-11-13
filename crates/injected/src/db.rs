#[cxx::bridge]
mod ffi {
    struct EntityItem {
        pub name: String,
        pub id: u16,
    }

    extern "C++" {
        include!("cxx/entity.hpp");
        fn list_entities(ptr: usize) -> Vec<EntityItem>;
    }
}

use crate::{
    models::Memory,
    search::{decode_pc, find_inst},
};
use hex_literal::hex;

pub unsafe fn list_entities(mem: &Memory, start: usize) {
    let entity_ptr = mem.at_exe(decode_pc(
        mem.exe,
        find_inst(
            mem.exe,
            &hex!("48 8B 90 B0 81 03 00 48 B9 02 55 A7 74 52 9D 51"),
            start,
        ) - 7,
    ));

    let entities = ffi::list_entities(mem.r64(entity_ptr));
    for item in entities {
        log::info!("{:?}: {}", item.name, item.id);
    }
}
