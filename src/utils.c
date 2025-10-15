#include "utils.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define STR_BUF_LEN 1024

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

#ifndef MIN_LOG_LEVEL
#define MIN_LOG_LEVEL U_LOG_INFO
#endif

void log_msg(LOG_LEVEL level, const char *fmt, ...)
{
    if(level < MIN_LOG_LEVEL) return;

    va_list args;
    va_start(args, fmt);

    char buffer[256] = {0};

    switch (level)
    {
    case U_LOG_DEBUG:
        strcpy(buffer, "DEBUG: ");
        break;
    case U_LOG_INFO:
        strcpy(buffer, "INFO: ");
        break;
    case U_LOG_WARN:
        strcpy(buffer, "WARNING: ");
        break;
    case U_LOG_ERROR:
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

char *str_lower(const char *text)
{
    static char buffer[STR_BUF_LEN] = { 0 };
    memset(buffer, 0, STR_BUF_LEN);

    if (text != NULL)
    {
        for (int i = 0; (i < STR_BUF_LEN- 1) && (text[i] != '\0'); i++)
        {
            if ((text[i] >= 'A') && (text[i] <= 'Z')) buffer[i] = text[i] + 32;
            else buffer[i] = text[i];
        }
    } else return NULL;

    return buffer;
}
