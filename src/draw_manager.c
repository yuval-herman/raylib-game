#include "draw_manager.h"
#include "raylib.h"

void draw_window_make()
{
    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_MAXIMIZED);
    // Width and height are arbitrary, window is maximized
    InitWindow(800, 600, "empty game, full potential");

    SetTargetFPS(60);
}

void draw_window_destroy()
{
    CloseWindow();
}

bool draw_window_should_close() { return WindowShouldClose(); }

void draw_draw()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    EndDrawing();
}