#if !defined(DRAW_MANAGER_H)
#define DRAW_MANAGER_H

#include <stdbool.h>

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

typedef struct Draw_Line {
  Draw_Vector2 start;
  Draw_Vector2 end;
  float width;
  Draw_Color color;
} Draw_Line;

Draw_Circle* draw_register_circle(Draw_Circle circle);
Draw_Line* draw_register_line(Draw_Line line);

void draw_window_make();
void draw_window_destroy();
bool draw_window_should_close();
void draw_draw();

#endif // DRAW_MANAGER_H
