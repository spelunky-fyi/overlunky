use crate::{
    memory::*,
    search::{decode_imm, decode_pc, find_inst},
};
use byteorder::*;
use hex_literal::*;
#[derive(Copy, Clone)]
pub struct State {
    location: usize,
    off_items: usize,
    off_layers: usize,
    ptr_load_item: usize,
}

fn get_load_item(memory: &Memory) -> usize {
    let Memory {
        exe, after_bundle, ..
    } = memory;
    let needle = &hex!("BA 88 02 00 00");
    let off = find_inst(exe, needle, *after_bundle);
    let off: usize = find_inst(exe, needle, off + 5) + 8;

    memory.at_exe(off.wrapping_add(LE::read_i32(&exe[off + 1..]) as usize) + 5)
}

impl State {
    pub fn new(memory: &Memory) -> State {
        let Memory {
            exe,
            after_bundle: start,
            ..
        } = memory;
        // Global state pointer
        let location = memory.at_exe(decode_pc(
            exe,
            find_inst(exe, &hex!("83 78 0C 05 0F 85"), *start) - 15,
        ));
        // The offset of items field
        let off_items =
            decode_imm(exe, find_inst(exe, &hex!("33 D2 8B 41 28 01"), *start) - 7) as usize;
        let off_layers = decode_imm(
            exe,
            find_inst(exe, &hex!("C6 80 58 44 06 00 01 "), *start) - 7,
        ) as usize;
        let off_send = find_inst(exe, &hex!("45 8D 41 50"), *start) + 12;
        write_mem(memory.at_exe(off_send), &hex!("31 C0 31 D2 90"));
        State {
            location,
            off_items,
            off_layers,
            ptr_load_item: get_load_item(&memory),
        }
    }

    fn ptr(&self) -> usize {
        read_u64(self.location)
    }

    pub fn layer(&self, index: u8) -> Layer {
        Layer {
            pointer: read_u64(self.ptr() + self.off_layers + index as usize * 8),
            ptr_load_item: self.ptr_load_item,
            state: self.ptr(),
        }
    }

    pub fn items(&self) -> Items {
        let pointer = read_u64(self.ptr() + self.off_items);
        Items { pointer }
    }
}

pub struct Layer {
    pointer: usize,
    ptr_load_item: usize,
    state: usize,
}

impl Layer {
    pub unsafe fn spawn_entity(&self, id: usize, x: f32, y: f32) {
        let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
            std::mem::transmute(self.ptr_load_item);
        let addr: usize = load_item(self.pointer, id, x, y);
        log::info!("Spawned {:x?}", addr);
    }

    pub unsafe fn spawn_door(&self, x: f32, y: f32, w: u8, l: u8, f: u8, t: u8) {
        let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
            std::mem::transmute(self.ptr_load_item);
        let screen: u8 = read_u8(self.state + 0x10);
        let mut addr: usize = 0;
        if screen == 11 {
            log::info!("In camp, spawning starting exit");
            addr = load_item(self.pointer, 25, x, y);
        } else if screen == 12 {
            log::info!("In game, spawning regular exit");
            addr = load_item(self.pointer, 23, x, y);
        }
        if addr != 0 {
            log::info!("Spawned door {:x?}", addr);
            let array: [u8; 5] = [1, l, f, w, t];
            log::info!("Making door go to {:?}", array);
            write_mem(addr + 0xc1, &array);
        }
    }
}

pub struct Items {
    pointer: usize,
}

impl Items {
    pub fn player(&self, index: usize) -> Option<Player> {
        let pointer = read_u64(self.pointer + 8 + index * 8);
        match pointer {
            0 => None,
            _ => Some(Player { pointer }),
        }
    }
}

trait ItemDefinition {
    fn unique_id(&self) -> u32;
}

#[derive(Copy, Clone)]
pub struct Player {
    pointer: usize,
}

impl Player {
    pub fn unique_id(&self) -> u32 {
        read_u32(self.pointer + 0x38)
    }
    pub fn overlay(&self) -> Option<Player> {
        match read_u64(self.pointer + 0x10) {
            0 => None,
            pointer => Some(Player { pointer }),
        }
    }
    pub fn position(&self) -> (f32, f32) {
        // Return the resolved position
        // self.overlay() exists if player is riding something / etc
        let (x, y) = self.position_self();
        log::info!("Item #{}: Position is {}, {}", self.unique_id(), x, y);
        match self.overlay() {
            Some(new) => {
                let (_x, _y) = new.position();
                (x + _x, y + _y)
            }
            None => (x, y),
        }
    }

    pub fn teleport(&self, dx: f32, dy: f32) {
        let topmost = self.topmost();
        let (x, y) = topmost.position();
        log::info!("Teleporting to {}, {}", x + dx, y + dy);
        write_mem(topmost.pointer + 0x40, &(x + dx).to_le_bytes());
        write_mem(topmost.pointer + 0x44, &(y + dy).to_le_bytes());
    }

    fn topmost(&self) -> Player {
        let mut topmost = self.clone();
        loop {
            match topmost.overlay() {
                Some(new) => {
                    topmost.pointer = new.pointer;
                }
                None => break,
            }
        }
        topmost
    }

    fn position_self(&self) -> (f32, f32) {
        (read_f32(self.pointer + 0x40), read_f32(self.pointer + 0x44))
    }

    pub fn layer(&self) -> u8 {
        read_u8(self.pointer + 0x98)
    }
}
