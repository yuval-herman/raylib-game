#if !defined(DRAW_MANAGER_H)
#define DRAW_MANAGER_H
#include <stddef.h>
#include <stdbool.h>

// X(enum, struct, low-case name)
#define DRAW_SHAPES                     \
X(CIRCLE,    Draw_Circle,    circle)    \
X(RECTANGLE, Draw_Rectangle, rectangle) \
X(LINE,      Draw_Line,      line)

typedef struct Draw_Vector2 {float x, y;} Draw_Vector2;

typedef struct Draw_Color {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} Draw_Color;

typedef struct Draw_Circle {
  Draw_Vector2 pos;
  float radius;
  Draw_Color color;
} Draw_Circle;

typedef struct Draw_Rectangle {
  Draw_Vector2 pos; // top-left position
  float width;
  float height;
  Draw_Color color;
} Draw_Rectangle;

typedef struct Draw_Line {
  Draw_Vector2 start;
  Draw_Vector2 end;
  float width;
  Draw_Color color;
} Draw_Line;

#define X(n_enum, n_struct, n_low) size_t draw_register_##n_low(n_struct n_low, int z_index);
DRAW_SHAPES
#undef X

// Returns a shape pointer. The pointer needs to be cast to a shape to be used.
// Use more specific `draw_get_*` functions for better saftey.
void* draw_get_shape(size_t handle);
#define X(n_enum, n_struct, n_low) n_struct* draw_get_##n_low(size_t handle);
DRAW_SHAPES
#undef X

void draw_window_make();
void draw_window_destroy();
bool draw_window_should_close();
void draw_draw();

#endif // DRAW_MANAGER_H
