#ifndef SHARED_H_HEADER
#define SHARED_H_HEADER

#include <stdbool.h>
#include <string.h>

#define NOB_WARN_DEPRECATED
#include "nob.h"

#define SRC_DIR "src/"
#define BUILD_DIR "build/"
#define INCLUDE_DIR BUILD_DIR "include/"

#ifdef _WIN32
#define OUTPUT_FILE BUILD_DIR "main.exe"
#define TESTS_OUTPUT_FILE BUILD_DIR "tests.exe"
#else
#define OUTPUT_FILE BUILD_DIR "main"
#define TESTS_OUTPUT_FILE BUILD_DIR "tests"
#endif

#define BOX2D_LIB "box2d"
#define RAYLIB_LIB "raylib"
#define PCG_C_LIB "pcg_random"
#define TINYCTHREADS_LIB "tinycthreads"
#define GENANN_LIB "genann"

#define lib_file_name(libname) "lib" libname ".a"

#define print_separator(level) nob_log(level, "==================================")

typedef struct LibData
{
    // Library name
    const char *name;

    // List of .c files
    const char **src_files;
    const size_t src_files_count;
    // List of .h files
    const char **header_files;
    const size_t header_files_count;

    // Optional: used for -I when compiling. Set to null if unrequired
    const char **include_dirs;
    const size_t include_dirs_count;

    // Optional: values passed directly to the compiler. Pass optimization flags here
    const char **custom_flags;
    const size_t custom_flags_count;
} LibData;

static inline const char *get_path_last_part(const char *path)
{
    const char *loc = path;
    const char *last = loc;

    while (*loc != '\0')
    {
        // TODO: test for windows, probably needs different delim
        if (*loc == '/')
            last = loc + 1;
        loc++;
    }
    return last;
}

#endif // SHARED_H_HEADER
