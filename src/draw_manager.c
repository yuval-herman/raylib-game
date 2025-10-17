#include "draw_manager.h"
#include "raylib.h"
#include "rlgl.h"
#include "utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SHAPE_ARRAY_INIT 32

typedef enum Draw_shapes {
    #define X(n_enum, n_struct, n_low) n_enum,
    DRAW_SHAPES
    #undef X
    VOID_SHAPE // Used to signify unused space in shape array
} Draw_shapes;

typedef union Shapes_union{
    #define X(n_enum, n_struct, n_low) n_struct n_enum;
    DRAW_SHAPES
    #undef X
} Shapes_union;

typedef struct Draw_shape {
    int z_index;
    Draw_shapes type;
    Shapes_union shape;
} Draw_shape;

typedef struct Shapes_array {
  Draw_shape* shapes;
  // counts the number of non-void shapes in the array
  size_t used_count;
  // counts the number of to total amount of shapes in the array
  size_t total_count;
  size_t capacity;
} Shapes_array;

Shapes_array reg_shapes = {0};
Draw_shape* reg_shapes_buffer = {0};

static inline void init_reg_shapes() {
    if(reg_shapes.capacity==0) {
      reg_shapes.shapes = calloc(SHAPE_ARRAY_INIT, sizeof reg_shapes.shapes[0]);
      reg_shapes_buffer = calloc(SHAPE_ARRAY_INIT, sizeof reg_shapes.shapes[0]);
      reg_shapes.capacity = SHAPE_ARRAY_INIT;
    }
}

size_t register_shape(Draw_shape shape) {
    init_reg_shapes();
    size_t shape_idx;
#ifdef NDEBUG
    bool found_empty = false;
#endif
    if(reg_shapes.used_count < reg_shapes.total_count) {
        for (size_t i=0; i<reg_shapes.total_count; i++) {
            if(reg_shapes.shapes[i].type==VOID_SHAPE) {
                shape_idx = i;
#ifdef NDEBUG
                found_empty = true;
#endif
                break;
            }
        }
#ifdef NDEBUG
        assert(found_empty);
#endif
    }
    else if (reg_shapes.capacity<=reg_shapes.total_count) {
      reg_shapes.capacity *= 2;
      reg_shapes.shapes = realloc(reg_shapes.shapes, sizeof reg_shapes.shapes[0] * reg_shapes.capacity);
      reg_shapes_buffer = realloc(reg_shapes_buffer, sizeof reg_shapes_buffer[0] * reg_shapes.capacity);
      shape_idx = reg_shapes.total_count;
      reg_shapes.total_count++;
    }
    else {
      shape_idx = reg_shapes.total_count;
      reg_shapes.total_count++;
    }
    Draw_shape *new_shape = &reg_shapes.shapes[shape_idx];
    *new_shape = shape;
    reg_shapes.used_count++;
    return shape_idx;
}

void draw_remove_shape(size_t handle) {
    assert(handle<reg_shapes.total_count);
    Draw_shape* shape = reg_shapes.shapes+handle;
    // double remove does nothing
    if(shape->type == VOID_SHAPE) {
        log_msg(U_LOG_WARN, "attempting shape double free");
        return;
    }
    log_debug("removing shape at %ld", handle);
    shape->type = VOID_SHAPE;
    reg_shapes.used_count--;
}

#define X(n_enum, n_struct, n_low) size_t draw_register_##n_low(n_struct n_low, int z_index) { \
    log_debug("registering draw "#n_low); \
    return register_shape((Draw_shape){.z_index = z_index, .type=n_enum, .shape={.n_enum = n_low}});}
DRAW_SHAPES
#undef X

#define X(n_enum, n_struct, n_low) n_struct* draw_get_##n_low(size_t handle) { \
    assert(handle<reg_shapes.total_count); \
    Draw_shape* shape = reg_shapes.shapes+handle; \
    assert(shape->type == n_enum); \
    return &shape->shape.n_enum;}
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
    case VOID_SHAPE: break;
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

    init_reg_shapes();
}

void draw_window_destroy()
{
    CloseWindow();
}

bool draw_window_should_close() { return WindowShouldClose(); }

int z_index_cmp(const void* a, const void* b) {
    Draw_shape arg1 = *(const Draw_shape*)a;
    Draw_shape arg2 = *(const Draw_shape*)b;
 
    if (arg1.z_index < arg2.z_index) return -1;
    if (arg1.z_index > arg2.z_index) return 1;
    return 0;
}

void draw_draw()
{
    static Camera2D camera = {.zoom = 10,
       .offset = (Vector2){.x = 500,
       .y = 300}};

    memcpy(reg_shapes_buffer, reg_shapes.shapes, reg_shapes.total_count * sizeof reg_shapes_buffer[0]);
    qsort(reg_shapes_buffer, reg_shapes.total_count, sizeof reg_shapes_buffer[0], z_index_cmp);

    log_debug("starting draw call. drawing %ld shapes out of %ld total shapes and memory reserved for %ld shapes.", reg_shapes.used_count, reg_shapes.total_count, reg_shapes.capacity);
   
    BeginDrawing();
    ClearBackground(RAYWHITE);
        BeginMode2D(camera);
            rlScalef(1,-1,1);
            for (size_t i=0; i<reg_shapes.total_count; i++) {
                draw_shape(reg_shapes_buffer[i]);
            }
        EndMode2D();
    EndDrawing();
}
