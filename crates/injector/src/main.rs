mod injector;

use injector::*;
use std::{thread, time};

fn get_dll_path() -> std::path::PathBuf {
    std::env::current_exe()
        .unwrap()
        .parent()
        .unwrap()
        .join("injected.dll")
}

fn main() {
    env_logger::Builder::new()
        .filter(None, log::LevelFilter::Debug)
        .init();

    let path = get_dll_path();

    if !path.exists() {
        log::error!("DLL not found! {}", path.to_str().unwrap());
        return;
    }

    let (_, temp_path) = tempfile::Builder::new()
        .suffix(".dll")
        .tempfile()
        .unwrap()
        .keep()
        .unwrap();

    if std::fs::copy(path, temp_path.clone()).is_err() {
        log::error!("Error copying DLL into temporary path!");
        return;
    }

    unsafe {
        log::info!("Searching for Spel2.exe process...");
        let mut started: bool = true;
        let proc = loop {
            if let Some(proc) = find_process("Spel2.exe") {
                break proc;
            }
            started = false;
            thread::sleep(time::Duration::from_millis(1000));
        };
        log::info!("Found Spel2.exe PID: {}", proc.pid);
        if !started {
            log::info!("Waiting a while for the game to load...");
            thread::sleep(time::Duration::from_millis(3000));
        }
        inject_dll(&proc, temp_path.to_str().unwrap());
        call(
            &proc,
            find_function(&proc, temp_path.to_str().unwrap(), "main"),
            std::mem::transmute(std::process::id() as usize),
        );
    }
}
