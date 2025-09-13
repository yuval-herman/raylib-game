#ifndef PHYSICS_HEADER
#define PHYSICS_HEADER

#include "assert.h"
#include "box2d.h"

#define TIME_STEP 1.0f / 60.0f
#define SUB_STEP_COUNT 4
#define GROUND_EXTENT 1000

#define ARRAY_COUNT(A) (int)(sizeof(A) / sizeof(A[0]))

b2WorldId physics_make_world();
b2BodyId physics_make_ground(b2WorldId world_id);

#define getRectOrigin(rect) \
    (Vector2){rect.width / 2, 0}

#define b2rVec(vec) \
    (Vector2) { vec.x, -vec.y }

#define r2bVec(vec) \
    (b2Vec2) { vec.x, -vec.y }

#endif // PHYSICS_HEADER
