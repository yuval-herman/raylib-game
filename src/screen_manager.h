#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

typedef enum Game_screen {
#define X(n_upper, n_lower, init_params) \
SCREEN_##n_upper,
#include "screens/screens.def"
#undef X
} Game_screen;

void screen_init();
void screen_destroy();
void set_screen(Game_screen screen);
void screen_draw();
void screen_update();

#endif
