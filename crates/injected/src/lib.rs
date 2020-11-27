mod critical_section;
mod db;
mod memory;
mod models;
mod search;
mod ui;

use std::thread;
use std::time;

use backtrace::Backtrace;
use byteorder::*;
use critical_section::CriticalSectionManager;
use db::list_entities;
use hex_literal::*;
use memory::{read_u64, Memory};
use models::{Mount, State};
use search::{decode_imm, decode_pc, find_inst};

use winapi::um::{
    consoleapi::SetConsoleCtrlHandler,
    processthreadsapi::ExitThread,
    wincon::AttachConsole,
    wincon::FreeConsole,
    wincon::{CTRL_BREAK_EVENT, CTRL_CLOSE_EVENT, CTRL_C_EVENT},
};

#[macro_use] extern crate log;
use log::Level;

#[no_mangle]
pub extern "C" fn DllMain(_: *const u8, _reason: u32, _: *const u8) -> u32 {
    1 // TRUE
}

fn get_api(memory: &Memory) -> usize {
    let exe = memory.exe();
    let after_bundle = memory.after_bundle;
    let off = find_inst(exe, &hex!("48 8B 50 10 48 89"), after_bundle) - 5;
    let off = off.wrapping_add(LE::read_i32(&exe[off + 1..]) as usize) + 5;

    memory.at_exe(decode_pc(exe, off + 6))
}

unsafe extern "system" fn ctrl_handler(ctrl_type: u32) -> i32 {
    match ctrl_type {
        CTRL_C_EVENT | CTRL_BREAK_EVENT | CTRL_CLOSE_EVENT => {
            log::debug!("Console detached, you can now close this window.");
            FreeConsole();
            1
        }
        _ => 0,
    }
}

unsafe fn attach_stdout(pid: u32) {
    let env = env_logger::Env::default()
        .filter_or(env_logger::DEFAULT_FILTER_ENV, "info");
    env_logger::Builder::from_env(env).init();
    if log_enabled!(Level::Debug) {
        AttachConsole(pid);
        SetConsoleCtrlHandler(Some(ctrl_handler), 1);
    }
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

    unsafe fn swap_chain(&self) -> usize {
        read_u64(self.renderer() + self.swap_chain_off)
    }
}

unsafe fn mount_poc() {
    // Spawns caveman riding turkey
    if false {
        let state = State::new();
        let player = state.items().player(0).unwrap();
        let position = player.position();
        let layer = state.layer(player.layer());
        let turkey: Mount = layer
            .spawn_entity(884, position.0 + 1.0, position.1, false)
            .into();
        let caveman = layer.spawn_entity_over(225, turkey.into(), -0.05, 0.52);
        turkey.carry(caveman);
        turkey.tame(true);
        return;
    }
}

#[no_mangle]
unsafe extern "C" fn main(handle: u32) {
    attach_stdout(handle);
    set_panic_hook();
    log::debug!("Game injected! Press Ctrl+C to detach this window from the process.");

    let memory = Memory::get();
    let state = State::new();
    loop {
        let entities = list_entities(&memory);
        if entities.len() != 0 {
            ui::create_box(&entities);
            break;
        }
        thread::sleep(time::Duration::from_millis(100));
    }

    let api = API::new(&memory);

    if let Err(err) = ui::ffi::init_hooks(api.swap_chain()) {
        log::error!("{}", err);
        return;
    }
    if log_enabled!(Level::Debug) {
        let c = CriticalSectionManager::new();
        loop {
            log::debug!("Enter entity #IDs to spawn, one per line >");
            let mut buffer = String::new();
            std::io::stdin().read_line(&mut buffer).unwrap();
            if let Ok(item) = buffer.trim().parse::<usize>() {
                // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
                let mut _lock = c.lock();

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
                            state.layer(layer).spawn_entity(item, x, y, false);
                        }
                    }
                }
            }
        }
    }
}
