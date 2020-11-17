use byteorder::*;
use std::ptr;
use winapi::um::libloaderapi::GetModuleHandleA;

use crate::search::find_after_bundle;

pub struct Memory {
    pub exe_ptr: usize,
    pub after_bundle: usize,
}

macro_rules! round_up {
    ($i: expr, $div: expr) => {
        (($i + $div - 1) / $div) * $div
    };
}

unsafe fn memory_view<'a>(addr: usize) -> &'a mut [u8] {
    std::slice::from_raw_parts_mut(std::mem::transmute(addr), isize::MAX as _)
}

pub fn write_mem_prot(addr: usize, payload: &[u8], prot: bool) {
    let mut old_protect: u32 = 0;
    unsafe {
        let page = addr & !0xFFF;
        let size = round_up!((addr + payload.len() - page), 0x1000);
        let page: *mut std::ffi::c_void = std::mem::transmute(page);
        if prot {
            winapi::um::memoryapi::VirtualProtect(
                page,
                size,
                winapi::um::winnt::PAGE_EXECUTE_READWRITE,
                &mut old_protect,
            );
        }
        &mut memory_view(addr)[..payload.len()].copy_from_slice(payload);
        if prot {
            winapi::um::memoryapi::VirtualProtect(page, size, old_protect, &mut old_protect);
        }
    }
}

pub fn write_mem(addr: usize, payload: &[u8]) {
    write_mem_prot(addr, payload, false);
}

pub fn read_u8(addr: usize) -> u8 {
    unsafe {
        let mem = std::slice::from_raw_parts_mut(std::mem::transmute(1 as usize), addr);
        mem[addr - 1]
    }
}

pub fn read_u64(addr: usize) -> usize {
    unsafe { LE::read_u64(memory_view(addr)) as _ }
}

pub fn read_u32(addr: usize) -> u32 {
    unsafe { LE::read_u32(memory_view(addr)) }
}

pub fn read_f32(addr: usize) -> f32 {
    unsafe { LE::read_f32(memory_view(addr)) }
}

impl Memory {
    pub unsafe fn new() -> Memory {
        let spel2_ptr = GetModuleHandleA("Spel2.exe\0".as_ptr() as *const i8);
        let exe = memory_view(std::mem::transmute(spel2_ptr));

        // Skipping bundle for faster memory search
        let after_bundle = find_after_bundle(exe);

        Memory {
            exe_ptr: spel2_ptr as usize,
            after_bundle,
        }
    }

    pub fn at_exe(&self, offset: usize) -> usize {
        self.exe_ptr + offset
    }

    pub fn exe(&self) -> &mut [u8] {
        unsafe { memory_view(std::mem::transmute(self.exe_ptr)) }
    }
}
