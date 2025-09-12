#ifndef SHARED_H_HEADER
#define SHARED_H_HEADER

#include <stdbool.h>

#define BUILD_DIR "build/"
#define INCLUDE_DIR BUILD_DIR "include/"

#if defined(_MSC_VER)
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "/W4", "/Zi", "/nologo", "/D_CRT_SECURE_NO_WARNINGS")
#define nob_cc_include(cmd, target) cmd_append(cmd, "/I./" EXTERNAL_FOLDER "genann", "/I./raylib/" target "-build/include", "/I./box2d/" target "-build/include")
#define nob_linker(cmd, target) cmd_append(cmd, "/LIBPATH:./raylib/" target "-build/lib", "raylib.lib", "/LIBPATH:./box2d/" target "-build/lib", "box2d.lib")
#else
#define nob_cc_flags(cmd) cmd_append(cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-Wno-override-init-side-effects", "-D_POSIX_SOURCE");
#define nob_cc_include(cmd, target)                       \
    do                                                    \
    {                                                     \
        cmd_append(cmd, "-I./" SRC_FOLDER);               \
        cmd_append(cmd, "-I./" EXTERNAL_FOLDER "genann"); \
        cmd_append(cmd, "-I./" INCLUDE_DIR);              \
    } while (0)
#define nob_linker(cmd, target)            \
    do                                     \
    {                                      \
        cmd_append(cmd, "-L./build");      \
        cmd_append(cmd, "-lbox2d", "-lm"); \
        cmd_append(cmd, "-lraylib");       \
        cmd_append(cmd, "-lpcg_random");   \
    } while (0)
#endif

#define NOB_STRIP_PREFIX
#include "../nob.h"

bool cmd_prep(Cmd *cmd, char *target,
              bool move_window,
              bool debug,
              bool optimize)
{
    if (strcmp(target, "windows") == 0)
    {
#ifdef _WIN32
        nob_cc(cmd);
#else
        nob_cmd_append(cmd, "x86_64-w64-mingw32-gcc");
#endif
    }
    else if (strcmp(target, "linux") == 0)
    {
#ifdef _WIN32
        nob_log(ERROR, "Cross compilation on windows is not supported");
        exit(1);
#else
        nob_cc(cmd);
#endif
    }
    else
    {
        nob_log(ERROR, "Target '%s' is not supported", target);
        exit(1);
    }

    nob_cc_flags(cmd);
    if (move_window)
        cmd_append(cmd, "-DMOVE_WINDOW");

    if (debug)
        cmd_append(cmd, "-ggdb", "-O0");
    else if (optimize)
        cmd_append(cmd, "-O3", "-march=native");
    nob_cc_include(cmd, target);
    nob_cc_output(cmd, OUTPUT_FILE);
    nob_cc_inputs(cmd,
                  SRC_FOLDER "main.c",
                  SRC_FOLDER "physics.c",
                  SRC_FOLDER "creature.c",
                  SRC_FOLDER "trainer.c",
                  SRC_FOLDER "random.c",
                  EXTERNAL_FOLDER "genann/genann.c", );
    nob_linker(cmd, target);

    return true;
}

bool compile_program(Cmd *cmd,
                     char *target,
                     bool move_window,
                     bool debug,
                     bool optimize)
{
    cmd_prep(cmd, target, move_window, debug, optimize);
    if (!cmd_run(cmd))
        return false;
    return true;
}

#endif // SHARED_H_HEADER
