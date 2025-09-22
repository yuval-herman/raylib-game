#if !defined(DRAW_MANAGER_H)
#define DRAW_MANAGER_H

#include <stdbool.h>

void draw_window_make();
void draw_window_destroy();
bool draw_window_should_close();
void draw_draw();

#endif // DRAW_MANAGER_H