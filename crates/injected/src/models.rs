use crate::{
    memory::*,
    search::{decode_imm, decode_pc, find_inst},
};
use byteorder::*;
use cached::proc_macro::cached;
use hex_literal::*;

pub struct API {
    api: *const usize,
    swap_chain_off: usize,
}

fn get_api(memory: &Memory) -> usize {
    let exe = memory.exe();
    let after_bundle = memory.after_bundle;
    let off = find_inst(exe, &hex!("48 8B 50 10 48 89"), after_bundle) - 5;
    let off = off.wrapping_add(LE::read_i32(&exe[off + 1..]) as usize) + 5;

    memory.at_exe(decode_pc(exe, off + 6))
}

impl API {
    pub unsafe fn new(memory: &Memory) -> API {
        let api: *const usize = std::mem::transmute(get_api(&memory));
        let off = decode_imm(
            memory.exe(),
            find_inst(
                memory.exe(),
                &hex!("BA F0 FF FF FF 41 B8 00 00 00 90"),
                memory.after_bundle,
            ) + 17,
        );

        API {
            api,
            swap_chain_off: off,
        }
    }

    unsafe fn renderer(&self) -> usize {
        read_u64(*self.api + 0x10)
    }

    pub unsafe fn swap_chain(&self) -> usize {
        read_u64(self.renderer() + self.swap_chain_off)
    }
}

pub struct State {
    location: usize,
    off_items: usize,
    off_layers: usize,
    addr_damage: usize,
    addr_insta: usize,
    addr_zoom: usize,
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

fn decode_call(off: usize) -> usize {
    let memory = Memory::get();
    off.wrapping_add(LE::read_i32(&memory.exe()[off + 1..]) as usize) + 5
}

#[cached]
fn get_load_item() -> usize {
    let memory = Memory::get();
    let after_bundle = memory.after_bundle;
    let exe = memory.exe();
    let needle = &hex!("BA B9 01 00 00");
    let off = find_inst(exe, needle, after_bundle);
    let off = find_inst(exe, needle, off + 5);
    let off = find_inst(exe, needle, off + 5);
    let off = find_inst(exe, &hex!("E8"), off + 5);

    memory.at_exe(decode_call(off))
}

#[cached]
fn get_load_item_over() -> usize {
    let memory = Memory::get();
    let off = find_inst(
        memory.exe(),
        &hex!("BA B5 00 00 00 C6 44 "),
        memory.after_bundle,
    );
    let off = find_inst(memory.exe(), &hex!("E8"), off + 5);
    memory.at_exe(decode_call(off))
}

#[cached]
fn get_camera() -> usize {
    let memory = Memory::get();
    let off = find_inst(
        memory.exe(),
        &hex!("C7 87 CC 00 00 00 00 00"),
        memory.after_bundle,
    );
    let off = find_inst(memory.exe(), &hex!("F3 0F 11 05"), off) + 1;
    memory.at_exe(decode_pc(memory.exe(), off))
}

#[cached]
fn get_zoom() -> usize {
    let memory = Memory::get();
    let off = find_inst(
        memory.exe(),
        &hex!("E8 89 49 08 00 48 8B 48 10 C7 81 E8 04 08 00"),
        memory.after_bundle,
    );
    memory.at_exe(off + 15)
}

impl State {
    pub fn new() -> State {
        let memory = Memory::get();
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
        write_mem_prot(memory.at_exe(off_send), &hex!("31 C0 31 D2 90 90"), true);
        let addr_damage = memory.at_exe(find_inst(
            exe,
            &hex!("89 5C 24 20 55 56 57 41 56 41 57 48 81 EC 90 00 00 00"),
            start,
        )) - 1;
        let addr_insta = memory.at_exe(find_inst(
            exe,
            &hex!("57 41 54 48 83 EC 58 48 89 B4 24 80 00 00 00 44 0F B6 E2 4C 89 7C 24 50"),
            start,
        )) - 1;
        let mut addr_zoom = start;
        for _ in 0..3 {
            addr_zoom = find_inst(exe, &hex!("48 8B 48 10 C7 81"), addr_zoom + 1);
        }
        let addr_zoom = memory.at_exe(addr_zoom) + 10;
        State {
            location,
            off_items,
            off_layers,
            addr_damage,
            addr_insta,
            addr_zoom,
        }
    }

    fn ptr(&self) -> usize {
        read_u64(self.location)
    }

    pub fn layer(&self, index: u8) -> Layer {
        Layer {
            pointer: read_u64(self.ptr() + self.off_layers + index as usize * 8),
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

    pub fn zoom(&self, level: f32) {
        log::debug!("Zoom level: {:?}", level);
        write_mem_prot(self.addr_zoom, &level.to_le_bytes(), true);
    }
}

pub struct Layer {
    pointer: usize,
    state: usize,
}

impl Layer {
    pub unsafe fn spawn_entity(&self, id: usize, x: f32, y: f32, s: bool) -> Entity {
        let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
            std::mem::transmute(get_load_item());
        if !s {
            let addr: usize = load_item(self.pointer, id, x, y);
            log::debug!("Spawned {:x?}", addr);
            Entity { pointer: addr }
        } else {
            let cx = read_f32(get_camera());
            let cy = read_f32(get_camera() + 4);
            let cz = read_f32(get_zoom());
            let rx = cx + 0.74 * cz * x;
            let ry = cy + 0.41625 * cz * y;
            let addr: usize = load_item(self.pointer, id, rx, ry);
            log::debug!("Spawned {:x?}", addr);
            Entity { pointer: addr }
        }
    }

    pub unsafe fn spawn_entity_over(&self, id: usize, overlay: Entity, x: f32, y: f32) -> Entity {
        let load_item_over: extern "C" fn(usize, usize, usize, f32, f32, bool) -> usize =
            std::mem::transmute(get_load_item_over());

        let pointer = load_item_over(self.pointer, id, overlay.pointer, x, y, true);
        Entity { pointer }
    }

    pub unsafe fn spawn_door(&self, x: f32, y: f32, w: u8, l: u8, f: u8, t: u8) {
        let screen: u8 = read_u8(self.state + 0x10);
        let entity = match screen {
            11 => {
                log::debug!("In camp, spawning starting exit");
                self.spawn_entity(25, x, y, false)
            }
            12 => {
                log::debug!("In game, spawning regular exit");
                self.spawn_entity(23, x, y, false)
            }
            _ => return,
        };
        log::info!("Spawned door {:x?}", entity.pointer);
        Door::from(entity).set_target(w, l, f, t);
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
                log::debug!("Teleporting to {}, {}", x, y);
                write_mem(px, &x.to_le_bytes());
                write_mem(py, &y.to_le_bytes());
            } else {
                // screen coordinates -1..1
                log::debug!("Teleporting to screen {}, {}", x, y);
                let px = topmost.pointer + 0x40;
                let py = topmost.pointer + 0x44;
                let cx = read_f32(get_camera());
                let cy = read_f32(get_camera() + 4);
                let cz = read_f32(get_zoom());
                log::debug!("Camera is at {}, {}", cx, cy);
                x = cx + 0.74 * cz * dx;
                y = cy + 0.41625 * cz * dy;
                log::debug!("Teleporting to {}, {}", x, y);
                write_mem(px, &x.to_le_bytes());
                write_mem(py, &y.to_le_bytes());
            }
            // reset downforce
            let off = read_u64(topmost.pointer + 0x80);
            let off = read_u64(off + 0x128);
            let off = off + 0x104;
            let df: f32 = 0.0;
            write_mem(off, &df.to_le_bytes());
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
    let memory = Memory::get();
    let off = find_inst(
        memory.exe(),
        &hex!("BA E1 00 00 00 49 8B CD "),
        memory.after_bundle,
    );
    let off = find_inst(memory.exe(), &hex!("E8"), off + 1);
    let off = find_inst(memory.exe(), &hex!("E8"), off + 1);

    memory.at_exe(decode_call(off))
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

    pub fn status(&self) -> PlayerStatus {
        PlayerStatus {
            pointer: read_u64(self.entity.pointer + 0x138),
        }
    }
}
pub struct PlayerStatus {
    pointer: usize,
}

impl PlayerStatus {
    pub fn bomb(&self) -> u8 {
        read_u8(self.pointer + 4)
    }
    pub fn rope(&self) -> u8 {
        read_u8(self.pointer + 5)
    }
    pub fn set_bomb(&self, value: u8) {
        write_mem(self.pointer + 4, &[value])
    }
    pub fn set_rope(&self, value: u8) {
        write_mem(self.pointer + 5, &[value])
    }
}

entity!(Door);

impl Door {
    fn set_target(&self, w: u8, l: u8, f: u8, t: u8) {
        let array: [u8; 5] = [1, l, f, w, t];
        log::debug!("Making door go to {:?}", array);
        write_mem(self.entity.pointer + 0xc1, &array);
    }
}
