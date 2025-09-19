#ifndef SHARED_H_HEADER
#define SHARED_H_HEADER

#include <stdbool.h>

#define NOB_WARN_DEPRECATED
#include "nob.h"

#define SRC_DIR "src/"
#define BUILD_DIR "build/"
#define INCLUDE_DIR BUILD_DIR "include/"

#ifdef _WIN32
#define OUTPUT_FILE BUILD_DIR "main.exe"
#else
#define OUTPUT_FILE BUILD_DIR "main"
#endif

#define BOX2D_LIB "box2d"
#define RAYLIB_LIB "raylib"
#define PCG_C_LIB "pcg_random"

#define BOX2D_LIB_FILE "lib" BOX2D_LIB ".a"
#define RAYLIB_LIB_FILE "lib" RAYLIB_LIB ".a"
#define PCG_C_LIB_FILE "lib" PCG_C_LIB ".a"

#define print_separator(level) nob_log(level, "==================================")

#endif // SHARED_H_HEADER
