#include "draw_manager.h"
#include "raylib.h"
#include "assert.h"
#include "utils.h"
#include <stdlib.h>

#define SHAPE_ARRAY_INIT 32

#define DRAW_SHAPES    \
X(CIRCLE, Draw_Circle) \
X(LINE,   Draw_Line)

typedef enum Draw_shapes {
    #define X(name, type) name,
    DRAW_SHAPES
    #undef X
} Draw_shapes;

typedef union Shapes_union{
    #define X(name, type) type name;
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

Draw_Circle* draw_register_circle(Draw_Circle circle) {
    log_debug("registering draw circle");
    return &register_shape((Draw_shape){CIRCLE, {.CIRCLE = circle}})->CIRCLE;
}

Draw_Line* draw_register_line(Draw_Line line) {
    log_debug("registering draw line");
    return &register_shape((Draw_shape){LINE, {.LINE = line}})->LINE;
}

#define vec2r(vec) ((Vector2){vec.x, vec.y})
#define color2r(color) ((Color){color.r, color.g, color.b, color.a})

void draw_shape(Draw_shape shape) {
    switch (shape.type) {
    case CIRCLE:
    {
      log_debug("Drawing circle");
      Draw_Circle circle = shape.shape.CIRCLE;
      DrawCircleV(vec2r(circle.pos), circle.radius, color2r(circle.color));
    }
    break;
    case LINE:
    {
      log_debug("Drawing line");
      Draw_Line line = shape.shape.LINE;
      DrawLineEx(vec2r(line.start), vec2r(line.end), line.width, color2r(line.color));
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

    SetTargetFPS(60);
}

void draw_window_destroy()
{
    CloseWindow();
}

bool draw_window_should_close() { return WindowShouldClose(); }

void draw_draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (unsigned int i=0; i<reg_shapes.count; i++) {
        draw_shape(reg_shapes.shapes[i]);
    }

    EndDrawing();
}
