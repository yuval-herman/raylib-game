#include "box2d.h"
#include "construct.h"
#include "draw_manager.h"
#include "painters/construct_painter.h"
#include "physics.h"
#include "random.h"
#include "screen_manager.h"

int main(void) {
  draw_window_make();
  screen_init();
  while (!draw_window_should_close()) {
    screen_draw();
    screen_update();
    // draw_draw();
    // b2World_Step(world_id, TIME_STEP, SUB_STEP_COUNT);
    // construct_update(construct, INST_NONE);
    // construct_painter_update(painter);
  }

  draw_window_destroy();
  screen_destroy();

  // construct_painter_destroy(painter);
  // construct_destroy(construct);
  // random_destroy(rng);

  return 0;
}
