#include "physics.h"

b2WorldId world_id;
b2BodyId ground_id;

void physics_make_ground()
{
    b2BodyDef body_def = b2DefaultBodyDef();
    body_def.position = (b2Vec2){0.0f, -10.0f};

    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.material.friction = 1.0f;

    b2Polygon polygon = b2MakeBox(GROUND_EXTENT, 10.0f);

    ground_id = b2CreateBody(world_id, &body_def);

    b2CreatePolygonShape(ground_id, &shape_def, &polygon);
}

void physics_init_world()
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
    world_id = b2CreateWorld(&worldDef);
    b2World_EnableSleeping(world_id, false);

    physics_make_ground();
}

Vector2 b2rVec(b2Vec2 vec)
{
    return (Vector2){vec.x, -vec.y};
}
b2Vec2 r2bVec(b2Vec2 vec)
{
    return (b2Vec2){vec.x, -vec.y};
}