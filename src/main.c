#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"
#include "assert.h"
#include "physics.h"
#include "creature.h"
#include "trainer.h"

int main(void)
{
    // #==============================================================
    //                       BOX2D INITIALIZATION
    // #==============================================================
    physics_init_world();
    Rectangle groundBoxRect = {.width = GROUND_EXTENT * 2, .height = 20};
    Vector2 groundBoxRectOrigin = getRectOrigin(groundBoxRect);

    // #==============================================================
    //                     CREATURE INITIALIZATION
    // #==============================================================
    Creature creature = creature_make();
    creature_train(&creature);

    // #==============================================================
    //                       RAYLIB INITIALIZATION
    // #==============================================================
    const int screenWidth = 1200;
    const int screenHeight = 600;

    SetRandomSeed(time(0));
    srand(time(0));
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
    b2DestroyWorld(world_id);
    return 0;
}
