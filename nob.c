#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#define NOB_WARN_DEPRECATED

#include "build_src/shared.h"
#include "build_src/box2d-build.h"
#include "build_src/raylib-build.h"
#include "build_src/pcg-c-build.h"

#define FLAG_IMPLEMENTATION
#define FLAG_PUSH_DASH_DASH_BACK
#include "flag.h"

#define NOB_IMPLEMENTATION
#include "nob.h"

#ifdef _WIN32
#define DEFAULT_TARGET "windows"
#else
#define DEFAULT_TARGET "linux"
#endif

#define FLAG_SET(flag) flag ? "set" : "unset"

Cmd cmd = {0};

void usage(FILE *stream)
{
    fprintf(stream, "Usage: ./nob [OPTIONS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

bool build_libs()
{
    if (!nob_mkdir_if_not_exists(BUILD_DIR) || !nob_mkdir_if_not_exists(INCLUDE_DIR))
    {
        nob_log(ERROR, "failed creating build directory");
        return false;
    }

    // ##### box2d
    if (!nob_file_exists(BUILD_DIR BOX2D_LIB_FILE))
    {
        if (!build_box2d(&cmd))
        {
            nob_log(ERROR, "box2d build failed");
            return false;
        }
    }
    else
    {
        nob_log(NOB_INFO, BOX2D_LIB_FILE " found, skiping box2d build");
    }

    // ##### raylib
    if (!nob_file_exists(BUILD_DIR RAYLIB_LIB_FILE))
    {
        if (!build_raylib(&cmd))
        {
            nob_log(ERROR, "raylib build failed");
            return false;
        }
    }
    else
    {
        nob_log(NOB_INFO, RAYLIB_LIB_FILE " found, skiping raylib build");
    }

    // ##### pcg-c
    if (!nob_file_exists(BUILD_DIR PCG_C_LIB_FILE))
    {
        if (!build_pcg_c(&cmd))
        {
            nob_log(ERROR, "pcg_c build failed");
            return false;
        }
    }
    else
    {
        nob_log(NOB_INFO, PCG_C_LIB_FILE " found, skiping pcg_c build");
    }
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv,
                               "build_src/shared.h",
                               "build_src/box2d-build.h",
                               "build_src/raylib-build.h",
                               "build_src/pcg-c-build.h");

    bool *help = flag_bool("help", false, "Print this help to stdout and exit with 0");
    bool *run = flag_bool("run", false, "Run main after compilation");
    bool *move_window = flag_bool("move_window", false, "Make the game window appear in the top-right corner of the screen. This is helpful for development.");
    bool *debug = flag_bool("debug", false, "Compile with debug symbols");
    bool *optimize = flag_bool("optimize", false, "Enable compiler optimizations. This is ignored when used with -debug");
    bool *force = flag_bool("force", false, "Forces rebuild even if files were not updated");
    Flag_List *custom_defines = flag_list("define", "Define a symbol for the preprocessor, passed directly to the compiler");
    // bool *run_optimizer = flag_bool("run_optimizer", false, "Instead of building the program normally, this will run an optimizer on the values defined in trainer.h and report back findings");
    // char **target = flag_str("target", DEFAULT_TARGET, "Compilation target (windows/linux)");

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

    nob_log(INFO, "==================================");
    // nob_log(INFO, "Compilation target: %s", *target);
    nob_log(INFO, "Flags:");
    nob_log(INFO, "\t\t%-15s:\t%5s", "move_window", FLAG_SET(*move_window));
    nob_log(INFO, "\t\t%-15s:\t%5s", "debug", FLAG_SET(*debug));
    nob_log(INFO, "\t\t%-15s:\t%5s", "optimize", FLAG_SET(*optimize));
    nob_log(INFO, "\t\t%-15s:\t%5s", "force", FLAG_SET(*force));
    // nob_log(INFO, "\t\t%-15s:\t%5s", "run_optimizer", FLAG_SET(*run_optimizer));
    nob_log(INFO, "==================================");

    if (custom_defines->count > 0)
    {
        nob_log(INFO, "Custom definitions:");
        nob_log(INFO, "==================================");
        for (size_t i = 0; i < custom_defines->count; i++)
        {
            nob_log(INFO, "\t\t%s", custom_defines->items[i]);
        }
        nob_log(INFO, "==================================");
    }

    if (!build_libs())
        return 1;

#if 0
    if (*run_optimizer)
    {
        const int search_runs = 200;
        const int avg_runs = 5;
        float best_fitness = 0, best_trend = 0;
        OptimizerValues best_op_values;

        RandomState *rng = random_make_seed();
        String_Builder sb = {0};

        for (int search = 0; search < search_runs; search++)
        {
            nob_log(INFO, "Running search #%d best so far: fitness=%9.6f trend=%9.6f", search, best_fitness, best_trend);
            write_values_to_file(best_op_values);

            float fitness = 0, trend = 0;
            OptimizerValues op_values = random_op_values(rng);
            for (int i = 0; i < avg_runs; i++)
            {
                // prep normal build command
                if (!cmd_prep(&cmd, *target, *move_window, *debug, *optimize))
                    return 1;
                // add optimizer values to try and compile
                append_optimizer_flags(&cmd, op_values);
                nob_minimal_log_level = NOB_WARNING;
                if (!cmd_run(&cmd))
                    return 1;

                // run optimizer
                cmd_append(&cmd, "./" OUTPUT_FILE);
                if (!cmd_run(&cmd, .stdout_path = "./opt_stdout.txt", .stderr_path = "./opt_stderr.txt"))
                    return 1;
                nob_minimal_log_level = NOB_INFO;

                // get optimizer values result
                sb.count = 0;
                nob_read_entire_file("./opt_stdout.txt", &sb);
                float loc_fitness = 0, loc_trend = 0;

                sscanf(sb.items, "%a,%a", &loc_fitness, &loc_trend);
                nob_log(INFO, "avg run #%d fitness=%9.6f trend=%9.6f", i, loc_fitness, loc_trend);
                fitness += loc_fitness;
                trend += loc_trend;
            }
            fitness /= avg_runs;
            trend /= avg_runs;
            if (trend * 100 + fitness > best_trend * 100 + best_fitness)
            {
                best_fitness = fitness;
                best_trend = trend;
                best_op_values = op_values;
            }

            if (fitness > 100)
            {
                nob_log(WARNING, "Some parse error or something happened, this fitness value will be ignored");
            }
        }
        delete_file("./opt_stdout.txt");
        delete_file("./opt_stderr.txt");
        write_values_to_file(best_op_values);
        nob_log(INFO, "best values and achieved fitness written to ./opt_output.txt");
        nob_log(INFO, "overall best fitness was %g", best_fitness);
        return 0;
    }
#endif
    File_Paths no_path_files = {0};
    File_Paths src_files = {0};
    String_Builder src_sb = {0};
    bool read_dir_success;

    if (!*force)
    {
        read_dir_success = read_entire_dir(SRC_DIR, &no_path_files);
        if (read_dir_success)
        {
            for (size_t i = 0; i < no_path_files.count; i++)
            {
                src_sb.count = 0;
                sb_append_cstr(&src_sb, SRC_DIR);
                sb_append_cstr(&src_sb, "/");
                sb_append_cstr(&src_sb, no_path_files.items[i]);
                sb_append_null(&src_sb);
                da_append(&src_files, temp_strdup(src_sb.items));
            }
        }
    }

    // TODO detect flag changes, such as move_window, as also requiring a rebuild
    if (!read_dir_success || *force || nob_needs_rebuild(OUTPUT_FILE, src_files.items, src_files.count))
    {
        if (!compile_program(&cmd, *move_window, *debug, *optimize, *custom_defines))
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