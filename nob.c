
#include "build_src/libs/box2d-build.h"
#include "build_src/libs/raylib-build.h"
#include "build_src/libs/pcg-c-build.h"
#include "build_src/libs/tinycthreads-build.h"
#include "build_src/lib-build.h"

#define NOB_EXPERIMENTAL_DELETE_OLD
#include "build_src/shared.h"

#define NOB_IMPLEMENTATION
#include "build_src/nob.h"

#define FLAG_IMPLEMENTATION
#define FLAG_PUSH_DASH_DASH_BACK
#include "flag.h"

#define BUILD_SRC_FILES "build_src/shared.h",            \
                        "build_src/lib-build.h",         \
                        "build_src/libs/box2d-build.h",  \
                        "build_src/libs/raylib-build.h", \
                        "build_src/libs/pcg-c-build.h",  \
                        "build_src/libs/tinycthreads-build.h"

#define BOOL2STR(flag) flag ? "set" : "unset"

#define try_lib_build(lib_name, lib_data)                            \
    do                                                               \
    {                                                                \
        if (!nob_file_exists(BUILD_DIR "lib" lib_name ".a"))         \
        {                                                            \
            if (!build_lib(&cmd, lib_data))                          \
            {                                                        \
                nob_log(NOB_ERROR, lib_name " build failed");        \
                return false;                                        \
            }                                                        \
        }                                                            \
        else                                                         \
        {                                                            \
            nob_log(NOB_INFO, "lib" lib_name ".a"                    \
                              " found, skiping " lib_name " build"); \
        }                                                            \
    } while (0);

typedef struct BuildFlags
{
    const bool help;
    const bool run;
    const bool debug;
    const bool optimize;
    const bool force;
    const bool disable_threads;
    const Flag_List custom_defines;
} BuildFlags;

Nob_Cmd cmd = {0};

void usage(FILE *stream)
{
    fprintf(stream, "Usage: ./nob [OPTIONS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

bool build_libs()
{
    nob_log(NOB_INFO, "Building libraries");
    print_separator(NOB_INFO);
    if (!nob_mkdir_if_not_exists(BUILD_DIR) || !nob_mkdir_if_not_exists(INCLUDE_DIR))
    {
        nob_log(NOB_ERROR, "failed creating build directory");
        return false;
    }

    try_lib_build(BOX2D_LIB, box2d_data);
    try_lib_build(RAYLIB_LIB, raylib_data);
    try_lib_build(PCG_C_LIB, pcg_c_data);
    try_lib_build(TINYCTHREADS_LIB, tinycthreads_data);

    print_separator(NOB_INFO);
    return true;
}

// This function exits on error
BuildFlags parse_flags(int argc, char **argv)
{
    bool *help = flag_bool("help", false, "Print this help to stdout and exit with 0");
    bool *run = flag_bool("run", false, "Run main after compilation");
    bool *move_window = flag_bool("move_window", false, "Make the game window appear in the top-right corner of the screen. This is helpful for development.");
    bool *debug = flag_bool("debug", false, "Compile with debug symbols");
    bool *optimize = flag_bool("optimize", false, "Enable compiler optimizations. This is ignored when used with -debug");
    bool *force = flag_bool("force", false, "Forces rebuild even if files were not updated");
    bool *disable_threads = flag_bool("disable_threads", false, "Disables multithreading in compiled program");
    Flag_List *custom_defines = flag_list("define", "Define a symbol for the preprocessor, passed directly to the compiler");

    if (!flag_parse(argc, argv))
    {
        usage(stderr);
        flag_print_error(stderr);
        exit(1);
    }
    return (BuildFlags){
        .help = *help,
        .run = *run,
        .debug = *debug,
        .optimize = *optimize,
        .force = *force,
        .disable_threads = *disable_threads,
        .custom_defines = *custom_defines,
    };
}

void print_set_flags(const BuildFlags flags)
{
    print_separator(NOB_INFO);

    nob_log(NOB_INFO, "Flags:");
    nob_log(NOB_INFO, "\t\t%-15s:\t%5s", "debug", BOOL2STR(flags.debug));
    nob_log(NOB_INFO, "\t\t%-15s:\t%5s", "optimize", BOOL2STR(flags.optimize));
    nob_log(NOB_INFO, "\t\t%-15s:\t%5s", "force", BOOL2STR(flags.force));
    print_separator(NOB_INFO);

    if (flags.custom_defines.count > 0)
    {
        nob_log(NOB_INFO, "Custom definitions:");
        print_separator(NOB_INFO);
        for (size_t i = 0; i < flags.custom_defines.count; i++)
        {
            nob_log(NOB_INFO, "\t\t%s", flags.custom_defines.items[i]);
        }
        print_separator(NOB_INFO);
    }
}

bool is_exec_stale()
{
    // TODO detect flag changes, such as -optimize, as also requiring a rebuild

    Nob_File_Paths no_path_files = {0};
    Nob_File_Paths src_files = {0};
    Nob_String_Builder src_sb = {0};
    bool read_dir_success;

    read_dir_success = nob_read_entire_dir(SRC_DIR, &no_path_files);
    if (read_dir_success)
    {
        for (size_t i = 0; i < no_path_files.count; i++)
        {
            src_sb.count = 0;
            nob_sb_append_cstr(&src_sb, SRC_DIR);
            nob_sb_append_cstr(&src_sb, "/");
            nob_sb_append_cstr(&src_sb, no_path_files.items[i]);
            nob_sb_append_null(&src_sb);
            nob_da_append(&src_files, nob_temp_strdup(src_sb.items));
        }
    }
    return !read_dir_success || nob_needs_rebuild(OUTPUT_FILE, src_files.items, src_files.count);
}

bool compile_main(const BuildFlags flags)
{
    nob_cc(&cmd);
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-Wno-override-init-side-effects", "-D_POSIX_SOURCE");

    for (size_t i = 0; i < flags.custom_defines.count; i++)
    {
        nob_cmd_append(&cmd, nob_temp_sprintf("-D%s", flags.custom_defines.items[i]));
    }
    if (!flags.disable_threads)
    {
        nob_cmd_append(&cmd, "-DENABLE_THREADS=true");
    }

    if (flags.debug)
        nob_cmd_append(&cmd, "-g", "-O0", "-fsanitize=address,undefined");
    else if (flags.optimize)
        nob_cmd_append(&cmd, "-O3", "-march=native");

    nob_cmd_append(&cmd, "-I./" SRC_DIR);
    nob_cmd_append(&cmd, "-I./" INCLUDE_DIR);

    nob_cc_output(&cmd, OUTPUT_FILE);
    nob_cc_inputs(&cmd,
                  SRC_DIR "main.c",
                  SRC_DIR "physics.c",
                  SRC_DIR "creature.c",
                  SRC_DIR "trainer.c",
                  SRC_DIR "random.c",
                  SRC_DIR "genann.c",
                  SRC_DIR "utils.c", );
    nob_cmd_append(&cmd, BUILD_DIR lib_file_name(BOX2D_LIB), "-lm");
    nob_cmd_append(&cmd, BUILD_DIR lib_file_name(RAYLIB_LIB));
    nob_cmd_append(&cmd, BUILD_DIR lib_file_name(PCG_C_LIB));

#ifdef _WIN32
    nob_cmd_append(&cmd, "-lopengl32", "-lgdi32", "-lwinmm", "-lshell32");
#endif

    if (!nob_cmd_run(&cmd))
        return false;

    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, BUILD_SRC_FILES);

    const BuildFlags b_flags = parse_flags(argc, argv);

    if (b_flags.help)
    {
        usage(stdout);
        exit(0);
    }

    print_set_flags(b_flags);

    if (!build_libs())
        return 1;

    if (b_flags.force || is_exec_stale())
    {
        if (!compile_main(b_flags))
            return 1;
    }
    else
    {
        nob_log(NOB_INFO, "No rebuild needed");
    }

    if (b_flags.run)
    {
        nob_cmd_append(&cmd, "./" OUTPUT_FILE);
        if (!nob_cmd_run(&cmd))
            return 1;
    }

    return 0;
}