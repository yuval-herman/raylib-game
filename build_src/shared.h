#ifndef SHARED_H_HEADER
#define SHARED_H_HEADER

#include <stdbool.h>

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

#define nob_cc_flags(cmd) cmd_append(cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-Wno-override-init-side-effects", "-D_POSIX_SOURCE");
#define nob_cc_include(cmd)                                      \
    do                                                           \
    {                                                            \
        cmd_append(cmd, "-I./" SRC_DIR);                         \
        cmd_append(cmd, "-I./" SRC_DIR "external/genann");       \
        cmd_append(cmd, "-I./" SRC_DIR "external/tinycthreads"); \
        cmd_append(cmd, "-I./" INCLUDE_DIR);                     \
    } while (0)

#ifdef _WIN32
#define WINDOWS_FLAGS cmd_append(cmd, "-lopengl32", "-lgdi32", "-lwinmm", "-lshell32");
#else
#define WINDOWS_FLAGS
#endif
#define nob_linker(cmd)                                   \
    do                                                    \
    {                                                     \
        cmd_append(cmd, BUILD_DIR BOX2D_LIB_FILE, "-lm"); \
        cmd_append(cmd, BUILD_DIR RAYLIB_LIB_FILE);       \
        cmd_append(cmd, BUILD_DIR PCG_C_LIB_FILE);        \
        WINDOWS_FLAGS                                     \
    } while (0)

#define NOB_STRIP_PREFIX
#include "../nob.h"

bool cmd_prep(Cmd *cmd,
              bool move_window,
              bool debug,
              bool optimize)
{
    nob_cc(cmd);
    nob_cc_flags(cmd);
    if (move_window)
        cmd_append(cmd, "-DMOVE_WINDOW");
    // TODO: currently on, should add a flag to disable and also actually disable not activate one thread
    cmd_append(cmd, "-DENABLE_THREADS=true");

    if (debug)
        cmd_append(cmd, "-g", "-O0", "-fsanitize=address,undefined");
    else if (optimize)
        cmd_append(cmd, "-O3", "-march=native");
    nob_cc_include(cmd);
    nob_cc_output(cmd, OUTPUT_FILE);
    nob_cc_inputs(cmd,
                  SRC_DIR "main.c",
                  SRC_DIR "physics.c",
                  SRC_DIR "creature.c",
                  SRC_DIR "trainer.c",
                  SRC_DIR "random.c",
                  SRC_DIR "utils.c",
                  SRC_DIR "external/genann/genann.c",
#ifdef _WIN32
                  SRC_DIR "external/tinycthreads/tinycthread.c",
#endif
    );
    nob_linker(cmd);

    return true;
}

bool compile_program(Cmd *cmd,
                     bool move_window,
                     bool debug,
                     bool optimize)
{
    cmd_prep(cmd, move_window, debug, optimize);
    if (!cmd_run(cmd))
        return false;
    return true;
}

#endif // SHARED_H_HEADER
