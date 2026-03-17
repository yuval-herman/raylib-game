#ifndef PHYSICS_H
#define PHYSICS_H

#include "box2d.h"

extern b2WorldId worldId;

void phys_init();
void phys_step();

#endif // PHYSICS_H
