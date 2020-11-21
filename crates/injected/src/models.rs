use crate::{
    memory::*,
    search::{decode_imm, decode_pc, find_inst},
};
use byteorder::*;
use hex_literal::*;
use std::sync::Once;

static INIT: Once = Once::new();
pub struct State {
    location: usize,
    off_items: usize,
    off_layers: usize,
    ptr_load_item: usize,
    addr_damage: usize,
    addr_insta: usize,
}

macro_rules! entity {
    ($T: ident) => {
        #[derive(Debug, Copy, Clone)]
        pub struct $T {
            entity: Entity,
        }
        impl From<usize> for $T {
            fn from(pointer: usize) -> Self {
                let entity = Entity { pointer };
                Self { entity }
            }
        }

        impl From<Entity> for $T {
            fn from(entity: Entity) -> Self {
                Self { entity }
            }
        }

        impl From<$T> for Entity {
            fn from(object: $T) -> Self {
                Self {
                    pointer: object.entity.pointer,
                }
            }
        }
    };
}

fn decode_call(memory: &Memory, off: usize) -> usize {
    off.wrapping_add(LE::read_i32(&memory.exe()[off + 1..]) as usize) + 5
}

fn get_load_item(memory: &Memory) -> usize {
    let after_bundle = memory.after_bundle;
    let exe = memory.exe();
    let needle = &hex!("BA B9 01 00 00");
    let off = find_inst(exe, needle, after_bundle);
    let off = find_inst(exe, needle, off + 5);
    let off = find_inst(exe, needle, off + 5);
    let off = find_inst(exe, &hex!("E8"), off + 5);

    memory.at_exe(decode_call(memory, off))
}

fn get_load_item_over(memory: &Memory) -> usize {
    let off = find_inst(
        memory.exe(),
        &hex!("BA B5 00 00 00 C6 44 "),
        memory.after_bundle,
    );
    let off = find_inst(memory.exe(), &hex!("E8"), off + 5);
    memory.at_exe(decode_call(memory, off))
}

static mut CAMERA_OFF: usize = 0;
fn get_camera(memory: &Memory) -> usize {
    unsafe {
        INIT.call_once(|| {
            let off = find_inst(
                memory.exe(),
                &hex!("C7 87 CC 00 00 00 00 00"),
                memory.after_bundle,
            );
            let off = find_inst(memory.exe(), &hex!("F3 0F 11 05"), off) + 1;
            CAMERA_OFF = memory.at_exe(decode_pc(memory.exe(), off));
        });
        CAMERA_OFF
    }
}

impl State {
    pub fn new(memory: &Memory) -> State {
        // Global state pointer
        let exe = memory.exe();
        let start = memory.after_bundle;
        let location = memory.at_exe(decode_pc(
            exe,
            find_inst(exe, &hex!("83 78 0C 05 0F 85"), start) - 15,
        ));
        // The offset of items field
        let off_items = decode_imm(exe, find_inst(exe, &hex!("33 D2 8B 41 28 01"), start) - 7);
        let off_layers = decode_imm(
            exe,
            find_inst(exe, &hex!("C6 80 58 44 06 00 01 "), start) - 7,
        );
        let off_send = find_inst(exe, &hex!("45 8D 41 50"), start) + 12;
        write_mem_prot(memory.at_exe(off_send), &hex!("31 C0 31 D2 90"), true);
        let addr_damage = memory.at_exe(find_inst(exe, &hex!("89 5C 24 20 55 56 57 41 56 41 57 48 81 EC 90 00 00 00"), start)) - 1;
        let addr_insta = memory.at_exe(find_inst(exe, &hex!("57 41 54 48 83 EC 58 48 89 B4 24 80 00 00 00 44 0F B6 E2 4C 89 7C 24 50"), start)) - 1;
        State {
            location,
            off_items,
            off_layers,
            ptr_load_item: get_load_item(&memory),
            addr_damage,
            addr_insta,
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

    pub fn godmode(&self, g: bool) {
        log::debug!("God mode: {:?}", g);
        if g {
            write_mem_prot(self.addr_damage, &hex!("C3"), true);
            write_mem_prot(self.addr_insta, &hex!("C3"), true);
        } else {
            write_mem_prot(self.addr_damage, &hex!("48"), true);
            write_mem_prot(self.addr_insta, &hex!("40"), true);
        }
    }
}

pub struct Layer {
    pointer: usize,
    ptr_load_item: usize,
    state: usize,
}

impl Layer {
    pub unsafe fn spawn_entity(&self, id: usize, x: f32, y: f32, s: bool) -> Entity {
        let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
            std::mem::transmute(self.ptr_load_item);
        if !s {
            let addr: usize = load_item(self.pointer, id, x, y);
            log::info!("Spawned {:x?}", addr);
            Entity { pointer: addr }
        } else {
            let memory = Memory::new();
            let cx = read_f32(get_camera(&memory));
            let cy = read_f32(get_camera(&memory) + 4);
            let rx = cx + 10.0 * x;
            let ry = cy + 5.625 * y;
            let addr: usize = load_item(self.pointer, id, rx, ry);
            log::info!("Spawned {:x?}", addr);
            Entity { pointer: addr }
        }
    }

    pub unsafe fn spawn_entity_over(&self, id: usize, overlay: Entity, x: f32, y: f32) -> Entity {
        let load_item_over: extern "C" fn(usize, usize, usize, f32, f32, bool) -> usize =
            std::mem::transmute(get_load_item_over(&Memory::new()));

        let pointer = load_item_over(self.pointer, id, overlay.pointer, x, y, true);
        Entity { pointer }
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
            _ => Some(Player::from(pointer)),
        }
    }
}

trait ItemDefinition {
    fn unique_id(&self) -> u32;
}

#[derive(Copy, Clone, Debug)]
pub struct Entity {
    pointer: usize,
}

impl Entity {
    pub fn unique_id(&self) -> u32 {
        read_u32(self.pointer + 0x38)
    }
    pub fn overlay(&self) -> Option<Entity> {
        match read_u64(self.pointer + 0x10) {
            0 => None,
            pointer => Some(Entity { pointer }),
        }
    }
    pub fn position(&self) -> (f32, f32) {
        // Return the resolved position
        // self.overlay() exists if player is riding something / etc
        let (x, y) = self.position_self();
        log::debug!("Item #{}: Position is {}, {}", self.unique_id(), x, y);
        match self.overlay() {
            Some(new) => {
                let (_x, _y) = new.position();
                (x + _x, y + _y)
            }
            None => (x, y),
        }
    }

    pub fn teleport(&self, dx: f32, dy: f32, s: bool) {
        // e.g. topmost == turkey if riding turkey. player has relative coordinate to turkey.
        let topmost = self.topmost();
        let (mut x, mut y) = topmost.position();
        if self.pointer != 0 {
            if !s {
                // player relative coordinates
                x += dx;
                y += dy;
                let px = topmost.pointer + 0x40;
                let py = topmost.pointer + 0x44;
                log::info!("Teleporting to {}, {}", x, y);
                write_mem(px, &x.to_le_bytes());
                write_mem(py, &y.to_le_bytes());
            } else {
                // screen coordinates -1..1
                log::info!("Teleporting to screen {}, {}", x, y);
                let px = topmost.pointer + 0x40;
                let py = topmost.pointer + 0x44;
                unsafe {
                    let memory = Memory::new();
                    let cx = read_f32(get_camera(&memory));
                    let cy = read_f32(get_camera(&memory) + 4);
                    log::info!("Camera is at {}, {}", cx, cy);
                    x = (cx + 10.0 * dx).round();
                    y = (cy + 5.625 * dy).round();
                    log::info!("Teleporting to {}, {}", x, y);
                    write_mem(px, &x.to_le_bytes());
                    write_mem(py, &y.to_le_bytes());
                }
            }
        }
    }

    fn topmost(&self) -> Entity {
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

    pub fn position_self(&self) -> (f32, f32) {
        (read_f32(self.pointer + 0x40), read_f32(self.pointer + 0x44))
    }

    pub fn layer(&self) -> u8 {
        read_u8(self.pointer + 0x98)
    }
}

entity!(Mount);

impl Mount {
    pub unsafe fn carry(&self, rider: Entity) {
        let carry: unsafe extern "C" fn(usize, usize) = std::mem::transmute(get_carry());
        log::info!("{}", read_u8(rider.pointer + 0x10e));
        write_mem(rider.pointer + 0x10e, &[11u8]);
        carry(self.entity.pointer, rider.pointer)
    }

    pub unsafe fn tame(&self, value: bool) {
        let pointer = self.entity.pointer;
        write_mem(pointer + 0x149, &[value as u8]);
        let flags = read_u64(pointer + 0x30);
        write_mem(pointer + 0x30, &(flags | 0x20000).to_le_bytes());
    }
}

unsafe fn get_carry() -> usize {
    let memory = Memory::new();
    let off = find_inst(
        memory.exe(),
        &hex!("BA E1 00 00 00 49 8B CD "),
        memory.after_bundle,
    );
    let off = find_inst(memory.exe(), &hex!("E8"), off + 1);
    let off = find_inst(memory.exe(), &hex!("E8"), off + 1);

    memory.at_exe(decode_call(&memory, off))
}

entity!(Player);

impl Player {
    pub fn position(&self) -> (f32, f32) {
        self.entity.position()
    }

    pub fn layer(&self) -> u8 {
        self.entity.layer()
    }

    pub fn teleport(&self, dx: f32, dy: f32, s: bool) {
        self.entity.teleport(dx, dy, s)
    }
}
