// Use dummy lock for now.

pub struct CriticalSectionManager;
pub struct CriticalSection;

impl CriticalSectionManager {
    pub fn new() -> CriticalSectionManager {
        CriticalSectionManager {}
    }
    pub fn lock(&self) -> CriticalSection {
        CriticalSection{}
    }
}

/*
use winapi::um::processthreadsapi::OpenThread;
use winapi::um::tlhelp32::*;
use winapi::um::winnt::{HANDLE, THREAD_ALL_ACCESS};

pub struct CriticalSectionManager {
    main_thread: HANDLE,
}

impl CriticalSectionManager {
    pub fn new() -> CriticalSectionManager {
        let main_thread = 0 as HANDLE;
        // let pid = std::process::id();
        // unsafe {
        //     let snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        //     let mut entry = THREADENTRY32 {
        //         dwSize: std::mem::size_of::<THREADENTRY32>() as u32,
        //         cntUsage: 0,
        //         th32ThreadID: 0,
        //         th32OwnerProcessID: 0,
        //         tpBasePri: 0,
        //         tpDeltaPri: 0,
        //         dwFlags: 0,
        //     };
        //     let mut res = Thread32First(snapshot, &mut entry);
        //     while res != 0 {
        //         if entry.th32OwnerProcessID == pid {
        //             main_thread = OpenThread(THREAD_ALL_ACCESS, 0, entry.th32ThreadID);
        //             break;
        //         }
        //         res = Thread32Next(snapshot, &mut entry);
        //     }
        // }
        CriticalSectionManager { main_thread }
    }
    pub unsafe fn lock(&self) -> CriticalSection {
        // SuspendThread(self.main_thread);
        CriticalSection {
            thread: self.main_thread,
        }
    }
}

pub struct CriticalSection {
    thread: HANDLE,
}

impl Drop for CriticalSection {
    fn drop(&mut self) {
        // unsafe {
        //     message("resume!".to_string());
        //     ResumeThread(self.thread);
        // }
    }
}
*/
