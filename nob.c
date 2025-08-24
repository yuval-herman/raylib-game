#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

#if defined(_MSC_VER)
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "/W4", "/Zi", "/nologo", "/D_CRT_SECURE_NO_WARNINGS")
#define nob_cc_include(cmd, target) cmd_append(cmd, "/I./raylib/" target "-build/include", "/I./box2d/" target "-build/include")
#define nob_linker(cmd, target) cmd_append(cmd, "/LIBPATH:./raylib/" target "-build/lib", "raylib.lib", "/LIBPATH:./box2d/" target "-build/lib", "box2d.lib")
#else
#define nob_cc_flags(cmd) cmd_append(cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-Wno-override-init-side-effects", "-D_POSIX_SOURCE", "-ggdb");
#define nob_cc_include(cmd, target)                                           \
    do                                                                        \
    {                                                                         \
        cmd_append(cmd, temp_sprintf("-I./raylib/%s-build/include", target)); \
        cmd_append(cmd, temp_sprintf("-I./box2d/%s-build/include", target));  \
    } while (0)
#define nob_linker(cmd, target)                                                            \
    do                                                                                     \
    {                                                                                      \
        cmd_append(cmd, temp_sprintf("-L./raylib/%s-build/lib", target), "-lraylib");      \
        cmd_append(cmd, temp_sprintf("-L./box2d/%s-build/lib", target), "-lbox2d", "-lm"); \
    } while (0)
#endif

#include "nob.h"

#define FLAG_IMPLEMENTATION
#define FLAG_PUSH_DASH_DASH_BACK
#include "flag.h"

#ifdef _WIN32
#define DEFAULT_TARGET "windows"
#else
#define DEFAULT_TARGET "linux"
#endif

void usage(FILE *stream)
{
    fprintf(stream, "Usage: ./nob [OPTIONS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    bool *help = flag_bool("help", false, "Print this help to stdout and exit with 0");
    bool *run = flag_bool("run", false, "Run main after compilation");
    char **target = flag_str("target", DEFAULT_TARGET, "Compilation target (windows/linux)");

    if (!flag_parse(argc, argv))
    {
        usage(stderr);
        flag_print_error(stderr);
        exit(1);
    }

    if (*help)
    {
        usage(stdout);
        exit(0);
    }

    if (!mkdir_if_not_exists(BUILD_FOLDER))
        return 1;

    nob_log(INFO, "Compilation target: %s", *target);

    Cmd cmd = {0};

    if (strcmp(*target, "windows") == 0)
    {
#ifdef _WIN32
        nob_cc(&cmd);
#else
        nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
#endif
    }
    else if (strcmp(*target, "linux") == 0)
    {
#ifdef _WIN32
        nob_log(ERROR, "Cross compilation on windows is not supported");
        exit(1);
#else
        nob_cc(&cmd);
#endif
    }
    else
    {
        nob_log(ERROR, "Target '%s' is not supported", *target);
        exit(1);
    }

    nob_cc_flags(&cmd);
    nob_cc_include(&cmd, *target);
    nob_cc_output(&cmd, BUILD_FOLDER "main");
    nob_cc_inputs(&cmd, SRC_FOLDER "main.c");
    nob_linker(&cmd, *target);

    if (!cmd_run_sync_and_reset(&cmd))
        return 1;

    if (*run)
    {
        cmd_append(&cmd, "./" BUILD_FOLDER "main");
        if (!cmd_run_sync_and_reset(&cmd))
            return 1;
    }

    return 0;
}