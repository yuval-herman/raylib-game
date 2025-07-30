#include "raylib.h"
#include <math.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "empty game, full potential");

    SetTargetFPS(60);

    const char *text = "HELLO WORLD!";
    const int font_size = 20;
    const int text_length = MeasureText(text, font_size);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText(text, (screenWidth - text_length) / 2, (screenHeight - font_size) / 2, font_size, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
