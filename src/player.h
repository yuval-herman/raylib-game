#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

void player_init();
void player_update(float deltaTime);
void player_draw();
Rectangle player_get_rect();
Vector2 player_get_velocity();
int player_get_is_facing_right();
float player_get_dash_cooldown();
int player_get_can_double_jump();

#endif // PLAYER_H
