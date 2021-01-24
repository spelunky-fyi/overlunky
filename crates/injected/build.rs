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
        "crates/injected/toml11tests/check.cpp",
        "crates/injected/toml11/tests\check_serialization.cpp",
        "crates/injected/toml11/tests\check_toml_test.cpp",
        "crates/injected/toml11/tests\test_comments.cpp",
        "crates/injected/toml11/tests/test_datetime.cpp",
        "crates/injected/toml11/tests\test_error_detection.cpp",
        "crates/injected/toml11/tests\test_expect.cpp",
        "crates/injected/toml11/tests\test_extended_conversions.cpp",
        "crates/injected/toml11/tests\test_find.cpp",
        "crates/injected/toml11/tests\test_find_or.cpp",
        "crates/injected/toml11/tests\test_format_error.cpp",
        "crates/injected/toml11/tests\test_get.cpp",
        "crates/injected/toml11/tests\test_get_or.cpp",
        "crates/injected/toml11/tests\test_lex_boolean.cpp",
        "crates/injected/toml11/tests\test_lex_datetime.cpp",
        "crates/injected/toml11/tests\test_lex_floating.cpp",
        "crates/injected/toml11/tests\test_lex_integer.cpp",
        "crates/injected/toml11/tests\test_lex_key_comment.cpp",
        "crates/injected/toml11/tests\test_lex_string.cpp",
        "crates/injected/toml11/tests\test_literals.cpp",
        "crates/injected/toml11/tests\test_multiple_translation_unit_1.cpp",
        "crates/injected/toml11/tests\test_multiple_translation_unit_2.cpp",
        "crates/injected/toml11/tests\test_parse_array.cpp",
        "crates/injected/toml11/tests\test_parse_boolean.cpp",
        "crates/injected/toml11/tests\test_parse_datetime.cpp",
        "crates\injected\toml11\tests\test_parse_file.cpp",
        "crates/injected/toml11/tests\test_parse_floating.cpp",
        "crates/injected/toml11/tests\test_parse_inline_table.cpp",
        "crates/injected/toml11/tests\test_parse_integer.cpp",
        "crates/injected/toml11/tests\test_parse_key.cpp",
        "crates/injected/toml11/tests\test_parse_string.cpp",
        "crates/injected/toml11/tests\test_parse_table.cpp",
        "crates/injected/toml11/tests\test_parse_table_key.cpp",
        "crates/injected/toml11/tests\test_parse_unicode.cpp",
        "crates/injected/toml11/tests\test_result.cpp",
        "crates/injected/toml11/tests\test_serialize_file.cpp",
        "crates/injected/toml11/tests\test_string.cpp",
        "crates/injected/toml11/tests\test_traits.cpp",
        "crates/injected/toml11/tests\test_utility.cpp",
        "crates/injected/toml11/tests\test_value.cpp",
        "crates/injected/toml11/tests\test_windows.cpp"
    ];
    let headers = ["cxx/entity.hpp", "cxx/ui.hpp"];
    let bridges = ["src/db.rs", "src/ui.rs"];

    cxx_build::bridges(&bridges) // returns a cc::Build
        .include(env!("CARGO_MANIFEST_DIR"))
        .include(env!("CARGO_MANIFEST_DIR").to_owned() + "/imgui")
        .files(&cxx)
        .flag_if_supported("-std=c++17")
        // ImGui uses XInput1_4.dll by default. These two variables prevents it.
        // https://github.com/ocornut/imgui/issues/2716
        .define("IMGUI_IMPL_WIN32_DISABLE_GAMEPAD", "1")
        .define("IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT", "1")
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
