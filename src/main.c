#include "box2d.h"
#include "constants.h"
#include "id.h"
#include "math_functions.h"
#include "physics.h"
#include "player.h"
#include "raylib.h"
#include "types.h"
#include "utils.h"

Camera2D camera = {.zoom = 1};
b2BodyId platform_id = {0};

void update_camera(float player_x, float player_y) {
  camera.offset.x = GetScreenWidth() / 2.0;
  camera.offset.y = GetScreenHeight() / 2.0;
  camera.target.x = player_x;
  camera.target.y = player_y;
}

void draw_debug_menu() {
  Vector2 player_position = player_get_postion();
  b2Vec2 platform_postion = b2Body_GetPosition(platform_id);
  
  const int font_size = 20;
  const int x_offset = 10;
  int y_offset = 10;
  DrawFPS(x_offset, y_offset);

  DrawText(TextFormat("Platform Position: x: %.2f, y: %.2f", platform_postion.x,
                      platform_postion.y),
           x_offset, y_offset += font_size, font_size, BLACK);

  DrawText(TextFormat("Position: x: %.2f, y: %.2f", player_position.x,
                      player_position.y),
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

  phys_init();
  player_init();

  SetTargetFPS(FPS);
  bool showDebugMenu = true;
  Rectangle platform = {50, -30, 100, 10};

  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.position = R2BV2(platform);
  bodyDef.type = b2_staticBody;
  platform_id = b2CreateBody(worldId, &bodyDef);

  b2ShapeDef shapeDef = b2DefaultShapeDef();
  b2Polygon polygon = b2MakeBox(platform.width / 2.0f, platform.width / 2.0f);
  b2CreatePolygonShape(platform_id, &shapeDef, &polygon);

  while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    player_update(deltaTime, platform);
    Vector2 player_position = player_get_postion();

    update_camera(player_position.x, player_position.y);
    phys_step();

    // ---------------------

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(RAYWHITE);

    player_draw();
    DrawRectangle(0, GROUND_LEVEL, 200, 10, BLACK);
    DrawRectangle(platform.x, platform.y, platform.width, platform.height,
                  GREEN);

    EndMode2D();

    // UI
    if (IsKeyPressed(SHOW_DEBUG_MENU_BUTTON))
      showDebugMenu = !showDebugMenu;

    if (showDebugMenu)
      draw_debug_menu();

    EndDrawing();
  }
  return 0;
}
