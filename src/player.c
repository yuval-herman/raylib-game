#include "player.h"
#include "constants.h"
#include "raylib.h"
#include "utils.h"
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

Texture2D player_texture;

void player_init() {
  player_texture = LoadTexture("Sprites/raylibGamePlayer.png");
}
void player_draw() {
  static Rectangle drawRect = {
      .height = PLAYER_HEIGHT, .width = PLAYER_WIDTH, .x = 0, .y = 0};
  drawRect.width = PLAYER_WIDTH * direction;
  DrawTextureRec(player_texture, drawRect,
                 (Vector2){player_rect.x, player_rect.y}, WHITE);
}
void update_cooldowns(float deltaTime) {
  if (dashCooldown > 0)
    dashCooldown -= deltaTime;
  if (dashDuration > 0)
    dashDuration -= deltaTime;
}

void update_dash() {
  if (IsKeyPressed(DASH_BUTTON) && dashCooldown <= 0) {
    player_velocity.x += DASH_POWER * direction;
    player_velocity.y = 0;
    dashCooldown = DASH_COOLDOWN;
    dashDuration = DASH_DURATION;
  }
}

void update_gravity(float deltaTime) {
  if (player_rect.y >= GROUND_LEVEL - player_rect.height) { // Is grounded
    player_rect.y = GROUND_LEVEL - player_rect.height;
    player_velocity.y = 0;
    isGrounded = true;
    canDoubleJump = true;
  } else { // Isn't grounded
    if (player_velocity.y > JUMP_HANG_THRESHOLD || player_velocity.y < 0)
      player_velocity.y += GRAVITY_LEVEL * deltaTime;
    else
      player_velocity.y += JUMP_HANG_GRAVITY_LEVEL * deltaTime;
    isGrounded = false;
  }
}

bool isOneKeyPressed(const KeyboardKey *keys, int keys_length) {
  for (int i = 0; i < keys_length; ++i) {
    if (IsKeyPressed(keys[i]))
      return true;
  }
  return false;
}

bool isOneKeyDown(const KeyboardKey *keys, int keys_length) {
  for (int i = 0; i < keys_length; ++i) {
    if (IsKeyDown(keys[i]))
      return true;
  }
  return false;
}

bool isOneKeyReleased(const KeyboardKey *keys, int keys_length) {
  for (int i = 0; i < keys_length; ++i) {
    if (IsKeyReleased(keys[i]))
      return true;
  }
  return false;
}

void update_jump(float deltaTime) {
  if (isOneKeyPressed(JUMP_BUTTONS, ARRAY_LEN(JUMP_BUTTONS))) {
    if (isGrounded)
      player_velocity.y = -JUMP_POWER;
    else if (canDoubleJump) {
      player_velocity.y = -JUMP_POWER;
      canDoubleJump = false;
    }
  }
  if (isOneKeyReleased(JUMP_BUTTONS, ARRAY_LEN(JUMP_BUTTONS)) && player_velocity.y < 0) {
    player_velocity.y *= 0.5;
  }
}

void update_move(float deltaTime) {
  if (isOneKeyDown(GO_LEFT_BUTTONS, ARRAY_LEN(GO_LEFT_BUTTONS))) {
    player_velocity.x -= MOVEMENT_SPEED;
    direction = DIRECTION_LEFT;
  } else if (isOneKeyDown(GO_RIGHT_BUTTONS, ARRAY_LEN(GO_RIGHT_BUTTONS))) {
    player_velocity.x += MOVEMENT_SPEED;
    direction = DIRECTION_RIGHT;
  } else {
    player_velocity.x /= DECELERATION;
  }

  if (fabsf(player_velocity.x) > MAX_MOVEMENT_SPEED)
    player_velocity.x = MAX_MOVEMENT_SPEED * direction;
}

void player_update(float deltaTime) {
  update_dash();
  update_cooldowns(deltaTime);

  if (dashDuration <= 0) {
    update_gravity(deltaTime);
    update_jump(deltaTime);
    update_move(deltaTime);
  }

  player_rect.x += player_velocity.x * deltaTime;
  player_rect.y += player_velocity.y * deltaTime;
}

Rectangle player_get_rect() { return player_rect; }

float player_get_speed() { return player_velocity.x; }
