#include "player.h"
#include "constants.h"
#include "raylib.h"
#include "utils.h"
#include "input.h"
#include <math.h>
#include <stdbool.h>

const KeyboardKey JUMP_BUTTONS[] = {KEY_SPACE, KEY_UP, KEY_W};
const KeyboardKey GO_LEFT_BUTTONS[] = {KEY_LEFT, KEY_A};
const KeyboardKey GO_RIGHT_BUTTONS[] = {KEY_RIGHT, KEY_D};
const KeyboardKey DASH_BUTTON = KEY_LEFT_SHIFT;

typedef enum { DIRECTION_LEFT = -1, DIRECTION_RIGHT = 1 } Direction;

static bool canDoubleJump;
static bool isGrounded;
static Direction direction = 1;
static float dashCooldown = 0;
static float dashDuration = 0;

static Vector2 player_velocity = {0};

static Rectangle player_rect = {
    .height = PLAYER_HEIGHT, .width = PLAYER_WIDTH, .y = 2};
Rectangle player_feet = { .height = 1, .width = PLAYER_WIDTH - 2 };
Rectangle player_head = { .height = 1, .width = PLAYER_WIDTH - 2 };
Rectangle player_left = { .height = PLAYER_HEIGHT - 1, .width = 1 };
Rectangle player_right = { .height = PLAYER_HEIGHT - 1, .width = 1 };

Texture2D player_texture;

void player_init() {
  player_texture = LoadTexture("Sprites/raylibGamePlayer.png");
}

void player_draw() {
  static Rectangle drawRect = {
      .height = PLAYER_HEIGHT, .width = PLAYER_WIDTH, .x = 0, .y = 0};
  drawRect.width = PLAYER_WIDTH * direction;
  Color tint = WHITE;

  // Tint the player when dash is unavailable
  if (dashCooldown > 0) {
    tint.r *= (1 - 0.5 * dashCooldown / DASH_COOLDOWN);
    tint.g *= (1 - 0.5 * dashCooldown / DASH_COOLDOWN);
    tint.b *= (1 - 0.5 * dashCooldown / DASH_COOLDOWN);
  }
  DrawTextureRec(player_texture, drawRect,
                 (Vector2){player_rect.x, player_rect.y}, tint);
}
void update_cooldowns(float deltaTime) {
  if (dashCooldown > 0)
    dashCooldown -= deltaTime;
  if (dashDuration > 0)
    dashDuration -= deltaTime;
}

void update_dash() {
  if (IsKeyPressed(DASH_BUTTON) && dashCooldown <= 0) {
    player_velocity.x = DASH_POWER * direction;
    player_velocity.y = 0;
    dashCooldown = DASH_COOLDOWN;
    dashDuration = DASH_DURATION;
  }
}

void update_gravity(float deltaTime, Rectangle ground) {
  if (player_rect.y >= GROUND_LEVEL - player_rect.height || CheckCollisionRecs(player_feet, ground)) { // Is grounded
    if (player_rect.y >= GROUND_LEVEL - player_rect.height) player_rect.y = GROUND_LEVEL - player_rect.height;
    else player_rect.y = ground.y - player_rect.height;
    player_velocity.y = 0;
    isGrounded = true;
    canDoubleJump = true;
  } else { // Isn't grounded
    if (player_velocity.y > JUMP_HANG_THRESHOLD || player_velocity.y < 0)
      player_velocity.y += GRAVITY_LEVEL * deltaTime;
    else
      player_velocity.y += JUMP_HANG_GRAVITY_LEVEL * deltaTime;
    isGrounded = false;

    if (CheckCollisionRecs(player_head, ground)) {
      player_velocity.y = 0;
      player_rect.y = ground.y + ground.height;
    }
  }
}

void update_jump() {
  if (isOneKeyPressed(JUMP_BUTTONS, ARRAY_LEN(JUMP_BUTTONS))) {
    if (isGrounded)
      player_velocity.y = -JUMP_POWER;
    else if (canDoubleJump) {
      player_velocity.y = -JUMP_POWER;
      canDoubleJump = false;
    }
  }
  // Shorten jump height if jump button is released
  if (isOneKeyReleased(JUMP_BUTTONS, ARRAY_LEN(JUMP_BUTTONS)) &&
      player_velocity.y < 0) {
    player_velocity.y *= 0.5;
  }
}

void update_move(Rectangle ground) {
  if (isOneKeyDown(GO_LEFT_BUTTONS, ARRAY_LEN(GO_LEFT_BUTTONS))) {
    player_velocity.x -= MOVEMENT_SPEED;
    direction = DIRECTION_LEFT;
    if (CheckCollisionRecs(player_left, ground)) player_velocity.x = 0;
  } else if (isOneKeyDown(GO_RIGHT_BUTTONS, ARRAY_LEN(GO_RIGHT_BUTTONS))) {
    player_velocity.x += MOVEMENT_SPEED;
    direction = DIRECTION_RIGHT;
    if (CheckCollisionRecs(player_right, ground)) player_velocity.x = 0;
  } else {
    player_velocity.x /= DECELERATION;
  }

  if (fabsf(player_velocity.x) > MAX_MOVEMENT_SPEED)
    player_velocity.x = MAX_MOVEMENT_SPEED * direction;
}

void player_update(float deltaTime, Rectangle ground) {
  update_dash();
  update_cooldowns(deltaTime);

  if (dashDuration <= 0) {
    update_gravity(deltaTime, ground);
    update_jump();
    update_move(ground);
  }

  player_rect.x += player_velocity.x * deltaTime;
  player_rect.y += player_velocity.y * deltaTime;
  player_feet.x = player_rect.x + 1;
  player_feet.y = player_rect.y + player_rect.height - 1;
  player_head.x = player_rect.x + 1;
  player_head.y = player_rect.y;
  player_left.x = player_rect.x;
  player_left.y = player_rect.y;
  player_right.x = player_rect.x + player_rect.width - 1;
  player_right.y = player_rect.y;
}

Rectangle player_get_rect() { return player_rect; }

Vector2 player_get_velocity() { return player_velocity; }

int player_get_is_facing_right() { return direction; }

float player_get_dash_cooldown() { return dashCooldown >= 0 ? dashCooldown : 0; }

int player_get_can_double_jump() { return canDoubleJump; }

Rectangle player_get_feet() { return player_feet; }
