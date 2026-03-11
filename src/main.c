#include <stdio.h>
#include "raylib.h"

#define SCREEN_HEIGHT 1440
#define SCREEN_WIDTH 2560

int main(void)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "our game");
  while (true)
  {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("For new beginnings!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 72, BLACK);
    EndDrawing();

    if (IsKeyPressed(KEY_Q))
    {
      return 0;
    }
  }
  return 0;
}
