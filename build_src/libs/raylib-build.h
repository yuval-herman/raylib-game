#include "../shared.h"

LibData raylib_data = {
    .name = RAYLIB_LIB,
    .src_files = (const char *[8]){
        "external/raylib/src/rcore.c",
        "external/raylib/src/rshapes.c",
        "external/raylib/src/rtextures.c",
        "external/raylib/src/rtext.c",
        "external/raylib/src/utils.c",
        "external/raylib/src/rglfw.c",
        "external/raylib/src/rmodels.c",
        "external/raylib/src/raudio.c",
    },
    .src_files_count = 8,

    .header_files = (const char *[4]){
        "external/raylib/src/raylib.h",
        "external/raylib/src/raymath.h",
        "external/raylib/src/rcamera.h",
        "external/raylib/src/rlgl.h",
    },
    .header_files_count = 4,

    .include_dirs = (const char *[2]){
        "external/raylib/src",
        "external/raylib/src/external/glfw/include",
    },
    .include_dirs_count = 2,

    .custom_flags = (const char *[
#if _WIN32
        6
#else
        7
#endif
]){"-D_GNU_SOURCE", "-U_GNU_SOURCE", "-DPLATFORM_DESKTOP_GLFW", "-DGRAPHICS_API_OPENGL_33", "-std=c99",
#if _WIN32
    "-DUNICODE"
#else
        "-fPIC",
        "-D_GLFW_X11",
#endif
    },

    .custom_flags_count =
#if _WIN32
        6
#else
        7
#endif
    ,
};
