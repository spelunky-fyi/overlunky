mod critical_section;
mod db;
mod memory;
mod models;
mod search;
mod ui;

use backtrace::Backtrace;
use byteorder::*;
use critical_section::CriticalSectionManager;
use db::list_entities;
use hex_literal::*;
use memory::{read_u64, Memory};
use models::State;
use search::{decode_imm, decode_pc, find_inst};

use winapi::um::{
    consoleapi::SetConsoleCtrlHandler,
    processthreadsapi::ExitThread,
    wincon::AttachConsole,
    wincon::FreeConsole,
    wincon::{CTRL_BREAK_EVENT, CTRL_CLOSE_EVENT, CTRL_C_EVENT},
};

#[no_mangle]
pub extern "C" fn DllMain(_: *const u8, _reason: u32, _: *const u8) -> u32 {
    1 // TRUE
}

fn get_api(memory: &Memory) -> usize {
    let Memory {
        exe, after_bundle, ..
    } = memory;
    let off = find_inst(exe, &hex!("48 8B 50 10 48 89"), *after_bundle) - 5;
    let off = off.wrapping_add(LE::read_i32(&exe[off + 1..]) as usize) + 5;

    memory.at_exe(decode_pc(exe, off + 6))
}

unsafe extern "system" fn ctrl_handler(ctrl_type: u32) -> i32 {
    match ctrl_type {
        CTRL_C_EVENT | CTRL_BREAK_EVENT | CTRL_CLOSE_EVENT => {
            log::info!("Console detached, you can now close this window.");
            FreeConsole();
            1
        }
        _ => 0,
    }
}

unsafe fn attach_stdout(pid: u32) {
    AttachConsole(pid);
    SetConsoleCtrlHandler(Some(ctrl_handler), 1);
    env_logger::Builder::new()
        .filter(None, log::LevelFilter::Debug)
        .init();
}

unsafe fn set_panic_hook() {
    std::panic::set_hook(Box::new(|panic_info| {
        if let Some(s) = panic_info.payload().downcast_ref::<&str>() {
            log::error!("panic: {}", s);
        }
        log::error!("{:?}", Backtrace::new());
        FreeConsole();
        ExitThread(0);
    }));
}

struct API {
    api: *const usize,
    swap_chain_off: usize,
}

impl API {
    unsafe fn new(memory: &Memory) -> API {
        let api: *const usize = std::mem::transmute(get_api(&memory));
        let off = decode_imm(
            memory.exe,
            find_inst(
                memory.exe,
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

    unsafe fn swap_chain(&self) -> usize {
        read_u64(self.renderer() + self.swap_chain_off)
    }
}

#[no_mangle]
unsafe extern "C" fn main(handle: u32) {
    attach_stdout(handle);
    set_panic_hook();
    log::info!("Game injected! Press Ctrl+C to detach this window from the process.");

    let memory = Memory::new();
    let state = State::new(&memory);
    let entities = list_entities(&memory);
    ui::create_box(&entities);

    let api = API::new(&memory);

    if let Err(err) = ui::ffi::init_hooks(api.swap_chain()) {
        log::error!("{}", err);
        return;
    }
    let c = CriticalSectionManager::new();
    loop {
        println!("Enter entity #IDs to spawn, one per line >");
        let mut buffer = String::new();
        std::io::stdin().read_line(&mut buffer).unwrap();
        let item = buffer.trim().parse::<usize>().unwrap_or(0);

        if item == 0 {
            continue;
        }

        {
            // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
            let mut _lock = c.lock();

            match state.items().player(0) {
                None => {
                    log::error!("Player not initialized yet. Select a character first!");
                }
                Some(player) => {
                    let (x, y) = player.position();
                    let layer = player.layer();
                    state.layer(layer).spawn_entity(item, x, y);
                }
            }
        }
    }
}
