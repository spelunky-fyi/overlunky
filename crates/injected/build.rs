// Build C++ bridge
fn main() {
    let cxx = [
        "cxx/entity.cpp",
        "cxx/ui.cpp",
        "imgui/examples/imgui_impl_dx11.cpp",
        "imgui/examples/imgui_impl_win32.cpp",
        "imgui/imgui.cpp",
        "imgui/imgui_demo.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_widgets.cpp",
    ];
    let headers = ["cxx/entity.hpp", "cxx/ui.hpp"];
    let bridges = ["src/db.rs", "src/ui.rs"];

    cxx_build::bridges(&bridges) // returns a cc::Build
        .include(env!("CARGO_MANIFEST_DIR"))
        .include(env!("CARGO_MANIFEST_DIR").to_owned() + "/imgui")
        .files(&cxx)
        .flag_if_supported("-std=c++17")
        .compile("cxxbridge");

    for bridge in &bridges {
        println!("cargo:rerun-if-changed={}", bridge);
    }

    for file in &cxx {
        println!("cargo:rerun-if-changed={}", file);
    }

    for file in &headers {
        println!("cargo:rerun-if-changed={}", file);
    }
}
