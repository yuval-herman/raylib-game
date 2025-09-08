#ifndef PHYSICS_HEADER
#define PHYSICS_HEADER

#include "assert.h"
#include "box2d.h"
#include "raylib.h"

#define TIME_STEP 1.0f / 60.0f
#define SUB_STEP_COUNT 4
#define GROUND_EXTENT 1000

#define ARRAY_COUNT(A) (int)(sizeof(A) / sizeof(A[0]))

b2WorldId physics_make_world();
b2BodyId physics_make_ground(b2WorldId world_id);

#define getRectOrigin(rect) \
    (Vector2){rect.width / 2, 0}

Vector2 b2rVec(b2Vec2 vec);

b2Vec2 r2bVec(b2Vec2 vec);

#endif // PHYSICS_HEADER
