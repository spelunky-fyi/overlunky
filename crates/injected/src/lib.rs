#![feature(asm)]
mod critical_section;
mod db;
mod memory;
mod models;
mod search;
mod ui;

#[allow(dead_code)]
mod example;

use backtrace::Backtrace;
use critical_section::CriticalSectionManager;
use db::list_entities;
use log::log_enabled;
use memory::Memory;
use models::{State, API};
use std::thread;
use std::time;

use winapi::um::{
    consoleapi::SetConsoleCtrlHandler,
    processthreadsapi::ExitThread,
    wincon::AttachConsole,
    wincon::FreeConsole,
    wincon::{CTRL_BREAK_EVENT, CTRL_CLOSE_EVENT, CTRL_C_EVENT},
};

use chrono::prelude::*;
use std::fs::OpenOptions;
use std::io::Write;

#[no_mangle]
pub extern "C" fn DllMain(_: *const u8, _reason: u32, _: *const u8) -> u32 {
    1 // TRUE
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
    let env = env_logger::Env::default().filter_or(env_logger::DEFAULT_FILTER_ENV, "info");
    env_logger::Builder::from_env(env).init();
    if log_enabled!(log::Level::Debug) {
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

#[no_mangle]
unsafe extern "C" fn run(pid: u32) {
    attach_stdout(pid);
    set_panic_hook();
    match OpenOptions::new()
        .write(true)
        .append(true)
        .open("spelunky.log")
    {
        Ok(mut file) => {
            let local: DateTime<Local> = Local::now();
            match file.write_all(
                &local
                    .format("%X: Overlunky loaded\n")
                    .to_string()
                    .into_bytes(),
            ) {
                Ok(o) => log::debug!("{:?}", o),
                Err(e) => log::error!("{:?}", e),
            }
        }
        Err(err) => {
            log::error!("{:?}", err);
        }
    }
    log::debug!("Game injected! Press Ctrl+C to detach this window from the process.");
    let memory = Memory::get();
    let state = State::new();
    loop {
        let entities = list_entities();
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
    if log_enabled!(log::Level::Debug) {
        let c = CriticalSectionManager::new();
        loop {
            log::debug!("Enter entity #IDs to spawn, one per line >");
            let mut buffer = String::new();
            std::io::stdin().read_line(&mut buffer).unwrap();
            if let Ok(id) = buffer.trim().parse::<usize>() {
                // This is RAII-style implementation for suspending the main thread, for preventing race conditions.
                let mut _lock = c.lock();

                match state.items().player(0) {
                    None => {
                        log::error!("Player not initialized yet. Select a character first!");
                    }
                    Some(player) => {
                        let (x, y) = player.position();
                        let layer = player.layer();
                        state.layer(layer).spawn_entity(id, x, y, false, 0.0, 0.0);
                    }
                }
            }
        }
    }
}
