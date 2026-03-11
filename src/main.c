#include "raylib.h"

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "our game");
  const char *text = "For new beginnings!";
  const Vector2 text_measurements =
      MeasureTextEx(GetFontDefault(), text, 30, 1);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText(text, (GetScreenWidth() - text_measurements.x) / 2, (GetScreenHeight() - text_measurements.y) / 2, 30, BLACK);
    EndDrawing();
  }
  return 0;
}
