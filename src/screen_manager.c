#include "screen_manager.h"
#include "box2d.h"
#include "draw_manager.h"
#include "id.h"
#include "painters/construct_painter.h"
#include "random.h"
#include "screens/runner.c"

Game_screen current_screen = 0;
RandomState *rng;
Construct *construct;
b2WorldId world_id;
Runner_screen_context *ctx_runner;

void screen_init() {
  rng = random_make();

  world_id = physics_make_world();
  physics_make_ground(world_id);

  construct = construct_make(world_id);
  construct_add_node(construct, (Construct_node){1, (b2Vec2){10, 10}});
  construct_add_node(construct, (Construct_node){1, (b2Vec2){1, 0}});
  construct_add_joint(construct, (Construct_joint){0, 1, true});
  construct_finalize(construct, rng);

  ctx_runner = init_runner_screen(construct, world_id);
}

void screen_destroy() {
  destroy_runner_screen(ctx_runner);
  random_destroy(rng);
  construct_destroy(construct);
  b2DestroyWorld(world_id);
}

void set_screen(Game_screen screen) { current_screen = screen; }

void screen_draw() {
  switch (current_screen) {
  case SCREEN_RUNNER:
    runner_screen_draw(ctx_runner);
    break;
  }
  draw_draw();
}

void screen_update() {
  switch (current_screen) {
  case SCREEN_RUNNER:
    runner_screen_update(ctx_runner);
    break;
  }
}
