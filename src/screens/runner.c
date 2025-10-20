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
} screen_context_runner;

screen_context_runner *screen_init_runner(Construct *construct, b2WorldId world_id) {
  screen_context_runner *ctx = malloc(sizeof(screen_context_runner));

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

void destroy_screen_runner(screen_context_runner *ctx) {
  construct_painter_destroy(ctx->painter);
  free(ctx);
}

void screen_draw_runner(screen_context_runner *ctx) {
  construct_painter_update(ctx->painter);
}

void screen_update_runner(screen_context_runner *ctx) {
  b2World_Step(ctx->world_id, TIME_STEP, SUB_STEP_COUNT);
  construct_update(ctx->construct, INST_NONE);
}
