#include "constants.h"
#include "player.h"
#include "raylib.h"

Camera2D camera = {.zoom = 1};

void update_camera(float player_x, float player_y) {
  camera.offset.x = GetScreenWidth() / 2.0;
  camera.offset.y = GetScreenHeight() / 2.0;
  camera.target.x = player_x;
  camera.target.y = player_y;
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "our game");

  SetTargetFPS(FPS);
  bool showFps;

  player_init();

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    player_update(deltaTime);
    const Rectangle player_rect = player_get_rect();
    update_camera(player_rect.x, player_rect.y);

    if (IsKeyPressed(SHOW_FPS_BUTTON))
      showFps = !showFps;

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    player_draw();
    DrawRectangle(0, 0, 200, 10, BLACK);

    EndMode2D();

    // UI
    if (showFps)
      DrawFPS(15, 15);

    DrawText(TextFormat("%f", player_get_speed()), 15, 30, 30, BLACK);

    EndDrawing();
  }
  return 0;
}
