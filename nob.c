#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define EXTERNAL_FOLDER SRC_FOLDER "external/"
#define OUTPUT_FILE BUILD_FOLDER "main"

#if defined(_MSC_VER)
#define nob_cc_flags(cmd) nob_cmd_append(cmd, "/W4", "/Zi", "/nologo", "/D_CRT_SECURE_NO_WARNINGS")
#define nob_cc_include(cmd, target) cmd_append(cmd, "/I./" EXTERNAL_FOLDER "genann", "/I./raylib/" target "-build/include", "/I./box2d/" target "-build/include")
#define nob_linker(cmd, target) cmd_append(cmd, "/LIBPATH:./raylib/" target "-build/lib", "raylib.lib", "/LIBPATH:./box2d/" target "-build/lib", "box2d.lib")
#else
#define nob_cc_flags(cmd) cmd_append(cmd, "-Wall", "-Wextra", "-Wswitch-enum", "-Wno-override-init-side-effects", "-D_POSIX_SOURCE");
#define nob_cc_include(cmd, target)                                           \
    do                                                                        \
    {                                                                         \
        cmd_append(cmd, "-I./" EXTERNAL_FOLDER "genann");                     \
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

#define FLAG_SET(flag) flag ? "set" : "unset"

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
    bool *move_window = flag_bool("move_window", false, "Make the game window appear in the top-right corner of the screen. This is helpful for development.");
    bool *debug = flag_bool("debug", false, "Compile with debug symbols");
    bool *optimize = flag_bool("optimize", false, "Enable compiler optimizations. This is ignored when used with -debug");
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

    nob_log(INFO, "==================================");
    nob_log(INFO, "Compilation target: %s", *target);
    nob_log(INFO, "Flags:");
    nob_log(INFO, "\t\tmove_window:\t%s", FLAG_SET(*move_window));
    nob_log(INFO, "\t\tdebug:\t\t%s", FLAG_SET(*debug));
    nob_log(INFO, "\t\toptimize:\t\t%s", FLAG_SET(*optimize));
    nob_log(INFO, "==================================");

    Cmd cmd = {0};

    File_Paths no_path_files = {0};
    File_Paths src_files = {0};
    String_Builder src_sb = {0};
    bool read_dir_success = read_entire_dir(SRC_FOLDER, &no_path_files);
    if (read_dir_success)
    {
        for (size_t i = 0; i < no_path_files.count; i++)
        {
            src_sb.count = 0;
            sb_append_cstr(&src_sb, SRC_FOLDER);
            sb_append_cstr(&src_sb, "/");
            sb_append_cstr(&src_sb, no_path_files.items[i]);
            sb_append_null(&src_sb);
            da_append(&src_files, temp_strdup(src_sb.items));
        }
    }

    // TODO detect flag changes, such as move_window, as also requiring a rebuild
    if (!readdir || nob_needs_rebuild(OUTPUT_FILE, src_files.items, src_files.count))
    {
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
        if (*move_window)
            cmd_append(&cmd, "-DMOVE_WINDOW");
        if (*debug)
            cmd_append(&cmd, "-ggdb", "-Og");
        else if (*optimize)
            cmd_append(&cmd, "-O3", "-march=native");
        nob_cc_include(&cmd, *target);
        nob_cc_output(&cmd, OUTPUT_FILE);
        nob_cc_inputs(&cmd,
                      SRC_FOLDER "main.c",
                      SRC_FOLDER "physics.c",
                      SRC_FOLDER "creature.c",
                      SRC_FOLDER "trainer.c",
                      EXTERNAL_FOLDER "genann/genann.c", );
        nob_linker(&cmd, *target);

        if (!cmd_run(&cmd))
            return 1;
    }
    else
    {
        nob_log(INFO, "No rebuild needed");
    }

    if (*run)
    {
        cmd_append(&cmd, "./" OUTPUT_FILE);
        if (!cmd_run(&cmd))
            return 1;
    }

    return 0;
}