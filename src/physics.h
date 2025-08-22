#if !defined(PHYSICS_HEADER)
#define PHYSICS_HEADER

#include "assert.h"
#include "box2d.h"

b2WorldId worldId;
float timeStep = 1.0f / 60.0f;
int subStepCount = 4;

void initWorld()
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
    worldId = b2CreateWorld(&worldDef);
    assert(b2World_IsValid(worldId));
}

typedef struct MakeBodyParams
{
    b2BodyDef body;
    b2Polygon polygon;
    b2ShapeDef shape;
} MakeBodyParams;

#define makeBody(...) _makeBody((MakeBodyParams){ \
    .body = b2DefaultBodyDef(),                   \
    .shape = b2DefaultShapeDef(),                 \
    __VA_ARGS__})

b2BodyId _makeBody(MakeBodyParams params)
{
    b2BodyId bodyId = b2CreateBody(worldId, &params.body);
    assert(b2Body_IsValid(bodyId));

    b2CreatePolygonShape(bodyId, &params.shape, &params.polygon);

    return bodyId;
}

#define setRaylibPos(p1, p2) \
    do                       \
    {                        \
        p1.x = p2.x;         \
        p1.y = -p2.y;        \
    } while (0)
#define getRectOrigin(rect) (Vector2){rect.width / 2, rect.height / 2}

#endif // PHYSICS_HEADER
