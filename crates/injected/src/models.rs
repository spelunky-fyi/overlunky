use crate::search::{decode_imm, decode_pc, find_after_bundle, find_inst};
use byteorder::*;
use hex_literal::*;
use winapi::um::libloaderapi::LoadLibraryA;

pub struct Memory<'a> {
    pub mem: &'a [u8],
    pub exe: &'a [u8],
    pub exe_ptr: usize,
    pub after_bundle: usize,
}

unsafe fn memory_view<'a>(addr: *mut u8) -> &'a mut [u8] {
    std::slice::from_raw_parts_mut(addr, usize::MAX)
}

impl<'a> Memory<'a> {
    pub unsafe fn new() -> Memory<'a> {
        let spel2_ptr = LoadLibraryA("Spel2.exe\0".as_ptr() as *const i8);
        let exe = memory_view(spel2_ptr as *mut u8);
        let mem = memory_view(std::ptr::null_mut());

        // Skipping bundle for faster memory search
        let after_bundle = find_after_bundle(exe);

        Memory {
            mem,
            exe,
            exe_ptr: spel2_ptr as usize,
            after_bundle,
        }
    }

    pub fn r64(&self, addr: usize) -> usize {
        LE::read_u64(&self.mem[addr..]) as usize
    }

    pub fn write(&self, addr: usize, payload: &[u8], size: usize) {
        let mut old_protect: u32 = 0;
        unsafe {
            winapi::um::memoryapi::VirtualProtect(
                std::mem::transmute(addr),
                size,
                winapi::um::winnt::PAGE_EXECUTE_READWRITE,
                &mut old_protect,
            );
            &mut memory_view(std::ptr::null_mut())[addr..addr + payload.len()].copy_from_slice(payload);
            winapi::um::memoryapi::VirtualProtect(
                std::mem::transmute(addr),
                size,
                old_protect,
                &mut old_protect,
            );
        }
    }

    pub fn f32(&self, addr: usize) -> f32 {
        LE::read_f32(&self.mem[addr..])
    }

    pub fn at_exe(&self, offset: usize) -> usize {
        self.exe_ptr + offset
    }
}

pub struct State<'a> {
    memory: &'a Memory<'a>,
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

impl<'a> State<'a> {
    pub fn new(memory: &'a Memory) -> State<'a> {
        let start = memory.after_bundle;
        // Global state pointer
        let location = decode_pc(
            memory.exe,
            find_inst(memory.exe, &hex!("83 78 0C 05 0F 85"), start) - 15,
        );
        // The offset of items field
        let off_items = decode_imm(
            memory.exe,
            find_inst(memory.exe, &hex!("33 D2 8B 41 28 01"), start) - 7,
        ) as usize;
        let off_layers = decode_imm(
            memory.exe,
            find_inst(memory.exe, &hex!("C6 80 58 44 06 00 01 "), start) - 7,
        ) as usize;
        let off_send = find_inst(memory.exe, &hex!("45 8D 41 50"), start) + 12;
        memory.write(memory.at_exe(off_send), &hex!("31 C0 31 D2 90"), 0x1000);
        State {
            memory,
            location,
            off_items,
            off_layers,
            ptr_load_item: get_load_item(&memory),
        }
    }

    fn ptr(&self) -> usize {
        LE::read_u64(&self.memory.exe[self.location..]) as usize
    }

    pub fn layer(&self, index: u8) -> Layer {
        Layer {
            pointer: self
                .memory
                .r64(self.ptr() + self.off_layers + index as usize * 8),
            ptr_load_item: self.ptr_load_item,
        }
    }

    pub fn items(&self) -> Items {
        let pointer = self.memory.r64(self.ptr() + self.off_items);
        Items {
            memory: self.memory,
            pointer,
        }
    }
}

pub struct Layer {
    pointer: usize,
    ptr_load_item: usize,
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
        let addr: usize = load_item(self.pointer, 23, x, y);
        log::info!("Spawned door {:x?}", addr);
        let array: [u8; 5] = [1, l, f, w, t];
        log::info!("Making a door to {:x?}", array);
        &mut memory_view(std::ptr::null_mut())[addr+0xc1..addr + 0xc6].copy_from_slice(&array);
    }
}

pub struct Items<'a> {
    memory: &'a Memory<'a>,
    pointer: usize,
}

impl<'a> Items<'a> {
    pub fn player(&self, index: usize) -> Option<Player> {
        let pointer = self.memory.r64(self.pointer + 8 + index * 8);
        match pointer {
            0 => None,
            _ => Some(Player {
                memory: self.memory,
                pointer,
            }),
        }
    }
}

pub struct Player<'a> {
    memory: &'a Memory<'a>,
    pointer: usize,
}

impl<'a> Player<'a> {
    pub fn position(&self) -> (f32, f32) {
        // "overlay" exists if player is riding something / etc
        let (mut x, mut y) = (0.0, 0.0);
        let mut overlay = self.pointer;
        loop {
            let (_x, _y) = (
                self.memory.f32(overlay + 0x40),
                self.memory.f32(overlay + 0x44),
            );
            x += _x;
            y += _y;
            overlay = self.memory.r64(overlay + 0x10);
            if overlay == 0 {
                break;
            }
        }
        log::info!("Position is {}, {}", x, y);
        (x, y)
    }

    pub fn teleport(&self, dx: f32, dy: f32) {
        let (mut x, mut y) = self.position();
        if self.pointer != 0 {
            x += dx;
            y += dy;
            let px = self.pointer + 0x40;
            let py = self.pointer + 0x44;
            log::info!("Teleporting to {}, {}", x, y);
            unsafe {
                &mut memory_view(std::ptr::null_mut())[px..px + 4].copy_from_slice(&x.to_le_bytes());
                &mut memory_view(std::ptr::null_mut())[py..py + 4].copy_from_slice(&y.to_le_bytes());
            }
        }
    }

    pub fn layer(&self) -> u8 {
        self.memory.mem[self.pointer + 0x98]
    }
}
