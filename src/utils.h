#ifndef _UTILS_H
#define _UTILS_H

#include <assert.h>
#include <stdbool.h>
#include "raylib.h"

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

typedef enum
{
    U_LOG_DEBUG=0,
    U_LOG_INFO,
    U_LOG_WARN,
    U_LOG_ERROR
} LOG_LEVEL;

// Number of CPU cores on the machine
int n_cores(void);

#ifdef NDEBUG
#define log_debug(fmt, ...) ((void)0)
#else
// Using `log_debug` instead of `log_msg(LOG_DEBUG, msg)` allows this calls to be remove at release.
// If a debug log should stay in release, use `log_msg(LOG_DEBUG, msg)` instead
#define log_debug(fmt, ...) log_msg(U_LOG_DEBUG, fmt, ##__VA_ARGS__)
#endif

/* Log to stderr with timestamp, level, and printf-style formatting */
void log_msg(LOG_LEVEL level, const char *fmt, ...);

// Lowers the case for a string. Uses internal buffer of `STR_BUF_LEN`
char *str_lower(const char *text);

bool isOneKeyPressed(const KeyboardKey *keys, int keys_length);

bool isOneKeyDown(const KeyboardKey *keys, int keys_length);

bool isOneKeyReleased(const KeyboardKey *keys, int keys_length);
#endif // _UTILS_H
