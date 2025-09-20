#if !defined(LIB_BUILD_H)
#define LIB_BUILD_H

#include <stdbool.h>
#include "shared.h"

bool build_lib(Nob_Cmd *cmd, const LibData lib_data)
{
    size_t tmp_mark = nob_temp_save();
    const char *build_artifacts_dir = nob_temp_sprintf("%s%s/", BUILD_DIR, lib_data.name);

    if (!nob_mkdir_if_not_exists(build_artifacts_dir))
        return false;

    // Copy header files to include dir
    for (size_t i = 0; i < lib_data.header_files_count; i++)
    {
        if (!nob_copy_file(lib_data.header_files[i], nob_temp_sprintf("%s%s", INCLUDE_DIR, get_path_last_part(lib_data.header_files[i]))))
            return false;
    }

    Nob_Procs procs = {0};

    // Compile object files
    for (size_t src_i = 0; src_i < lib_data.src_files_count; src_i++)
    {
        // TODO: Support other compilers?
        nob_cmd_append(cmd, "gcc");

        if (lib_data.include_dirs_count > 0 && lib_data.include_dirs != NULL)
        {
            for (size_t include_i = 0; include_i < lib_data.include_dirs_count; include_i++)
            {
                nob_cmd_append(cmd, nob_temp_sprintf("%s%s", "-I", lib_data.include_dirs[include_i]));
            }
        }
        if (lib_data.custom_flags_count > 0 && lib_data.custom_flags != NULL)
        {
            for (size_t flags_i = 0; flags_i < lib_data.custom_flags_count; flags_i++)
            {
                nob_cmd_append(cmd, lib_data.custom_flags[flags_i]);
            }
        }

        nob_cmd_append(cmd, "-o");
        nob_cmd_append(cmd, nob_temp_sprintf("%s%s.o", build_artifacts_dir, get_path_last_part(lib_data.src_files[src_i])));

        nob_cmd_append(cmd, "-c");
        nob_cmd_append(cmd, lib_data.src_files[src_i]);

        if (!nob_cmd_run(cmd, .async = &procs))
            return false;
    }
    if (!nob_procs_flush(&procs))
        return false;

    // Package .o files to static library
    const char *lib_file = nob_temp_sprintf("%slib%s.a", BUILD_DIR, lib_data.name);
    nob_cmd_append(cmd, "ar", "rcs", lib_file);
    for (size_t src_i = 0; src_i < lib_data.src_files_count; src_i++)
    {
        nob_cmd_append(cmd, nob_temp_sprintf("%s%s.o", build_artifacts_dir, get_path_last_part(lib_data.src_files[src_i])));
    }
    if (!nob_cmd_run(cmd))
        return false;

    nob_temp_rewind(tmp_mark);
    // optimize library
    nob_cmd_append(cmd, "ranlib", lib_file);
    if (!nob_cmd_run(cmd))
    {
        nob_log(NOB_WARNING, "failed optimizing library");
    }
    return true;
}

#endif // LIB_BUILD_H
