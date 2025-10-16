#include "construct.h"
#include "draw_manager.h"
#include "id.h"
#include "painters/construct_painter.h"
#include "physics.h"
#include <stdlib.h>

typedef struct {
  Construct_Painter *painter;
  Construct *construct;
  b2WorldId world_id;
} Runner_screen_context;

Runner_screen_context *init_runner_screen(Construct *construct, b2WorldId world_id) {
  Runner_screen_context *ctx = malloc(sizeof(Runner_screen_context));

  draw_register_rectangle(
      (Draw_Rectangle){
          .pos = (Draw_Vector2){-GROUND_EXTENT, -GROUND_HEIGHT},
          .width = GROUND_EXTENT * 2,
          .height = GROUND_HEIGHT,
          .color = {0, 0, 0, 255},
      },
      0);
  Construct_Painter *painter = construct_painter_make(construct);
  ctx->painter = painter;
  ctx->construct = construct;
  ctx->world_id = world_id;
  return ctx;
}

void destroy_runner_screen(Runner_screen_context *ctx) {
  construct_painter_destroy(ctx->painter);
  free(ctx);
}

void runner_screen_draw(Runner_screen_context *ctx) {
  construct_painter_update(ctx->painter);
}

void runner_screen_update(Runner_screen_context *ctx) {
  b2World_Step(ctx->world_id, TIME_STEP, SUB_STEP_COUNT);
  construct_update(ctx->construct, INST_NONE);
}
