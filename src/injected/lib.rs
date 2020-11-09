mod critical_section;
mod models;
mod search;

use byteorder::*;
use critical_section::CriticalSectionManager;
use hex_literal::*;
use models::{Memory, State};
use search::{find_after_bundle, find_inst};
use std::ffi::CString;
use std::ptr;
use winapi::um::libloaderapi::LoadLibraryA;

#[no_mangle]
pub extern "C" fn DllMain(_: *const u8, _reason: u32, _: *const u8) -> u32 {
    1 // TRUE
}

unsafe fn memory_view<'a>(addr: *mut u8) -> &'a mut [u8] {
    std::slice::from_raw_parts_mut(addr, usize::MAX)
}

fn get_load_item(exe: &[u8], start: usize) -> usize {
    let needle = &hex!("BA 88 02 00 00");
    let off = find_inst(exe, needle, start);
    let off: usize = find_inst(exe, needle, off + 5) + 8;

    off.wrapping_add(LE::read_i32(&exe[off + 1..]) as usize) + 5
}

use winapi::um::wincon::AttachConsole;

unsafe fn attach_stdout(pid: u32) {
    AttachConsole(pid);
    env_logger::Builder::new()
        .filter(None, log::LevelFilter::Debug)
        .init();
}

#[no_mangle]
unsafe extern "C" fn main(pid: u32) {
    attach_stdout(pid);
    log::info!("Hello from injected library!");

    let spel2_name = CString::new("Spel2.exe").unwrap();
    let spel2_ptr = LoadLibraryA(spel2_name.as_ptr());
    let exe = memory_view(spel2_ptr as *mut u8);
    let mem = memory_view(ptr::null_mut());

    // Skipping bundle for faster memory search
    let after_bundle = find_after_bundle(exe);

    let memory = Memory { mem, exe };
    let state = State::new(&memory, after_bundle);

    let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
        std::mem::transmute(get_load_item(exe, after_bundle) + spel2_ptr as usize);

    let c = CriticalSectionManager::new();
    {
        // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
        let mut _lock = c.lock();

        match state.items().player(0) {
            None => {
                log::error!("Player not initialized yet. Select a character first!");
            }
            Some(player) => {
                let (x, y) = player.position();
                let layer = state.layer(0);
                log::debug!("Player X, Y: {}, {}", x, y);
                load_item(layer, 878, x, y);
            }
        }
    }
}
