/*
 * Copyright (c) 2020 bangoc
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug_printf.h"

const char *debug_printf_version = "1.0.0";
const char *debug_printf_level_config_name = "DEBUG_PRINTF_LEVEL";
const char *debug_printf_level_names[] = {"ALL",     "TRACE", "DEBUG", "INFO",
                                          "WARNING", "ERROR", "NONE"};

//void printfLog(int level, const char *file, int line, const char *fmt, ...) {
//
//  const char *filename = (strrchr(file, '/') ? strrchr(file, '/') + 1 : file);
//
//  time_t t = time(NULL);
//  struct tm *lt = localtime(&t);
//  va_list args;
//  char buffer[16];
//  buffer[strftime(buffer, sizeof(buffer), "%H:%M:%S", lt)] = '\0';
//  fprintf(stdout, "%s %-8s %s:%d: ", buffer, debug_printf_level_names[level],
//          filename, line);
//
//  va_start(args, fmt);
//  vfprintf(stdout, fmt, args);
//  va_end(args);
//  fprintf(stdout, "\n");
//  fflush(stdout);
//}


void printfLog(int level, const char *file, int line, const char *fmt, ...) {

	const char *filename = (strrchr(file, '/') ? strrchr(file, '/') + 1 : file);

	struct tm lt;
	time_t t = time(NULL);
	errno_t err = localtime_s(&lt, &t);
	if (err)
	{
		printf("Invalid argument to _localtime_s.");
		return;
	}


	va_list args;
	char buffer[16];
	buffer[strftime(buffer, sizeof(buffer), "%H:%M:%S", &lt)] = '\0';
	fprintf(stdout, "%s %-8s %s:%d: ", buffer, debug_printf_level_names[level],
		filename, line);

	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fprintf(stdout, "\n");
	fflush(stdout);
}
