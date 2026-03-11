#include "raylib.h"

const int GROUND_LEVEL = 0;
const float GRAVITY_LEVEL = 1000;
const int JUMP_POWER = 400;
const int MOVEMENT_SPEED = 400;
const int FPS = 60;

bool canDoubleJump;
bool isGrounded;

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
  if (player_rect.y >= GROUND_LEVEL - player_rect.height) { // Is grounded
    player_rect.y = GROUND_LEVEL - player_rect.height;
    player_velocity.y = 0;
    isGrounded = true;
    canDoubleJump = true;
  } else { // Isn't grounded
    player_velocity.y += GRAVITY_LEVEL * GetFrameTime();
    isGrounded = false;
  }


  if (IsKeyPressed(KEY_SPACE)) {
    if (isGrounded)
        player_velocity.y = -JUMP_POWER;
    else if (canDoubleJump) {
        player_velocity.y = -JUMP_POWER;
        canDoubleJump = false;
    }
  }

  if (IsKeyReleased(KEY_SPACE) && player_velocity.y < 0)
  {
    player_velocity.y *= 0.5;
  }

  if (IsKeyDown(KEY_LEFT)) {
    player_velocity.x -= MOVEMENT_SPEED;
  } else if (IsKeyDown(KEY_RIGHT)) {
    player_velocity.x += MOVEMENT_SPEED;
  }

  player_velocity.x /= 2;
  player_rect.x += player_velocity.x * GetFrameTime();
  player_rect.y += player_velocity.y * GetFrameTime();
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "our game");

  SetTargetFPS(FPS);

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
