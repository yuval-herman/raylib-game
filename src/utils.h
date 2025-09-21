#ifndef _UTILS_H
#define _UTILS_H

typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LOG_LEVEL;

// Number of CPU cores on the machine
int n_cores(void);
/* Log to stderr with timestamp, level, and printf-style formatting */
void log_msg(LOG_LEVEL level, const char *fmt, ...);

#endif // _UTILS_H