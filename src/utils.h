#ifndef _UTILS_H
#define _UTILS_H

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LOG_LEVEL;

// Number of CPU cores on the machine
int n_cores(void);

#ifdef NDEBUG
#define log_debug(fmt, ...) ((void)0)
#else
// Using `log_debug` instead of `log_msg(LOG_DEBUG, msg)` allows this calls to be remove at release.
// If a debug log should stay in release, use `log_msg(LOG_DEBUG, msg)` instead
#define log_debug(fmt, ...) log_msg(LOG_DEBUG, fmt, ##__VA_ARGS__)
#endif

/* Log to stderr with timestamp, level, and printf-style formatting */
void log_msg(LOG_LEVEL level, const char *fmt, ...);

#endif // _UTILS_H