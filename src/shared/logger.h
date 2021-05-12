#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define COMMON_FORMATTER(name, format, ...)                 \
    try                                                     \
    {                                                       \
        fmt::print("[" name "] " format "\n", __VA_ARGS__); \
    }                                                       \
    catch (fmt::format_error & e)                           \
    {                                                       \
        puts("Formatting exception:" format);               \
        puts(__FILE__ " at " TOSTRING(__LINE__));           \
        puts(e.what());                                     \
    }                                                       \
    catch (...)                                             \
    {                                                       \
    }

#define PANIC(format, ...)                              \
    do                                                  \
    {                                                   \
        COMMON_FORMATTER("error", format, __VA_ARGS__); \
        std::exit(-1);                                  \
    } while (false)

#define DEBUG(format, ...)                              \
    do                                                  \
    {                                                   \
        COMMON_FORMATTER("debug", format, __VA_ARGS__); \
    } while (false)

#define INFO(format, ...)                              \
    do                                                 \
    {                                                  \
        COMMON_FORMATTER("info", format, __VA_ARGS__); \
    } while (false)
