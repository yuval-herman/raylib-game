#include "draw_manager.h"
#include "raylib.h"
#include "rlgl.h"
#include "assert.h"
#include "utils.h"
#include <stdlib.h>

#define SHAPE_ARRAY_INIT 32

typedef enum Draw_shapes {
    #define X(n_enum, n_struct, n_low) n_enum,
    DRAW_SHAPES
    #undef X
} Draw_shapes;

typedef union Shapes_union{
    #define X(n_enum, n_struct, n_low) n_struct n_enum;
    DRAW_SHAPES
    #undef X
} Shapes_union;

typedef struct Draw_shape {
    Draw_shapes type;
    Shapes_union shape;
} Draw_shape;

typedef struct Shapes_array {
  Draw_shape* shapes;
  unsigned int count;
  unsigned int capacity;
} Shapes_array;

Shapes_array reg_shapes = {0};

Shapes_union* register_shape(Draw_shape shape) {
    if(reg_shapes.capacity==0) {
      reg_shapes.shapes = calloc(SHAPE_ARRAY_INIT, sizeof reg_shapes.shapes[0]);
      reg_shapes.capacity = SHAPE_ARRAY_INIT;
    }
    if (reg_shapes.capacity<=reg_shapes.count) {
      unsigned int new_cap = sizeof reg_shapes.shapes[0] * reg_shapes.capacity*2;
      reg_shapes.shapes = realloc(reg_shapes.shapes, new_cap);
      reg_shapes.capacity = new_cap;
    }
    Draw_shape *new_shape = &reg_shapes.shapes[reg_shapes.count++];
    *new_shape = shape;
    return &new_shape->shape;
}

#define X(n_enum, n_struct, n_low) n_struct* draw_register_##n_low(n_struct n_low) { \
    log_debug("registering draw "#n_low); \
    return &register_shape((Draw_shape){n_enum, {.n_enum = n_low}})->n_enum;}
DRAW_SHAPES
#undef X

#define vec2r(vec) ((Vector2){vec.x, vec.y})
#define color2r(color) ((Color){color.r, color.g, color.b, color.a})
#define rect2r(rect) ((Rectangle){.x=rect.pos.x,.y=rect.pos.y,.width=rect.width,.height=rect.height})

void draw_shape(Draw_shape shape) {
    switch (shape.type) {
    case CIRCLE:
    {
      Draw_Circle circle = shape.shape.CIRCLE;
      log_debug("Drawing circle: {%.2f,%.2f} rad: %.2f color: {%d,%d,%d,%d}",
                circle.pos.x,
                circle.pos.y,
                circle.radius,
                circle.color.r,
                circle.color.g,
                circle.color.b,
                circle.color.a);
      if (circle.color.a == 0) log_msg(U_LOG_WARN, "Drawing circle with zero opacity");
      DrawCircleV(vec2r(circle.pos), circle.radius, color2r(circle.color));
    }
    break;
    case LINE:
    {
      Draw_Line line = shape.shape.LINE;
      log_debug("Drawing line: {%.2f,%.2f} {%.2f,%.2f} width: %.2f color: {%d,%d,%d,%d}",
                line.start.x,
                line.start.y,
                line.end.x,
                line.end.y,
                line.width,
                line.color.r,
                line.color.g,
                line.color.b,
                line.color.a);
      if (line.color.a == 0) log_msg(U_LOG_WARN, "Drawing line with zero opacity");
      DrawLineEx(vec2r(line.start), vec2r(line.end), line.width, color2r(line.color));
    }
    break;
    case RECTANGLE:
    {
      Draw_Rectangle rect = shape.shape.RECTANGLE;
      log_debug("Drawing rectangle: {%.2f,%.2f} w: %.2f h: %.2f color: {%d,%d,%d,%d}",
                rect.pos.x,
                rect.pos.y,
                rect.width,
                rect.height,
                rect.color.r,
                rect.color.g,
                rect.color.b,
                rect.color.a);
      if (rect.color.a == 0) log_msg(U_LOG_WARN, "Drawing rectangle with zero opacity");
      DrawRectangleRec(rect2r(rect), color2r(rect.color));
    }
    break;
    default:
        assert(false);
    }
}

void draw_window_make()
{
    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_MAXIMIZED);
    // Width and height are arbitrary, window is maximized
    InitWindow(800, 600, "empty game, full potential");

    rlSetCullFace(RL_CULL_FACE_FRONT);
    SetTargetFPS(60);
}

void draw_window_destroy()
{
    CloseWindow();
}

bool draw_window_should_close() { return WindowShouldClose(); }

void draw_draw()
{
    static Camera2D camera = {.zoom = 10,
       .offset = (Vector2){.x = 500,
       .y = 300}};

    BeginDrawing();
    ClearBackground(RAYWHITE);
        BeginMode2D(camera);
            rlScalef(1,-1,1);
            for (unsigned int i=0; i<reg_shapes.count; i++) {
                draw_shape(reg_shapes.shapes[i]);
            }
        EndMode2D();
    EndDrawing();
}
