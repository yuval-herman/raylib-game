#ifndef PHYSICS_H
#define PHYSICS_H

#include "box2d.h"

extern b2WorldId worldId;

void phys_init();
void phys_step();

// Convert box2d 2d vector to raylib
#define B2RV2(vec)                                                             \
  (Vector2) { .x = vec.x, .y = vec.y }
// Convert raylib 2d vector to box2d
#define R2BV2(vec)                                                             \
  (b2Vec2) { .x = vec.x, .y = vec.y }

#endif // PHYSICS_H
