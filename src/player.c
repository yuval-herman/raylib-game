#include "player.h"
#include "box2d.h"
#include "constants.h"
#include "math_functions.h"
#include "physics.h"
#include "raylib.h"
#include "utils.h"
#include "input.h"

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

static b2BodyId player_id = {0};
  
Texture2D player_texture;

void player_init() {
  player_texture = LoadTexture("Sprites/raylibGamePlayer.png");

  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type = b2_kinematicBody;
  player_id = b2CreateBody(worldId, &bodyDef);

  b2ShapeDef shapeDef = b2DefaultShapeDef();
  b2Polygon polygon = b2MakeBox(PLAYER_WIDTH / 2.0f, PLAYER_HEIGHT / 2.0f);
  b2CreatePolygonShape(player_id, &shapeDef, &polygon);
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

  b2Vec2 play_pos = b2Body_GetPosition(player_id);

  DrawTextureRec(player_texture, drawRect, B2RV2(play_pos), tint);
}

void update_cooldowns(float deltaTime) {
  if (dashCooldown > 0)
    dashCooldown -= deltaTime;
  if (dashDuration > 0)
    dashDuration -= deltaTime;
}

void update_dash() {
  if (IsKeyPressed(DASH_BUTTON) && dashCooldown <= 0) {
    b2Body_SetLinearVelocity(player_id,
                             (b2Vec2){.x = DASH_POWER * direction, .y = 0});
    dashCooldown = DASH_COOLDOWN;
    dashDuration = DASH_DURATION;
  }
}

void update_gravity(float deltaTime, Rectangle ground) {
  b2Vec2 player_position = b2Body_GetPosition(player_id);
  b2Vec2 player_velocity = b2Body_GetLinearVelocity(player_id);

  if (player_position.y >= GROUND_LEVEL - PLAYER_HEIGHT) { // Is grounded
    if (player_position.y >= GROUND_LEVEL - PLAYER_HEIGHT)
      player_position.y = GROUND_LEVEL - PLAYER_HEIGHT;
    else
      player_position.y = ground.y - PLAYER_HEIGHT;
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
  b2Body_SetLinearVelocity(player_id, player_velocity);
}

void update_jump() {
  b2Vec2 player_velocity = b2Body_GetLinearVelocity(player_id);
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

  b2Body_SetLinearVelocity(player_id, player_velocity);
}

void update_move() {
  b2Vec2 player_velocity = b2Body_GetLinearVelocity(player_id);
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
  b2Body_SetLinearVelocity(player_id, player_velocity);
}

void player_update(float deltaTime, Rectangle ground) {
  update_dash();
  update_cooldowns(deltaTime);

  if (dashDuration <= 0) {
    update_gravity(deltaTime, ground);
    update_jump();
    update_move();
  }

  // player_rect.x += player_velocity.x * deltaTime;
  // player_rect.y += player_velocity.y * deltaTime;
}

Vector2 player_get_velocity() { return B2RV2(b2Body_GetLinearVelocity(player_id)); }

Vector2 player_get_postion() {return B2RV2(b2Body_GetPosition(player_id));}

int player_get_is_facing_right() { return direction; }

float player_get_dash_cooldown() { return dashCooldown >= 0 ? dashCooldown : 0; }

int player_get_can_double_jump() { return canDoubleJump; }
