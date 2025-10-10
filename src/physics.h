#ifndef PHYSICS_HEADER
#define PHYSICS_HEADER

#include "assert.h"
#include "box2d.h"

#define TIME_STEP 1.0f / 60.0f
#define SUB_STEP_COUNT 4
#define GROUND_EXTENT 1000
#define GROUND_HEIGHT 3

b2WorldId physics_make_world();
b2BodyId physics_make_ground(b2WorldId world_id);

#endif // PHYSICS_HEADER
