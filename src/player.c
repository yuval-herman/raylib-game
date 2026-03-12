#include "player.h"
#include "constants.h"
#include "raylib.h"
#include <stdbool.h>

const KeyboardKey JUMP_BUTTON = KEY_SPACE;
const KeyboardKey LEFT_BUTTON = KEY_LEFT;
const KeyboardKey RIGHT_BUTTON = KEY_RIGHT;
const KeyboardKey DASH_BUTTON = KEY_LEFT_SHIFT;

static bool canDoubleJump;
static bool isGrounded;
static bool isFacingRight = true;
static float dashCooldown = 0;
static float dashDuration = 0;

static Rectangle player_rect = {.height = PLAYER_HEIGHT, .width = PLAYER_WIDTH, .y = 2};
static Vector2 player_velocity = {0};

void update_cooldowns()
{
  float frameTime = GetFrameTime();
  if (dashCooldown > 0) dashCooldown -= frameTime;
  if (dashDuration > 0) dashDuration -= frameTime;
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
      if (player_velocity.y > JUMP_HANG_THRESHOLD || player_velocity.y < 0)
        player_velocity.y += GRAVITY_LEVEL * frameTime;
      else
        player_velocity.y += JUMP_HANG_GRAVITY_LEVEL * frameTime;
      isGrounded = false;
    }

    // Jump
    if (IsKeyPressed(JUMP_BUTTON)) {
      if (isGrounded)
          player_velocity.y = -JUMP_POWER;
      else if (canDoubleJump) {
          player_velocity.y = -JUMP_POWER;
          canDoubleJump = false;
      }
    }
    if (IsKeyReleased(JUMP_BUTTON) && player_velocity.y < 0)
    {
      player_velocity.y *= 0.5;
    }

    // Move
    if (IsKeyDown(LEFT_BUTTON)) {
      player_velocity.x -= MOVEMENT_SPEED;
      if (player_velocity.x < - MAX_MOVEMENT_SPEED) player_velocity.x = -MAX_MOVEMENT_SPEED;
      if (isFacingRight) player_rect.width = -PLAYER_WIDTH;
      isFacingRight = false;
    } else if (IsKeyDown(RIGHT_BUTTON)) {
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
  if (IsKeyPressed(DASH_BUTTON) && dashCooldown <= 0) {
    player_velocity.x += DASH_POWER * (isFacingRight ? 1 : -1);
    player_velocity.y = 0;
    dashCooldown = DASH_COOLDOWN;
    dashDuration = DASH_DURATION;
  }

  player_rect.x += player_velocity.x * frameTime;
  player_rect.y += player_velocity.y * frameTime;
  update_cooldowns();
}

Rectangle get_player_rect() {
  return player_rect;
}
