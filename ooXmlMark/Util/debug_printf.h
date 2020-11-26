/**
 * Copyright (c) 2020 bangoc
 *
 */

#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H

#include <stdarg.h>
#include <stdio.h>

extern const char *debug_printf_version;
extern const char *debug_printf_level_config_name;
extern const char *debug_printf_level_names[];

enum { DEBUG_ALL, DEBUG_TRACE, DEBUG_DEBUG, DEBUG_INFO, DEBUG_WARNING, DEBUG_ERROR, DEBUG_NONE };


#define printfTrace(...) printfLog(DEBUG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define printfDebug(...) printfLog(DEBUG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define printfInfo(...) printfLog(DEBUG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define printfWarning(...) printfLog(DEBUG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define printfError(...) printfLog(DEBUG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

void printfLog(int level, const char *file, int line, const char *fmt, ...);

#endif // DEBUG_PRINTF_H
