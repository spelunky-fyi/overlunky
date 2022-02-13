add_library(shared INTERFACE)
target_include_directories(shared INTERFACE .)
target_link_libraries(shared INTERFACE
        fmt
        overlunky_version)
target_precompile_headers(shared INTERFACE
        "logger.h"
        <fmt/format.h>
        <fmt/ranges.h>
        <string>
        <string_view>
        <array>
        <vector>
        <deque>
        <map>
        <unordered_map>
        <set>
        <unordered_set>
        <fstream>
        <regex>
        <filesystem>
        <variant>
        <functional>
        <algorithm>
        <codecvt>
        <iomanip>
        <locale>
        <mutex>)

target_sources(shared INTERFACE logger.h olfont.h)
