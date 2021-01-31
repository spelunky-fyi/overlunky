#pragma once
#include <cstdarg>
#include <cstdio>
#include <stdlib.h>

#define COMMON_FORMATTER(name)            \
    va_list args;                   \
    va_start(args, format);         \
    fputs("[" name "] ", stderr);            \
    vfprintf(stderr, format, args); \
    fputs("\n", stderr);            \
    va_end(args)

[[noreturn]] static void PANIC(const char *format, ...)
{
    COMMON_FORMATTER("error");
    exit(-1);
}

static void DEBUG(const char *format, ...)
{
    COMMON_FORMATTER("debug");
}

static void INFO(const char *format, ...)
{
    COMMON_FORMATTER("info");
}

