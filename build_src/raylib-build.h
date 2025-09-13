#include "shared.h"

#define RAYLIB_DIR "external/raylib/"
#define RAYLIB_SRC RAYLIB_DIR "src/"

#define RAYLIB_BUILD BUILD_DIR "raylib/"

#define RAYLIB_OPTIMIZE_FLAGS(cmd) nob_cmd_append(cmd, "-O3")

bool build_raylib(Nob_Cmd *cmd)
{
    const char *src_files[] = {
        "rcore.c",
        "rshapes.c",
        "rtextures.c",
        "rtext.c",
        "utils.c",
        "rglfw.c",
        "rmodels.c",
        "raudio.c",
    };

    const char *header_files[] = {
        "raylib.h",
        "raymath.h",
        "rcamera.h",
        "rlgl.h",
    };

    nob_mkdir_if_not_exists(RAYLIB_BUILD);

    for (size_t i = 0; i < ARRAY_LEN(header_files); i++)
    {
        if (!nob_copy_file(temp_sprintf("%s%s", RAYLIB_SRC, header_files[i]), temp_sprintf("%s%s", INCLUDE_DIR, header_files[i])))
            return false;
    }

    Nob_Procs procs = {0};
    String_Builder sb = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); i++)
    {
        nob_cmd_append(cmd, "gcc");

        sb.count = 0;
        // setoutput dir and replace '.c' with '.o' in file name
        sb_append_cstr(&sb, RAYLIB_BUILD);
        sb_append_buf(&sb, src_files[i], strlen(src_files[i]) - 2);
        sb_append_cstr(&sb, ".o");
        sb_append_null(&sb);

        nob_cmd_append(cmd, "-o");
        nob_cmd_append(cmd, temp_strdup(sb.items));

        nob_cmd_append(cmd, "-c");

        sb.count = 0;
        sb_append_cstr(&sb, RAYLIB_SRC);
        sb_append_cstr(&sb, src_files[i]);
        sb_append_null(&sb);

        nob_cmd_append(cmd, temp_strdup(sb.items));

        nob_cmd_append(cmd, "-D_GNU_SOURCE", "-U_GNU_SOURCE",
                       "-DPLATFORM_DESKTOP_GLFW", "-DGRAPHICS_API_OPENGL_33",
                       "-std=c99",
#if _WIN32
                       "-DUNICODE"
#else
                       "-fPIC",
                       "-D_GLFW_X11"
#endif
        );
        // TODO: build 2 versions (or more), one for debug, one for speed (optmize)
        RAYLIB_OPTIMIZE_FLAGS(cmd);

        nob_cmd_append(cmd, "-I./" RAYLIB_SRC, "-I./" RAYLIB_SRC "external/glfw/include");

        if (!nob_cmd_run(cmd, .async = &procs))
            return false;
    }
    if (!nob_procs_flush(&procs))
        return false;

    nob_cmd_append(cmd, "ar", "rcs", BUILD_DIR RAYLIB_LIB_FILE);
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); i++)
    {
        sb.count = 0;
        // setoutput dir and replace '.c' with '.o' in file name
        sb_append_cstr(&sb, RAYLIB_BUILD);
        sb_append_buf(&sb, src_files[i], strlen(src_files[i]) - 2);
        sb_append_cstr(&sb, ".o");
        sb_append_null(&sb);

        nob_cmd_append(cmd, temp_strdup(sb.items));
    }
    if (!nob_cmd_run(cmd))
        return false;

    // optimize library
    cmd_append(cmd, "ranlib", BUILD_DIR RAYLIB_LIB_FILE);
    if (!nob_cmd_run(cmd))
    {
        nob_log(WARNING, "failed optimizing library");
    }

    return true;
}
