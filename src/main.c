#include "raylib.h"

const int GROUND_LEVEL = 0;
const float GRAVITY_LEVEL = 1;

Camera2D camera = {.zoom = 1};
Rectangle player_rect = {.height = 20, .width = 20, .y = 2};
Vector2 player_velocity = {0};

void update_camera() {
  camera.offset.x = GetScreenWidth() / 2.0;
  camera.offset.y = GetScreenHeight() / 2.0;
  camera.target.x = player_rect.x;
  camera.target.y = player_rect.y;
}

void update_player() {
  if (player_rect.y >= GROUND_LEVEL - player_rect.height) {
    player_rect.y = GROUND_LEVEL - player_rect.height;
    player_velocity.y = 0;
    if (IsKeyDown(KEY_SPACE)) {
      player_velocity.y -= 10;
    }
  } else {
    player_velocity.y += GRAVITY_LEVEL;
  }
  if (IsKeyDown(KEY_LEFT)) {
    player_velocity.x -= 10;
  } else if (IsKeyDown(KEY_RIGHT)) {
    player_velocity.x += 10;
  }
  player_velocity.x /= 2;
  player_rect.x += player_velocity.x;
  player_rect.y += player_velocity.y;
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "our game");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    update_camera();
    update_player();

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    DrawRectangleRec(player_rect, RED);
    DrawRectangle(0, 0, 200, 10, BLACK);

    EndDrawing();
  }
  return 0;
}
