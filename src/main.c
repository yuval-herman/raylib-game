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

void show_debug_menu() {
  const Rectangle player_rect = player_get_rect();
  const int font_size = 20;
  const int x_offset = 10;
  int y_offset = 10;
  DrawFPS(x_offset, y_offset);

  DrawText(
      TextFormat("Position: x: %.2f, y: %.2f", player_rect.x, player_rect.y),
      x_offset, y_offset += font_size, font_size, BLACK);

  DrawText(TextFormat("Velocity: x: %.2f, y: %.2f", player_get_velocity().x,
                      player_get_velocity().y),
           x_offset, y_offset += font_size, font_size, BLACK);

  DrawText(TextFormat("Direction: %s",
                      player_get_is_facing_right() == 1 ? "Right" : "Left"),
           x_offset, y_offset += font_size, font_size, BLACK);

  DrawText(TextFormat("Can dash: %s",
                      player_get_dash_cooldown() == 0 ? "true" : "false"),
           x_offset, y_offset += font_size, font_size, BLACK);

  DrawText(TextFormat("Can double jump: %s",
                      player_get_can_double_jump() ? "true" : "false"),
           x_offset, y_offset += font_size, font_size, BLACK);
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "our game");

  SetTargetFPS(FPS);
  bool showDebugMenu;
  Rectangle platform = {50, -30, 100, 10};

  player_init();

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    player_update(deltaTime, platform);
    const Rectangle player_rect = player_get_rect();

    update_camera(player_rect.x, player_rect.y);

    if (IsKeyPressed(SHOW_DEBUG_MENU_BUTTON))
      showDebugMenu = !showDebugMenu;

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(RAYWHITE);

    player_draw();
    DrawRectangle(0, GROUND_LEVEL, 200, 10, BLACK);
    DrawRectangle(platform.x, platform.y, platform.width, platform.height,
                  GREEN);

    EndMode2D();

    // UI
    if (showDebugMenu)
      show_debug_menu();

    EndDrawing();
  }
  return 0;
}
