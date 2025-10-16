#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

typedef enum Game_screen {
  SCREEN_RUNNER,
} Game_screen;

void screen_init();
void screen_destroy();
void set_screen(Game_screen screen);
void screen_draw();
void screen_update();

#endif
