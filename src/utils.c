#include "utils.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

int n_cores(void)
{
#if defined(ENABLE_THREADS) && ENABLE_THREADS == true
#ifdef _WIN32
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);
    return siSysInfo.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
#else
    return 1;
#endif
}

void log_msg(LOG_LEVEL level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[256] = {0};

    switch (level)
    {
    case LOG_DEBUG:
        strcpy(buffer, "DEBUG: ");
        break;
    case LOG_INFO:
        strcpy(buffer, "INFO: ");
        break;
    case LOG_WARN:
        strcpy(buffer, "WARNING: ");
        break;
    case LOG_ERROR:
        strcpy(buffer, "ERROR: ");
        break;
    default:
        break;
    }

    unsigned int textSize = (unsigned int)strlen(fmt);
    memcpy(buffer + strlen(buffer), fmt, (textSize < (256 - 12)) ? textSize : (256 - 12));
    strcat(buffer, "\n");
    vfprintf(stderr, buffer, args);
    va_end(args);
}
