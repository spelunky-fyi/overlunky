// Build C++ bridge
fn main() {
    let cxx = [
        "cxx/injector.cpp",
        "cxx/main.cpp",
    ];
    let headers = ["cxx/entity.hpp", "cxx/ui.hpp"];
    let bridges: [&str; 0] = [];

    cxx_build::bridges(&bridges) // returns a cc::Build
        .include(env!("CARGO_MANIFEST_DIR"))
        .files(&cxx)
        .flag_if_supported("-std=c++17")
        .flag_if_supported("/std:c++17")
        .flag_if_supported("/EHsc")
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
