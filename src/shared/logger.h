#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>

#define COMMON_FORMATTER(name, format, ...)        \
    fmt::print("[" name "] " format, __VA_ARGS__);

#define PANIC(format, ...)                              \
    do {                                                \
        COMMON_FORMATTER("error", format, __VA_ARGS__); \
        std::exit(-1);                                  \
    } while(false)

#define DEBUG(format, ...)                              \
    do {                                                \
        COMMON_FORMATTER("debug", format, __VA_ARGS__); \
    } while (false)
#define INFO(format, ...)                              \
    do {                                               \
        COMMON_FORMATTER("info", format, __VA_ARGS__); \
    } while (false)
