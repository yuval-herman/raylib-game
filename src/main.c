#include "raylib.h"
#include "physics.h"
#include "assert.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    // #==============================================================
    //                       RAYLIB INITIALIZATION
    // #==============================================================
    const int screenWidth = 800;
    const int screenHeight = 450;

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
    //                       BOX2D INITIALIZATION
    // #==============================================================
    initWorld();

    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};

    b2BodyId groundId = makeBody(.body = groundBodyDef, .polygon = b2MakeBox(50.0f, 10.0f));
    Rectangle groundBoxRect = {.width = 100, .height = 20};
    Vector2 groundBoxRectOrigin = getRectOrigin(groundBoxRect);

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 20.0f};

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.restitution = 0.5f;

    b2BodyId bodyId = makeBody(.body = bodyDef, .polygon = b2MakeBox(1.0f, 1.0f), .shape = shapeDef);
    Rectangle dynamicBoxRect = {.width = 2, .height = 2};
    Vector2 dynamicBoxRectOrigin = getRectOrigin(dynamicBoxRect);

    // #==============================================================
    //                           GAME LOOP
    // #==============================================================

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        b2World_Step(worldId, timeStep, subStepCount);

        b2Vec2 pos = b2Body_GetPosition(bodyId);
        float rotation = b2Rot_GetAngle(b2Body_GetRotation(bodyId)) * -RAD2DEG;
        setRaylibPos(dynamicBoxRect, pos);

        DrawRectanglePro(dynamicBoxRect, dynamicBoxRectOrigin,
                         rotation, RED);

        pos = b2Body_GetPosition(groundId);
        rotation = b2Rot_GetAngle(b2Body_GetRotation(groundId)) * -RAD2DEG;
        setRaylibPos(groundBoxRect, pos);

        DrawRectanglePro(groundBoxRect, groundBoxRectOrigin, rotation, BLACK);

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    // meaningless to put here since the OS will clean this. But this might remind me
    // if I change the logic later and it will be needed. It won't hurt anyway...
    b2DestroyWorld(worldId);
    return 0;
}
