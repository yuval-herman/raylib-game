#include "player.h"
#include "raylib.h"
#include "constants.h"


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
  const Texture2D player_texture = LoadTextureFromImage(LoadImage("Sprites/raylibGamePlayer.png"));
bool showFps;

  while (!WindowShouldClose()) {
    update_player();
    const Rectangle player_rect = get_player_rect();
    update_camera(player_rect.x, player_rect.y);

  if (IsKeyPressed(SHOW_FPS_BUTTON)) showFps = !showFps;

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    Rectangle source = { 0, 0, player_rect.width, player_rect.height };
    DrawTextureRec(player_texture, source, (Vector2){player_rect.x, player_rect.y}, WHITE);
    DrawRectangle(0, 0, 200, 10, BLACK);

    EndMode2D();

    // UI
    if (showFps) DrawText(TextFormat("%d", GetFPS()), 15, 15, 30, BLACK);

    EndDrawing();
  }
  return 0;
}
