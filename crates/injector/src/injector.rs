use std::{ffi, mem, ptr};
use sysinfo::*;
use winapi::shared::minwindef::LPVOID;
use winapi::um::errhandlingapi::GetLastError;
use winapi::um::libloaderapi::{GetProcAddress, LoadLibraryA};
use winapi::um::memoryapi::{VirtualAllocEx, VirtualQueryEx, WriteProcessMemory};
use winapi::um::minwinbase::LPTHREAD_START_ROUTINE;
use winapi::um::processthreadsapi::{CreateRemoteThread, OpenProcess};
use winapi::um::psapi::GetModuleBaseNameA;
use winapi::um::synchapi::WaitForSingleObject;
use winapi::um::winbase::INFINITE;
use winapi::um::winnt::MEMORY_BASIC_INFORMATION;
use winapi::um::winnt::{HANDLE, MEM_COMMIT, PROCESS_ALL_ACCESS};

pub struct MemoryMap {
    addr: usize,
    name: String,
}

pub struct Process {
    handle: HANDLE,
    pub pid: Pid,
}

pub unsafe fn memory_map(proc: &Process) -> Vec<MemoryMap> {
    let mut result: Vec<MemoryMap> = vec![];
    let mut cur = 0;
    let mut mbf: MEMORY_BASIC_INFORMATION = mem::zeroed();
    let mut buffer = vec![0u8; 0x1000];

    while VirtualQueryEx(
        proc.handle,
        mem::transmute(cur),
        &mut mbf,
        mem::size_of::<MEMORY_BASIC_INFORMATION>(),
    ) != 0
    {
        let res = GetModuleBaseNameA(
            proc.handle,
            mem::transmute(cur),
            buffer.as_mut_ptr() as *mut i8,
            buffer.len() as u32,
        );
        if res != 0 {
            let name = std::str::from_utf8_unchecked(&buffer[0..res as usize]);
            result.push(MemoryMap {
                addr: cur,
                name: name.to_string(),
            })
        };
        cur += mbf.RegionSize;
    }
    result
}

pub unsafe fn find_base(proc: &Process, name: &str) -> Option<usize> {
    let map = memory_map(proc);
    let mut res = map
        .iter()
        .filter(|item| name.to_lowercase().contains(&item.name.to_lowercase()));

    Some(res.next()?.addr)
}

unsafe fn alloc(proc: &Process, size: usize) -> LPVOID {
    let res = VirtualAllocEx(
        proc.handle,
        ptr::null_mut(),
        (size + 0xFFF) & !0xFFF,
        MEM_COMMIT,
        0x40,
    );
    if res.is_null() {
        panic!(format!("Allocation failed: {:x}", GetLastError()))
    }
    log::debug!("Allocated memory: {:x}", res as usize);
    res
}

unsafe fn alloc_str(proc: &Process, str: &str) -> LPVOID {
    let addr = alloc(proc, str.len() + 1);
    write_mem(proc, addr, str);
    addr
}

unsafe fn write_mem(proc: &Process, addr: LPVOID, str: &str) {
    WriteProcessMemory(
        proc.handle,
        addr,
        str.as_ptr() as LPVOID,
        str.len(),
        ptr::null_mut(),
    );
}

macro_rules! to_cstr {
    ($l:expr) => {
        ffi::CString::new($l).unwrap()
    };
}

pub unsafe fn find_function(
    proc: &Process,
    library: &str,
    function: &str,
) -> LPTHREAD_START_ROUTINE {
    let library_name = to_cstr!(library);
    let function_name = to_cstr!(function);
    let library_ptr = LoadLibraryA(library_name.as_ptr());
    mem::transmute(
        (GetProcAddress(library_ptr, function_name.as_ptr()) as usize)
            .wrapping_sub(library_ptr as usize)
            .wrapping_add(find_base(proc, library).unwrap()),
    )
}

pub unsafe fn inject_dll(proc: &Process, name: &str) {
    let str = alloc_str(proc, name);
    log::debug!("Injecting DLL into process... {}", name);
    call(
        proc,
        find_function(proc, "kernel32.dll", "LoadLibraryA"),
        str,
    );
}

pub unsafe fn call(proc: &Process, addr: LPTHREAD_START_ROUTINE, args: LPVOID) {
    log::info!(
        "Calling: {:x}",
        std::mem::transmute::<LPTHREAD_START_ROUTINE, usize>(addr)
    );
    let handle = CreateRemoteThread(
        proc.handle,
        ptr::null_mut(),
        0,
        addr,
        args,
        0,
        ptr::null_mut(),
    );
    WaitForSingleObject(handle, INFINITE);
}

pub unsafe fn find_process(name: &str) -> Option<Process> {
    let mut system = sysinfo::System::new_all();
    log::debug!("Refreshing the process list...");
    system.refresh_processes();

    log::debug!(
        "Iterating through {} processes...",
        system.get_processes().len()
    );

    for (pid, proc_) in system.get_processes() {
        if proc_.name().to_lowercase() == name.to_lowercase() {
            let handle = OpenProcess(PROCESS_ALL_ACCESS, 0, *pid as u32);
            return Some(Process { handle, pid: *pid });
        }
    }

    None
}
