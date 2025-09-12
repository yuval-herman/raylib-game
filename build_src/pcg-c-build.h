#include "shared.h"

#define PCG_C_DIR "external/pcg-c/"
#define PCG_C_SRC PCG_C_DIR "src/"
#define PCG_C_INCLUDE PCG_C_DIR "include/"

#define PCG_C_BUILD BUILD_DIR "pcg_c/"

#define PCG_C_OPTIMIZE_FLAGS(cmd) nob_cmd_append(cmd, "-O3")

bool build_pcg_c(Nob_Cmd *cmd)
{

    const char *src_files[] = {
        "pcg-advance-8.c",
        "pcg-advance-16.c",
        "pcg-advance-32.c",
        "pcg-advance-64.c",
        "pcg-advance-128.c",
        "pcg-output-8.c",
        "pcg-output-16.c",
        "pcg-output-32.c",
        "pcg-output-64.c",
        "pcg-output-128.c",
        "pcg-rngs-8.c",
        "pcg-rngs-16.c",
        "pcg-rngs-32.c",
        "pcg-rngs-64.c",
        "pcg-rngs-128.c",
        "pcg-global-32.c",
        "pcg-global-64.c",
    };
    nob_mkdir_if_not_exists(PCG_C_BUILD);
    if (!nob_copy_directory_recursively(PCG_C_INCLUDE, INCLUDE_DIR))
        return false;

    Nob_Procs procs = {0};
    String_Builder sb = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); i++)
    {
        nob_cmd_append(cmd, "gcc");

        sb.count = 0;
        // setoutput dir and replace '.c' with '.o' in file name
        sb_append_cstr(&sb, PCG_C_BUILD);
        sb_append_buf(&sb, src_files[i], strlen(src_files[i]) - 2);
        sb_append_cstr(&sb, ".o");
        sb_append_null(&sb);

        nob_cmd_append(cmd, "-o");
        nob_cmd_append(cmd, temp_strdup(sb.items));

        nob_cmd_append(cmd, "-c");

        sb.count = 0;
        sb_append_cstr(&sb, PCG_C_SRC);
        sb_append_cstr(&sb, src_files[i]);
        sb_append_null(&sb);

        nob_cmd_append(cmd, strdup(sb.items));

        nob_cmd_append(cmd, "-std=c99");
        // TODO: build 2 versions (or more), one for debug, one for speed (optmize)
        PCG_C_OPTIMIZE_FLAGS(cmd);

        nob_cmd_append(cmd, "-I./" PCG_C_INCLUDE);

        if (!nob_cmd_run(cmd, .async = &procs))
            return false;
    }
    if (!nob_procs_flush(&procs))
        return false;

    nob_cmd_append(cmd, "ar", "rcs", BUILD_DIR "libpcg_random.a");
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); i++)
    {
        sb.count = 0;
        // setoutput dir and replace '.c' with '.o' in file name
        sb_append_cstr(&sb, PCG_C_BUILD);
        sb_append_buf(&sb, src_files[i], strlen(src_files[i]) - 2);
        sb_append_cstr(&sb, ".o");
        sb_append_null(&sb);

        nob_cmd_append(cmd, strdup(sb.items));
    }
    if (!nob_cmd_run(cmd))
        return false;

    // optimize library
    cmd_append(cmd, "ranlib", BUILD_DIR "libpcg_random.a");
    if (nob_cmd_run(cmd))
    {
        nob_log(WARNING, "failed optimizing library");
    }

    return true;
}
