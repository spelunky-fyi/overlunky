use crate::{
    critical_section::get_main_thread,
    db::ffi::EntityDB,
    memory::*,
    search::{decode_imm, decode_pc, find_inst},
};
use byteorder::*;
use cached::proc_macro::cached;
use hex_literal::*;
use ntapi::ntpsapi::{NtQueryInformationThread, ThreadBasicInformation, THREAD_BASIC_INFORMATION};

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
    let mut addr_zoom = memory.after_bundle;
    for _ in 0..3 {
        addr_zoom = find_inst(memory.exe(), &hex!("48 8B 48 10 C7 81"), addr_zoom + 1);
        log::debug!("Zoom: {:x?}", addr_zoom);
    }
    memory.at_exe(addr_zoom) + 10
}

#[cached]
fn heap_base() -> usize {
    let main = get_main_thread();
    let mut tib: THREAD_BASIC_INFORMATION = unsafe { std::mem::zeroed() };

    unsafe {
        NtQueryInformationThread(
            main,
            ThreadBasicInformation,
            std::mem::transmute(&mut tib),
            std::mem::size_of::<THREAD_BASIC_INFORMATION>() as u32,
            std::ptr::null_mut(),
        );

        let result = (*tib.TebBaseAddress).ThreadLocalStoragePointer;
        read_u64(read_u64(std::mem::transmute(result)) + 0x130)
    }
}

fn function_start(off: usize) -> usize {
    let mut off = off;
    off &= !0xf;
    while read_u8(off - 1) != 0xcc {
        off -= 0x10;
    }
    off
}

#[cached]
fn get_damage() -> usize {
    // TODO: get vtable of character and calculate the offset
    let memory = Memory::get();
    let mut off = memory.after_bundle;
    loop {
        // sub rsp, 0x90; xor reg, reg
        off = find_inst(memory.exe(), &hex!("48 81 EC 90 00 00 00"), off + 1);
        if read_u8(memory.at_exe(off + 7)) == 0x33 || read_u8(memory.at_exe(off + 8)) == 0x33 {
            break;
        }
    }

    function_start(memory.at_exe(off))
}

#[cached]
fn get_insta() -> usize {
    let memory = Memory::get();
    let exe = memory.exe();
    let mut off = memory.after_bundle;
    off = find_inst(memory.exe(), &hex!("40 53 56 41 54 41 55 48 83 EC 58"), off + 1); // Spel2.exe+21E37920
    function_start(memory.at_exe(off))
}

impl State {
    pub fn new() -> State {
        let memory = Memory::get();
        // Global state pointer
        let exe = memory.exe();
        let start = memory.after_bundle;
        let location = memory.at_exe(decode_pc(
            exe,
            find_inst(
                exe,
                &hex!("48 8B 05"),
                find_inst(exe, &hex!("32 01 74"), start) - 0x100,
            ),
        ));
        // The offset of items field
        let off_items = decode_imm(exe, find_inst(exe, &hex!("3C 11 41 0F 95 C0"), start) + 11);
        let off_layers = decode_imm(
            exe,
            find_inst(exe, &hex!("C6 80 58 44 06 00 01 "), start) - 7,
        );
        let off_send = find_inst(exe, &hex!("45 8D 41 50"), start) + 12;
        write_mem_prot(memory.at_exe(off_send), &hex!("31 C0 31 D2 90 90"), true);
        let addr_damage = get_damage();
        let addr_insta = get_insta();
        log::debug!("damage: 0x{:x}, insta: 0x{:x}", addr_damage, addr_insta);
        let addr_zoom = get_zoom();
        State {
            location,
            off_items,
            off_layers,
            addr_damage,
            addr_insta,
            addr_zoom,
        }
    }

    pub fn ptr(&self) -> usize {
        let p = read_u64(self.location) + heap_base();
        log::debug!("State: {:x?}", p);
        p
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

    pub fn zoom(&self, mut level: f32) {
        let memory = Memory::get();

        // This technically sets camp zoom but not interactively :(
        //let mut addr_zoom = find_inst(memory.exe(), &hex!("C7 80 E8 04 08 00"), memory.after_bundle);
        //write_mem_prot(memory.at_exe(addr_zoom + 6), &level.to_le_bytes(), true);
        //addr_zoom = memory.after_bundle;

        let roomx_addr = self.ptr() + 0x44;
        let roomx: u8 = read_u8(roomx_addr);
        if level == 0.0 {
            level = match roomx {
                1 => 9.50,
                2 => 16.29,
                3 => 23.08,
                4 => 29.87,
                5 => 36.66,
                6 => 43.45,
                7 => 50.24,
                8 => 57.03,
                _ => 13.5
            }
        }
        let mut addr_zoom = memory.after_bundle;
        let mut real_addr;
        for i in 0..3 {
            addr_zoom = find_inst(memory.exe(), &hex!("48 8B 48 10 C7 81"), addr_zoom + 1);
            if i > 0 {
                real_addr = memory.at_exe(addr_zoom) + 10;
                write_mem_prot(real_addr, &level.to_le_bytes(), true);
            }
        }
    }

    pub fn click_position(&self, x: f32, y: f32) -> (f32, f32) {
        let screen: u8 = read_u8(self.ptr() + 0x10);
        let cz: f32 = match screen {
            11 => {
                13.5
            },
            13 => {
                13.5
            },
            14 => {
                13.5
            },
            _ => read_f32(get_zoom())
        };
        let cx = read_f32(get_camera());
        let cy = read_f32(get_camera() + 4);
        let rx = cx + 0.74 * cz * x;
        let ry = cy + 0.41625 * cz * y;
        (rx, ry)
    }
}

pub struct Layer {
    pointer: usize,
    state: usize,
}

impl Layer {
    pub unsafe fn spawn_entity(&self, id: usize, mut x: f32, mut y: f32, s: bool, vx: f32, vy: f32, snap: bool) -> Entity {
        let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
            std::mem::transmute(get_load_item());
        if !s {
            if snap {
                x = x.round();
                y = y.round();
            }
            let addr: usize = load_item(self.pointer, id, x, y);
            log::debug!("Spawned {:x?}", addr);
            Entity { pointer: addr }
        } else {
            let state = State::new();
            let (mut rx, mut ry) = state.click_position(x, y);
            if snap && vx.abs() + vy.abs() <= 0.04 {
                rx = rx.round();
                ry = ry.round();
            }
            let addr: usize = load_item(self.pointer, id, rx, ry);
            if vx.abs() + vy.abs() > 0.04 {
                write_mem(addr + 0x100, &vx.to_le_bytes());
                write_mem(addr + 0x104, &vy.to_le_bytes());
            }
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
                self.spawn_entity(25, x.round(), y.round(), false, 0.0, 0.0, true)
            }
            12 => {
                log::debug!("In game, spawning regular exit");
                self.spawn_entity(23, x.round(), y.round(), false, 0.0, 0.0, true)
            }
            _ => return,
        };
        log::info!("Spawned door {:x?}", entity.pointer);
        Door::from(entity).set_target(w, l, f, t);
    }

    pub unsafe fn items(&self) -> &'static [Entity] {
        let vector = self.pointer + 0x8;
        log::debug!(
            "{:x} {:} {:}",
            read_u64(vector),
            read_u32(vector + 0x14) as usize,
            read_u32(vector + 0x10) as usize,
        );
        std::slice::from_raw_parts(
            std::mem::transmute(read_u64(vector)),
            read_u32(vector + 0x14) as usize,
        )
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
    pub fn _type(&self) -> &EntityDB {
        unsafe { std::mem::transmute(read_u64(self.pointer + 0x8)) }
    }

    pub fn unique_id(&self) -> u32 {
        read_u32(self.pointer + 0x38)
    }

    pub fn flags(&self) -> u32 {
        read_u32(self.pointer + 0x30)
    }
    pub fn set_flags(&self, f: u32) {
        write_mem(self.pointer + 0x30, &f.to_le_bytes());
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

    pub fn teleport(&self, dx: f32, dy: f32, s: bool, vx: f32, vy: f32, snap: bool) {
        // e.g. topmost == turkey if riding turkey. player has relative coordinate to turkey.
        let topmost = self.topmost();
        let (mut x, mut y) = topmost.position();
        if !s {
            // player relative coordinates
            x += dx;
            y += dy;
            if snap {
                x = x.round();
                y = y.round();
            }
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
            let state = State::new();
            let (mut x, mut y) = state.click_position(dx, dy);
            if snap && vx.abs() + vy.abs() <= 0.04 {
                x = x.round();
                y = y.round();
            }
            log::debug!("Teleporting to {}, {}", x, y);
            write_mem(px, &x.to_le_bytes());
            write_mem(py, &y.to_le_bytes());
        }
        // wheeee (doesnt really work)
        write_mem(topmost.pointer + 0x100, &vx.to_le_bytes());
        write_mem(topmost.pointer + 0x104, &vy.to_le_bytes());
        return;
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

    pub fn ptr(&self) -> usize {
        self.pointer
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

    pub fn teleport(&self, dx: f32, dy: f32, s: bool, vx: f32, vy: f32, snap: bool) {
        self.entity.teleport(dx, dy, s, vx, vy, snap)
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
