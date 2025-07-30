#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

#if defined(_MSC_VER)
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "/W4", "/nologo", "/D_CRT_SECURE_NO_WARNINGS")
#define nob_linker(cmd) cmd_append(cmd, "-I./raylib/windows-build/include", "-L./raylib/windows-build/lib", "-lraylib")
#else
#define nob_cc_flags(cmd) cmd_append(cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-D_POSIX_SOURCE", "-ggdb");
#define cc_include(cmd) cmd_append(cmd, "-I./raylib/linux-build/include")
#define nob_linker(cmd) cmd_append(cmd, "-L./raylib/linux-build/lib", "-lraylib", "-lm")
#endif

#include "nob.h"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!mkdir_if_not_exists(BUILD_FOLDER))
        return 1;

    Cmd cmd = {0};
    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_output(&cmd, BUILD_FOLDER "main");
    nob_cc_inputs(&cmd, SRC_FOLDER "main.c");
    nob_linker(&cmd);

    if (!cmd_run_sync_and_reset(&cmd))
        return 1;

    if (argc > 1)
    {
        if (strcmp(argv[1], "run") == 0)
        {
            cmd_append(&cmd, "./" BUILD_FOLDER "main");
            if (!cmd_run_sync_and_reset(&cmd))
                return 1;
        }
    }

    return 0;
}