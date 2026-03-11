#include "raylib.h"

const int GROUND_LEVEL = 0;
const float GRAVITY_LEVEL = 1000;
const float DECELERATION = 2;
const int JUMP_POWER = 400;
const int MOVEMENT_SPEED = 25;
const int MAX_MOVEMENT_SPEED = 300;
const int DASH_POWER = 500;
const float DASH_COOLDOWN = 0.5;
const float DASH_DURATION = 0.2;
const int FPS = 60;

const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 20;

bool canDoubleJump;
bool isGrounded;
bool isFacingRight = true;
float dashCooldown = 0;
float dashDuration = 0;

Camera2D camera = {.zoom = 1};
Rectangle player_rect = {.height = PLAYER_HEIGHT, .width = PLAYER_WIDTH, .y = 2};
Vector2 player_velocity = {0};

void update_camera() {
  camera.offset.x = GetScreenWidth() / 2.0;
  camera.offset.y = GetScreenHeight() / 2.0;
  camera.target.x = player_rect.x;
  camera.target.y = player_rect.y;
}

void update_player() {
  float frameTime = GetFrameTime();

  if (dashDuration <= 0) {  
    // Gravity
    if (player_rect.y >= GROUND_LEVEL - player_rect.height) { // Is grounded
      player_rect.y = GROUND_LEVEL - player_rect.height;
      player_velocity.y = 0;
      isGrounded = true;
      canDoubleJump = true;
    } else { // Isn't grounded
      player_velocity.y += GRAVITY_LEVEL * frameTime;
      isGrounded = false;
    }

    // Jump
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

    // Move
    if (IsKeyDown(KEY_LEFT)) {
      player_velocity.x -= MOVEMENT_SPEED;
      if (player_velocity.x < - MAX_MOVEMENT_SPEED) player_velocity.x = -MAX_MOVEMENT_SPEED;
      if (isFacingRight) player_rect.width = -PLAYER_WIDTH;
      isFacingRight = false;
    } else if (IsKeyDown(KEY_RIGHT)) {
      player_velocity.x += MOVEMENT_SPEED;
      if (player_velocity.x > MAX_MOVEMENT_SPEED) player_velocity.x = MAX_MOVEMENT_SPEED;
      if (!isFacingRight) player_rect.width = PLAYER_WIDTH;
      isFacingRight = true;
    }
    else {
      player_velocity.x /= DECELERATION;
    }
  }

  // Dash
  if (IsKeyPressed(KEY_LEFT_SHIFT) && dashCooldown <= 0) {
    player_velocity.x += DASH_POWER * (isFacingRight ? 1 : -1);
    player_velocity.y = 0;
    dashCooldown = DASH_COOLDOWN;
    dashDuration = DASH_DURATION;
  }

  player_rect.x += player_velocity.x * frameTime;
  player_rect.y += player_velocity.y * frameTime;
}

void update_cooldowns()
{
  float frameTime = GetFrameTime();
  if (dashCooldown > 0) dashCooldown -= frameTime;
  if (dashDuration > 0) dashDuration -= frameTime;
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "our game");

  SetTargetFPS(FPS);

  while (!WindowShouldClose()) {
    update_camera();
    update_player();
    update_cooldowns();

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(RAYWHITE);

    Rectangle source = { 0, 0, player_rect.width, player_rect.height };
    DrawTextureRec(LoadTextureFromImage(LoadImage("Sprites/raylibGamePlayer.png")), source, (Vector2){player_rect.x, player_rect.y}, WHITE);
    DrawRectangle(0, 0, 200, 10, BLACK);

    EndDrawing();
  }
  return 0;
}
