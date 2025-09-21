#include "raylib.h"

void init_window()
{
    const int screenWidth = GetMonitorWidth(0);
    const int screenHeight = GetMonitorHeight(0);

    InitWindow(screenWidth, screenHeight, "game title");

    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_MAXIMIZED);
    InitWindow(screenWidth, screenHeight, "empty game, full potential");

    SetTraceLogLevel(LOG_INFO);
    SetTargetFPS(60);
}

void deinit_window()
{
    CloseWindow();
}

bool window_should_close() { return WindowShouldClose(); }

void draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    EndDrawing();
}