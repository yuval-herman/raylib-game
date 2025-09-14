#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "assert.h"
#include "physics.h"
#include "creature.h"
#include "trainer.h"

// This is actually a copy of the actual TraceLog function from raylib,
// Just instead of logging to stdout, it logs to stderr.
void errLog(int logType, const char *text, va_list args)
{

    char buffer[256] = {0};

    switch (logType)
    {
    case LOG_TRACE:
        strcpy(buffer, "TRACE: ");
        break;
    case LOG_DEBUG:
        strcpy(buffer, "DEBUG: ");
        break;
    case LOG_INFO:
        strcpy(buffer, "INFO: ");
        break;
    case LOG_WARNING:
        strcpy(buffer, "WARNING: ");
        break;
    case LOG_ERROR:
        strcpy(buffer, "ERROR: ");
        break;
    case LOG_FATAL:
        strcpy(buffer, "FATAL: ");
        break;
    default:
        break;
    }

    unsigned int textSize = (unsigned int)strlen(text);
    memcpy(buffer + strlen(buffer), text, (textSize < (256 - 12)) ? textSize : (256 - 12));
    strcat(buffer, "\n");
    vfprintf(stderr, buffer, args);
    fflush(stderr);

    if (logType == LOG_FATAL)
        exit(EXIT_FAILURE); // If fatal logging, exit program
}

int main(void)
{
    RandomState *rng = random_make_seed();

    // #==============================================================
    //                       BOX2D INITIALIZATION
    // #==============================================================
    b2WorldId world_id = physics_make_world();
    physics_make_ground(world_id);

    Rectangle groundBoxRect = {.width = GROUND_EXTENT * 2, .height = 20};
    Vector2 groundBoxRectOrigin = getRectOrigin(groundBoxRect);

    // #==============================================================
    //                     CREATURE INITIALIZATION
    // #==============================================================
    b2Vec2 node_pos[] = {{1, 7.5}, {5, 0}, {9, 7.5}, {5, 3.25}};
    JointData joints[] = {
        {0, 1, 0},
        {1, 2, 0},
        {2, 0, 0},
        {3, 1, 0},
        {3, 2, 0},
        {3, 0, 0},
    };
    Creature creature = creature_make(rng, world_id, node_pos, ARRAY_COUNT(node_pos), joints, ARRAY_COUNT(joints));
#ifdef OPTIMIZER_RUN
    SetTraceLogCallback(errLog);
    TraceLog(LOG_INFO, "Running in optimizer mode");
    if (creature_train(&creature) != 0)
    {
        TraceLog(LOG_ERROR, "error while training");
        return 1;
    }
    return 0;
#else
    if (creature_train(&creature) != 0)
    {
        TraceLog(LOG_ERROR, "error while training");
        return 1;
    }
#endif

    // #==============================================================
    //                       RAYLIB INITIALIZATION
    // #==============================================================
    const int screenWidth = 1200;
    const int screenHeight = 600;

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "empty game, full potential");

#ifdef MOVE_WINDOW
    {
        int monitor = GetCurrentMonitor();
        int mon_width = GetMonitorWidth(monitor);
        SetWindowPosition(mon_width - screenWidth, 0);
    }
#endif

    SetTraceLogLevel(LOG_INFO);
    SetTargetFPS(60);

    Camera2D camera = {.zoom = 10, .offset = (Vector2){.x = screenWidth / 2, .y = 300}};

    // #==============================================================
    //                           GAME LOOP
    // #==============================================================

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);

        DrawRectanglePro(groundBoxRect, groundBoxRectOrigin, 0, BLACK);
        creature_update(&creature);
        creature_draw(creature);

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    // meaningless to put here since the OS will clean this. But this might remind me
    // if I change the logic later and it will be needed. It won't hurt anyway...
    random_destroy(rng);
    creature_destroy(&creature);
    b2DestroyWorld(world_id);
    return 0;
}
