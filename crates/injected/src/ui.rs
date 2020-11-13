#[cxx::bridge]
pub mod ffi {
    extern "C++" {
        include!("cxx/ui.hpp");
        fn create_box(_: Vec<String>);
        fn init_hooks(_: usize);
    }
}
