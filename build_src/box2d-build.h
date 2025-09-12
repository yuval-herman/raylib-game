#include "shared.h"

#define BOX2D_DIR "external/box2d/"
#define BOX2D_SRC BOX2D_DIR "src/"
#define BOX2D_INCLUDE BOX2D_DIR "include/"

#define BOX2D_BUILD BUILD_DIR "box2d/"

#define BOX2D_OPTIMIZE_FLAGS(cmd) nob_cmd_append(cmd, "-O3", "-DNDEBUG")

bool build_box2d(Nob_Cmd *cmd)
{
    const char *src_files[] = {
        "aabb.c",
        "arena_allocator.c",
        "array.c",
        "bitset.c",
        "body.c",
        "broad_phase.c",
        "constraint_graph.c",
        "contact.c",
        "contact_solver.c",
        "core.c",
        "distance.c",
        "distance_joint.c",
        "dynamic_tree.c",
        "geometry.c",
        "hull.c",
        "id_pool.c",
        "island.c",
        "joint.c",
        "manifold.c",
        "math_functions.c",
        "motor_joint.c",
        "mover.c",
        "physics_world.c",
        "prismatic_joint.c",
        "revolute_joint.c",
        "sensor.c",
        "shape.c",
        "solver.c",
        "solver_set.c",
        "table.c",
        "timer.c",
        "types.c",
        "weld_joint.c",
        "wheel_joint.c",
    };
    if (!nob_mkdir_if_not_exists(BOX2D_BUILD))
        return false;
    if (!nob_copy_directory_recursively(BOX2D_INCLUDE "box2d", INCLUDE_DIR))
        return false;

    Nob_Procs procs = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); i++)
    {
        nob_cmd_append(cmd, "gcc");
        nob_cmd_append(cmd, "-I" BOX2D_SRC);
        nob_cmd_append(cmd, "-I" BOX2D_INCLUDE);

        nob_cmd_append(cmd, "-DBOX2D_VALIDATE", "-std=gnu17", "-fvisibility=hidden", "-ffp-contract=off", "-pedantic");
        // TODO: build 2 versions (or more), one for debug, one for speed (optmize)
        BOX2D_OPTIMIZE_FLAGS(cmd);

        nob_cmd_append(cmd, "-o");
        nob_cmd_append(cmd, nob_temp_sprintf("%s%s.o", BOX2D_BUILD, src_files[i]));

        nob_cmd_append(cmd, "-c");
        nob_cmd_append(cmd, nob_temp_sprintf("%s%s", BOX2D_SRC, src_files[i]));

        if (!nob_cmd_run(cmd, .async = &procs))
            return false;
    }
    if (!nob_procs_flush(&procs))
        return false;

    nob_cmd_append(cmd, "ar", "rcs", BUILD_DIR "libbox2d.a");
    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); i++)
    {
        nob_cmd_append(cmd, nob_temp_sprintf("%s%s.o", BOX2D_BUILD, src_files[i]));
    }
    if (!nob_cmd_run(cmd))
        return false;

    // optimize library
    cmd_append(cmd, "ranlib", BUILD_DIR "libbox2d.a");
    if (nob_cmd_run(cmd))
    {
        nob_log(WARNING, "failed optimizing library");
    }
    return true;
}
