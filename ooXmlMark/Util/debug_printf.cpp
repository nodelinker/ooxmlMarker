/*
 * Copyright (c) 2020 bangoc
 *
 */

#include <memory>
#include <string>
#include <stdexcept>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug_printf.h"
#include "Logger.h"

const char *debug_printf_version = "1.0.0";
const char *debug_printf_level_config_name = "DEBUG_PRINTF_LEVEL";
const char *debug_printf_level_names[] = { "ALL", "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "NONE" };


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


std::string string_format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy_s(&formatted[0], n, fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf_s(&formatted[0], n, _TRUNCATE, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}

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


	//va_list args;
	//char buffer[16];
	//buffer[strftime(buffer, sizeof(buffer), "%H:%M:%S", &lt)] = '\0';
	//fprintf(stdout, "%s %-8s %s:%d: ", buffer, debug_printf_level_names[level],
	//	filename, line);

	//va_start(args, fmt);
	//vfprintf(stdout, fmt, args);
	//va_end(args);
	//fprintf(stdout, "\n");
	//fflush(stdout);
}
