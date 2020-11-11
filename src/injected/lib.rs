mod critical_section;
mod models;
mod search;

use backtrace::Backtrace;
use byteorder::*;
use critical_section::CriticalSectionManager;
use hex_literal::*;
use models::{Memory, State};
use search::{decode_pc, find_after_bundle, find_inst};
use std::ptr;
use winapi::um::{
    consoleapi::SetConsoleCtrlHandler, libloaderapi::LoadLibraryA, processthreadsapi::ExitThread,
    wincon::AttachConsole,
};

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

fn get_api(exe: &[u8], start: usize) -> usize {
    let off = find_inst(exe, &hex!("48 8B 50 10 48 89"), start) - 5;
    let off = off.wrapping_add(LE::read_i32(&exe[off + 1..]) as usize) + 5;

    decode_pc(exe, off + 6)
}

unsafe fn attach_stdout(pid: u32) {
    AttachConsole(pid);
    SetConsoleCtrlHandler(None, 1);
    env_logger::Builder::new()
        .filter(None, log::LevelFilter::Debug)
        .init();
}

unsafe fn set_panic_hook() {
    std::panic::set_hook(Box::new(|info| {
        log::error!("{:?}", Backtrace::new());
        ExitThread(0);
    }));
}

#[no_mangle]
unsafe extern "C" fn main(handle: u32) {
    attach_stdout(handle);
    set_panic_hook();
    log::info!("Hello from injected library!");

    let spel2_ptr = LoadLibraryA("Spel2.exe\0".as_ptr() as *const i8);
    let exe = memory_view(spel2_ptr as *mut u8);
    let mem = memory_view(ptr::null_mut());

    // Skipping bundle for faster memory search
    let after_bundle = find_after_bundle(exe);

    let memory = Memory { mem, exe };
    let state = State::new(&memory, after_bundle);

    let api: *const usize = std::mem::transmute(get_api(exe, after_bundle) + spel2_ptr as usize);
    log::info!("API(): {:x}", *api);

    let load_item: extern "C" fn(usize, usize, f32, f32) -> usize =
        std::mem::transmute(get_load_item(exe, after_bundle) + spel2_ptr as usize);

    let c = CriticalSectionManager::new();
    {
        // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
        let mut _lock = c.lock();

        loop {
            println!("Select entity to spawn >");
            let mut buffer = String::new();
            std::io::stdin().read_line(&mut buffer).unwrap();
            let item = buffer.trim().parse::<usize>().unwrap_or(0);

            if item == 0 {
                break;
            }

            match state.items().player(0) {
                None => {
                    log::error!("Player not initialized yet. Select a character first!");
                }
                Some(player) => {
                    let (x, y) = player.position();
                    let layer = player.layer();
                    log::debug!("Player X, Y: {}, {}", x, y);
                    load_item(state.layer(layer), item, x, y);
                }
            }
        }
    }
}
