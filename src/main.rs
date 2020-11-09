mod injector;

use injector::*;

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
        match find_process("Spel2.exe") {
            None => println!("Cannot find process!"),
            Some(proc) => {
                log::info!("Found spelunky 2 PID: {}", proc.pid);
                inject_dll(&proc, temp_path.to_str().unwrap());
                call(
                    &proc,
                    find_function(temp_path.to_str().unwrap(), "main"),
                    std::ptr::null_mut(),
                );
            }
        }
    }
}
