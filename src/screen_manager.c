#include "screen_manager.h"
#include "box2d.h"
#include "draw_manager.h"
#include "id.h"
#include "painters/construct_painter.h"
#include "random.h"
#include "screens/runner.c"
#include <assert.h>


// declare all function and types for all screens to ensure they exist
#define X(n_upper, n_lower, init_params) \
screen_context_##n_lower *screen_init_##n_lower(init_params); \
void destroy_screen_##n_lower(screen_context_##n_lower *ctx); \
void screen_draw_##n_lower(screen_context_##n_lower *ctx); \
void screen_update_##n_lower(screen_context_##n_lower *ctx);
#include "screens/screens.def"
#undef X

Game_screen current_screen = 0;
RandomState *rng;
Construct *construct;
b2WorldId world_id;
screen_context_runner *ctx_runner;

void screen_init() {
  rng = random_make();

  world_id = physics_make_world();
  physics_make_ground(world_id);

  construct = construct_make(world_id);
  construct_add_node(construct, (Construct_node){1, (b2Vec2){10, 10}});
  construct_add_node(construct, (Construct_node){1, (b2Vec2){1, 0}});
  construct_add_joint(construct, (Construct_joint){0, 1, true});
  construct_finalize(construct, rng);

  ctx_runner = screen_init_runner(construct, world_id);
}

void screen_destroy() {
  destroy_screen_runner(ctx_runner);
  random_destroy(rng);
  construct_destroy(construct);
  b2DestroyWorld(world_id);
}

void set_screen(Game_screen screen) { current_screen = screen; }

void screen_draw() {
  switch (current_screen) {
#define X(n_upper, n_lower, init_params) \
  case SCREEN_##n_upper: \
    screen_draw_##n_lower(ctx_##n_lower); \
    break;
  #include "screens/screens.def"
  #undef X
  default: assert(false);
  }
  draw_draw();
}

void screen_update() {
  switch (current_screen) {
#define X(n_upper, n_lower, init_params) \
  case SCREEN_##n_upper: \
    screen_update_##n_lower(ctx_##n_lower); \
    break;
  #include "screens/screens.def"
  #undef X
  default: assert(false);
  }
}
