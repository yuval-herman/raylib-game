#include "../shared.h"

LibData pcg_c_data = {
    .name = PCG_C_LIB,
    .src_files = (const char *[17]){
        "external/pcg-c/src/pcg-advance-8.c",
        "external/pcg-c/src/pcg-advance-16.c",
        "external/pcg-c/src/pcg-advance-32.c",
        "external/pcg-c/src/pcg-advance-64.c",
        "external/pcg-c/src/pcg-advance-128.c",
        "external/pcg-c/src/pcg-output-8.c",
        "external/pcg-c/src/pcg-output-16.c",
        "external/pcg-c/src/pcg-output-32.c",
        "external/pcg-c/src/pcg-output-64.c",
        "external/pcg-c/src/pcg-output-128.c",
        "external/pcg-c/src/pcg-rngs-8.c",
        "external/pcg-c/src/pcg-rngs-16.c",
        "external/pcg-c/src/pcg-rngs-32.c",
        "external/pcg-c/src/pcg-rngs-64.c",
        "external/pcg-c/src/pcg-rngs-128.c",
        "external/pcg-c/src/pcg-global-32.c",
        "external/pcg-c/src/pcg-global-64.c",
    },
    .src_files_count = 17,

    .header_files = (const char *[1]){
        "external/pcg-c/include/pcg_variants.h",
    },
    .header_files_count = 1,

    .include_dirs = (const char *[1]){"external/pcg-c/include"},
    .include_dirs_count = 1,

    .custom_flags = (const char *[1]){"-O3"},
    .custom_flags_count = 1,
};
