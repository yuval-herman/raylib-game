#include "../shared.h"

LibData tinycthreads_data = {
    .name = TINYCTHREADS_LIB,
    .src_files = (const char *[1]){
        "external/tinycthreads/tinycthread.c",
    },
    .src_files_count = 1,

    .header_files = (const char *[1]){
        "external/tinycthreads/tinycthread.h",
    },
    .header_files_count = 1,

    .custom_flags = (const char *[1]){"-O3"},
    .custom_flags_count = 1,
};
