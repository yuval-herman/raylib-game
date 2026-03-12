#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

void player_init();
void player_update(float deltaTime);
void player_draw();
Rectangle player_get_rect();
float player_get_speed();

#endif // PLAYER_H
