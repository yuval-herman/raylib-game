#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

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